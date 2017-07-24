/**
@file GameFunctions.h
@author Rob Thomas
@brief Contains functions for the Lunar Lander game.
@details This file contains the many functions that process and draw the 
lunar lander game.
*/
#ifndef LUNAR_LANDER_GAMEFUNCTIONS_H
#define LUNAR_LANDER_GAMEFUNCTIONS_H

#include <SDL2/SDL.h>

#include <stdbool.h>

#include "GameObjects.h"

/**
@def GRAVITY
@brief A constant for the downward acceleration per tick of the lander.
*/
#define GRAVITY 0.005

/**
@def LANDING_THRESHOLD
@brief A constant that dictates the max velocity for a landing to take place.
*/
#define LANDING_THRESHOLD 1

/**
@def UP_THRUST_POWER
@brief A constant that dictates how much the vertical velocity is increased by
when the UP arrow key is pressed.
*/
#define UP_THRUST_POWER 0.04

/**
@def LEFT_THRUST_POWER
@brief A constant that dictates how much the horizonal velocity is increased by
when the LEFT arrow key is pressed.
*/
#define LEFT_THRUST_POWER 0.1

/**
@def RIGHT_THRUST_POWER
@brief A constant that dictates how much the horizontal velocity is decreased by
when the RIGHT arrow key is pressed.
*/
#define RIGHT_THRUST_POWER 0.1

/**
@def THRUST_FUEL_COST
@brief A constant that dictates how much fuel is lost by thrusting for one tick.
*/
#define THRUST_FUEL_COST 1

/**
@def SCORE_FOR_LANDING
@brief The default amount of score to gain from a proper landing,
later multiplied by the score multiplier for that landing spot.
*/
#define SCORE_FOR_LANDING 100

/**
@def CRASH_FUEL_COST
@brief The default amount of fuel lost when the lander crashes.
*/
#define CRASH_FUEL_COST 200

/**
@def TEXT_Y_DELTA
@brief The default distance (in pixels) between rows of text draw in the game.
*/
#define TEXT_Y_DELTA 12

/**
@def HORIZONTAL_SCROLL_THRESHOLD
@brief A constant that dictates how close (as a ratio of WINDOW_WIDTH) 
the lander must be to the horizontal edges of the window to begin scrolling.
*/
#define HORIZONTAL_SCROLL_THRESHOLD 4

/**
@def TOP_SCROLL_THRESHOLD
@brief A constant that dictates how close (as a ratio of WINDOW_HEIGHT) 
the lander must be to the top edge of the window to begin scrolling.
*/
#define TOP_SCROLL_THRESHOLD 8

/**
@def BOTTOM_SCROLL_THRESHOLD
@brief A constant that dictates how close (as a ratio of WINDOW_HEIGHT) 
the lander must be to the bottom edge of the window to begin scrolling.
*/
#define BOTTOM_SCROLL_THRESHOLD 2

/**
@def SCORE_MOD_FLASH_TIME
@brief Defines the number of ms that that score modifiers are flashed
for (and not flashed for).
*/
#define SCORE_MOD_FLASH_TIME 500


/**
@fn draw
@brief Draws the game to the window.
@param state The GameState representing the state of the game at this instant.
*/
void draw (GameState state);

/**
@fn drawLander
@brief Draws the lander to the window.
@param state The current GameState struct.
*/
void drawLander (GameState state);

/**
@fn drawTerrain
@brief Draws the terrain of the level using the list of vertices.
@param state The current GameState struct.
*/
void drawTerrain (GameState state);

/**
@fn drawHeightMap
@brief Draws a pixel representation of the height map of the terrain. Used for
debug purposes only.
@param state The current GameState struct.
*/
void drawHeightMap (GameState state);

/**
@fn drawScoreModifiers
@brief Draws flashing score modifiers below each strip of flat landing terrain.
@param state The current GameState struct.
*/
void drawScoreModifiers (GameState state);

/**
@fn drawStandardInfo
@brief Writes to the window information about the game and lander.
@param state The current GameState struct.
*/
void drawStandardInfo (GameState state);

/**
@fn drawDebugInfo
@brief Draws text to the screen (in red) displaying debug info.
@param state The current GameState struct.
*/
void drawDebugInfo (GameState state);

/**
@fn handleEvents
@brief Processes all user events submitted up to this point.
@param state Pointer to the current GameState struct.
@return False if the user attempted to quit the game, true otherwise.
*/
bool handleEvents (GameState *state);

/**
@fn handleKey
@brief Processes a keystroke made by the user.
@param state Pointer to the current GameState struct.
@param event Pointer to the event of the keystroke in question.
*/
void handleKey (GameState *state, SDL_Event *event);

/**
@fn applyTick
@brief Applies one tick of game time to the game state. 
Independent of user action.
@param state Pointer to the current GameState struct.
*/
void applyTick (GameState *state);

/**
@fn scrollFocusPoint
@brief Scrolls the screen if the lander gets too close to one edge.
@param state Pointer to the current GameState struct.
*/
void scrollFocusPoint (GameState *state);

/**
@fn collisionDetected
@brief Detects if a collision has occurred between the lander and terrain.
@param state The GameState struct representing the current state of the game.
@param landingType An int buffer, will be overwritten with 1 if a proper landing
occurred or 2 if a crash occurred. 0 if no collision detected.
@return True if a collision was detected, false otherwise.
*/
bool collisionDetected (GameState state, int *landingType);

/**
@fn isLandingSpeed
@brief Reports whether or not the lander is going slow enough for a proper 
landing.
@param state The current GameState struct.
@return True if the lander's speed is at or under LANDING_THRESHOLD.
False otherwise.
*/
bool isLandingSpeed (GameState state);

/**
@fn isFlatLand
@brief Approximates if the terrain between x positions x1, x2, and x3 is flat.
@param state The current GameState struct.
@param x1 An x coordinate to measure terrain at.
@param x2 An x coordinate to measure terrain at.
@param x3 An x coordinate to measure terrain at.
@return True if the terrain is at the same height at each of the given 
positions. False otherwise.
*/
bool isFlatLand (GameState state, int x1, int x2, int x3);

/**
@fn applyCollision
@brief Alters the game state following a collision given the type of collision.
@param state Pointer to the current GameState struct.
@param landingType The type of collision that has occurred.
*/
void applyCollision(GameState *state, int landingType);


/**
@fn showCollisionMessage
@brief Displays a message for the user about what type of collision happened.
@param state Pointer to the current GameState struct.
@param score The score gained (if positive or 0) OR the fuel lost (if negative).
*/
void showCollisionMessage (GameState *state, int score);

/**
@fn waitForResponse
@brief Waits for the user to respond with a mouse button press or a key press 
that is NOT one of the arrow keys.
@param state Pointer to the current GameState struct.
*/
void waitForResponse (GameState *state);

/**
@fn gameOver
@brief Reports whether or not the game is over now.
@param state The GameState struct representing the current state of the game.
@return True if the land has no more fuel. False otherwise.
*/
bool gameOver (GameState state);

/**
@fn hardReset
@brief Refreshes the game as if it were just relaunched.
@param state Pointer to the current GameState struct.
*/
void hardReset (GameState *state);

/**
@fn softReset
@brief Respawns the lander and continues the game.
@param state Pointer to the current GameState struct.
*/
void softReset (GameState *state);

/**
@fn getMinutes
@brief Returns the number of elapsed minutes in game time (mod 100).
@param state The current GameState struct.
@return The number of minutes elapsed while playing.
*/
int getMinutes (GameState state);

/**
@fn getSeconds 
@brief Returns the number of elapsed seconds in game time (mod 60).
@param state The current GameState struct.
@return The number of seconds elapsed while playing.
*/
int getSeconds (GameState state);

/**
@fn getAltitude 
@brief Returns the distance between the terrain and the lander.
@param state The current GameState struct.
@return The distance (in pixels) between the terrain and lander.
*/

int getAltitude (GameState state);

/**
@fn getVelocity 
@brief Calculates the magnitude of the lander's velocity.
@param state The GameState struct representing the current state of the game.
@return The magnitude of the lander's velocity as a positive double.
*/
double getVelocity (GameState state);

/**
@fn min
@brief Returns the smaller of two ints.
@param first One of two ints to compare.
@param second One of two ints to compare.
@return The smaller of the two ints.
*/
int min (int first, int second);

/**
@fn max
@brief Returns the larger of two ints.
@param first One of two ints to compare.
@param second One of two ints to compare.
@return The larger of the two ints.
*/
int max (int first, int second);

#endif /* LUNAR_LANDER_GAMEFUNCTIONS_H */