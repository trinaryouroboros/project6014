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

// JMS_GFX 2011: Merged the resolution Factor stuff from UQM-HD.

#include "../globdata.h"
#include "lifeform.h"
#include "planets.h"
#include "libs/mathlib.h"
#include "libs/log.h"


//#define DEBUG_SURFACE

const BYTE *Elements;
const PlanetFrame *PlanData;

static COUNT
CalcMineralDeposits (SYSTEM_INFO *SysInfoPtr, COUNT which_deposit)
{
	BYTE j;
	COUNT num_deposits;
	const ELEMENT_ENTRY *eptr;

	eptr = &SysInfoPtr->PlanetInfo.PlanDataPtr->UsefulElements[0];
	num_deposits = 0;
	j = NUM_USEFUL_ELEMENTS;
	do
	{
		BYTE num_possible;

		num_possible = (BYTE)((BYTE)TFB_Random ()
				% (DEPOSIT_QUANTITY (eptr->Density) + 1));
		while (num_possible--)
		{
#define MEDIUM_DEPOSIT_THRESHOLD 150
#define LARGE_DEPOSIT_THRESHOLD 225
			DWORD rand_val;
			UWORD loword, hiword;
			COUNT deposit_quality_fine,
						deposit_quality_gross;

			deposit_quality_fine = ((COUNT)TFB_Random () % 100)
			  + (
			     DEPOSIT_QUALITY (eptr->Density)
			     + SysInfoPtr->StarSize
			     ) * 50;

			// BW: Check whether we're in the NE quadrant
			if ((GLOBAL_SIS (log_x) > UNIVERSE_TO_LOGX(5000)) && (GLOBAL_SIS (log_y) < UNIVERSE_TO_LOGY(6000)))
			  {
			    deposit_quality_fine = (COUNT)(deposit_quality_fine * DEPLETION_RATE);
			  }
			
			if (deposit_quality_fine < MEDIUM_DEPOSIT_THRESHOLD)
			  deposit_quality_gross = 0;
			else if (deposit_quality_fine < LARGE_DEPOSIT_THRESHOLD)
			  deposit_quality_gross = 1;
			else
			  deposit_quality_gross = 2;
			
			rand_val = TFB_Random ();
			loword = LOWORD (rand_val);
			hiword = HIWORD (rand_val);
			
			if (RESOLUTION_FACTOR == 0)
				SysInfoPtr->PlanetInfo.CurPt.x = (LOBYTE (loword) % (MAP_WIDTH - (8 << 1))) + 8;
			else
				SysInfoPtr->PlanetInfo.CurPt.x = loword % (MAP_WIDTH - (8 << 1)) + 8; // JMS_GFX: Replaced previous line with this line (BYTE was too small for 640x480 maps.)
			
			if (RESOLUTION_FACTOR == 0)
				SysInfoPtr->PlanetInfo.CurPt.y = (HIBYTE (loword) % (MAP_HEIGHT - (8 << 1))) + 8;
			else
				SysInfoPtr->PlanetInfo.CurPt.y = hiword % (MAP_HEIGHT - (8 << 1)) + 8;  // JMS_GFX: Replaced previous line with this line (BYTE was too small for 1280x960 maps.)
			
			SysInfoPtr->PlanetInfo.CurDensity = MAKE_WORD (deposit_quality_gross, deposit_quality_fine / 10 + 1);
			SysInfoPtr->PlanetInfo.CurType = eptr->ElementType;
#ifdef DEBUG_SURFACE
			log_add (log_Debug, "\t\t%d units of %Fs",
					SysInfoPtr->PlanetInfo.CurDensity,
					Elements[eptr->ElementType].name);
#endif /* DEBUG_SURFACE */
			if ((num_deposits >= which_deposit && !(SysInfoPtr->PlanetInfo.ScanRetrieveMask[MINERAL_SCAN] & (1L << num_deposits)))
					|| ++num_deposits == sizeof (DWORD) * 8)
				goto ExitCalcMinerals;
		}
		++eptr;
	} while (--j);

ExitCalcMinerals:
	return (num_deposits);
}

DWORD
GenerateMineralDeposits (SYSTEM_INFO *SysInfoPtr, COUNT *pwhich_deposit)
{
	DWORD old_rand;

	old_rand = TFB_SeedRandom (SysInfoPtr->PlanetInfo.ScanSeed[MINERAL_SCAN]);
	*pwhich_deposit = CalcMineralDeposits (SysInfoPtr, *pwhich_deposit);
	return (TFB_SeedRandom (old_rand));
}

static COUNT
CalcLifeForms (SYSTEM_INFO *SysInfoPtr, COUNT which_life)
{
	COUNT num_life_forms;

	num_life_forms = 0;
	if (PLANSIZE (SysInfoPtr->PlanetInfo.PlanDataPtr->Type) == GAS_GIANT)
		SysInfoPtr->PlanetInfo.LifeChance = -1;
	else
	{
#define MIN_LIFE_CHANCE 10
		SIZE life_var;

		life_var = 0;

		if (SysInfoPtr->PlanetInfo.SurfaceTemperature < -151)
			life_var -= 300;
		else if (SysInfoPtr->PlanetInfo.SurfaceTemperature < -51)
			life_var -= 100;
		else if (SysInfoPtr->PlanetInfo.SurfaceTemperature < 0)
			life_var += 100;
		else if (SysInfoPtr->PlanetInfo.SurfaceTemperature < 50)
			life_var += 300;
		else if (SysInfoPtr->PlanetInfo.SurfaceTemperature < 150)
			life_var += 50;
		else if (SysInfoPtr->PlanetInfo.SurfaceTemperature < 250)
			life_var -= 100;
		else if (SysInfoPtr->PlanetInfo.SurfaceTemperature < 500)
			life_var -= 400;
		else
			life_var -= 800;

		if (SysInfoPtr->PlanetInfo.AtmoDensity == 0)
			life_var -= 1000;
		else if (SysInfoPtr->PlanetInfo.AtmoDensity < 15)
			life_var += 100;
		else if (SysInfoPtr->PlanetInfo.AtmoDensity < 30)
			life_var += 200;
		else if (SysInfoPtr->PlanetInfo.AtmoDensity < 100)
			life_var += 300;
		else if (SysInfoPtr->PlanetInfo.AtmoDensity < 1000)
			life_var += 150;
		else if (SysInfoPtr->PlanetInfo.AtmoDensity < 2500)
			;
		else
			life_var -= 100;

#ifndef NOTYET
		life_var += 200 + 80 + 80;
#else /* NOTYET */
		if (SysInfoPtr->PlanetInfo.SurfaceGravity < 10)
			;
		else if (SysInfoPtr->PlanetInfo.SurfaceGravity < 35)
			life_var += 50;
		else if (SysInfoPtr->PlanetInfo.SurfaceGravity < 75)
			life_var += 100;
		else if (SysInfoPtr->PlanetInfo.SurfaceGravity < 150)
			life_var += 200;
		else if (SysInfoPtr->PlanetInfo.SurfaceGravity < 400)
			life_var += 50;
		else if (SysInfoPtr->PlanetInfo.SurfaceGravity < 800)
			;
		else
			life_var -= 100;

		if (SysInfoPtr->PlanetInfo.Tectonics < 1)
			life_var += 80;
		else if (SysInfoPtr->PlanetInfo.Tectonics < 2)
			life_var += 70;
		else if (SysInfoPtr->PlanetInfo.Tectonics < 3)
			life_var += 60;
		else if (SysInfoPtr->PlanetInfo.Tectonics < 4)
			life_var += 50;
		else if (SysInfoPtr->PlanetInfo.Tectonics < 5)
			life_var += 25;
		else if (SysInfoPtr->PlanetInfo.Tectonics < 6)
			;
		else
			life_var -= 100;

		if (SysInfoPtr->PlanetInfo.Weather < 1)
			life_var += 80;
		else if (SysInfoPtr->PlanetInfo.Weather < 2)
			life_var += 70;
		else if (SysInfoPtr->PlanetInfo.Weather < 3)
			life_var += 60;
		else if (SysInfoPtr->PlanetInfo.Weather < 4)
			life_var += 50;
		else if (SysInfoPtr->PlanetInfo.Weather < 5)
			life_var += 25;
		else if (SysInfoPtr->PlanetInfo.Weather < 6)
			;
		else
			life_var -= 100;
#endif /* NOTYET */

		SysInfoPtr->PlanetInfo.LifeChance = life_var;

		life_var = (COUNT)TFB_Random () & 1023;
		if (life_var < SysInfoPtr->PlanetInfo.LifeChance
				|| (SysInfoPtr->PlanetInfo.LifeChance < MIN_LIFE_CHANCE
				&& life_var < MIN_LIFE_CHANCE))
		{
			BYTE num_types;

			num_types = (BYTE)(((BYTE)TFB_Random () % MAX_LIFE_VARIATION) + 1);
			do
			{
				BYTE index, num_creatures, range_types;
				DWORD rand_val; // JMS_GFX: Was UWORD
				UWORD loword, hiword;
				BOOLEAN zoneA, zoneB, zoneC;

				rand_val = (UWORD)TFB_Random ();
				
				// BW: Compute which life forms should appear				
				zoneA = (LOGX_TO_UNIVERSE(GLOBAL_SIS (log_x)) + LOGY_TO_UNIVERSE(GLOBAL_SIS (log_y)) > 9000);
				zoneB = (LOGX_TO_UNIVERSE(GLOBAL_SIS (log_x)) + 3 * LOGY_TO_UNIVERSE(GLOBAL_SIS (log_y)) < 21000);
				zoneC = (3 * LOGX_TO_UNIVERSE(GLOBAL_SIS (log_x)) + LOGY_TO_UNIVERSE(GLOBAL_SIS (log_y)) < 19000);
				
				range_types = 0;
				if (zoneA)
				  range_types += NUM_CREATURE_TYPES;
				if (zoneB)
				  range_types += NUM_B_CREATURE_TYPES;
				if (zoneC)
				  range_types += NUM_C_CREATURE_TYPES;

				//index = LOBYTE (rand_val) % range_types;
				index = LOBYTE ((UWORD)rand_val) % range_types; // JMS_GFX
				
				// BW: adjust index so that it takes creatures from the correct set.
				if (!zoneA)
				  {
				    index += NUM_CREATURE_TYPES + NUM_SPECIAL_CREATURE_TYPES;
				    if (!zoneB)
				      index += NUM_B_CREATURE_TYPES;
				  }
				if (zoneA && index >= NUM_CREATURE_TYPES)
				  {
				    index += NUM_SPECIAL_CREATURE_TYPES;
				    if (!zoneB)
				      index += NUM_B_CREATURE_TYPES;
				  }
	
				// BW: Reduce amounts in the NE quadrant
				if ((GLOBAL_SIS (log_x) > UNIVERSE_TO_LOGX(5000)) && (GLOBAL_SIS (log_y) < UNIVERSE_TO_LOGY(6000)))
				  num_creatures = (BYTE)((HIBYTE ((UWORD)rand_val) % 3) + 1);
				else
				  num_creatures = (BYTE)((HIBYTE ((UWORD)rand_val) % 10) + 1);

				do
				{
					rand_val = (DWORD)TFB_Random ();
					loword = LOWORD (rand_val);
					hiword = HIWORD (rand_val);
					
					/*
					if (RESOLUTION_FACTOR == 0)
						SysInfoPtr->PlanetInfo.CurPt.x = (LOBYTE (rand_val) % (MAP_WIDTH - (8 << 1))) + 8;
					else
						SysInfoPtr->PlanetInfo.CurPt.x = rand_val % (MAP_WIDTH - (8 << 1)) + 8; // JMS_GFX: Replaced previous line with this line (BYTE was too small for 640x480 maps.)
					
					SysInfoPtr->PlanetInfo.CurPt.y = (HIBYTE (rand_val) % (MAP_HEIGHT - (8 << 1))) + 8; // JMS_GFX
					*/
					
					if (RESOLUTION_FACTOR == 0)
						SysInfoPtr->PlanetInfo.CurPt.x = (LOBYTE ((UWORD)rand_val) % (MAP_WIDTH - (8 << 1))) + 8;
					else
						SysInfoPtr->PlanetInfo.CurPt.x = loword % (MAP_WIDTH - (8 << 1)) + 8; // JMS_GFX: Replaced previous line with this line (BYTE was too small for 640x480 maps.)
					
					if (RESOLUTION_FACTOR == 0)
						SysInfoPtr->PlanetInfo.CurPt.y = (HIBYTE ((UWORD)rand_val) % (MAP_HEIGHT - (8 << 1))) + 8;
					else
						SysInfoPtr->PlanetInfo.CurPt.y = hiword % (MAP_HEIGHT - (8 << 1)) + 8;  // JMS_GFX: Replaced previous line with this line (BYTE was too small for 1280x960 maps.)
					
					SysInfoPtr->PlanetInfo.CurType = index;

					if ((num_life_forms >= which_life 
						&& !(SysInfoPtr->PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN] & (1L << num_life_forms)))
						|| ++num_life_forms == sizeof (DWORD) * 8)
					{
						num_types = 1;
						break;
					}
				} while (--num_creatures);
			} while (--num_types);
		}
#ifdef DEBUG_SURFACE
		else
			log_add (log_Debug, "It's dead, Jim! (%d >= %d)", life_var,
				SysInfoPtr->PlanetInfo.LifeChance);
#endif /* DEBUG_SURFACE */
	}

	return (num_life_forms);
}

DWORD
GenerateLifeForms (SYSTEM_INFO *SysInfoPtr, COUNT *pwhich_life)
{
	DWORD old_rand;

	old_rand = TFB_SeedRandom (SysInfoPtr->PlanetInfo.ScanSeed[BIOLOGICAL_SCAN]);
	*pwhich_life = CalcLifeForms (SysInfoPtr, *pwhich_life);
	return (TFB_SeedRandom (old_rand));
}
