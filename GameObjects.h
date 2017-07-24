/**
@file GameObjects.h
@author Rob Thomas
@brief Contains the GameState struct and other useful structs for Lunar Lander.
@details This file defines the GameState struct, which is used for tracking the
state of the game.
*/

#ifndef LUNAR_LANDER_GAMEOBJECTS_H
#define LUNAR_LANDER_GAMEOBJECTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


/**
@typedef Vertex
@brief A struct representing a vertex along the terrain.
@details Indicates a point where the slope of the terrain changes. These 
		 Vertexes are strung together to create an outline of the terrain, which
		 is transformed into the height map. The pointer to the next Vertex is
		 included in each struct. It is assumed that:
		 -no Vertex has X < 0.
		 -no Vertex has Y < 0.
		 -the next Vertex has X >= the current Vertex's X.
		 -the last Vertex has a NULL pointer to the next Vertex.
*/
typedef struct Vertex
{
	/* The X and Y coordinates of the Vertex. */
	int X;
	int Y;

	/* The next Vertex to look at. */
	struct Vertex *next;
} Vertex;

/**
@typedef Flat
@brief Contains data about landing strips and their score multipliers.
*/

typedef struct Flat
{
	/* The starting X and Y coordinates of the strip. */
	int X;
	int Y;

	/* The length of the strip (in pixels). */
	Uint16 length;

	/* The score multiplier for landing at this strip (not lower than 0). */
	Uint16 scoreModifier;

	/* Pointer to the next Flat in the list. */
	struct Flat *next;
} Flat;

/**
@typedef Terrain
@brief Contains data about a single column of terrain to be drawn.
@details The Terrain struct encapsulates all the data needed by the height map
		 to draw the terrain. The height map will in fact be an array of
		 Terrain structs.
*/
typedef struct Terrain
{
	/* The first Vertex in the Vertex list. */
	Vertex *firstVertex;

	/* The first Flat in the Flat list. */
	Flat *firstFlat;

	/* An array of size levelWidth that holds the height of terrain at each
	   X position. */
	Uint16 *heightMap;
} Terrain;

/**
@typedef Lander
@brief A struct representing the player's lunar lander.
@details The Lander struct encapsulates data about the lander's size, position, 
and velocity.
*/
typedef struct Lander
{
	/* The absolute (real) positions of the lander's bottom-left corner. */
	float realX;
	float realY;

	/* The rounded X and Y positions of the lander's bottom-left corner. */
	Uint16 X;
	Uint16 Y;

	/* The length (left to right) and height (bottom to top) of the lander. */
	Uint16 length;
	Uint16 height;

	/* The vertical and horizontal velocities of the lander. 
	   Vertical velocity:   + is up, - is down.
	   Horizontal velocity: + is right, - is left. 
	*/
	float vertVelocity;
	float horVelocity;
} Lander;

/**
@typedef GameState
@brief A struct to track and alter the state of the game consistently.
@details The GameState struct is used to maintain consistency in the state of
the game while moving between functions and to simplify altering the game due
to the user's input.
*/
typedef struct GameState
{
	/* The lander in the game. */
	Lander *lander;

	/* The dimensions of the level (in pixels). */
	Uint16 levelWidth;
	Uint16 levelHeight;

	/* @TODO: implement the Terrain struct... */
	/* The terrain's heightmap. */
	Terrain *terrain;

	/* The time (in ms) that the game was launched at. */
	Uint32 timeStart;

	/* The game's elapsed time. */
	Uint32 timeElapsed;

	/* The player's score. */
	Uint16 score;

	/* The fuel left in the lander. */
	Uint16 fuel;

	/* The window and renderer used for drawing. */
	SDL_Window *window;
	SDL_Renderer *renderer;

	/* The top-left corner of the slice of level being displayed on-screen. */
	Sint16 focusPointX;
	Uint16 focusPointY;

	/* The real (fractional) dimensions of the focus point. */
	float realFocusPointX;
	float realFocusPointY;

	/* The audio device to play sound files. */
	SDL_AudioDeviceID audioDevice;

	/* Sound files. */
	Mix_Chunk *thrust;
	Mix_Chunk *boom;
	Mix_Chunk *ding;

	/* Textures and sprites. */

} GameState;

#endif /* LUNAR_LANDER_GAMEOBJECTS_H */