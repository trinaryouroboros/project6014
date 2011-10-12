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

// JMS 2010: Removed shofixti maidens

#include "genall.h"
#include "../lander.h"
#include "../lifeform.h"
#include "../planets.h"
#include "../../build.h"
#include "../../comm.h"
#include "../../encount.h"
#include "../../globdata.h"
#include "../../ipdisp.h"
#include "../../nameref.h"
#include "../../setup.h"
#include "../../sounds.h"
#include "../../state.h"
#include "libs/mathlib.h"


static bool GenerateVux_generatePlanets (SOLARSYS_STATE *solarSys);
static bool GenerateVux_generateMoons (SOLARSYS_STATE *solarSys,
		PLANET_DESC *planet);
static bool GenerateVux_generateOrbital (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world);
static bool GenerateVux_generateEnergy (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world, COUNT *whichNode);
static bool GenerateVux_generateLife (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world, COUNT *whichNode);

const GenerateFunctions generateVuxFunctions = {
	/* .initNpcs         = */ GenerateDefault_initNpcs,
	/* .reinitNpcs       = */ GenerateDefault_reinitNpcs,
	/* .uninitNpcs       = */ GenerateDefault_uninitNpcs,
	/* .generatePlanets  = */ GenerateVux_generatePlanets,
	/* .generateMoons    = */ GenerateVux_generateMoons,
	/* .generateName     = */ GenerateDefault_generateName,
	/* .generateOrbital  = */ GenerateVux_generateOrbital,
	/* .generateMinerals = */ GenerateDefault_generateMinerals,
	/* .generateEnergy   = */ GenerateVux_generateEnergy,
	/* .generateLife     = */ GenerateVux_generateLife,
};


static bool
GenerateVux_generatePlanets (SOLARSYS_STATE *solarSys)
{
	COUNT angle;

	GenerateDefault_generatePlanets (solarSys);

	if (CurStarDescPtr->Index == MAIDENS_DEFINED)
	{
		GenerateDefault_generatePlanets (solarSys);
				// XXX: this is the second time that this function is
				// called. Is it safe to remove one, or does this change
				// the RNG so that the outcome is different?
		solarSys->PlanetDesc[0].data_index = REDUX_WORLD;
		solarSys->PlanetDesc[0].radius = EARTH_RADIUS * 212L / 100;
		angle = ARCTAN (solarSys->PlanetDesc[0].location.x,
				solarSys->PlanetDesc[0].location.y);
		solarSys->PlanetDesc[0].location.x =
				COSINE (angle, solarSys->PlanetDesc[0].radius);
		solarSys->PlanetDesc[0].location.y =
				SINE (angle, solarSys->PlanetDesc[0].radius);
	}
	else
	{
		if (CurStarDescPtr->Index == VUX_DEFINED)
		{
			solarSys->PlanetDesc[0].data_index = REDUX_WORLD | PLANET_SHIELDED;
			solarSys->PlanetDesc[0].flags = BLUE_SHIELD;
			solarSys->PlanetDesc[0].NumPlanets = 2;
			solarSys->PlanetDesc[0].radius = EARTH_RADIUS * 42L / 100;
			angle = HALF_CIRCLE + OCTANT;
		}
		else /* if (CurStarDescPtr->Index == VUX_BEAST_DEFINED) */
		{
			memmove (&solarSys->PlanetDesc[1], &solarSys->PlanetDesc[0],
					sizeof (solarSys->PlanetDesc[0])
					* solarSys->SunDesc[0].NumPlanets);
			++solarSys->SunDesc[0].NumPlanets;

			angle = HALF_CIRCLE - OCTANT;
			solarSys->PlanetDesc[0].data_index = WATER_WORLD;
			solarSys->PlanetDesc[0].radius = EARTH_RADIUS * 110L / 100;
			solarSys->PlanetDesc[0].NumPlanets = 0;
		}

		solarSys->PlanetDesc[0].location.x =
				COSINE (angle, solarSys->PlanetDesc[0].radius);
		solarSys->PlanetDesc[0].location.y =
				SINE (angle, solarSys->PlanetDesc[0].radius);
		solarSys->PlanetDesc[0].rand_seed = MAKE_DWORD (
				solarSys->PlanetDesc[0].location.x,
				solarSys->PlanetDesc[0].location.y);
	}
	return true;
}

static bool
GenerateVux_generateMoons (SOLARSYS_STATE *solarSys, PLANET_DESC *planet)
{
	GenerateDefault_generateMoons (solarSys, planet);
	if (CurStarDescPtr->Index == VUX_DEFINED
	    && matchWorld(solarSys, planet, 0, MATCH_PLANET))
	{
		TFB_SeedRandom(10026855);
		GenerateDefault_generateMoons (solarSys, planet);
		
		solarSys->MoonDesc[0].data_index = (BYTE)~0;
		solarSys->MoonDesc[0].radius = MIN_MOON_RADIUS;
		solarSys->MoonDesc[0].location.x =
			COSINE (QUADRANT, pSolarSysState->MoonDesc[0].radius);
		solarSys->MoonDesc[0].location.y =
			SINE (QUADRANT, pSolarSysState->MoonDesc[0].radius);
	}

	return true;
}

static bool
GenerateVux_generateOrbital (SOLARSYS_STATE *solarSys, PLANET_DESC *world)
{
	if (CurStarDescPtr->Index == VUX_DEFINED
	    && matchWorld(solarSys, world, 0, 0))
	{
		InitCommunication (VUX_CONVERSATION);
	}
	else
	{
		GenerateDefault_generateOrbital (solarSys, world);
	}
	return true;
}

static bool
GenerateVux_generateEnergy (SOLARSYS_STATE *solarSys, PLANET_DESC *world,
		COUNT *whichNode)
{
	if (matchWorld (solarSys, world, 0, MATCH_PLANET)
			&& CurStarDescPtr->Index != VUX_BEAST_DEFINED)
	{
		/*if (CurStarDescPtr->Index == MAIDENS_DEFINED
				&& !GET_GAME_STATE (SHOFIXTI_MAIDENS))
		{
			solarSys->SysInfo.PlanetInfo.CurPt.x = MAP_WIDTH / 3;
			solarSys->SysInfo.PlanetInfo.CurPt.y = MAP_HEIGHT * 5 / 8;
			solarSys->SysInfo.PlanetInfo.CurDensity = 0;
			solarSys->SysInfo.PlanetInfo.CurType = 0;
			if (!(solarSys->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
					& (1L << 0))
					&& *whichNode == (COUNT)~0)
				*whichNode = 1;
			else
			{
				*whichNode = 0;
				if (solarSys->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
						& (1L << 0))
				{
					SET_GAME_STATE (SHOFIXTI_MAIDENS, 1);
					SET_GAME_STATE (MAIDENS_ON_SHIP, 1);
				}
			}
			return true;
			}*/

		if (CurStarDescPtr->Index == VUX_DEFINED)
		{
			COUNT i;
			COUNT nodeI;
			DWORD rand_val;
			DWORD old_rand;

			old_rand = TFB_SeedRandom (
					solarSys->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN]);

			nodeI = 0;
			i = 0;
			do
			{
				rand_val = TFB_Random ();
				solarSys->SysInfo.PlanetInfo.CurPt.x =
						(LOBYTE (LOWORD (rand_val)) % (MAP_WIDTH - (8 << 1))) + 8;
				solarSys->SysInfo.PlanetInfo.CurPt.y =
						(HIBYTE (LOWORD (rand_val)) % (MAP_HEIGHT - (8 << 1))) + 8;
				solarSys->SysInfo.PlanetInfo.CurType = 1;
				solarSys->SysInfo.PlanetInfo.CurDensity = 0;
				if (nodeI >= *whichNode
						&& !(solarSys->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
						& (1L << i)))
					break;
				++nodeI;
			} while (++i < 16);
			*whichNode = nodeI;

			TFB_SeedRandom (old_rand);
			return true;
		}
	}

	*whichNode = 0;
	return true;
}

static bool
GenerateVux_generateLife (SOLARSYS_STATE *solarSys, PLANET_DESC *world,
		COUNT *whichNode)
{
	if (CurStarDescPtr->Index == MAIDENS_DEFINED
			&& matchWorld (solarSys, world, 0, MATCH_PLANET))
	{
		COUNT i;
		DWORD old_rand;

		old_rand = TFB_SeedRandom (
				solarSys->SysInfo.PlanetInfo.ScanSeed[BIOLOGICAL_SCAN]);

		i = 0;
		do
		{
			DWORD rand_val = TFB_Random ();
			solarSys->SysInfo.PlanetInfo.CurPt.x =
					(LOBYTE (LOWORD (rand_val)) % (MAP_WIDTH - (8 << 1))) + 8;
			solarSys->SysInfo.PlanetInfo.CurPt.y =
					(HIBYTE (LOWORD (rand_val)) % (MAP_HEIGHT - (8 << 1))) + 8;
			if (i < 4)
				solarSys->SysInfo.PlanetInfo.CurType = 9;
			else if (i < 8)
				solarSys->SysInfo.PlanetInfo.CurType = 14;
			else /* if (i < 12) */
				solarSys->SysInfo.PlanetInfo.CurType = 18;
			if (i >= *whichNode
					&& !(solarSys->SysInfo.PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN]
					& (1L << i)))
				break;
		} while (++i < 12);
		*whichNode = i;

		TFB_SeedRandom (old_rand);
		return true;
	}

	if (CurStarDescPtr->Index == VUX_BEAST_DEFINED
			&& matchWorld (solarSys, world, 0, MATCH_PLANET))
	{
		COUNT i;
		DWORD old_rand;

		old_rand = TFB_SeedRandom (
				solarSys->SysInfo.PlanetInfo.ScanSeed[BIOLOGICAL_SCAN]);

		i = 0;
		do
		{
			DWORD rand_val = TFB_Random ();
			solarSys->SysInfo.PlanetInfo.CurPt.x =
					(LOBYTE (LOWORD (rand_val)) % (MAP_WIDTH - (8 << 1))) + 8;
			solarSys->SysInfo.PlanetInfo.CurPt.y =
					(HIBYTE (LOWORD (rand_val)) % (MAP_HEIGHT - (8 << 1))) + 8;
			if (i == 0)
				solarSys->SysInfo.PlanetInfo.CurType = NUM_CREATURE_TYPES + 2;
			else if (i <= 5)
					/* {SPEED_MOTIONLESS | DANGER_NORMAL, MAKE_BYTE (5, 3)}, */
				solarSys->SysInfo.PlanetInfo.CurType = 3;
			else /* if (i <= 10) */
					/* {BEHAVIOR_UNPREDICTABLE | SPEED_SLOW | DANGER_NORMAL, MAKE_BYTE (3, 8)}, */
				solarSys->SysInfo.PlanetInfo.CurType = 8;
			if (i >= *whichNode
					&& !(solarSys->SysInfo.PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN]
					& (1L << i)))
				break;
			else if (i == 0
					&& (solarSys->SysInfo.PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN]
					& (1L << i))
					&& !GET_GAME_STATE (VUX_BEAST))
			{
				UnbatchGraphics ();
				DoDiscoveryReport (MenuSounds);
				BatchGraphics ();
				SetLanderTakeoff ();

				SET_GAME_STATE (VUX_BEAST, 1);
				SET_GAME_STATE (VUX_BEAST_ON_SHIP, 1);
			}
		} while (++i < 11);
		*whichNode = i;

		TFB_SeedRandom (old_rand);
		return true;
	}

	GenerateDefault_generateLife (solarSys, world, whichNode);
	return true;
}

