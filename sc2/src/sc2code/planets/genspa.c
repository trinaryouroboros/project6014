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

#include "build.h"
#include "encount.h"
#include "globdata.h"
#include "ipdisp.h"
#include "lifeform.h"
#include "nameref.h"
#include "resinst.h"
#include "state.h"
#include "planets/genall.h"
#include "libs/mathlib.h"


void
GenerateSpathi (BYTE control)
{
	COUNT angle;

	switch (control)
	{
		case GENERATE_MOONS:
			GenerateRandomIP (GENERATE_MOONS);
			if (pSolarSysState->pBaseDesc == &pSolarSysState->PlanetDesc[0])
			{


/*Spathi moon*/

				pSolarSysState->MoonDesc[0].data_index = PELLUCID_WORLD;
				pSolarSysState->MoonDesc[0].radius = MIN_MOON_RADIUS + MOON_DELTA;
				angle = 58;
				pSolarSysState->MoonDesc[0].location.x =
						COSINE (angle, pSolarSysState->MoonDesc[0].radius);
				pSolarSysState->MoonDesc[0].location.y =
						SINE (angle, pSolarSysState->MoonDesc[0].radius);
			
/*Spathi Starbase*/
				pSolarSysState->MoonDesc[1].data_index = (BYTE)~0;
				pSolarSysState->MoonDesc[1].radius = MIN_MOON_RADIUS;
				angle = HALF_CIRCLE + QUADRANT;
				pSolarSysState->MoonDesc[1].location.x =
						COSINE (angle, pSolarSysState->MoonDesc[1].radius);
				pSolarSysState->MoonDesc[1].location.y =
						SINE (angle, pSolarSysState->MoonDesc[1].radius);

			}
			break;
		case GENERATE_PLANETS:
		{
			PLANET_DESC *pMinPlanet;

			pMinPlanet = &pSolarSysState->PlanetDesc[0];
			pSolarSysState->SunDesc[0].NumPlanets = 1;
			FillOrbits (pSolarSysState,
					pSolarSysState->SunDesc[0].NumPlanets, pMinPlanet, FALSE);

			pMinPlanet->radius = EARTH_RADIUS * 1150L / 100;
			angle = ARCTAN (pMinPlanet->location.x, pMinPlanet->location.y);
			pMinPlanet->location.x =
					COSINE (angle, pMinPlanet->radius);
			pMinPlanet->location.y =
					SINE (angle, pMinPlanet->radius);
			pMinPlanet->data_index = WATER_WORLD | PLANET_SHIELDED;
			pMinPlanet->NumPlanets = 2;
			break;
		}
		case GENERATE_ORBITAL:
			if (pSolarSysState->pOrbitalDesc->pPrevDesc == &pSolarSysState->PlanetDesc[0]
					&& pSolarSysState->pOrbitalDesc == &pSolarSysState->MoonDesc[1])
			{
				pSolarSysState->MenuState.Initialized += 2;
				InitCommunication (SPATHI_CONVERSATION);
				pSolarSysState->MenuState.Initialized -= 2;
				break;
			}
			else
			{
				GenerateRandomIP (GENERATE_ORBITAL);
			}
		default:
			GenerateRandomIP (control);
			break;
	}
}


