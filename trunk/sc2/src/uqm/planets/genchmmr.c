//Copyright Paul Reiche, Fred Ford. 1992-2002

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

// JMS 2010: -Added Chmmr to starbase
//			 -Chmmr home planet is now restricted from landing

#include "genall.h"
#include "lander.h"
#include "../build.h"
#include "../encount.h"
#include "../setup.h"
#include "../state.h"
#include "../sounds.h"
#include "libs/mathlib.h"


void
GenerateChmmr (BYTE control)
{
	switch (control)
	{
		case GENERATE_MOONS:
			GenerateRandomIP (GENERATE_MOONS);
			if (pSolarSysState->pBaseDesc == &pSolarSysState->PlanetDesc[1])
			{
				COUNT angle;
				DWORD rand_val;

				pSolarSysState->MoonDesc[0].data_index = HIERARCHY_STARBASE;
				pSolarSysState->MoonDesc[0].radius = MIN_MOON_RADIUS;
				rand_val = TFB_Random ();
				angle = NORMALIZE_ANGLE (LOWORD (rand_val));
				pSolarSysState->MoonDesc[0].location.x =
						COSINE (angle, pSolarSysState->MoonDesc[0].radius);
				pSolarSysState->MoonDesc[0].location.y =
						SINE (angle, pSolarSysState->MoonDesc[0].radius);
			}
			break;
		case GENERATE_PLANETS:
			GenerateRandomIP (GENERATE_PLANETS);
			pSolarSysState->PlanetDesc[1].data_index = SAPPHIRE_WORLD;
			pSolarSysState->PlanetDesc[1].flags = PLANET_RESTRICTED; // JMS: Can't land on Chmmr homeworld
			pSolarSysState->PlanetDesc[1].NumPlanets = 1;
			break;
		case GENERATE_ORBITAL:
			if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[1])
			{
				if (GET_GAME_STATE (CHMMR_UNLEASHED))
				{
					pSolarSysState->MenuState.Initialized += 2;
					SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 7);
					InitCommunication (CHMMR_CONVERSATION);
					pSolarSysState->MenuState.Initialized -= 2;

					if (GET_GAME_STATE (CHMMR_BOMB_STATE) == 2)
					{
						GLOBAL (CurrentActivity) |= END_INTERPLANETARY;
					}
					break;
				}
				
			}
				/* Starbase */
			else if (pSolarSysState->pOrbitalDesc->pPrevDesc == &pSolarSysState->PlanetDesc[1]
					&& pSolarSysState->pOrbitalDesc == &pSolarSysState->MoonDesc[0])
			{
				// JMS: The Chmmr reside in starbase
				CloneShipFragment (CHMMR_SHIP, &GLOBAL (npc_built_ship_q), INFINITE_FLEET);
				
				// JMS: This code summons starbase subroutine. (The global_flags_and_data == ~0
				// is checked in Starcon2Main function in starcon.c)
				GLOBAL (CurrentActivity) |= START_ENCOUNTER;
				SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, (BYTE)~0);
				
				// JMS: Necessary to empty the NPC ship queue after visit.
				// Otherwise next encounter with a ship crashes the game.
				if (!(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)))
				{
					GLOBAL (CurrentActivity) &= ~START_INTERPLANETARY;
					ReinitQueue (&GLOBAL (npc_built_ship_q));
					GetGroupInfo (GROUPS_RANDOM, GROUP_LOAD_IP);
				}
				
				break;
			}
		default:
			GenerateRandomIP (control);
			break;
	}
}
