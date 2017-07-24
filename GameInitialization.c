/**
@file GameInitialization.c
@author Rob Thomas
@brief Contains constants and functions for initializing the Lunar Lander game.
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

#include "GameObjects.h"

#include "GameInitialization.h"


/**
@fn initializeGameState
@brief Initializes a GameState's fields.
@param state Pointer to the struct to initialize.
@param lander Pointer to an empty Lander struct.
@param terrain Pointer to a terrain struct whose data members have been 
initialized.
@param fileName String containing the name of the file to read vertices from.
*/
void initializeGameState (GameState *state, Lander *lander, Terrain *terrain,
						  char *fileName)
{
	/*** First fill the Lander passed in. ***/
	lander->realX = LANDER_X_START;
	lander->realY = LANDER_Y_START;
	lander->X = LANDER_X_START;
	lander->Y = LANDER_Y_START;
	lander->length = LANDER_LENGTH;
	lander->height = LANDER_HEIGHT;
	lander->horVelocity = LANDER_VX_START;
	lander->vertVelocity = LANDER_VY_START;


	/*** Fill the GameState passed in with the Lander and defaults values. ***/
	state->lander = lander;

	state->levelWidth = LEVEL_WIDTH;
	state->levelHeight = LEVEL_HEIGHT;
	
	state->terrain = terrain;
	buildHeightMap(fileName, terrain->heightMap, state->levelWidth, 
		           terrain->firstVertex);
	findLandingStrips(state);

	state->window = NULL;
	state->renderer = NULL;

	state->focusPointX = 0;
	state->focusPointY = WINDOW_HEIGHT;

	state->realFocusPointX = 0;
	state->realFocusPointY = WINDOW_HEIGHT;

	state->timeStart = -1;
	state->timeElapsed = 0;
	state->score = 0;
	state->fuel = FUEL_START;
}

/**
@fn initializeSDL
@brief Initializes SDL to allow the use of its functions and features.
@param title The string title to name the window.
@param state Pointer to the already-initialized GameState struct.
*/
bool initializeSDL (char *title, GameState *state)
{
	if (title == NULL)
	{
		return false;
	}
	if (state == NULL)
	{
		return false;
	}

	/* Initialize SDL with video and audio systems. */
	if( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) ) 
	{
		return false;
	}

	/* Create a window centered in the screen and assign it to state. */
	if (!( state->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, 
		                                    SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, 
		                                    WINDOW_HEIGHT, SDL_WINDOW_SHOWN) ))
	{
		return false;
	}

	/* Create a renderer for the window with VSYNC and hardware acceleration
	   enabled. */
	if (!( state->renderer = SDL_CreateRenderer(state->window, -1, 
												SDL_RENDERER_ACCELERATED | 
												SDL_RENDERER_PRESENTVSYNC) ))
	{
		return false;
	}


	return true;
}

/**
@fn initializeSound
@brief Initializes the sound files used by the game.
@param state Pointer to the initialized GameState struct.
*/
bool initializeSound (GameState *state)
{
	/* Initialize SDL_mixer first. */
	if (Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1)
	{
		return false;
	}

	/* Load the sound effects next. */
	state->thrust = Mix_LoadWAV("thrust.wav");
	state->boom = Mix_LoadWAV("boom.wav");
	state->ding = Mix_LoadWAV("land.wav");

	if (state->thrust == NULL || state->boom == NULL)
	{
		return false;
	}

	return true;
}

/**
@fn cleanAndExit
@brief Cleans up SDL's subsystems and exits the program.
@param state Pointer to the current GameState struct.
@oaram errorCode The code to pass to exit().
*/
void cleanAndExit(GameState *state, int errorCode)
{
	/* Free the window and renderer. */
	SDL_DestroyRenderer(state->renderer);
	SDL_DestroyWindow(state->window);

	/* Free the audio chunks and mixer. */
	Mix_FreeChunk(state->thrust);
	Mix_FreeChunk(state->boom);
	Mix_CloseAudio();

	/* Exit SDL. */
	if(SDL_WasInit(0))
	{
		SDL_Quit();
	}

	/* Free the list of Vertexes used for drawing terrain. */
	freeVertexList(state->terrain->firstVertex);
	/* Free the list of Flats. */
	freeFlatList(state->terrain->firstFlat);

	/* Exit with the given code. */
	exit(errorCode);
}

/**
@fn getFlatLevel
@brief Fills in an empty height map with a flat level at Y = heightOfTerrain.
@details Fills in an empty height map with the same value at each position. 
The level produced by this will be completely flat and will have terrain at 
the heightOfTerrain.
@param heightMap An empty Uint16 array of size levelWidth.
@param heightOfTerrain The height (in pixels) to create flat terrain at.
0 = bottommost pixel of screen.
*/
void getFlatLevel(Uint16 *heightMap, Uint16 heightOfTerrain)
{
	for (int i = 0; i < LEVEL_WIDTH; i++)
	{
		heightMap[i] = heightOfTerrain;
	}
}

/**
@fn buildHeightMap
@brief Builds the terrain height map from an input file of vertices.
@details This function dynamically constructs the terrain height map from an 
input file containing the X and Y coordinates of each vertex. The vertices are
line-delineated and the X and Y coordinates are separated by a single space.
A list of Vertexes is dynamically allocated and MUST BE FREED later. 
@param fileName The file of vertices to build the height map from.
@param heightMap The array of Uint16s that will contain the height map.
@param levelWidth The width (in pixels) of the level whose height map is
being built.
@param first Pointer to an initialized Vertex struct to be used as the first
Vertex in the vertex list.
*/
void buildHeightMap (char *fileName, Uint16 *heightMap, Uint16 levelWidth,
					 Vertex *first)
{
	float fHeightMap[levelWidth];
	float slopeMap[levelWidth];

	/*** Read in the vertices from the input file. ***/
	readVertexList(fileName, first, levelWidth);

	/*** Define the height map based on the vertices: ***/

	/*** First, for each Vertex in the list, insert the slope to the next
		   Vertex at the current Vertex's point. ***/
	buildSlopeMap(slopeMap, fHeightMap, first, levelWidth);

	/*** Next, fill in fHeightMap with the fractional values at each X. ***/
			/* The first column will always have a vertex, so set its height to
			   that Vertex's height. */
	buildFHeightMap(fHeightMap, slopeMap, first, levelWidth);

	/*** Then, insert the rounded height into the heightMap. 
	     (Heights are rounded up.) ***/
	for (int x = 0; x < levelWidth; x++)
	{
		heightMap[x] = (Uint16)( ceil(fHeightMap[x]) );
	}
}

/**
@fn buildSlopeMap
@brief Fills in the slope map with the slope of the terrain at each column.
@param slopeMap The map of the terrain's slope to be filled. An array of size 
levelWidth.
@param fHeightMap An array of size levelWidth. fHeightMap will only be altered 
wherever an undefined slope is found.
@param first Pointer to the first Vertex in the linked Vertex list.
@param levelWidth The width of the level (in pixels).
*/
void buildSlopeMap (float *slopeMap, float *fHeightMap, Vertex *first, 
	                Uint16 levelWidth)
{
	Vertex *current = first;

	while (current->next != NULL)
	{
		/* Check for undefined (straight up or down) slope. If so, move along
		   the Vertex list until one is found that is beyond the current X, 
		   and set the height at the current point to be the highest of the Y
		   values of the Vertexes scanned. */
		if (current->X == current->next->X)
		{
			/* Track the largest Y value seen yet for this undefined slope. */
			int maxY;
			if (current->Y >= current->next->Y)
			{
				maxY = current->Y;
			}
			else
			{
				maxY = current->next->Y;
			}

			current = current->next;

			/* Move along the list until a different X value is found, 
			   indicating the end of the undefined slope. */
			while (current->X == current->next->X)
			{
				if (current->next->Y > maxY)
				{
					maxY = current->next->Y;
				}

				current = current->next;
			}

			/* Set the height and slope at that X to indicate an undefined
			   slope. Also set the height of the next column to -1 so that
			   it is evident that the slope must be undefined. This must be done
			   because there is no way to represent undefined using a float. */
			fHeightMap[current->X] = (float)(maxY);
			/* A slope of 0 leading to a height of -1.0 indicates an undefined 
			   slope. heightMap[X + 1] will be overwritten to the correct value 
			   later. */
			slopeMap[current->X] = 0;
			if (current->X < levelWidth - 1)
			{
				fHeightMap[current->X + 1] = -1.0;
			}

			/* Fill in the slope to the next X (this slope is defined.) */
			float slope = ( (float)(current->next->Y - current->Y) / 
							(float)(current->next->X - current->X) );

			/* Now, fill in every slope from this X to the next with the
			   calculated slope. */
			for (int x = current->X + 1; x < current->next->X; x++)
			{
				slopeMap[x] = slope;
			}
		}

		else
		{
			/* If the slope is not undefined, calculate it. */
			float slope = ( (float)(current->next->Y - current->Y) / 
							(float)(current->next->X - current->X) );

			/* Now, fill in every slope from this X to the next with the
			   calculated slope. */
			for (int x = current->X; x < current->next->X; x++)
			{
				slopeMap[x] = slope;
			}
		}

		/* Advance to the next Vertex. */
		current = current->next;
	}
}

/**
@fn buildFHeightMap
@brief Fills in the fractional height map with the fractional height of the 
terrain at each column.
@param fHeightMap The fractional height map to be built. An array of size 
levelWidth.
@param fHeightMap The map of the terrain's slope. An array of size levelWidth.
@param first Pointer to the first Vertex in the linked Vertex list.
@param levelWidth The width of the level (in pixels).
*/
void buildFHeightMap (float *fHeightMap, float *slopeMap, Vertex *first, 
	                  Uint16 levelWidth)
{
	/* Make sure the first Vertex actually has X = 0. */
	if (first->X != 0)
	{
		fprintf(stderr, "Error encountered building fHeightMap - first Vertex has non-zero X.\n");
		exit(EXIT_MAP_FAIL);
	}

	fHeightMap[0] = (float)(first->Y);

	for (int x = 1; x < levelWidth; x++)
	{
		/* Check for an undefined slope. */
		if (slopeMap[x] == 0 && x < levelWidth - 1 && 
			fHeightMap[x + 1] == -1.0)
		{
			/* Undefined slope detected. The fractional height has already been
			   properly defined. The fractional height of the next pixel is
			   equal to this height plus the slope at the next pixel. */
			fHeightMap[x+1] = fHeightMap[x] + slopeMap[x+1];
			x++;
			continue;
		}

		/* Otherwise, the fractional height is equal to the previous fractional
		   height plus the slope at the previous point. */
		fHeightMap[x] = fHeightMap[x-1] + slopeMap[x-1];
	}
}

/**
@fn readVertexList
@brief Reads in the list of vertices describing the terrain from an input file
and converts it into a linked list of Vertexes.
@param fileName The name of the file to read from.
@param first Pointer to the first Vertex in the list (statically allocated).
@param levelWidth The width of the level (in pixels).
*/
void readVertexList(char* fileName, Vertex *first, Uint16 levelWidth)
{
	FILE *file;
	char fileOpenMode = 'r';
	int X, Y, prevX = -1;
	Vertex *current;

	current = NULL;

	/*** Open the file. ***/
	if (!( file = fopen(fileName, &fileOpenMode) ))
	{
		fprintf(stderr, "Problem encountered opening file.\nfileName: %s\n", 
			    fileName);

		exit(EXIT_FOPEN_FAIL);
	}

	/* Prepare a linked list of Vertex structs to be dynamically allocated. */


	/*** Read in vertices until the end of file is reached or a bad vertex
	     is found. A bad vertex is one whose x is less than the previous 
	     vertex's, whose x is less than 0, or whose y is less than 0. ***/
	while (fscanf(file, "%d %d\n", &X, &Y) == 2)
	{
		/* Catch negative X or Y. */
		if (X < 0 || Y < 0 || X >= levelWidth)
		{
			/* If a bad vertex is found, close the file and exit. */
			if (fclose(file))
			{
				fprintf(stderr, "Problem encountered trying to close file %s\n", 
				    fileName);
			}

			/* Free the vertex list. */
			freeVertexList(first);

			if (X < 0 || Y < 0)
			{
				fprintf(stderr, "Vertex with X<0 or Y<0 found in file.\nfileName: %s\n", 
				    fileName);
			}
			else
			{
				fprintf(stderr, "Vertex with X>levelWidth found.\nfileName: %s\n", 
				    fileName);
			}
			

			exit(EXIT_BADFILE_FAIL);
		}
		/* Catch an X that is less than the previous X. */
		if (X < prevX)
		{
			/* If a bad vertex is found, close the file and exit. */
			if (fclose(file))
			{
				fprintf(stderr, "Problem encountered trying to close file %s\n", 
				    fileName);
			}

			/* Free the vertex list. */
			freeVertexList(first);

			fprintf(stderr, "Vertex earlier than previous one found in file.\nfileName: %s\n", 
				    fileName);

			exit(EXIT_BADFILE_FAIL);
		}


		/* If this is the first vertex, define the previous X to be the 
		   current X and define the first Vertex in the linked list. */
		if (prevX == -1)
		{
			prevX = X;

			/* If X of the first Vertex given isn't 0, then make the first 
			   Vertex have X = 0 and Y = 0 and make the current Vertex the
			   second in the list. */
			if (X != 0)
			{
				first->X = 0;
				first->Y = 0;

				/* Prepare a new Vertex. */
				first->next = (Vertex*)malloc(sizeof(Vertex));
				current = first->next;

				current->X = X;
				current->Y = Y;
			}
			else
			{
				first->X = X;
				first->Y = Y;

				current = first;
			}
		}
		/* Otherwise, make a new Vertex and define its X and Y with what 
		   was input. */
		else
		{
			current->next = (Vertex*)malloc(sizeof(Vertex));

			current = current->next;

			current->X = X;
			current->Y = Y;
		}
	}

	/* After reading is over, check if current is NULL. If so, an empty file
	   was given. */
	if (current == NULL)
	{
		fprintf(stderr, "File given was empty.\nfileName: %s\n", fileName);

		exit(EXIT_EMPTYFILE_FAIL);
	}
	/* If the last Vertex's X is not levelWidth - 1 or Y is not the same as the
	   first Vertex's Y, then make a new Vertex with the appropriate values. */
	if (current->X != levelWidth - 1 || current->Y != first->Y)
	{
		current->next = (Vertex*)malloc(sizeof(Vertex));

		current = current->next;

		current->X = levelWidth - 1;
		current->Y = first->Y;
	}
	/* Set current's next Vertex to NULL to indicate the end of the list. */
	current->next = NULL;

	/*** Close the file. ***/
	if (fclose(file))
	{
		fprintf(stderr, "Problem encountered trying to close file %s\n", 
		    fileName);
	}
}

/**
@fn findLandingStrips
@brief Finds vertices that define flat strips of terrain at which the lander can
safely land. 
@details Following the construction of the Vertex list, searches along the list 
for adjacent vertices at the same Y value. Then, creates a list of Flat structs
indicating where to display the flashing score indicator.
@param state The already initialized GameState struct.
*/
void findLandingStrips (GameState *state)
{
	Flat *currentFlat = state->terrain->firstFlat;
	Vertex *current = state->terrain->firstVertex;
	currentFlat->X = -1;
	Flat *prev = currentFlat;

	if (current == NULL)
	{
		fprintf(stderr, "Vertex list found to be empty while finding landing strips.\n");
		return;
	}

	while (current->next != NULL)
	{
		/*** Scroll along the list of vertices until you find two adjacent 
			 vertices with the same Y value but different X values. ***/
		if (current->Y == current->next->Y)
		{
			Vertex *end = current->next;

			currentFlat->X = current->X;
			currentFlat->Y = current->Y;


			/*** Scroll along the list of vertices until you find a Vertex with 
				 a different Y value (identifies the end of the Flat). ***/
			while (end->next != NULL && end->next->Y == end->Y)
			{
				end = end->next;
			}

			/*** Determine the length of the Flat. ***/
			currentFlat->length = (end->X - current->X);

			/*** Determine the score modifier of the Flat. ***/
			if (currentFlat->length < FLAT_LAND_BASE)
			{
				currentFlat->scoreModifier = TOP_SCORE_TIER + 1;
			}
			else
			{
				currentFlat->scoreModifier = TOP_SCORE_TIER - 
				((currentFlat->length - FLAT_LAND_BASE) / FLAT_LAND_INCREMENT);
			}

			/* Prevent score modifiers below 1. */
			if (currentFlat->scoreModifier < 1)
			{
				currentFlat->scoreModifier = 1;
			}

			/* Set Flats that are too short to have a score modifier 0. */
			if (currentFlat->scoreModifier > TOP_SCORE_TIER)
			{
				currentFlat->scoreModifier = 0;
			}

			/*** Create the next Flat in the list. ***/
			currentFlat->next = (Flat*)malloc(sizeof(Flat));

			prev = currentFlat;
			currentFlat = currentFlat->next;

			current = end;
		}

		else
		{	
			current = current->next;
		}
	}

	/*** Cap the flat list with a NULL pointer. ***/
	prev->next = NULL;

	/*** If this is still the first flat, then set the first flat to NULL. ***/
	if (prev->X == -1)
	{
		state->terrain->firstFlat = NULL;
	} 
}


/**
@fn freeVertexList
@brief Frees the linked list of Vertexes used for drawing terrain. Doesn't free
the first Vertex because it is statically allocated.
@param first Pointer to the first Vertex in the list.
*/
void freeVertexList (Vertex *first)
{
	Vertex *prev, *current;

	/* Note: the first Vertex is statically allocated. DO NOT FREE IT. */
	prev = first->next;
	current = prev->next;

	while (prev != NULL)
	{
		/* Free prev. */
		free(prev);

		/* Set prev to current. */
		prev = current;

		/* Advance current. */
		if (current != NULL)
		{
			current = current->next;
		}
	}
}

/**
@fn freeFlatList
@brief Frees the linked list of Flat structs.
@param first Pointer to the first Flat in the list.
*/
void freeFlatList (Flat* first)
{
	Flat *prev, *current;

	/* Note: the first Vertex is statically allocated. DO NOT FREE IT. */
	if (first->next != NULL)
	{
		prev = first->next;
		current = prev->next;

		while (prev != NULL)
		{
			/* Free prev. */
			free(prev);

			/* Set prev to current. */
			prev = current;

			/* Advance current. */
			if (current != NULL)
			{
				current = current->next;
			}
		}
	}
}
