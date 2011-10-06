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

static bool GenerateHint2_generateOrbital (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world);
static bool GenerateHint2_generateEnergy (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world, COUNT *whichNode);


const GenerateFunctions generateHint2Functions = {
	/* .initNpcs         = */ GenerateDefault_initNpcs,
	/* .reinitNpcs       = */ GenerateDefault_reinitNpcs,
	/* .uninitNpcs       = */ GenerateDefault_uninitNpcs,
	/* .generatePlanets  = */ GenerateDefault_generatePlanets,
	/* .generateMoons    = */ GenerateDefault_generateMoons,
	/* .generateName     = */ GenerateDefault_generateName,
	/* .generateOrbital  = */ GenerateHint2_generateOrbital,
	/* .generateMinerals = */ GenerateDefault_generateMinerals,
	/* .generateEnergy   = */ GenerateHint2_generateEnergy,
	/* .generateLife     = */ GenerateDefault_generateLife,
};


static bool
GenerateHint2_generateOrbital (SOLARSYS_STATE *solarSys, PLANET_DESC *world)
{
	COUNT i;
	DWORD rand_val;

	rand_val = DoPlanetaryAnalysis (&solarSys->SysInfo, solarSys->pOrbitalDesc);
	
	solarSys->SysInfo.PlanetInfo.ScanSeed[BIOLOGICAL_SCAN] = rand_val;
	i = (COUNT)~0;
	rand_val = GenerateLifeForms (&solarSys->SysInfo, &i);
	
	solarSys->SysInfo.PlanetInfo.ScanSeed[MINERAL_SCAN] = rand_val;
	i = (COUNT)~0;
	GenerateMineralDeposits (&solarSys->SysInfo, &i);
	
	solarSys->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN] = rand_val;
	if (matchWorld(solarSys, world, 1, MATCH_PLANET))
	{
		LoadStdLanderFont (&solarSys->SysInfo.PlanetInfo);
		solarSys->PlanetSideFrame[1] = CaptureDrawable (LoadGraphic (CIRCLES_C_MASK_PMAP_ANIM));
		solarSys->SysInfo.PlanetInfo.DiscoveryString = CaptureStringTable (LoadStringTable (GAMMAJANUS_STRTAB));
	}
	
	if (GET_GAME_STATE(YEHAT_PRECURSOR_ARTIFACT) > 0)
	{
		solarSys->SysInfo.PlanetInfo.DiscoveryString =
			SetRelStringTableIndex (solarSys->SysInfo.PlanetInfo.DiscoveryString,1);
	}
	
	if (GET_GAME_STATE(YEHAT_PRECURSOR_ARTIFACT) < 2)
		SET_GAME_STATE(YEHAT_PRECURSOR_ARTIFACT, 2);
	
	LoadPlanet (NULL);
	return true;
}


static bool
GenerateHint2_generateEnergy (SOLARSYS_STATE *solarSys,
			      PLANET_DESC *world, COUNT *whichNode)
{
	DWORD rand_val, old_rand;
	
	if (matchWorld(solarSys, world, 1, MATCH_PLANET))
	{
		old_rand = TFB_SeedRandom (solarSys->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN]);
		
		rand_val = TFB_Random ();
		solarSys->SysInfo.PlanetInfo.CurPt.x = (LOBYTE (LOWORD (rand_val)) % (MAP_WIDTH - (8 << 1))) + 8;
		solarSys->SysInfo.PlanetInfo.CurPt.y = (HIBYTE (LOWORD (rand_val)) % (MAP_HEIGHT - (8 << 1))) + 8;
		solarSys->SysInfo.PlanetInfo.CurType = 1;
		solarSys->SysInfo.PlanetInfo.CurDensity = 0;
		
		if (!(solarSys->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN] & (1L << 0)) && solarSys->CurNode == (COUNT)~0)
			solarSys->CurNode = 1;
		else
			solarSys->CurNode = 0;
		
		TFB_SeedRandom (old_rand);
		return true;
	}
	
	solarSys->CurNode = 0;
	return true;
}
