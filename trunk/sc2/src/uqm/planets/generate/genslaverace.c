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

#include "../../build.h"
#include "../../encount.h"
#include "../../globdata.h"
#include "../../ipdisp.h"
#include "../lander.h"
#include "../../nameref.h"
#include "../../resinst.h"
#include "../../setup.h"
#include "../../state.h"
#include "../../sounds.h"
#include "genall.h"
#include "libs/mathlib.h"

static bool GenerateSlaverace_generatePlanets (SOLARSYS_STATE *solarSys);
static bool GenerateSlaverace_generateOrbital (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world);


const GenerateFunctions generateSlaveraceFunctions = {
	/* .initNpcs         = */ GenerateDefault_initNpcs,
	/* .reinitNpcs       = */ GenerateDefault_reinitNpcs,
	/* .uninitNpcs       = */ GenerateDefault_uninitNpcs,
	/* .generatePlanets  = */ GenerateSlaverace_generatePlanets,
	/* .generateMoons    = */ GenerateDefault_generateMoons,
	/* .generateName     = */ GenerateDefault_generateName,
	/* .generateOrbital  = */ GenerateSlaverace_generateOrbital,
	/* .generateMinerals = */ GenerateDefault_generateMinerals,
	/* .generateEnergy   = */ GenerateDefault_generateEnergy,
	/* .generateLife     = */ GenerateDefault_generateLife,
};


static bool
GenerateSlaverace_generatePlanets (SOLARSYS_STATE *solarSys)
{
	COUNT angle;
	
	GenerateDefault_generatePlanets (solarSys);
	if (CurStarDescPtr->Index == SLAVERACE_DEFINED)
	{
		solarSys->PlanetDesc[0].data_index = WATER_WORLD;
		solarSys->PlanetDesc[0].radius = EARTH_RADIUS * 156L / 100;
		solarSys->PlanetDesc[0].NumPlanets = 0;
		angle = ARCTAN (
				solarSys->PlanetDesc[0].location.x,
				solarSys->PlanetDesc[0].location.y
				);
		solarSys->PlanetDesc[0].location.x =
			COSINE (angle, solarSys->PlanetDesc[0].radius);
		solarSys->PlanetDesc[0].location.y =
			SINE (angle, solarSys->PlanetDesc[0].radius);
	}
	return true;
}

static bool
GenerateSlaverace_generateOrbital (SOLARSYS_STATE *solarSys,
				   PLANET_DESC *world)
{
	if ((CurStarDescPtr->Index == SLAVERACE_DEFINED
	     && matchWorld(solarSys, world, 0, MATCH_PLANET))
	    || (CurStarDescPtr->Index == TAALO_PROTECTOR_DEFINED
		&& matchWorld(solarSys, world, 1, 2)
		&& !GET_GAME_STATE (TAALO_PROTECTOR)))
	{
		COUNT i;
		
		if ((CurStarDescPtr->Index == ORZ_DEFINED
		     || !GET_GAME_STATE (TAALO_UNPROTECTED))
		    && ActivateStarShip (ORZ_SHIP, SPHERE_TRACKING))
		{
			NotifyOthers (ORZ_SHIP, IPNL_ALL_CLEAR);
			PutGroupInfo (GROUPS_RANDOM, GROUP_SAVE_IP);
			ReinitQueue (&GLOBAL (ip_group_q));
			assert (CountLinks (&GLOBAL (npc_built_ship_q)) == 0);
			
			if (CurStarDescPtr->Index == SLAVERACE_DEFINED)
			{
				CloneShipFragment (ORZ_SHIP,
						   &GLOBAL (npc_built_ship_q), INFINITE_FLEET);
				SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 7);
			}
			else
			{
				for (i = 0; i < 14; ++i)
					CloneShipFragment (ORZ_SHIP,
							   &GLOBAL (npc_built_ship_q), 0);
				SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 6);
			}
			solarSys->MenuState.Initialized += 2;
			GLOBAL (CurrentActivity) |= START_INTERPLANETARY;
			InitCommunication (CHMMR_CONVERSATION);
			solarSys->MenuState.Initialized -= 2;
			
			if (GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD))
				return true;
			else
			{
				BOOLEAN OrzSurvivors;
				
				OrzSurvivors = GetHeadLink (&GLOBAL (npc_built_ship_q))
					&& (CurStarDescPtr->Index == SLAVERACE_DEFINED
					    || !GET_GAME_STATE (TAALO_UNPROTECTED));
				
				GLOBAL (CurrentActivity) &= ~START_INTERPLANETARY;
				ReinitQueue (&GLOBAL (npc_built_ship_q));
				GetGroupInfo (GROUPS_RANDOM, GROUP_LOAD_IP);
				
				if (OrzSurvivors)
					return true;
				
				LockMutex (GraphicsLock);
				RepairSISBorder ();
				UnlockMutex (GraphicsLock);
			}
		}
		
		SET_GAME_STATE (TAALO_UNPROTECTED, 1);
		if (CurStarDescPtr->Index == TAALO_PROTECTOR_DEFINED)
		{
			LoadStdLanderFont (&solarSys->SysInfo.PlanetInfo);
			solarSys->PlanetSideFrame[1] =
				CaptureDrawable (
						 LoadGraphic (TAALO_DEVICE_MASK_PMAP_ANIM)
						 );
			solarSys->SysInfo.PlanetInfo.DiscoveryString =
				CaptureStringTable (
						    LoadStringTable (TAALO_DEVICE_STRTAB)
						    );
		}
		else
		{
			LoadStdLanderFont (&solarSys->SysInfo.PlanetInfo);
			solarSys->PlanetSideFrame[1] =
				CaptureDrawable (
						 LoadGraphic (RUINS_MASK_PMAP_ANIM)
						 );
			solarSys->SysInfo.PlanetInfo.DiscoveryString =
				CaptureStringTable (
						    LoadStringTable (RUINS_STRTAB)
						    );
		}
	}
	GenerateDefault_generateOrbital (solarSys, world);
	return true;
}
