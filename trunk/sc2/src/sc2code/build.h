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

// JMS_GFX 2011: Merged resolution Factor stuff from UQM-HD.

#ifndef _BUILD_H
#define _BUILD_H

#include "races.h"
#include "displist.h"
#include "libs/compiler.h"

#define CREW_XOFFS RES_STAT_SCALE(4) // JMS_GFX
#define ENERGY_XOFFS RES_STAT_SCALE(52) // JMS_GFX
#define GAUGE_YOFFS (SHIP_INFO_HEIGHT - (10 << RESOLUTION_FACTOR)) // JMS_GFX
#define UNIT_WIDTH RES_STAT_SCALE(2)
#define UNIT_HEIGHT 1
#define STAT_WIDTH (1 + UNIT_WIDTH + 1 + UNIT_WIDTH + 1) // JMS_GFX

#define NAME_OFFSET 5
#define NUM_CAPTAINS_NAMES 16

#define PickCaptainName() (((COUNT)TFB_Random () \
								& (NUM_CAPTAINS_NAMES - 1)) \
								+ NAME_OFFSET)

extern HLINK Build (QUEUE *pQueue, SPECIES_ID SpeciesID);
extern HSHIPFRAG CloneShipFragment (COUNT shipIndex, QUEUE *pDstQueue,
		COUNT crew_level);
extern HLINK GetStarShipFromIndex (QUEUE *pShipQ, COUNT Index);
extern BYTE NameCaptain (QUEUE *pQueue, SPECIES_ID SpeciesID);

/* Possible values for the 'state' argument of
 * COUNT ActivateStarShip (COUNT which_ship, SIZE state)
 * See that function for more information.
 */
#define SET_ALLIED          0
#define SET_NOT_ALLIED     -1
#define CHECK_ALLIANCE     -2
#define REMOVE_BUILT       -3
#define ESCORT_WORTH       -4
#define SPHERE_KNOWN       -5
#define SPHERE_TRACKING    -6
#define ESCORTING_FLAGSHIP -7
#define FEASIBILITY_STUDY  -8
		/* Check for how many escort ships there's still space */

extern COUNT ActivateStarShip (COUNT which_ship, SIZE state);
extern COUNT GetIndexFromStarShip (QUEUE *pShipQ, HLINK hStarShip);
extern int SetEscortCrewComplement (COUNT which_ship, COUNT crew_level,
		BYTE captain);

extern RACE_DESC *load_ship (SPECIES_ID SpeciesID, BOOLEAN LoadBattleData);
extern void free_ship (RACE_DESC *RaceDescPtr, BOOLEAN FreeIconData,
		BOOLEAN FreeBattleData);

extern void DrawCrewFuelString (COORD y, SIZE state);
extern void ClearShipStatus (COORD y);
extern void OutlineShipStatus (COORD y);
extern void InitShipStatus (SHIP_INFO *ShipInfoPtr, BYTE captains_name_index,
		RECT *pClipRect);
extern void DeltaStatistics (SHIP_INFO *ShipInfoPtr, SIZE crew_delta,
		SIZE energy_delta);
extern void DrawBattleCrewAmount (SHIP_INFO *ShipInfoPtr);

#endif /* _BUILD_H */
