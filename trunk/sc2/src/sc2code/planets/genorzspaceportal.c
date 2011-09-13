// JMS 2009:
// Generates rift to ORZ space

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

// JMS 2009: Totally new file: Orz space portal

#include "build.h"
#include "genall.h"
#include "grpinfo.h"
#include "races.h"
#include "state.h"
#include "../encount.h"

#include "hyper.h"
// for SOL_X/SOL_Y

void
GenerateOrzSpacePortal (BYTE control)
{
	switch (control)
	{
		case INIT_NPCS:
			 
			CloneShipFragment (ORZ_SHIP, &GLOBAL (npc_built_ship_q), 0);
			GLOBAL (BattleGroupRef) = PutGroupInfo (GROUPS_ADD_NEW, 1);
			ReinitQueue (&GLOBAL (npc_built_ship_q));

			GenerateRandomIP (INIT_NPCS);
			break;
		case GENERATE_MOONS:
			//GenerateRandomIP (GENERATE_MOONS);
			break;
		case GENERATE_PLANETS:
			pSolarSysState->SunDesc[0].NumPlanets = 1;
			GenerateRandomIP (GENERATE_PLANETS);
			FillOrbits (pSolarSysState, pSolarSysState->SunDesc[0].NumPlanets, &pSolarSysState->PlanetDesc[0], FALSE);
			pSolarSysState->SunDesc[0].NumPlanets = 0;
			break;
		case GENERATE_ORBITAL:
			//GenerateRandomIP (GENERATE_ORBITAL);
			/* The PORTAL */
			if (pSolarSysState->pOrbitalDesc == &pSolarSysState->SunDesc[0])
			{
				if(!(GET_GAME_STATE(TEMPORAL_WRAPPER_ON_SHIP)))
				{  // Arilou Communication if one doesn't have proper protective device along
				   pSolarSysState->MenuState.Initialized += 2;
				   InitCommunication (ARILOU_CONVERSATION);
				   pSolarSysState->MenuState.Initialized -= 2;
				}
				else   
				{	// Thru the portal to ORZ space
					GLOBAL_SIS (log_x) = UNIVERSE_TO_LOGX (QUASI_SPACE_X);
					GLOBAL_SIS (log_y) = UNIVERSE_TO_LOGY (QUASI_SPACE_Y);
					SET_GAME_STATE (ORZ_SPACE_SIDE, 3);
					GLOBAL (CurrentActivity) |= END_INTERPLANETARY;
				}
			}
				break;
		default:
			GenerateRandomIP (control);
			break;
	}
}
