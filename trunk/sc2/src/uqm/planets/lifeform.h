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

#ifndef _LIFEFORM_H
#define _LIFEFORM_H

#include "libs/compiler.h"


#define BEHAVIOR_HUNT (0 << 0)
#define BEHAVIOR_FLEE (1 << 0)
#define BEHAVIOR_UNPREDICTABLE (2 << 0)

#define BEHAVIOR_MASK 0x03
#define BEHAVIOR_SHIFT 0

#define AWARENESS_LOW (0 << 2)
#define AWARENESS_MEDIUM (1 << 2)
#define AWARENESS_HIGH (2 << 2)

#define AWARENESS_MASK 0x0C
#define AWARENESS_SHIFT (BEHAVIOR_SHIFT + 2)

#define SPEED_MOTIONLESS (0 << 4)
#define SPEED_SLOW (1 << 4)
#define SPEED_MEDIUM (2 << 4)
#define SPEED_FAST (3 << 4)

#define SPEED_MASK 0x30
#define SPEED_SHIFT (AWARENESS_SHIFT + 2)

#define DANGER_HARMLESS (0 << 6)
#define DANGER_WEAK (1 << 6)
#define DANGER_NORMAL (2 << 6)
#define DANGER_MONSTROUS (3 << 6)

#define DANGER_MASK 0xC0
#define DANGER_SHIFT (SPEED_SHIFT + 2)

#define NUM_CREATURE_TYPES 23
#define NUM_SPECIAL_CREATURE_TYPES 3
#define NUM_B_CREATURE_TYPES 13 // BW: creatures appearing in the southern region
#define NUM_C_CREATURE_TYPES 13 // BW: creatures appearing in the western region
#define MAX_LIFE_VARIATION 3

#define CREATURE_AWARE (BYTE)(1 << 7)

// JMS: The SpecialAttributes of critters:
#define NONE 0
#define SHOOTS_LASER (1 << 0)
#define SHOOTS_LIMPET (1 << 1)
#define WHEN_DYING_EXPLODES (1 << 2)
#define WHEN_DYING_DIVIDES (1 << 3)
#define INVULNERABLE_PART_TIME (1 << 4)
#define INVULNERABLE_TO_BASIC_WEAPON (1 << 5)

#define SHOOTING_SPECIALS 0x03 // binary 11
#define WHEN_DYING_SPECIALS 0x0C // binary 1100
#define INVULNERABILITY_SPECIALS 0x30 // binary 110000

#define LIMPET_ACCEL (WORLD_TO_VELOCITY(1 << RESOLUTION_FACTOR))		// JMS_GFX
#define LIMPET_MAX_SPEED (WORLD_TO_VELOCITY(3 << RESOLUTION_FACTOR))	// JMS_GFX
#define LIMPET_LIFESPAN 50
#define MAX_LIMPETS_LO_SPEED 16 // JMS: Max number of limpets that can stick on the lander
#define MAX_LIMPETS_HI_SPEED 28 // JMS: Max number of limpets that can stick on the lander

#define CRITTER_EXPLOSION_PERCENT 60 // JMS: This concerns only WHEN_DYING_EXPLODES critters.

#define DIVIDED_CRITTER_NUMBER 3
#define DIVIDED_CRITTER_LIFESPAN 1

typedef struct
{
	BYTE Attributes, ValueAndHitPoints;
        COUNT FrameRate, SpecialAttributes;
} LIFEFORM_DESC;

// BW: FrameRate is an integer between 0 and 15
// where (FrameRate+1)/32 is (roughly ?) the number of seconds
// between two frames when animating the lifeform.
// This should be defined with ONE_SECOND to comply with the other animations
//
// JMS: SpecialAttributes is a 16-bit bitmask which houses all the new
// skills the creatures might have: shooting back, exploding etc.

extern const LIFEFORM_DESC CreatureData[];

#endif /* _LIFEFORM_H */

