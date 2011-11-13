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

// JMS 2011: - Totally new file: Shofixti colony

#include "../../build.h"
#include "../../encount.h"
#include "../../globdata.h"
#include "../../ipdisp.h"
#include "../lander.h"
#include "../../nameref.h"
#include "../../resinst.h"
#include "../../setup.h"
#include "../../state.h"
#include "genall.h"
#include "libs/mathlib.h"
#include "libs/log.h"
#include "../../grpinfo.h"


static bool GenerateShofixtiColony_generatePlanets (SOLARSYS_STATE *solarSys);
static bool GenerateShofixtiColony_generateMoons (SOLARSYS_STATE *solarSys, PLANET_DESC *planet);
static bool GenerateShofixtiColony_generateOrbital (SOLARSYS_STATE *solarSys, PLANET_DESC *world);

const GenerateFunctions generateShofixtiColonyFunctions = {
	/* .initNpcs         = */ GenerateDefault_initNpcs,
	/* .reinitNpcs       = */ GenerateDefault_reinitNpcs,
	/* .uninitNpcs       = */ GenerateDefault_uninitNpcs,
	/* .generatePlanets  = */ GenerateShofixtiColony_generatePlanets,
	/* .generateMoons    = */ GenerateShofixtiColony_generateMoons,
	/* .generateName     = */ GenerateDefault_generateName,
	/* .generateOrbital  = */ GenerateShofixtiColony_generateOrbital,
	/* .generateMinerals = */ GenerateDefault_generateMinerals,
	/* .generateEnergy   = */ GenerateDefault_generateEnergy,
	/* .generateLife     = */ GenerateDefault_generateLife,
};

static bool
GenerateShofixtiColony_generatePlanets (SOLARSYS_STATE *solarSys)
{
	COUNT angle;
	
	GenerateDefault_generatePlanets (solarSys);
	
	solarSys->PlanetDesc[0].data_index = XENOLITHIC_WORLD;
	solarSys->PlanetDesc[0].NumPlanets = 1;
	solarSys->PlanetDesc[0].radius = EARTH_RADIUS * 123L / 100;
	angle = ARCTAN (solarSys->PlanetDesc[0].location.x, solarSys->PlanetDesc[0].location.y);
	solarSys->PlanetDesc[0].location.x = COSINE (angle, solarSys->PlanetDesc[0].radius);
	solarSys->PlanetDesc[0].location.y = SINE (angle, solarSys->PlanetDesc[0].radius);
	ComputeSpeed(&solarSys->PlanetDesc[0], FALSE, 1);
	return true;
}


	
static bool
GenerateShofixtiColony_generateMoons (SOLARSYS_STATE *solarSys,
					 PLANET_DESC *planet)
{
	DWORD rand_val;
	
	GenerateDefault_generateMoons (solarSys, planet);
	if (matchWorld(solarSys, planet, 0, MATCH_PLANET))
	{
		COUNT angle;
		
		solarSys->MoonDesc[0].radius = MIN_MOON_RADIUS + (MAX_MOONS - 2) * MOON_DELTA;
		rand_val = TFB_Random ();
		angle = NORMALIZE_ANGLE (LOWORD (rand_val));
		solarSys->MoonDesc[0].location.x = COSINE (angle, solarSys->MoonDesc[0].radius);
		solarSys->MoonDesc[0].location.y = SINE (angle, solarSys->MoonDesc[0].radius);
		ComputeSpeed(&solarSys->MoonDesc[0], TRUE, 1);
	}
	return true;
}


static bool
GenerateShofixtiColony_generateOrbital (SOLARSYS_STATE *solarSys,
					   PLANET_DESC *world)
{
	if (matchWorld (solarSys, world, 0, MATCH_PLANET))
	{
		if (ActivateStarShip (SHOFIXTI_SHIP, SPHERE_TRACKING))
		{
			NotifyOthers (SHOFIXTI_SHIP, IPNL_ALL_CLEAR);
			PutGroupInfo (GROUPS_RANDOM, GROUP_SAVE_IP);
			ReinitQueue (&GLOBAL (ip_group_q));
			assert (CountLinks (&GLOBAL (npc_built_ship_q)) == 0);
			
			CloneShipFragment (SHOFIXTI_SHIP, &GLOBAL (npc_built_ship_q), 15);
			
			GLOBAL (CurrentActivity) |= START_INTERPLANETARY;
			SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 7);
			InitCommunication (SHOFIXTICOLONY_CONVERSATION);
			
			if (!(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)))
			{
				GLOBAL (CurrentActivity) &= ~START_INTERPLANETARY;
				ReinitQueue (&GLOBAL (npc_built_ship_q));
				GetGroupInfo (GROUPS_RANDOM, GROUP_LOAD_IP);
			}
			
			return true;
		}
		
		LoadStdLanderFont (&solarSys->SysInfo.PlanetInfo);
		solarSys->PlanetSideFrame[1] = CaptureDrawable (LoadGraphic (RUINS_MASK_PMAP_ANIM));
		solarSys->SysInfo.PlanetInfo.DiscoveryString = CaptureStringTable (LoadStringTable (RUINS_STRTAB));
	}
	
	GenerateDefault_generateOrbital (solarSys, world);
	return true;
}
