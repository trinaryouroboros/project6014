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

// JMS 2010:- Defined procyon system coords to PROCYON_X and PROCYON_Y for use in game start location, in globdata.c

// JMS_GFX 2011: Merged the resolution Factor stuff from UQM-HD.

#include "units.h"
// for UNIT_SCREEN_WIDTH/HEIGHT

#ifndef _HYPER_H
#define _HYPER_H

#include "element.h"
#define RADAR_SCAN_WIDTH ((UNIT_SCREEN_WIDTH * NUM_RADAR_SCREENS) >> RESOLUTION_FACTOR)
#define RADAR_SCAN_HEIGHT ((UNIT_SCREEN_HEIGHT * NUM_RADAR_SCREENS) >> RESOLUTION_FACTOR)

// Hyperspace coordinates of the naturally occuring portal into QuasiSpace
#define ARILOU_SPACE_X   438
#define ARILOU_SPACE_Y  6372

// QuasiSpace coordinates of the same portal
#define QUASI_SPACE_X  5000
#define QUASI_SPACE_Y  5000

// QuasiSpace coordinates of the Arilou home world
#define ARILOU_HOME_X  (QUASI_SPACE_X + ((RADAR_SCAN_WIDTH >> 1) * 3))
#define ARILOU_HOME_Y  (QUASI_SPACE_Y + ((RADAR_SCAN_HEIGHT >> 1) * 3))

// Hyperspace coordinates of the Sol system
// Should be the same as in plandata.c
#define SOL_X  6752
#define SOL_Y  7450

// JMS: Hyperspace coordinates of the Procyon system
// Should be the same as in plandata.c
#define PROCYON_X  5742
#define PROCYON_Y  8268

extern void FreeHyperData (void);
extern void check_hyperspace_encounter (void);
extern BOOLEAN hyper_transition (ELEMENT *ElementPtr);

#endif /* _HYPER_H */

