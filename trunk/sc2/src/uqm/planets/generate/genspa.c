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

#include "genall.h"
#include "../lifeform.h"
#include "../planets.h"
#include "../../build.h"
#include "../../comm.h"
#include "../../globdata.h"
#include "../../ipdisp.h"
#include "../../nameref.h"
#include "../../state.h"
#include "libs/mathlib.h"


static bool GenerateSpathi_generatePlanets (SOLARSYS_STATE *solarSys);
static bool GenerateSpathi_generateMoons (SOLARSYS_STATE *solarSys,
		PLANET_DESC *planet);
static bool GenerateSpathi_generateOrbital (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world);


const GenerateFunctions generateSpathiFunctions = {
	/* .initNpcs         = */ GenerateDefault_initNpcs,
	/* .reinitNpcs       = */ GenerateDefault_reinitNpcs,
	/* .uninitNpcs       = */ GenerateDefault_uninitNpcs,
	/* .generatePlanets  = */ GenerateSpathi_generatePlanets,
	/* .generateMoons    = */ GenerateSpathi_generateMoons,
	/* .generateName     = */ GenerateDefault_generateName,
	/* .generateOrbital  = */ GenerateSpathi_generateOrbital,
	/* .generateMinerals = */ GenerateDefault_generateMinerals,
	/* .generateEnergy   = */ GenerateDefault_generateEnergy,
	/* .generateLife     = */ GenerateDefault_generateLife,
};


static bool
GenerateSpathi_generatePlanets (SOLARSYS_STATE *solarSys)
{
	PLANET_DESC *pMinPlanet;
	COUNT angle;

	pMinPlanet = &solarSys->PlanetDesc[0];
	solarSys->SunDesc[0].NumPlanets = 1;
	FillOrbits (solarSys,
			solarSys->SunDesc[0].NumPlanets, pMinPlanet, FALSE);

	pMinPlanet->radius = EARTH_RADIUS * 1150L / 100;
	angle = ARCTAN (pMinPlanet->location.x, pMinPlanet->location.y);
	pMinPlanet->location.x = COSINE (angle, pMinPlanet->radius);
	pMinPlanet->location.y = SINE (angle, pMinPlanet->radius);
	pMinPlanet->data_index = WATER_WORLD | PLANET_SHIELDED;
	pMinPlanet->NumPlanets = 2;

	return true;
}

static bool
GenerateSpathi_generateMoons (SOLARSYS_STATE *solarSys, PLANET_DESC *planet)
{
	COUNT angle;

	GenerateDefault_generateMoons (solarSys, planet);

	if (matchWorld (solarSys, planet, 0, MATCH_PLANET))
	{

/*Spathi moon*/


		solarSys->MoonDesc[0].data_index = PELLUCID_WORLD;
		solarSys->MoonDesc[0].radius = MIN_MOON_RADIUS + MOON_DELTA;
		angle = 58;
		solarSys->MoonDesc[0].location.x =
				COSINE (angle, solarSys->MoonDesc[0].radius);
		solarSys->MoonDesc[0].location.y =
				SINE (angle, solarSys->MoonDesc[0].radius);
		/*Spathi Starbase*/
		pSolarSysState->MoonDesc[1].data_index = (BYTE)~0;
		pSolarSysState->MoonDesc[1].radius = MIN_MOON_RADIUS;
		angle = HALF_CIRCLE + QUADRANT;
		pSolarSysState->MoonDesc[1].location.x =
			COSINE (angle, pSolarSysState->MoonDesc[1].radius);
		pSolarSysState->MoonDesc[1].location.y =
			SINE (angle, pSolarSysState->MoonDesc[1].radius);
	}

	return true;
}

static bool
GenerateSpathi_generateOrbital (SOLARSYS_STATE *solarSys, PLANET_DESC *world)
{
	if (matchWorld (solarSys, world, 0, 1))
	{
		InitCommunication (SPATHI_CONVERSATION);		/* Spathiwa's moon */

		return true;
	}

	GenerateDefault_generateOrbital (solarSys, world);

	return true;
}
