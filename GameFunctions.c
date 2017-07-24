/**
@file GameFunctions.c
@author Rob Thomas
@brief Contains functions for the Lunar Lander game.
@details This file contains the many functions that process and draw the 
lunar lander game.
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "GameObjects.h"
#include "GameInitialization.h"

#include "GameFunctions.h"

/* @TODO implement variable levels. A level begins as an array of Vertex structs
		 which produces the height map (an array of Uint16s). A straight line is
		 drawn between each Vertex, supplying the height at each point on the
		 height map. */
/* @TODO implement level looping. If the end of level is reached, loop around
         to the beginning of the level seemlessly. */


/**
@fn draw
@brief Draws the game to the window.
@param state The GameState representing the state of the game at this instant.
*/
void draw (GameState state)
{
	/*** Clear screen to black. ***/
	SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
	SDL_RenderClear(state.renderer);

	/*** @TODO: If the lander is close to the ground, zoom in 2x. ***/

	/*** Draw the lander. ***/
	drawLander(state);

	/*** Draw terrain. ***/
	drawTerrain(state);

	/* @DEBUG: Optionally, draw the height map in red as well. */
	/* drawHeightMap(state); */

	/*** Draw score modifiers. ***/
	drawScoreModifiers(state);

	/*** Draw text. ***/
	drawStandardInfo(state);

	/*** @DEBUG: Optionally, draw debug text. ***/
	/* drawDebugInfo(state); */
	
	/*** @TODO: Draw background. ***/

	/*** Present the drawn renderer. ***/
	SDL_RenderPresent(state.renderer);
}

/**
@fn drawLander
@brief Draws the lander to the window.
@param state The current GameState struct.
*/
void drawLander (GameState state)
{
	/* Lander is drawn in white. */
	SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
		/* In current version, lander is represented by the line denoting
		   its bottom. 
		   Recall that the point at (0, 0) is at the TOP-left of the screen,
		   so the point at (0, LEVEL_HEIGHT) is at the BOTTOM-left of the
		   screen.*/

	/* Account for the lander being left of the focus point (wrapping around the
	   level border without the focus point doing so). In this case, shift the
	   lander's coordinates right by the length of the level. */
	if (state.lander->X < state.focusPointX)
	{
		SDL_RenderDrawLine(state.renderer, 
			(int)( state.lander->X + state.levelWidth - state.focusPointX), 
			(int)( state.focusPointY - state.lander->Y ),
			(int)( state.lander->X + state.lander->length + state.levelWidth - state.focusPointX ), 
			(int)( state.focusPointY - state.lander->Y ));
	}

	else
	{
		SDL_RenderDrawLine(state.renderer, 
		    (int)( state.lander->X - state.focusPointX), 
		    (int)( state.focusPointY - state.lander->Y ),
		    (int)( state.lander->X + state.lander->length - state.focusPointX ), 
		    (int)( state.focusPointY - state.lander->Y ));
	}
}

/**
@fn drawTerrain
@brief Draws the terrain of the level using the list of vertices.
@param state The current GameState struct.
*/
void drawTerrain (GameState state)
{
	/* Begin at the first Vertex. */
	Vertex *prev = NULL;
	Vertex *current = state.terrain->firstVertex;

	/* Scroll along the Vertex list until you find a Vertex whose X is greater 
	   than that of the focus point. Begin drawing at the Vertex BEFORE this 
	   one. */
	while (current != NULL && current->X <= state.focusPointX)
	{
		prev = current;

		current = current->next;
	}
	/* If there is no Vertex meeting this criterion, then the focus point is 
	   past all of the terrain - draw nothing and return. */
	if (current == NULL)
	{
		return;
	}
	/* If the first Vertex in the list meets this criterion, begin drawing
	   with it instead. */
	if (prev == NULL)
	{
		prev = state.terrain->firstVertex;
	}

	/* Set draw color to white. */
	SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);

	/* For each Vertex up to but not including the first Vertex whose X is 
	   greater than focusPointX + WINDOW_WIDTH (the window with focus), 
	   draw a line to the next Vertex. */
	int rightEdge = max(state.focusPointX + WINDOW_WIDTH, state.levelWidth);

	while (current != NULL && prev->X <= rightEdge)
	{
		SDL_RenderDrawLine( state.renderer, (int)(prev->X - state.focusPointX), 
			                (int)(state.focusPointY - prev->Y),
			                (int)(current->X - state.focusPointX),
			                (int)(state.focusPointY - current->Y) );

		prev = current;
		current = current->next;
	}

	/* Now, draw a line between each Vertex and the next so long as their Xs are
	   less than the right boundary of the focus window AND the right boundary
	   has wrapped around the level boundary. */
	rightEdge = (state.focusPointX + WINDOW_WIDTH) % state.levelWidth;

	if ( rightEdge <= state.focusPointX )
	{
		/* The Vertex with the lowest X will be Vertex first, so begin there. */
		prev = state.terrain->firstVertex;
		current = prev->next;

		while ( prev->X < rightEdge && current != NULL )
		{
			SDL_RenderDrawLine( state.renderer, 
								prev->X - state.focusPointX + state.levelWidth,
								state.focusPointY - prev->Y,
								current->X - state.focusPointX + state.levelWidth,
								state.focusPointY - current->Y );

			prev = prev->next;
			current = current->next;
		}
	}
}

/**
@fn drawHeightMap
@brief Draws a pixel representation of the height map of the terrain. Used for
debug purposes only.
@param state The current GameState struct.
*/
void drawHeightMap (GameState state)
{
	/* Establish the bounds of what to draw. */
	/*int iLow = max(state.focusPointX, 0);
	int iHigh = min(state.focusPointX + WINDOW_WIDTH, state.levelWidth);*/

	/* Set draw color to red. */
	SDL_SetRenderDrawColor(state.renderer, 255, 0, 0, 255);

	for (int i = 0; i < WINDOW_WIDTH; i++)
	{
		int X = (i + state.focusPointX) % state.levelWidth;
		SDL_RenderDrawPoint(state.renderer, 
							i, 
							state.focusPointY - state.terrain->heightMap[X]);
	}

	/*for (int i = iLow; i < iHigh; i++)
	{
		SDL_RenderDrawPoint(state.renderer, i - state.focusPointX, 
			 				(int)( state.focusPointY - state.terrain->heightMap[i] ));
	}*/
}

/**
@fn drawScoreModifiers
@brief Draws flashing score modifiers below each strip of flat landing terrain.
@param state The current GameState struct.
*/
void drawScoreModifiers (GameState state)
{
	int drawX, drawY;
	char text[8];
	Flat *current = state.terrain->firstFlat;

	/* Only show the score modifiers if this is in the first half of a second.*/
	if (state.timeElapsed % (SCORE_MOD_FLASH_TIME * 2) < SCORE_MOD_FLASH_TIME)
	{
		while (current != NULL)
		{
			/* For the current Flat, find the midpoint of its length, and draw under 
		   	   that point "xD", where D is the score modifier of the current Flat.*/
			if (current->scoreModifier > 0)
			{
				drawX = current->X + (current->length / 2) - 8 - state.focusPointX;
				if (drawX < 0)
				{
					drawX += state.levelWidth;
				}

				drawY = state.focusPointY - (current->Y - TEXT_Y_DELTA);
			
				sprintf(text, "x%1d", current->scoreModifier);
				stringRGBA(state.renderer, drawX, drawY, text, 255, 255, 255, 255);
			}

			current = current->next;
		}
	}
}


/**
@fn drawStandardInfo
@brief Writes to the window information about the game and lander.
@details In particular, writes text in white showing the user their score, the
amount of time they have played, the remaining fuel, the lander's altitude, and
the lander's horizontal and vertical velocities.
@param state The current GameState struct.
*/
void drawStandardInfo (GameState state)
{
	char text[64];
		/* Left side: Score, time, and fuel. */
	int writeX = WINDOW_WIDTH / 20;
	int writeY = WINDOW_HEIGHT / 20;

	sprintf(text, "SCORE: %04d", (int)(state.score));
	stringRGBA(state.renderer, writeX, writeY, text, 255, 255, 255, 255);

	writeY += TEXT_Y_DELTA;
	sprintf(text, "TIME:  %02d:%02d", getMinutes(state), getSeconds(state));
	stringRGBA(state.renderer, writeX, writeY, text, 255, 255, 255, 255);

	writeY += TEXT_Y_DELTA;
	sprintf(text, "FUEL:  %04d", (int)(state.fuel));
	stringRGBA(state.renderer, writeX, writeY, text, 255, 255, 255, 255);

		/* Right side: Altitude, horizontal speed, and vertical speed. */
	writeX = ((WINDOW_WIDTH * 19) / 20) - 188;
	writeY = WINDOW_HEIGHT / 20;

	sprintf(text, "ALTITUDE:          %04d", getAltitude(state));
	stringRGBA(state.renderer, writeX, writeY, text, 255, 255, 255, 255);

	writeY += TEXT_Y_DELTA;
	sprintf(text, "HORIZONTAL SPEED:  %03d", (int)(state.lander->horVelocity * 10));
	stringRGBA(state.renderer, writeX, writeY, text, 255, 255, 255, 255);

	writeY += TEXT_Y_DELTA;
	sprintf(text, "VERTICAL SPEED:    %03d", (int)(state.lander->vertVelocity * 25));
	stringRGBA(state.renderer, writeX, writeY, text, 255, 255, 255, 255);
}

/**
@fn drawDebugInfo
@brief Draws text to the screen (in red) displaying debug info.
@param state The current GameState struct.
*/
void drawDebugInfo (GameState state)
{
	char text[64];

	/*** Draw text in the top-middle of the screen. ***/
	int writeX = (WINDOW_WIDTH / 2) - 128;
	int writeY = WINDOW_HEIGHT / 20;

		/* Draw X and realX. */
	sprintf(text, "X: %04d  realX: %.2f", state.lander->X, state.lander->realX);
	stringRGBA(state.renderer, writeX, writeY, text, 255, 0, 0, 255);

		/* Draw Y and realY. */
	writeY += TEXT_Y_DELTA;
	sprintf(text, "Y: %04d  realY: %.2f", state.lander->Y, state.lander->realY);
	stringRGBA(state.renderer, writeX, writeY, text, 255, 0, 0, 255);

		/* Draw the focus point coordinates. */
	writeY += TEXT_Y_DELTA;
	sprintf(text, "FPoint X: %4d  Real FPoint X: %.2f", state.focusPointX, 
		    state.realFocusPointX);
	stringRGBA(state.renderer, writeX, writeY, text, 255, 0, 0, 255);

	writeY += TEXT_Y_DELTA;
	sprintf(text, "FPoint Y: %4d  Real FPoint Y: %.2f", state.focusPointY, 
		    state.realFocusPointY);
	stringRGBA(state.renderer, writeX, writeY, text, 255, 0, 0, 255);

	/* Draw the X coordinate of the focus window's right edge. */
	writeY += TEXT_Y_DELTA;
	sprintf(text, "Right edge of focus: %4d", 
		    (state.focusPointX + WINDOW_WIDTH) % state.levelWidth );
	stringRGBA(state.renderer, writeX, writeY, text, 255, 0, 0, 255);

		/* Draw the real velocities. */
	writeY += TEXT_Y_DELTA;
	sprintf(text, "horVelocity: %.2f  vertVelocity: %.2f", 
		    state.lander->horVelocity, state.lander->vertVelocity);
	stringRGBA(state.renderer, writeX, writeY, text, 255, 0, 0, 255);
}

/**
@fn handleEvents
@brief Processes all user events submitted up to this point.
@param state Pointer to the current GameState struct.
@return False if the user attempted to quit the game, true otherwise.
*/
bool handleEvents (GameState *state)
{
	SDL_Event event;
	bool run = true;

	/*** Loop through each unhandled event: ***/
	while (run && SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			/*** Check if the user closed the window. ***/
			case SDL_QUIT:
				run = false;
				break;
			/*** Check if the user made a keystroke. ***/
			case SDL_KEYDOWN:
			//case SDL_KEYUP:
				handleKey(state, &event);
				break;
			default:
				break;
		}
	}

	return run;
}

/**
@fn handleKey
@brief Processes a keystroke made by the user.
@param state Pointer to the current GameState struct.
@param event Pointer to the event of the keystroke in question.
*/
void handleKey (GameState *state, SDL_Event *event)
{
	switch(event->key.keysym.sym)
	{
		/*** If the user pressed the UP arrow, increase the lander's vertical
		     velocity and consume some fuel. ***/
		case SDLK_UP:
			if (state->fuel >= THRUST_FUEL_COST)
			{	
				state->lander->vertVelocity += UP_THRUST_POWER;
				state->fuel -= THRUST_FUEL_COST;

				/* Also play the thrust sound. */
				if ( Mix_PlayChannel(-1, state->thrust, 0) == -1 )
				{
					/*fprintf(stderr, "Problem playing thrust sound.\n");*/
				}
			}
			break;

		/*** If the user pressed the RIGHT arrow, decrease the lander's horizontal
		     velocity and consume some fuel. ***/
		case SDLK_RIGHT:
			if (state->fuel >= THRUST_FUEL_COST)
			{	
				state->lander->horVelocity -= RIGHT_THRUST_POWER;
				state->fuel -= THRUST_FUEL_COST;

				/* Also play the thrust sound. */
				if ( Mix_PlayChannel(-1, state->thrust, 0) == -1 )
				{
					/*fprintf(stderr, "Problem playing thrust sound.\n");*/
				}
			}
			break;

		/*** If the user pressed the LEFT arrow, increase the lander's horizontal
		     velocity and consume some fuel. ***/
		case SDLK_LEFT:
			if (state->fuel >= THRUST_FUEL_COST)
			{	
				state->lander->horVelocity += LEFT_THRUST_POWER;
				state->fuel -= THRUST_FUEL_COST;

				/* Also play the thrust sound. */
				if ( Mix_PlayChannel(-1, state->thrust, 0) == -1 )
				{
					/*fprintf(stderr, "Problem playing thrust sound.\n");*/
				}
			}
			break;

		/*** For any other keystroke, do nothing. ***/
		default:
			break;
	}
}

/**
@fn applyTick
@brief Applies one tick of game time to the game state. 
Independent of user action.
@param state Pointer to the current GameState struct.
*/
void applyTick (GameState *state)
{
	/*** If the state's start time hasn't been initialized yet, do so. ***/
	if (state->timeStart == -1)
	{
		state->timeStart = SDL_GetTicks();
	}

	/*** Decrease the lander's vertical velocity by GRAVITY constant. ***/
	state->lander->vertVelocity -= GRAVITY;

	/*** Change the lander's x and y positions by the corresponding 
	     velocities. ***/
	state->lander->realX += state->lander->horVelocity;
		/* If the lander's position surpasses any of the boundaries, 
		   wrap it around that boundary. */
	while ( state->lander->realX < 0.0 )
	{
		state->lander->realX += (float)(state->levelWidth);
	}
	while ( state->lander->realX >= (float)(state->levelWidth) )
	{
		state->lander->realX -= (float)(state->levelWidth);
	}

	state->lander->realY += state->lander->vertVelocity;

	state->lander->X = (int)(state->lander->realX);
	state->lander->Y = (int)(state->lander->realY);

		/* Prevent the lander from going beyond the bounds of the level
		   (this causes out-of-bounds errors). */
	/*if (state->lander->realX < 0)
	{
		state->lander->realX = 0;
		state->lander->X = 0;
	}
	else if (state->lander->realX >= state->levelWidth)
	{
		state->lander->realX = state->levelWidth - state->lander->length;
		state->lander->X = state->levelWidth - state->lander->length;
	}
	if (state->lander->realY < 1)
	{
		state->lander->realY = 10;
		state->lander->Y = 10;
	}*/


	/*** Scroll the focus as necessary. ***/
	scrollFocusPoint(state);

	/*** Find the current time and update the timer. ***/
	state->timeElapsed = SDL_GetTicks() - (state->timeStart);
}

/**
@fn scrollFocusPoint
@brief Scrolls the screen if the lander gets too close to one edge.
@param state Pointer to the current GameState struct.
*/
void scrollFocusPoint (GameState *state)
{
	/* Focus point's left edge is at X = focusPointX. Focus point's right edge
	   is at X = (focusPointX + WINDOW_WIDTH) % levelWidth */
	/*** Check to see if lander is close to the left or right edges of the 
	   window. ***/
		/* Right side: */
	if ( (state->lander->X + (WINDOW_WIDTH / HORIZONTAL_SCROLL_THRESHOLD))
		 /*% state->levelWidth*/ >=
		 (state->focusPointX + WINDOW_WIDTH) % (state->levelWidth + 1))
	{
		/* Shift the focus point right by the lander's x velocity. 
		   Wrap the focus point around the width of the level if necessary. */
		if (state->lander->horVelocity > 0)
		{
			state->realFocusPointX += state->lander->horVelocity;
			while ( state->realFocusPointX >= (float)(state->levelWidth) )
			{
				state->realFocusPointX -= (float)(state->levelWidth);
			}
			state->focusPointX = (int)(state->realFocusPointX);

			/* DO NOT scroll past the edge of the level. */
			/*if (state->focusPointX + WINDOW_WIDTH > state->levelWidth)
			{
				state->focusPointX = state->levelWidth - WINDOW_WIDTH;
			}*/
		}
	}
		/* Left side: */
	if ( state->lander->X - (WINDOW_WIDTH / HORIZONTAL_SCROLL_THRESHOLD)
		 <= state->focusPointX )
	{
		/* Shift the focus point left by the lander's x velocity. */
		if ( state->lander->horVelocity < 0 )
		{
			state->realFocusPointX += state->lander->horVelocity;
			while ( state->realFocusPointX < 0.0 )
			{
				state->realFocusPointX += (float)(state->levelWidth);
			}
			state->focusPointX = (int)(state->realFocusPointX);

			/* DO NOT scroll past the edge of the level. */
			/*if (state->focusPointX < 0 && WINDOW_WIDTH <= state->levelWidth)
			{
				state->focusPointX = 0;
			}*/
		}
	}
		

	/*** Check for the lander being too close to the bottom or top edges 
	   as well. ***/
		/* Top side: */
	if ( state->lander->Y + (WINDOW_HEIGHT / TOP_SCROLL_THRESHOLD) >=
		 state->focusPointY)
	{
		/* Shift the focus point up by the lander's y velocity. */
		if (state->lander->vertVelocity > 0)
		{
			state->realFocusPointY += state->lander->vertVelocity;
			state->focusPointY = (int)(state->realFocusPointY);
			/* The lander may scroll as far up as it wants,
			   no bounding check necessary. */
		}
	}
		/* Bottom side: */
	else if ( state->lander->Y - (WINDOW_HEIGHT / BOTTOM_SCROLL_THRESHOLD) <=
		 state->focusPointY - WINDOW_HEIGHT)
	{
		/* Shift the focus point down by the lander's y velocity. */
		if (state->lander->vertVelocity < 0)
		{
			state->realFocusPointY += state->lander->vertVelocity;
			state->focusPointY = (int)(state->realFocusPointY);

			/* DO NOT scroll past the bottom of the level. */
			/*if (state->focusPointY < WINDOW_HEIGHT)
			{
				state->focusPointY = WINDOW_HEIGHT;
			}*/
		}
	}
}

/**
@fn collisionDetected
@brief Detects if a collision has occurred between the lander and terrain.
@param state The GameState struct representing the current state of the game.
@param landingType An int buffer, will be overwritten with 1 if a proper landing
occurred or 2 if a crash occurred. 0 if no collision detected.
@return True if a collision was detected, false otherwise.
*/
bool collisionDetected (GameState state, int *landingType)
{
	/* A collision happens when one part of the bottom of the lander is at or 
	   below terrain level. Make sure that the entirety of the lander's bottom
	   has hit FLAT land. If not, the lander has crashed. Then, check the total 
	   velocity of the lander. If it's at or below the LANDING_THRESHOLD, 
	   a successful landing took place. Otherwise, a crash occurred. */

	int leftX, middleX, rightX;

	leftX = (int)(state.lander->X);
	middleX = (leftX + ( (int)(state.lander->length) / 2 )) % state.levelWidth;
	rightX = ( leftX + (int)(state.lander->length) ) % state.levelWidth;


	/*** Check if the left end of the lander's bottom is at or below the
	     terrain at its x position. ***/
	if (leftX >= 0 && leftX < state.levelWidth)
	{
		if (state.lander->Y <= state.terrain->heightMap[leftX])
		{
			/* Check for a proper landing. The speed of the lander must be at or
			   under the LANDING_THRESHOLD and the terrain must be flat. */
			if ( isLandingSpeed(state) && isFlatLand(state, leftX, middleX, rightX) )
			{
				*landingType = 1;

				/* Also play the landing sound. */
				if ( Mix_PlayChannel(-1, state.ding, 0) == -1 )
				{
					/*fprintf(stderr, "Problem playing ding sound.\n");*/
				}
			}
			/* Otherwise, a crash occurred. */
			else
			{
				*landingType = 2;

				/* Also play the boom sound. */
				if ( Mix_PlayChannel(-1, state.boom, 0) == -1 )
				{
					/*fprintf(stderr, "Problem playing boom sound.\n");*/
				}
			}

			return true;
		}
	}

	/*** Check if the middle of the lander's bottom is at or below the
	     terrain at its x position. ***/
	if (middleX >= 0 && middleX < state.levelWidth)
	{
		if (state.lander->Y <= state.terrain->heightMap[middleX])
		{
			/* Check for a proper landing. The speed of the lander must be at or
			   under the LANDING_THRESHOLD and the terrain must be flat. */
			if ( isLandingSpeed(state) && isFlatLand(state, leftX, middleX, rightX) )
			{
				*landingType = 1;

				/* Also play the landing sound. */
				if ( Mix_PlayChannel(-1, state.ding, 0) == -1 )
				{
					fprintf(stderr, "Problem playing ding sound.\n");
				}
			}
			/* Otherwise, a crash occurred. */
			else
			{
				*landingType = 2;

				/* Also play the boom sound. */
				if ( Mix_PlayChannel(-1, state.boom, 0) == -1 )
				{
					fprintf(stderr, "Problem playing boom sound.\n");
				}
			}

			return true;
		}
	}

	/*** Check if the right end of the lander's bottom is at or below the
	     terrain at its x position. ***/
	if (rightX >= 0 && rightX < state.levelWidth)
	{
		if (state.lander->Y <= state.terrain->heightMap[rightX])
		{
			/* Check for a proper landing. The speed of the lander must be at or
			   under the LANDING_THRESHOLD and the terrain must be flat. */
			if ( isLandingSpeed(state) && isFlatLand(state, leftX, middleX, rightX) )
			{
				*landingType = 1;
			}
			/* Otherwise, a crash occurred. */
			else
			{
				*landingType = 2;
			}

			return true;
		}
	}

	return false;
}

/**
@fn isLandingSpeed
@brief Reports whether or not the lander is going slow enough for a proper 
landing.
@param state The current GameState struct.
@return True if the lander's speed is at or under LANDING_THRESHOLD.
False otherwise.
*/
bool isLandingSpeed (GameState state)
{
	if ( (int)(getVelocity(state)) <= LANDING_THRESHOLD )
	{
		return true;
	}

	return false;
}

/**
@fn isFlatLand
@brief Approximates if the terrain between x positions x1, x2, and x3 is flat.
@details This function checks if the height of the terrain at x positions
x1, x2, and x3 is the same. If so, returns true.
@param state The current GameState struct.
@param x1 An x coordinate to measure terrain at.
@param x2 An x coordinate to measure terrain at.
@param x3 An x coordinate to measure terrain at.
@return True if the terrain is at the same height at each of the given 
positions. False otherwise.
*/
bool isFlatLand (GameState state, int x1, int x2, int x3)
{
	if (state.terrain->heightMap[x1] == state.terrain->heightMap[x2] &&
		state.terrain->heightMap[x1] == state.terrain->heightMap[x3])
	{
		return true;
	}

	return false;
}

/**
@fn applyCollision
@brief Alters the game state following a collision given the type of collision.
@details This function handles a collision. If it was a proper landing, then
the player's score is incremented. If it was a crash, then some of the player's
fuel is lost. The function then displays a message indicating the type of 
collision, how much score it yielded, and how much fuel was lost. The function 
waits for the user to respond with a mouse click or keystroke before releasing.
@param state Pointer to the current GameState struct.
@param landingType The type of collision that has occurred.
*/
void applyCollision(GameState *state, int landingType)
{
	/*** If landingType is 1 (good landing), increment the player's score. 
	     Then, display the type of landing for the user and how much score was
	     gained. Wait for the user to respond, and return. ***/
	if (landingType == 1)
	{
		Uint16 modifier = 0;

		/* Find the Flat representing the region just landed on. */
		Flat* currentFlat = state->terrain->firstFlat;

		while (currentFlat != NULL && 
			   currentFlat->X + currentFlat->length < state->lander->X)
		{
			currentFlat = currentFlat->next;
		}

		if (currentFlat != NULL)
		{
			modifier = currentFlat->scoreModifier;
		}

		/* Increment the player's score with SCORE_FOR_LANDING times the
		   modifier. */
		state->score += (SCORE_FOR_LANDING * modifier);

		showCollisionMessage(state, SCORE_FOR_LANDING * modifier);

		return;
	}

	/*** If landingType is 2 (crash), decrement the lander's fuel.
	     Then, display the type of landing for the user and how much fuel was
	     gained. Wait for the user to respond, and return. ***/
	if (landingType == 2)
	{
		state->fuel -= CRASH_FUEL_COST;

		showCollisionMessage(state, (CRASH_FUEL_COST * -1));

		return;
	}

	/*** If landingType is neither 1 nor 2, then an error has occurred. ***/
}

/**
@fn showCollisionMessage
@brief Displays a message for the user about what type of collision happened.
@details This function freezes the game and displays a message saying what kind
of collision occurred (landing or crash) and how much score was gained or fuel
was last. It then waits for the user to respond with a mouse click or keystroke
before releasing.
@param state Pointer to the current GameState struct.
@param score The score gained (if positive or 0) OR the fuel lost (if negative).
*/
void showCollisionMessage (GameState *state, int score)
{
	char text[64];
	int writeX, writeY;

	/*** If it's game over, display GAME OVER above the message and display
		 the final score on the line below that. ***/
	if (gameOver(*state))
	{
		writeX = (WINDOW_WIDTH / 2) - (8 * 5);
		writeY = (WINDOW_HEIGHT / 2) - (2 * TEXT_Y_DELTA);

		sprintf(text, "GAME  OVER");
		stringRGBA(state->renderer, writeX, writeY, text, 255, 255, 255, 255);

		writeX = (WINDOW_WIDTH / 2) - (8 * 8);
		writeY += TEXT_Y_DELTA;
		sprintf(text, "Final Score: %04d", state->score);
		stringRGBA(state->renderer, writeX, writeY, text, 255, 255, 255, 255);
	}

	/*** If score >= 0, display a successful landing message. ***/
	if (score >= 0)
	{
		/* Text is vertically and horizontally centered. */
		writeX = (WINDOW_WIDTH / 2) - (8 * 12);
		writeY = (WINDOW_HEIGHT / 2);
		sprintf(text, "You landed successfully!");
		stringRGBA(state->renderer, writeX, writeY, text, 255, 255, 255, 255);

		writeX = (WINDOW_WIDTH / 2) - (8 * 8);
		writeY += TEXT_Y_DELTA;
		sprintf(text, "Score gained: %03d", score);
		stringRGBA(state->renderer, writeX, writeY, text, 255, 255, 255, 255);
	}

	/*** Otherwise, display a crash message. ***/
	else
	{
		/* Text is vertically and horizontally centered. */
		writeX = (WINDOW_WIDTH / 2) - (8 * 6);
		writeY = (WINDOW_HEIGHT / 2);
		sprintf(text, "You crashed!");
		stringRGBA(state->renderer, writeX, writeY, text, 255, 255, 255, 255);

		writeX = (WINDOW_WIDTH / 2) - (8 * 7);
		writeY += TEXT_Y_DELTA;
		sprintf(text, "Fuel lost: %03d", (score * -1));
		stringRGBA(state->renderer, writeX, writeY, text, 255, 255, 255, 255);
	}

	SDL_RenderPresent(state->renderer);

	Uint32 timeWaiting = SDL_GetTicks();

	/*** Wait for a user event before releasing. ***/
	waitForResponse(state);

	/*** Update timeStart so that the time spent waiting is not counted as
	     game time. ***/
	timeWaiting = SDL_GetTicks() - timeWaiting;
	state->timeStart += timeWaiting;
}

/**
@fn waitForResponse
@brief Waits for the user to respond with a mouse button press or a key press 
that is NOT one of the arrow keys.
@param state Pointer to the current GameState struct.
*/
void waitForResponse (GameState *state)
{
	/* The user event must be a key press that is NOT one of the arrow
	   keys or a mouse click. */

	/* eventBuffer will hold the user event. */
	SDL_Event eventBuffer;

	/* Initialize eventBuffer with the first event the user provides. */
	if (!( SDL_WaitEvent(&eventBuffer) ))
	{
		fprintf(stderr, "Error occurred while waiting for response in showCollisionMessage.\n");

		cleanAndExit(state, EXIT_SDLWAIT_FAIL);
	}

	/* Loop until the user provides the right type of event. */
	bool waitForEvent = true;
	while (waitForEvent)
	{
		switch (eventBuffer.type)
		{
			/* Check for the user exiting the game. */
			case SDL_QUIT:
				/* If so, clean up and exit. */
				cleanAndExit(state, EXIT_SUCCESS);
				break;

			/* Check for a key press. */
			case SDL_KEYDOWN:
				/* If the key press is not one of the arrow keys, release. */
				switch (eventBuffer.key.keysym.sym)
				{
					case SDLK_UP:
					case SDLK_LEFT:
					case SDLK_RIGHT:
						break;
					default:
						waitForEvent = false;
						break;
				}
				break;

			/* Check for a mouse press. */
			case SDL_MOUSEBUTTONDOWN:
				waitForEvent = false;
				break;

			default:
				break;
		}

		/* Otherwise, wait for another event. */
		if (!( SDL_WaitEvent(&eventBuffer) ))
		{
			fprintf(stderr, "Error occurred while waiting for response in showCollisionMessage.\n");

			cleanAndExit(state, EXIT_SDLWAIT_FAIL);
		}
	}
}

/**
@fn gameOver
@brief Reports whether or not the game is over now.
@param state The GameState struct representing the current state of the game.
@return True if the land has no more fuel. False otherwise.
*/
bool gameOver (GameState state)
{
	/*** Return true if there is no more fuel. ***/
		/* Since fuel is an unsigned int, it will overflow to large positive
		   value when it falls below 0, so check for that. */
	if (state.fuel <= 0 || state.fuel > FUEL_START)
	{
		return true;
	}

	/*** Otherwise, return false. ***/
	return false;
}

/**
@fn hardReset
@brief Refreshes the game as if it were just relaunched.
@details Ends the current game, displaying the user's score and time. Then,
waits for the user to respond. Once the user responds, resets the GameState as
if the game were just launched.
@param state Pointer to the current GameState struct.
*/
void hardReset (GameState *state)
{
	/*** Draw a message showing the user's score and time. ***/

	/*** Reset the game state to initial state. ***/
		/* Reset lander's position. */
	state->lander->realX = LANDER_X_START;
	state->lander->realY = LANDER_Y_START;
	state->lander->X = (int)(state->lander->realX);
	state->lander->Y = (int)(state->lander->realY);
	state->lander->length = LANDER_LENGTH;
	state->lander->height = LANDER_HEIGHT;

		/* Reset the lander's horizontal and vertical velocities. */
	state->lander->horVelocity = LANDER_VX_START;
	state->lander->vertVelocity = LANDER_VY_START;

		/* Reset the focus point. */
	state->realFocusPointX = 0;
	state->realFocusPointY = WINDOW_HEIGHT;
	state->focusPointX = (int)(state->realFocusPointX);
	state->focusPointY = (int)(state->realFocusPointY);

		/* Reset score, time, and fuel. */
	state->score = 0;
	state->timeStart = -1;
	state->timeElapsed = 0;
	state->fuel = FUEL_START;

	/*** Wait for a user event, then release. ***/
}

/**
@fn softReset
@brief Respawns the lander and continues the game.
@details Moves the lander back to its original spawn position, then waits for
the user to respond. Once the user responds, releases the game and allows the
user to continue playing.
@param state Pointer to the current GameState struct.
*/
void softReset (GameState *state)
{
	/*** Reset the lander's position and velocities. ***/
	state->lander->realX = LANDER_X_START;
	state->lander->realY = LANDER_Y_START;
	state->lander->X = (int)(state->lander->realX);
	state->lander->Y = (int)(state->lander->realY);
	state->lander->length = LANDER_LENGTH;
	state->lander->height = LANDER_HEIGHT;
	state->lander->horVelocity = LANDER_VX_START;
	state->lander->vertVelocity = LANDER_VY_START;

		/* Reset the focus point. */
	state->realFocusPointX = 0;
	state->realFocusPointY = WINDOW_HEIGHT;
	state->focusPointX = (int)(state->realFocusPointX);
	state->focusPointY = (int)(state->realFocusPointY);

	/*** Wait for a user event, then release. ***/
}

/**
@fn getVelocity 
@brief Calculates the magnitude of the lander's velocity.
@param state The GameState struct representing the current state of the game.
@return The magnitude of the lander's velocity as a positive double.
*/
double getVelocity (GameState state)
{
	return sqrt( (double)
				((state.lander->vertVelocity * state.lander->vertVelocity)
			    + (state.lander->horVelocity * state.lander->horVelocity)));
}

/**
@fn getMinutes
@brief Returns the number of elapsed minutes in game time (mod 100).
@param state The current GameState struct.
@return The number of minutes elapsed while playing.
*/
int getMinutes (GameState state)
{
	/* Get the elapsed time (in ms) from the game state. */
	int t = state.timeElapsed;

	/* Convert to minutes. */
	t /= 60000;

	/* Mod by 100 (only two decimal places returned). */
	t = t % 100;

	return t;
}

/**
@fn getSeconds 
@brief Returns the number of elapsed seconds in game time (mod 60).
@param state The current GameState struct.
@return The number of seconds elapsed while playing.
*/
int getSeconds (GameState state)
{
	/* Get the elapsed time (in ms) from the game state. */
	int t = state.timeElapsed;

	/* Convert to seconds. */
	t /= 1000;

	/* Mod by 60. */
	t = t % 60;

	return t;
}

/**
@fn getAltitude 
@brief Returns the distance between the terrain and the lander.
@param state The current GameState struct.
@return The distance (in pixels) between the terrain and lander.
*/

int getAltitude (GameState state)
{
	int middleX = (int)(state.lander->X + (state.lander->length / 2)) 
				  % state.levelWidth;

	return state.lander->Y - state.terrain->heightMap[middleX];
}

/**
@fn min
@brief Returns the smaller of two ints.
@param first One of two ints to compare.
@param second One of two ints to compare.
@return The smaller of the two ints.
*/
int min (int first, int second)
{
	if (first <= second)
	{
		return first;
	}

	return second;
}

/**
@fn max
@brief Returns the larger of two ints.
@param first One of two ints to compare.
@param second One of two ints to compare.
@return The larger of the two ints.
*/
int max (int first, int second)
{
	if (first >= second)
	{
		return first;
	}

	return second;
}
