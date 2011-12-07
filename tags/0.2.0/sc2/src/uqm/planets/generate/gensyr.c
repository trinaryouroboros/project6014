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

#include "../../build.h"
#include "genall.h"
#include "../planets.h"
#include "../../comm.h"
#include "../../globdata.h"
#include "../../nameref.h"
#include "../../setup.h"
#include "../../state.h"

static bool GenerateSyreen_generatePlanets (SOLARSYS_STATE *solarSys);
static bool GenerateSyreen_generateMoons (SOLARSYS_STATE *solarSys,
		PLANET_DESC *planet);
static bool GenerateSyreen_generateOrbital (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world);


const GenerateFunctions generateSyreenFunctions = {
	/* .initNpcs         = */ GenerateDefault_initNpcs,
	/* .reinitNpcs       = */ GenerateDefault_reinitNpcs,
	/* .uninitNpcs       = */ GenerateDefault_uninitNpcs,
	/* .generatePlanets  = */ GenerateSyreen_generatePlanets,
	/* .generateMoons    = */ GenerateSyreen_generateMoons,
	/* .generateName     = */ GenerateDefault_generateName,
	/* .generateOrbital  = */ GenerateSyreen_generateOrbital,
	/* .generateMinerals = */ GenerateDefault_generateMinerals,
	/* .generateEnergy   = */ GenerateDefault_generateEnergy,
	/* .generateLife     = */ GenerateDefault_generateLife,
};


static bool
GenerateSyreen_generatePlanets (SOLARSYS_STATE *solarSys)
{
	GenerateDefault_generatePlanets (solarSys);

	solarSys->PlanetDesc[0].data_index = WATER_WORLD;
	solarSys->PlanetDesc[0].flags = PLANET_RESTRICTED; // JMS: Can't land on syreen turf
	solarSys->PlanetDesc[0].NumPlanets = 1;

	return true;
}

static bool
GenerateSyreen_generateMoons (SOLARSYS_STATE *solarSys, PLANET_DESC *planet)
{
	GenerateDefault_generateMoons (solarSys, planet);

	if (matchWorld (solarSys, planet, 0, MATCH_PLANET))
	{
		solarSys->MoonDesc[0].data_index = HIERARCHY_STARBASE;
		solarSys->MoonDesc[0].radius = MIN_MOON_RADIUS;
		solarSys->MoonDesc[0].location.x =
				COSINE (QUADRANT, solarSys->MoonDesc[0].radius);
		solarSys->MoonDesc[0].location.y =
				SINE (QUADRANT, solarSys->MoonDesc[0].radius);
		ComputeSpeed(&solarSys->MoonDesc[0], TRUE, 1);
	}

	return true;
}

static bool
GenerateSyreen_generateOrbital (SOLARSYS_STATE *solarSys, PLANET_DESC *world)
{
	if (matchWorld (solarSys, world, 0, MATCH_PLANET))
	{
		/* Syreen home planet */
		InitCommunication (SYREENHOME_CONVERSATION);
		return true;
	}

	if (matchWorld (solarSys, world, 0, 0))
	{
		/* Starbase */
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
		return true;
	}

	GenerateDefault_generateOrbital (solarSys, world);

	return true;
}

