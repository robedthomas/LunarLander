/**
@file Project03_01.c
@author Rob Thomas
@brief The main source file of the Lunar Lander game.
@details This file contains the main function of Project03_01 (Lunar Lander).
It handles the primary processing loop of the game.
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_framerate.h>

#include <stdlib.h>
#include <stdbool.h>

#include "GameInitialization.h"
#include "GameFunctions.h"
#include "GameObjects.h"


/**
@def FPS
@brief A constant for the number of frames per second to display at.
*/
#define FPS 30

/**
@fn main
@brief The main function for Project03_01.
*/
int main(int argc, char *argv[])
{
	GameState state;
	Lander lander;
	Terrain terrain;
	Vertex firstVertex;
	Flat firstFlat;
	Uint16 heightMap[LEVEL_WIDTH];
	char *fileName, defaultFileName[] = "terrain.txt";
	int landingType;
	FPSmanager frameManager;


	/*** Initialize game state. ***/
		/* Initialize Vertex first and the terrain. */
	firstVertex.X = 0;
	firstVertex.Y = 0;
	firstVertex.next = NULL;
	terrain.firstVertex = &firstVertex;
	terrain.heightMap = heightMap;
		/* Initialize the first Flat. */
	firstFlat.X = 0;
	firstFlat.Y = 0;
	firstFlat.length = 0;
	firstFlat.scoreModifier = -1;
	firstFlat.next = NULL;
	terrain.firstFlat = &firstFlat;
		/* Initialize the Mix chunks. */
	Mix_Chunk *thrust = NULL;
	Mix_Chunk *boom = NULL;
	state.thrust = thrust;
	state.boom = boom;

		/* If an argument has been passed in from the command line, assume it is
		   the name of the input file. Otherwise, assume the input file is named
		   "terrain.txt". */
	if (argc >= 2)
	{
		fileName = argv[1];
	}
	else
	{
		fileName = defaultFileName;
	}

		/* Initialize the GameState struct with the structs built. */
	initializeGameState(&state, &lander, &terrain, fileName);


	/*** Initialize SDL. ***/
	if (!( initializeSDL("Lunar Lander", &state) ))
	{
		/* Test for errors in SDL initialization. */

		if (SDL_GetError()[0] == '\0')
		{
			fprintf(stderr, "Invalid title or game state passed to initializeSDL.\n");
		}
		else
		{
			fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
		}

		/* Clean up and close the game. */
		cleanAndExit(&state, EXIT_SDLINIT_FAIL);
	}


	/*** Initialize audio. ***/
		/* Test for errors in audio initialization. */
	if (!( initializeSound(&state) ))
	{
		fprintf(stderr, "Error encountered while initializing sound.\n");

		cleanAndExit(&state, EXIT_SOUND_FAIL);
	}


	/*** Initialize the FPSmanager. ***/
	SDL_initFramerate(&frameManager);
    SDL_setFramerate(&frameManager, FPS);


	/*** Begin game loop: ***/
	while (true)
	{
		/* Handle events from the user. If the user wants to quit,
		   exit the loop. */
		if (!(handleEvents(&state)))
		{
			break;
		}

		/* Apply one tick of time. Happens regardless of input from user. */
		applyTick(&state);

		/* Check for any collisions and handle them. */
		if (collisionDetected(state, &landingType))
		{
			/* Apply the collision to the game. */
			applyCollision(&state, landingType);
			
			/* If the lander is out of fuel, restart the game completely. */
			if (gameOver(state))
			{
				hardReset(&state);
			}
			/* Otherwise, respawn the lander and continue playing as normal. */
			else
			{
				softReset(&state);
			}
		}

		/* Wait until it's time to draw again. */
		SDL_framerateDelay(&frameManager);
		/* Draw the game state to the screen. */
		draw(state);
	}


	/*** Once out of the game loop, clean up SDL and close. ***/
	cleanAndExit(&state, EXIT_SUCCESS);
}