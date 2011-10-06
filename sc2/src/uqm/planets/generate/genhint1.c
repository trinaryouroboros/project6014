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

// JMS 2010: - This is a completely new file.

#include "../../encount.h"
#include "../../globdata.h"
#include "../../nameref.h"
#include "../../resinst.h"
#include "genall.h"
#include "libs/mathlib.h"

static bool GenerateHint1_generatePlanets (SOLARSYS_STATE *solarSys);
static bool GenerateHint1_generateMoons (SOLARSYS_STATE *solarSys,
		PLANET_DESC *planet);
static bool GenerateHint1_generateOrbital (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world);
static bool GenerateHint1_generateEnergy (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world, COUNT *whichNode);
static bool GenerateHint1_generateLife (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world, COUNT *whichNode);


const GenerateFunctions generateHint1Functions = {
	/* .initNpcs         = */ GenerateDefault_initNpcs,
	/* .reinitNpcs       = */ GenerateDefault_reinitNpcs,
	/* .uninitNpcs       = */ GenerateDefault_uninitNpcs,
	/* .generatePlanets  = */ GenerateHint1_generatePlanets,
	/* .generateMoons    = */ GenerateHint1_generateMoons,
	/* .generateName     = */ GenerateDefault_generateName,
	/* .generateOrbital  = */ GenerateHint1_generateOrbital,
	/* .generateMinerals = */ GenerateDefault_generateMinerals,
	/* .generateEnergy   = */ GenerateHint1_generateEnergy,
	/* .generateLife     = */ GenerateHint1_generateLife,
};


BYTE which_hintworld_is_it ()
{
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
	return which_hintworld;
}

static bool GenerateHint1_generatePlanets (SOLARSYS_STATE *solarSys)
{
	BYTE which_hintworld = which_hintworld_is_it ();
	
	GenerateDefault_generatePlanets(solarSys);
	
	if (which_hintworld == 1)
	{
		COUNT angle;
		
		solarSys->SunDesc[0].NumPlanets = 2;
		
		solarSys->PlanetDesc[0].data_index = METAL_WORLD;
		solarSys->PlanetDesc[0].radius = EARTH_RADIUS * 64L / 100;
		angle = ARCTAN (
				solarSys->PlanetDesc[0].location.x,
				solarSys->PlanetDesc[0].location.y);
		solarSys->PlanetDesc[0].location.x = COSINE (angle, solarSys->PlanetDesc[0].radius);
		solarSys->PlanetDesc[0].location.y = SINE (angle, solarSys->PlanetDesc[0].radius);
		
		solarSys->PlanetDesc[1].data_index = PRIMORDIAL_WORLD;
		solarSys->PlanetDesc[1].radius = EARTH_RADIUS * 352L / 100;
		solarSys->PlanetDesc[1].NumPlanets = 1;
		angle = ARCTAN (
				solarSys->PlanetDesc[1].location.x,
				solarSys->PlanetDesc[1].location.y);
		solarSys->PlanetDesc[1].location.x = COSINE (angle, solarSys->PlanetDesc[1].radius);
		solarSys->PlanetDesc[1].location.y = SINE (angle, solarSys->PlanetDesc[1].radius);
	}
	return true;
}

static bool GenerateHint1_generateMoons (SOLARSYS_STATE *solarSys,
		PLANET_DESC *planet)
{
	DWORD rand_val;
	BYTE which_hintworld = which_hintworld_is_it ();

	GenerateDefault_generateMoons (solarSys, planet);

	if (matchWorld(solarSys, planet, 1, MATCH_PLANET)
	    && which_hintworld == 1)
	{
		COUNT angle;
		
		solarSys->MoonDesc[0].data_index = PELLUCID_WORLD;
		solarSys->MoonDesc[0].radius = MIN_MOON_RADIUS * 2;
		rand_val = TFB_Random ();
		angle = NORMALIZE_ANGLE (LOWORD (rand_val));
		solarSys->MoonDesc[0].location.x = COSINE (angle, solarSys->MoonDesc[0].radius);
		solarSys->MoonDesc[0].location.y = SINE (angle, solarSys->MoonDesc[0].radius);
	}	
	return true;
}

static bool GenerateHint1_generateOrbital (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world)
{
	COUNT i;
	DWORD rand_val;
	BYTE which_hintworld = which_hintworld_is_it ();
	if ((GET_GAME_STATE(HINT_WORLD_LOCATION) == which_hintworld)
	    || (which_hintworld == 1))
	{
		rand_val = DoPlanetaryAnalysis (&solarSys->SysInfo, solarSys->pOrbitalDesc);
		
		solarSys->SysInfo.PlanetInfo.ScanSeed[BIOLOGICAL_SCAN] = rand_val;
		i = (COUNT)~0;
		rand_val = GenerateLifeForms (&solarSys->SysInfo, &i);
		
		solarSys->SysInfo.PlanetInfo.ScanSeed[MINERAL_SCAN] = rand_val;
		i = (COUNT)~0;
		GenerateMineralDeposits (&solarSys->SysInfo, &i);
		
		solarSys->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN] = rand_val;
		
		if (matchWorld(solarSys, world, 0, MATCH_PLANET))
			solarSys->SysInfo.PlanetInfo.PlanetToSunDist = EARTH_RADIUS * 64L / 100;
		
		if (matchWorld(solarSys, world, 1, MATCH_PLANET))
		{
			solarSys->SysInfo.PlanetInfo.PlanetToSunDist = EARTH_RADIUS * 352L / 100;
			solarSys->SysInfo.PlanetInfo.AtmoDensity = 160;
			solarSys->SysInfo.PlanetInfo.Weather = 2;
			solarSys->SysInfo.PlanetInfo.PlanetDensity = 104;
			solarSys->SysInfo.PlanetInfo.PlanetRadius = 120;
			solarSys->SysInfo.PlanetInfo.Tectonics = 1;
			solarSys->SysInfo.PlanetInfo.RotationPeriod = 288;
			solarSys->SysInfo.PlanetInfo.SurfaceTemperature = -47;
			
			LoadStdLanderFont (&solarSys->SysInfo.PlanetInfo);
			solarSys->PlanetSideFrame[1] = CaptureDrawable (LoadGraphic (CIRCLES_A_MASK_PMAP_ANIM));
			solarSys->PlanetSideFrame[2] = CaptureDrawable (LoadGraphic (CIRCLES_B_MASK_PMAP_ANIM));
			solarSys->SysInfo.PlanetInfo.DiscoveryString = CaptureStringTable (LoadStringTable (LEFTHURRY_STRTAB));
			solarSys->SysInfo.PlanetInfo.Weather = 1;
			solarSys->SysInfo.PlanetInfo.Tectonics = 1;
		}
		
		LoadPlanet (NULL);
	}
	else
	{
		GenerateDefault_generateOrbital (solarSys, world);
	}
	return true;
}

static bool GenerateHint1_generateEnergy (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world, COUNT *whichNode)
{
	COUNT i;
	BYTE which_hintworld = which_hintworld_is_it ();
	if (!(GET_GAME_STATE(HINT_WORLD_LOCATION) == which_hintworld))
	{
		GenerateDefault_generateEnergy (solarSys, world, whichNode);
	}
	else
	{
		DWORD rand_val, old_rand;
		
		if (matchWorld(solarSys, world, 1, MATCH_PLANET))
		{
			COUNT which_node;
			
			old_rand = TFB_SeedRandom (solarSys->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN]);
			
			which_node = i = 0;
			
			do
			{
				rand_val = TFB_Random ();
				solarSys->SysInfo.PlanetInfo.CurPt.x = (LOBYTE (LOWORD (rand_val)) % (MAP_WIDTH - (8 << 1))) + 8;
				solarSys->SysInfo.PlanetInfo.CurPt.y = (HIBYTE (LOWORD (rand_val)) % (MAP_HEIGHT - (8 << 1))) + 8;
				solarSys->SysInfo.PlanetInfo.CurType = 1;
				solarSys->SysInfo.PlanetInfo.CurDensity = 0;
				if (which_node >= solarSys->CurNode
				    && !(solarSys->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN] & (1L << i)))
					break;
				++which_node;
			} while (++i < 24);
			
			solarSys->CurNode = which_node;
			
			TFB_SeedRandom (old_rand);
		}
		
		solarSys->CurNode = 0;
	}
	return true;
}

static bool GenerateHint1_generateLife (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world, COUNT *whichNode)
{
	BYTE which_hintworld = which_hintworld_is_it ();
	if (!(GET_GAME_STATE(HINT_WORLD_LOCATION) == which_hintworld))
	{
		GenerateDefault_generateLife (solarSys, world, whichNode);
	}
	else
	{
		if (matchWorld(solarSys, world, 1, MATCH_PLANET))
			solarSys->CurNode = 0;
                else
			GenerateDefault_generateLife (solarSys, world, whichNode);
	}
	return true;
}
