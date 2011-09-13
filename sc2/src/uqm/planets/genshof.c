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

#include "build.h"
#include "globdata.h"
#include "state.h"
#include "grpinfo.h"
#include "planets/genall.h"


void
GenerateShofixti (BYTE control)
{
	switch (control)
	{
		case GENERATE_PLANETS:
		{
			COUNT i;
			PLANET_DESC *pCurDesc;

#define NUM_PLANETS 6
			pSolarSysState->SunDesc[0].NumPlanets = NUM_PLANETS;
			for (i = 0, pCurDesc = pSolarSysState->PlanetDesc;
					i < NUM_PLANETS; ++i, ++pCurDesc)
			{
				pCurDesc->NumPlanets = 0;
				if (i < (NUM_PLANETS >> 1))
					pCurDesc->data_index = SELENIC_WORLD;
				else
					pCurDesc->data_index = METAL_WORLD;
			}

			FillOrbits (pSolarSysState,
					NUM_PLANETS, &pSolarSysState->PlanetDesc[0], TRUE);
			break;
		}
		default:
			GenerateRandomIP (control);
			break;
	}
}


