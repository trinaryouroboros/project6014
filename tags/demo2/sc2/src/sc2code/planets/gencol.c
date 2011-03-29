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

// JMS 2010: -Removed slave shield and the lone ur-quan ship from the system

#include "build.h"
#include "globdata.h"
#include "state.h"
#include "grpinfo.h"
#include "nameref.h" // JMS
#include "resinst.h" // JMS
#include "planets/genall.h"
#include "libs/mathlib.h" // JMS
#include "libs/log.h" // JMS
#include "lander.h" //JMS
#include "sounds.h" //JMS

void
GenerateColony (BYTE control)
{
	switch (control)
	{
		case INIT_NPCS:
		{
			HIPGROUP hGroup;

			/* JMS: No Ur-Quans in Vela anymore...
			 
			GLOBAL (BattleGroupRef) = GET_GAME_STATE_32 (COLONY_GRPOFFS0);
			if (GLOBAL (BattleGroupRef) == 0)
			{
				CloneShipFragment (URQUAN_SHIP,
						&GLOBAL (npc_built_ship_q), 0);
				GLOBAL (BattleGroupRef) = PutGroupInfo (GROUPS_ADD_NEW, 1);
				ReinitQueue (&GLOBAL (npc_built_ship_q));
				SET_GAME_STATE_32 (COLONY_GRPOFFS0, GLOBAL (BattleGroupRef));
			}*/

			GenerateRandomIP (INIT_NPCS);

			if (GLOBAL (BattleGroupRef)
					&& (hGroup = GetHeadLink (&GLOBAL (ip_group_q))))
			{
				IP_GROUP *GroupPtr;

				GroupPtr = LockIpGroup (&GLOBAL (ip_group_q), hGroup);
				GroupPtr->task = IN_ORBIT;
				GroupPtr->sys_loc = 0 + 1; /* orbitting colony */
				GroupPtr->dest_loc = 0 + 1; /* orbitting colony */
				GroupPtr->loc.x = 0;
				GroupPtr->loc.y = 0;
				GroupPtr->group_counter = 0;
				UnlockIpGroup (&GLOBAL (ip_group_q), hGroup);
			}
			break;
		}
		// JMS: The whole GENERATE_ENERGY case is new here. Its purpose is to create energy blip of the Precursor factory remnants.
		case GENERATE_ENERGY:
		{	
			DWORD rand_val, old_rand;
			
			if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
			{
				old_rand = TFB_SeedRandom (
						pSolarSysState->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN]);
				
				rand_val = TFB_Random ();
				pSolarSysState->SysInfo.PlanetInfo.CurPt.x = MAP_WIDTH * 5 / 9;
				pSolarSysState->SysInfo.PlanetInfo.CurPt.y = MAP_HEIGHT * 2 / 5;
				pSolarSysState->SysInfo.PlanetInfo.CurDensity = 0;
				pSolarSysState->SysInfo.PlanetInfo.CurType = 1;
				if (!(pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN] & (1L << 0))
					&& pSolarSysState->CurNode == (COUNT)~0)
				{
					pSolarSysState->CurNode = 1;
				}
				else
				{
					pSolarSysState->CurNode = 0;
					if (pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN] & (1L << 0))
					{
						SET_GAME_STATE (VELA_FACTORY_VISITED, 1);
					}
				}
				
				TFB_SeedRandom (old_rand);
				break;
			}
			pSolarSysState->CurNode = 0;
			break;
		}
		case GENERATE_PLANETS:
		{
			COUNT angle;
			PLANET_DESC *pMinPlanet;

			pMinPlanet = &pSolarSysState->PlanetDesc[0];
			FillOrbits (pSolarSysState, (BYTE)~0, pMinPlanet, FALSE);

			pMinPlanet->radius = EARTH_RADIUS * 115L / 100;
			angle = ARCTAN (pMinPlanet->location.x, pMinPlanet->location.y);
			pMinPlanet->location.x =
					COSINE (angle, pMinPlanet->radius);
			pMinPlanet->location.y =
					SINE (angle, pMinPlanet->radius);
			pMinPlanet->data_index = WATER_WORLD; // JMS: Vela colony is not shielded anymore.
			break;
		}
		case GENERATE_ORBITAL:
			if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
			{
				DoPlanetaryAnalysis (&pSolarSysState->SysInfo, pSolarSysState->pOrbitalDesc);
				
				// JMS: This displays the graphics of the Precursor factory remnants
				// and gives the landing team message upon finding it.
				LoadStdLanderFont (&pSolarSysState->SysInfo.PlanetInfo);
				pSolarSysState->PlanetSideFrame[1] =
					CaptureDrawable (LoadGraphic (UMGAH_BCS_MASK_PMAP_ANIM));
				pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
					CaptureStringTable (LoadStringTable (VELA_FACTORY_STRTAB));
				
				pSolarSysState->SysInfo.PlanetInfo.AtmoDensity =
						EARTH_ATMOSPHERE * 98 / 100;
				pSolarSysState->SysInfo.PlanetInfo.Weather = 0;
				pSolarSysState->SysInfo.PlanetInfo.Tectonics = 0;
				pSolarSysState->SysInfo.PlanetInfo.SurfaceTemperature = 28;

				LoadPlanet (NULL);
				break;
			}
		default:
			GenerateRandomIP (control);
			break;
	}
}
