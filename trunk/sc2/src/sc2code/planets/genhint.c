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

#include "encount.h"
#include "globdata.h"
#include "nameref.h"
#include "resinst.h"
#include "planets/genall.h"
#include "libs/mathlib.h"

// JMS 2010: - This is a completely new file.

void
GenerateHint (BYTE control)
{
	COUNT i;
	DWORD rand_val;
	
	BYTE which_hintworld;
	STAR_DESC *SDPtr;
	
	which_hintworld = 0;
	SDPtr = &star_array[0];
	
	while (SDPtr != CurStarDescPtr)
	{
		if (SDPtr->Index == HINT_DEFINED)
			++which_hintworld;
		++SDPtr;
	}
	
	if (!(GET_GAME_STATE(HINT_WORLD_LOCATION) == which_hintworld))
	{
		if (which_hintworld == 1)
		{
			switch (control)
			{
				case GENERATE_MOONS:
				{
					GenerateRandomIP (GENERATE_MOONS);
					if (pSolarSysState->pBaseDesc == &pSolarSysState->PlanetDesc[1])
					{
						COUNT angle;
			
						pSolarSysState->MoonDesc[0].data_index = PELLUCID_WORLD;
						pSolarSysState->MoonDesc[0].radius = MIN_MOON_RADIUS * 2;
						rand_val = TFB_Random ();
						angle = NORMALIZE_ANGLE (LOWORD (rand_val));
						pSolarSysState->MoonDesc[0].location.x = COSINE (angle, pSolarSysState->MoonDesc[0].radius);
						pSolarSysState->MoonDesc[0].location.y = SINE (angle, pSolarSysState->MoonDesc[0].radius);
					}
					break;
				}
				case GENERATE_PLANETS:
				{
					COUNT angle;
					GenerateRandomIP (GENERATE_PLANETS);
			
					if(which_hintworld == 1)
					{
						pSolarSysState->SunDesc[0].NumPlanets = 2;
				
						pSolarSysState->PlanetDesc[0].data_index = METAL_WORLD;
						pSolarSysState->PlanetDesc[0].radius = EARTH_RADIUS * 64L / 100;
						angle = ARCTAN (
										pSolarSysState->PlanetDesc[0].location.x,
										pSolarSysState->PlanetDesc[0].location.y);
						pSolarSysState->PlanetDesc[0].location.x = COSINE (angle, pSolarSysState->PlanetDesc[0].radius);
						pSolarSysState->PlanetDesc[0].location.y = SINE (angle, pSolarSysState->PlanetDesc[0].radius);
				
						pSolarSysState->PlanetDesc[1].data_index = PRIMORDIAL_WORLD;
						pSolarSysState->PlanetDesc[1].radius = EARTH_RADIUS * 352L / 100;
						pSolarSysState->PlanetDesc[1].NumPlanets = 1;
						angle = ARCTAN (
										pSolarSysState->PlanetDesc[1].location.x,
										pSolarSysState->PlanetDesc[1].location.y);
						pSolarSysState->PlanetDesc[1].location.x = COSINE (angle, pSolarSysState->PlanetDesc[1].radius);
						pSolarSysState->PlanetDesc[1].location.y = SINE (angle, pSolarSysState->PlanetDesc[1].radius);
					}
				break;
				}
				case GENERATE_ORBITAL:
				{
					rand_val = DoPlanetaryAnalysis (&pSolarSysState->SysInfo, pSolarSysState->pOrbitalDesc);
			
					pSolarSysState->SysInfo.PlanetInfo.ScanSeed[BIOLOGICAL_SCAN] = rand_val;
					i = (COUNT)~0;
					rand_val = GenerateLifeForms (&pSolarSysState->SysInfo, &i);
			
					pSolarSysState->SysInfo.PlanetInfo.ScanSeed[MINERAL_SCAN] = rand_val;
					i = (COUNT)~0;
					GenerateMineralDeposits (&pSolarSysState->SysInfo, &i);
			
					pSolarSysState->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN] = rand_val;
			
					if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
						pSolarSysState->SysInfo.PlanetInfo.PlanetToSunDist = EARTH_RADIUS * 64L / 100;
			
					if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[1])
					{
						pSolarSysState->SysInfo.PlanetInfo.PlanetToSunDist = EARTH_RADIUS * 352L / 100;
						pSolarSysState->SysInfo.PlanetInfo.AtmoDensity = 160;
						pSolarSysState->SysInfo.PlanetInfo.Weather = 2;
						pSolarSysState->SysInfo.PlanetInfo.PlanetDensity = 104;
						pSolarSysState->SysInfo.PlanetInfo.PlanetRadius = 120;
						pSolarSysState->SysInfo.PlanetInfo.Tectonics = 1;
						pSolarSysState->SysInfo.PlanetInfo.RotationPeriod = 288;
						pSolarSysState->SysInfo.PlanetInfo.SurfaceTemperature = -47;
				
						LoadStdLanderFont (&pSolarSysState->SysInfo.PlanetInfo);
						pSolarSysState->PlanetSideFrame[1] = CaptureDrawable (LoadGraphic (CIRCLES_A_MASK_PMAP_ANIM));
						pSolarSysState->PlanetSideFrame[2] = CaptureDrawable (LoadGraphic (CIRCLES_B_MASK_PMAP_ANIM));
						pSolarSysState->SysInfo.PlanetInfo.DiscoveryString = CaptureStringTable (LoadStringTable (LEFTHURRY_STRTAB));
						pSolarSysState->SysInfo.PlanetInfo.Weather = 1;
						pSolarSysState->SysInfo.PlanetInfo.Tectonics = 1;
					}
			
					LoadPlanet (NULL);
					break;
				}
				default:
					GenerateRandomIP (control);
					break;
			}
		}
		else
		{
			GenerateRandomIP (control);
		}
	}
	else 
	{
		switch (control)
		{
			case GENERATE_LIFE:
			{
				if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[1])
					pSolarSysState->CurNode = 0;
				break;
			}
			case GENERATE_ENERGY:
			{
				DWORD rand_val, old_rand;

				if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[1])
				{
					COUNT which_node;

					old_rand = TFB_SeedRandom (pSolarSysState->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN]);

					which_node = i = 0;
				
					do
					{
						rand_val = TFB_Random ();
						pSolarSysState->SysInfo.PlanetInfo.CurPt.x = (LOBYTE (LOWORD (rand_val)) % (MAP_WIDTH - (8 << 1))) + 8;
						pSolarSysState->SysInfo.PlanetInfo.CurPt.y = (HIBYTE (LOWORD (rand_val)) % (MAP_HEIGHT - (8 << 1))) + 8;
						pSolarSysState->SysInfo.PlanetInfo.CurType = 1;
						pSolarSysState->SysInfo.PlanetInfo.CurDensity = 0;
						if (which_node >= pSolarSysState->CurNode
								&& !(pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN] & (1L << i)))
							break;
						++which_node;
					} while (++i < 24);
				
					pSolarSysState->CurNode = which_node;

					TFB_SeedRandom (old_rand);
					break;
				}
			
				pSolarSysState->CurNode = 0;
				break;
			}
			case GENERATE_MOONS:
			{
				GenerateRandomIP (GENERATE_MOONS);
				if (pSolarSysState->pBaseDesc == &pSolarSysState->PlanetDesc[1] && which_hintworld == 1)
				{
					COUNT angle;
					
					pSolarSysState->MoonDesc[0].data_index = PELLUCID_WORLD;
					pSolarSysState->MoonDesc[0].radius = MIN_MOON_RADIUS * 2;
					rand_val = TFB_Random ();
					angle = NORMALIZE_ANGLE (LOWORD (rand_val));
					pSolarSysState->MoonDesc[0].location.x = COSINE (angle, pSolarSysState->MoonDesc[0].radius);
					pSolarSysState->MoonDesc[0].location.y = SINE (angle, pSolarSysState->MoonDesc[0].radius);
				}
				break;
			}
			case GENERATE_PLANETS:
			{
				COUNT angle;
				GenerateRandomIP (GENERATE_PLANETS);
				
				if (which_hintworld == 1)
				{
					pSolarSysState->SunDesc[0].NumPlanets = 2;
				
					pSolarSysState->PlanetDesc[0].data_index = METAL_WORLD;
					pSolarSysState->PlanetDesc[0].radius = EARTH_RADIUS * 64L / 100;
					angle = ARCTAN (
									pSolarSysState->PlanetDesc[0].location.x,
									pSolarSysState->PlanetDesc[0].location.y);
					pSolarSysState->PlanetDesc[0].location.x = COSINE (angle, pSolarSysState->PlanetDesc[0].radius);
					pSolarSysState->PlanetDesc[0].location.y = SINE (angle, pSolarSysState->PlanetDesc[0].radius);
				
					pSolarSysState->PlanetDesc[1].data_index = PRIMORDIAL_WORLD;
					pSolarSysState->PlanetDesc[1].radius = EARTH_RADIUS * 352L / 100;
					pSolarSysState->PlanetDesc[1].NumPlanets = 1;
					angle = ARCTAN (
									pSolarSysState->PlanetDesc[1].location.x,
									pSolarSysState->PlanetDesc[1].location.y);
					pSolarSysState->PlanetDesc[1].location.x = COSINE (angle, pSolarSysState->PlanetDesc[1].radius);
					pSolarSysState->PlanetDesc[1].location.y = SINE (angle, pSolarSysState->PlanetDesc[1].radius);
				}
				
				break;
			}
			case GENERATE_ORBITAL:
			{
				rand_val = DoPlanetaryAnalysis (&pSolarSysState->SysInfo, pSolarSysState->pOrbitalDesc);

				pSolarSysState->SysInfo.PlanetInfo.ScanSeed[BIOLOGICAL_SCAN] = rand_val;
				i = (COUNT)~0;
				rand_val = GenerateLifeForms (&pSolarSysState->SysInfo, &i);

				pSolarSysState->SysInfo.PlanetInfo.ScanSeed[MINERAL_SCAN] = rand_val;
				i = (COUNT)~0;
				GenerateMineralDeposits (&pSolarSysState->SysInfo, &i);

				pSolarSysState->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN] = rand_val;
			
				if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
					pSolarSysState->SysInfo.PlanetInfo.PlanetToSunDist = EARTH_RADIUS * 64L / 100;
			
				if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[1])
				{
					pSolarSysState->SysInfo.PlanetInfo.PlanetToSunDist = EARTH_RADIUS * 352L / 100;
					pSolarSysState->SysInfo.PlanetInfo.AtmoDensity = 160;
					pSolarSysState->SysInfo.PlanetInfo.Weather = 2;
					pSolarSysState->SysInfo.PlanetInfo.PlanetDensity = 104;
					pSolarSysState->SysInfo.PlanetInfo.PlanetRadius = 120;
					pSolarSysState->SysInfo.PlanetInfo.Tectonics = 1;
					pSolarSysState->SysInfo.PlanetInfo.RotationPeriod = 288;
					pSolarSysState->SysInfo.PlanetInfo.SurfaceTemperature = -47;
				
					LoadStdLanderFont (&pSolarSysState->SysInfo.PlanetInfo);
					pSolarSysState->PlanetSideFrame[1] = CaptureDrawable (LoadGraphic (CIRCLES_A_MASK_PMAP_ANIM));
					pSolarSysState->PlanetSideFrame[2] = CaptureDrawable (LoadGraphic (CIRCLES_B_MASK_PMAP_ANIM));
					pSolarSysState->SysInfo.PlanetInfo.DiscoveryString = CaptureStringTable (LoadStringTable (LEFTHURRY_STRTAB));
					pSolarSysState->SysInfo.PlanetInfo.Weather = 1;
					pSolarSysState->SysInfo.PlanetInfo.Tectonics = 1;
				}
		
				LoadPlanet (NULL);
				break;
			}
			default:
				GenerateRandomIP (control);
				break;
		}
	}
}


void
GenerateHint2 (BYTE control)
{
	COUNT i;
	DWORD rand_val;
	
	switch (control)
	{
		case GENERATE_ENERGY:
		{
			DWORD rand_val, old_rand;
			
			if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[1])
			{
				old_rand = TFB_SeedRandom (pSolarSysState->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN]);
				
				rand_val = TFB_Random ();
				pSolarSysState->SysInfo.PlanetInfo.CurPt.x = (LOBYTE (LOWORD (rand_val)) % (MAP_WIDTH - (8 << 1))) + 8;
				pSolarSysState->SysInfo.PlanetInfo.CurPt.y = (HIBYTE (LOWORD (rand_val)) % (MAP_HEIGHT - (8 << 1))) + 8;
				pSolarSysState->SysInfo.PlanetInfo.CurType = 1;
				pSolarSysState->SysInfo.PlanetInfo.CurDensity = 0;
				
				if (!(pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN] & (1L << 0)) && pSolarSysState->CurNode == (COUNT)~0)
					pSolarSysState->CurNode = 1;
				else
					pSolarSysState->CurNode = 0;
				
				TFB_SeedRandom (old_rand);
				break;
			}
			
			pSolarSysState->CurNode = 0;
			break;
		}
		case GENERATE_ORBITAL:
		{
			rand_val = DoPlanetaryAnalysis (&pSolarSysState->SysInfo, pSolarSysState->pOrbitalDesc);
			
			pSolarSysState->SysInfo.PlanetInfo.ScanSeed[BIOLOGICAL_SCAN] = rand_val;
			i = (COUNT)~0;
			rand_val = GenerateLifeForms (&pSolarSysState->SysInfo, &i);
			
			pSolarSysState->SysInfo.PlanetInfo.ScanSeed[MINERAL_SCAN] = rand_val;
			i = (COUNT)~0;
			GenerateMineralDeposits (&pSolarSysState->SysInfo, &i);
			
			pSolarSysState->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN] = rand_val;
			if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[1])
			{
				LoadStdLanderFont (&pSolarSysState->SysInfo.PlanetInfo);
				pSolarSysState->PlanetSideFrame[1] = CaptureDrawable (LoadGraphic (CIRCLES_C_MASK_PMAP_ANIM));
				pSolarSysState->SysInfo.PlanetInfo.DiscoveryString = CaptureStringTable (LoadStringTable (GAMMAJANUS_STRTAB));
			}
			
			if (GET_GAME_STATE(YEHAT_PRECURSOR_ARTIFACT) > 0)
			{
				pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
				SetRelStringTableIndex (pSolarSysState->SysInfo.PlanetInfo.DiscoveryString,1);
			}
			
			if (GET_GAME_STATE(YEHAT_PRECURSOR_ARTIFACT) < 2)
				SET_GAME_STATE(YEHAT_PRECURSOR_ARTIFACT, 2);
			
			LoadPlanet (NULL);
			break;
		}
		default:
			GenerateRandomIP (control);
			break;
	}
}
