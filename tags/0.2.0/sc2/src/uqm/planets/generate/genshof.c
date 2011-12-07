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

// JMS 2010: Removed all Tanaka/Katana related crap

#include "genall.h"
#include "../../build.h"
#include "../../globdata.h"
#include "../../grpinfo.h"
#include "../../state.h"
#include "../planets.h"


static bool GenerateShofixti_generatePlanets (SOLARSYS_STATE *solarSys);


const GenerateFunctions generateShofixtiFunctions = {
	/* .initNpcs         = */ GenerateDefault_initNpcs,
	/* .reinitNpcs       = */ GenerateDefault_reinitNpcs,
	/* .uninitNpcs       = */ GenerateDefault_uninitNpcs,
	/* .generatePlanets  = */ GenerateShofixti_generatePlanets,
	/* .generateMoons    = */ GenerateDefault_generateMoons,
	/* .generateName     = */ GenerateDefault_generateName,
	/* .generateOrbital  = */ GenerateDefault_generateOrbital,
	/* .generateMinerals = */ GenerateDefault_generateMinerals,
	/* .generateEnergy   = */ GenerateDefault_generateEnergy,
	/* .generateLife     = */ GenerateDefault_generateLife,
};


static bool
GenerateShofixti_generatePlanets (SOLARSYS_STATE *solarSys)
{
	COUNT i;

#define NUM_PLANETS 6
	solarSys->SunDesc[0].NumPlanets = NUM_PLANETS;
	for (i = 0; i < NUM_PLANETS; ++i)
	{
		PLANET_DESC *pCurDesc = &solarSys->PlanetDesc[i];

		pCurDesc->NumPlanets = 0;
		if (i < (NUM_PLANETS >> 1))
			pCurDesc->data_index = SELENIC_WORLD;
		else
			pCurDesc->data_index = METAL_WORLD;
	}

	FillOrbits (solarSys, NUM_PLANETS, solarSys->PlanetDesc, TRUE);

	return true;
}
