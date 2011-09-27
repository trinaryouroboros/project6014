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

// JMS 2010: -Syreen home planet is now restricted from landing
//			 -Syreen starbase fully operational

#include "build.h"
#include "encount.h"
#include "globdata.h"
#include "nameref.h"
#include "resinst.h"
#include "setup.h"
#include "state.h"
#include "planets/genall.h"


static void
GenerateTrap (BYTE control)
{
	switch (control)
	{
		case GENERATE_PLANETS:
		{
			COUNT angle;

			GenerateRandomIP (GENERATE_PLANETS);
			pSolarSysState->PlanetDesc[0].data_index = TELLURIC_WORLD;
			pSolarSysState->PlanetDesc[0].NumPlanets = 1;
			pSolarSysState->PlanetDesc[0].radius = EARTH_RADIUS * 203L / 100;
			angle = ARCTAN (
					pSolarSysState->PlanetDesc[0].location.x,
					pSolarSysState->PlanetDesc[0].location.y
					);
			pSolarSysState->PlanetDesc[0].location.x =
					COSINE (angle, pSolarSysState->PlanetDesc[0].radius);
			pSolarSysState->PlanetDesc[0].location.y =
					SINE (angle, pSolarSysState->PlanetDesc[0].radius);
			break;
		}
		case GENERATE_ORBITAL:
			GenerateRandomIP (GENERATE_ORBITAL);
			if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
			{
				pSolarSysState->SysInfo.PlanetInfo.AtmoDensity = EARTH_ATMOSPHERE * 2;
				pSolarSysState->SysInfo.PlanetInfo.SurfaceTemperature = 35;
				pSolarSysState->SysInfo.PlanetInfo.Weather = 3;
				pSolarSysState->SysInfo.PlanetInfo.Tectonics = 1;
			}
			break;
		default:
			GenerateRandomIP (control);
			break;
	}
}

void
GenerateSyreen (BYTE control)
{
	if (CurStarDescPtr && CurStarDescPtr->Index == MYCON_TRAP_DEFINED)
	{
		GenerateTrap (control);
		return;
	}

	switch (control)
	{
		case GENERATE_MOONS:
			GenerateRandomIP (GENERATE_MOONS);
			if (pSolarSysState->pBaseDesc == &pSolarSysState->PlanetDesc[0])
			{
				pSolarSysState->MoonDesc[0].data_index = HIERARCHY_STARBASE;
				pSolarSysState->MoonDesc[0].radius = MIN_MOON_RADIUS;
				pSolarSysState->MoonDesc[0].location.x =
						COSINE (QUADRANT, pSolarSysState->MoonDesc[0].radius);
				pSolarSysState->MoonDesc[0].location.y =
						SINE (QUADRANT, pSolarSysState->MoonDesc[0].radius);
			}
			break;
		case GENERATE_PLANETS:
		{
			GenerateRandomIP (GENERATE_PLANETS);
			pSolarSysState->PlanetDesc[0].data_index = WATER_WORLD;
			pSolarSysState->PlanetDesc[0].flags = PLANET_RESTRICTED; // JMS: Can't land on syreen turf
			pSolarSysState->PlanetDesc[0].NumPlanets = 1;
			break;
		}
		case GENERATE_ORBITAL:
			if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
			{
				pSolarSysState->MenuState.Initialized += 2;
				InitCommunication (SYREENHOME_CONVERSATION);
				pSolarSysState->MenuState.Initialized -= 2;
				break;
			}
				/* Starbase */
			else if (pSolarSysState->pOrbitalDesc->pPrevDesc == &pSolarSysState->PlanetDesc[0]
					&& pSolarSysState->pOrbitalDesc == &pSolarSysState->MoonDesc[0])
			{
				// JMS: The Syreen have a fleet in starbase
				CloneShipFragment (SYREEN_SHIP, &GLOBAL (npc_built_ship_q), INFINITE_FLEET);
				
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

