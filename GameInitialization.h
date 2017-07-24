/**
@file GameInitialization.h
@author Rob Thomas
@brief Contains constants and functions for initializing the Lunar Lander game.
*/

#ifndef LUNAR_LANDER_GAMEINITIALIZATION_H
#define LUNAR_LANDER_GAMEINITIALIZATION_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "GameObjects.h"

/* Exit error codes. */
#define EXIT_SUCCESS 0
#define EXIT_SDLINIT_FAIL 1
#define EXIT_SDLWAIT_FAIL 2
#define EXIT_FOPEN_FAIL 3
#define EXIT_BADFILE_FAIL 4
#define EXIT_EMPTYFILE_FAIL 5
#define EXIT_MAP_FAIL 6
#define EXIT_NO_VERTICES_FAIL 7
#define EXIT_SOUND_FAIL 8

/**
@def WINDOW_WIDTH
@brief The default width of the window (in pixels).
*/
#define WINDOW_WIDTH 900

/**
@def WINDOW_HEIGHT
@brief The default height of the window (in pixels).
*/
#define WINDOW_HEIGHT 500

/**
@def LEVEL_WIDTH
@brief The default width of the level (in pixels).
*/

#define LEVEL_WIDTH 900

/**
@def LEVEL_HEIGHT
@brief The default height of the level (in pixels).
*/

#define LEVEL_HEIGHT 500

/**
@def LANDER_LENGTH
@brief The default length of the lander (in pixels).
*/
#define LANDER_LENGTH 20

/**
@def LANDER_HEIGHT
@brief The default height of the lander (in pixels).
*/
#define LANDER_HEIGHT 25

/**
@def LANDER_X_START
@brief The default starting x coordinate (in pixels) of the bottom-left corner
of the lander.
*/
#define LANDER_X_START 100

/**
@def LANDER_Y_START
@brief The default starting y coordinate (in pixels) of the bottom-left corner
of the lander.
*/
#define LANDER_Y_START 375

/**
@def LANDER_VX_START
@brief The default starting horizontal velocity of the bottom-left corner
of the lander.
*/
#define LANDER_VX_START 0

/**
@def LANDER_VY_START
@brief The default starting vertical velocity of the bottom-left corner
of the lander.
*/
#define LANDER_VY_START 0

/**
@def FUEL_START
@brief The default starting fuel level.
*/
#define FUEL_START 1000

/**
@def TOP_SCORE_TIER
@brief The score multiplier of a landing strip of length FLAT_LAND_BASE.
*/
#define TOP_SCORE_TIER 5

/**
@def FLAT_LAND_BASE
@brief The base length a strip of flat land where lander can land safely.
*/
#define FLAT_LAND_BASE 24

/**
@def FLAT_LAND_INCREMENT
@brief The increment of a flat landing strip that will degrade its score tier.
For example, a landing strip of length FLAT_LAND_BASE + 3 * FLAT_LAND_INCREMENT
will have a score tier of TOP_SCORE_TIER - 3.
*/
#define FLAT_LAND_INCREMENT 8


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
						  char *fileName);

/**
@fn initializeSDL
@brief Initializes SDL to allow the use of its functions and features.
@param title The string title to name the window.
@param state Pointer to the already-initialized GameState struct.
*/
bool initializeSDL (char *title, GameState *state);

/**
@fn initializeSound
@brief Initializes the sound files used by the game.
@param state Pointer to the initialized GameState struct.
*/
bool initializeSound (GameState *state);

/**
@fn cleanAndExit
@brief Cleans up SDL's subsystems and exits the program.
@param state Pointer to the current GameState struct.
@oaram errorCode The code to pass to exit().
*/
void cleanAndExit(GameState *state, int errorCode);

/**
@fn getFlatLevel
@brief Fills in an empty height map with a flat level at Y = heightOfTerrain.
@param heightMap An empty Uint16 array of size levelWidth.
@param heightOfTerrain The height (in pixels) to create flat terrain at.
0 = bottommost pixel of screen.
*/
void getFlatLevel(Uint16 *heightMap, Uint16 heightOfTerrain);

/**
@fn buildHeightMap
@brief Builds the terrain height map from an input file of vertices. 
@param fileName The file of vertices to build the height map from.
@param heightMap The array of Uint16s that will contain the height map.
@param levelWidth The width (in pixels) of the level whose height map is
being built.
@param first Pointer to an initialized Vertex struct to be used as the first
Vertex in the vertex list.
*/
void buildHeightMap (char *fileName, Uint16 *heightMap, Uint16 levelWidth,
					 Vertex *first);

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
	                Uint16 levelWidth);

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
	                  Uint16 levelWidth);

/**
@fn readVertexList
@brief Reads in the list of vertices describing the terrain from an input file
and converts it into a linked list of Vertexes.
@param fileName The name of the file to read from.
@param first Pointer to the first Vertex in the list (statically allocated).
@param levelWidth The width of the level (in pixels).
*/
void readVertexList(char* fileName, Vertex *first, Uint16 levelWidth);

/**
@fn findLandingStrips
@brief Finds vertices that define flat strips of terrain at which the lander can
safely land. 
@param state The already initialized GameState struct.
*/
void findLandingStrips (GameState *state);

/**
@fn freeVertexList
@brief Frees the linked list of Vertexes used for drawing terrain. Doesn't free
the first Vertex because it is statically allocated.
@param first Pointer to the first Vertex in the list.
*/
void freeVertexList(Vertex *first);

/**
@fn freeFlatList
@brief Frees the linked list of Flat structs.
@param first Pointer to the first Flat in the list.
*/
void freeFlatList (Flat* first);


#endif /* LUNAR_LANDER_GAMEINITIALIZATION_H */