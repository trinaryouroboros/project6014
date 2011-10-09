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

#include "genall.h"
#include "../planets.h"
#include "../../build.h"
#include "../../globdata.h"
#include "../../grpinfo.h"
#include "../../nameref.h" // JMS
#include "../../resinst.h" // JMS
#include "../../sounds.h" //JMS
#include "../../state.h"
#include "libs/mathlib.h"
#include "libs/mathlib.h" // JMS
#include "libs/log.h" // JMS
#include "../lander.h" //JMS


static bool GenerateColony_initNpcs (SOLARSYS_STATE *solarSys);
static bool GenerateColony_generateEnergy (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world, COUNT *whichNode);
static bool GenerateColony_generatePlanets (SOLARSYS_STATE *solarSys);
static bool GenerateColony_generateOrbital (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world);


const GenerateFunctions generateColonyFunctions = {
	/* .initNpcs         = */ GenerateColony_initNpcs,
	/* .reinitNpcs       = */ GenerateDefault_reinitNpcs,
	/* .uninitNpcs       = */ GenerateDefault_uninitNpcs,
	/* .generatePlanets  = */ GenerateColony_generatePlanets,
	/* .generateMoons    = */ GenerateDefault_generateMoons,
	/* .generateName     = */ GenerateDefault_generateName,
	/* .generateOrbital  = */ GenerateColony_generateOrbital,
	/* .generateMinerals = */ GenerateDefault_generateMinerals,
	/* .generateEnergy   = */ GenerateColony_generateEnergy,
	/* .generateLife     = */ GenerateDefault_generateLife,
};


static bool
GenerateColony_initNpcs (SOLARSYS_STATE *solarSys)
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

	GenerateDefault_initNpcs (solarSys);

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

	return true;
}

// JMS: The whole GENERATE_ENERGY case is new here. Its purpose is to create energy blip of the Precursor factory remnants.
static bool
GenerateColony_generateEnergy (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world, COUNT *whichNode)
{
	DWORD rand_val, old_rand;
	
	if (solarSys->pOrbitalDesc == &solarSys->PlanetDesc[0])
		{
			old_rand = TFB_SeedRandom (
						   solarSys->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN]);
			
			rand_val = TFB_Random ();
			solarSys->SysInfo.PlanetInfo.CurPt.x = MAP_WIDTH * 5 / 9;
			solarSys->SysInfo.PlanetInfo.CurPt.y = MAP_HEIGHT * 2 / 5;
			solarSys->SysInfo.PlanetInfo.CurDensity = 0;
			solarSys->SysInfo.PlanetInfo.CurType = 1;
			if (!(solarSys->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN] & (1L << 0))
			    && solarSys->CurNode == (COUNT)~0)
				{
					solarSys->CurNode = 1;
				}
			else
				{
					solarSys->CurNode = 0;
					if (solarSys->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN] & (1L << 0))
						{
							SET_GAME_STATE (VELA_FACTORY_VISITED, 1);
						}
				}
			
			TFB_SeedRandom (old_rand);
			return true;
		}
	solarSys->CurNode = 0;
	return true;
}

static bool
GenerateColony_generatePlanets (SOLARSYS_STATE *solarSys)
{
	COUNT angle;
	PLANET_DESC *pMinPlanet;

	pMinPlanet = &solarSys->PlanetDesc[0];
	FillOrbits (solarSys, (BYTE)~0, pMinPlanet, FALSE);

	pMinPlanet->radius = EARTH_RADIUS * 115L / 100;
	angle = ARCTAN (pMinPlanet->location.x, pMinPlanet->location.y);
	pMinPlanet->location.x = COSINE (angle, pMinPlanet->radius);
	pMinPlanet->location.y = SINE (angle, pMinPlanet->radius);
	pMinPlanet->data_index = WATER_WORLD; // JMS: Vela colony is not shielded anymore.
	return true;
}

static bool
GenerateColony_generateOrbital (SOLARSYS_STATE *solarSys, PLANET_DESC *world)
{
	if (matchWorld (solarSys, world, 0, MATCH_PLANET))
	{
		DoPlanetaryAnalysis (&solarSys->SysInfo, world);

		// JMS: This displays the graphics of the Precursor factory remnants
		// and gives the landing team message upon finding it.
		LoadStdLanderFont (&solarSys->SysInfo.PlanetInfo);
		solarSys->PlanetSideFrame[1] =
			CaptureDrawable (LoadGraphic (UMGAH_BCS_MASK_PMAP_ANIM));
		solarSys->SysInfo.PlanetInfo.DiscoveryString =
			CaptureStringTable (LoadStringTable (VELA_FACTORY_STRTAB));
		
		solarSys->SysInfo.PlanetInfo.AtmoDensity =
			EARTH_ATMOSPHERE * 98 / 100;
		solarSys->SysInfo.PlanetInfo.Weather = 0;
		solarSys->SysInfo.PlanetInfo.Tectonics = 0;
		solarSys->SysInfo.PlanetInfo.SurfaceTemperature = 28;

		LoadPlanet (NULL);

		return true;
	}

	GenerateDefault_generateOrbital (solarSys, world);

	return true;
}

