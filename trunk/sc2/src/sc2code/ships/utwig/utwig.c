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

#include "ships/ship.h"
#include "ships/utwig/resinst.h"

#include "globdata.h"
#include "libs/mathlib.h"


#define MAX_CREW 20
#define MAX_ENERGY 20
#define ENERGY_REGENERATION 0
#define WEAPON_ENERGY_COST 0
#define SPECIAL_ENERGY_COST 1
#define ENERGY_WAIT 255
#define MAX_THRUST 36
#define THRUST_INCREMENT 6
#define TURN_WAIT 1
#define THRUST_WAIT 6
#define WEAPON_WAIT 7
#define SPECIAL_WAIT 12

#define SHIP_MASS 8
#define UTWIG_OFFSET 9
#define MISSILE_SPEED DISPLAY_TO_WORLD (30)
#define MISSILE_LIFE 10

static RACE_DESC utwig_desc =
{
	{ /* SHIP_INFO */
		FIRES_FORE | SHIELD_DEFENSE | LIGHT_POINT_DEFENSE,
		22, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY >> 1, MAX_ENERGY,
		UTWIG_RACE_STRINGS,
		UTWIG_ICON_MASK_PMAP_ANIM,
		UTWIG_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		0, /* Initial SoI radius */
		{ /* Known location (center of SoI) */
			0,0,
		},
	},
	{
		MAX_THRUST,
		THRUST_INCREMENT,
		ENERGY_REGENERATION,
		WEAPON_ENERGY_COST,
		SPECIAL_ENERGY_COST,
		ENERGY_WAIT,
		TURN_WAIT,
		THRUST_WAIT,
		WEAPON_WAIT,
		SPECIAL_WAIT,
		SHIP_MASS,
	},
	{
		{
			UTWIG_BIG_MASK_PMAP_ANIM,
			UTWIG_MED_MASK_PMAP_ANIM,
			UTWIG_SML_MASK_PMAP_ANIM,
		},
		{
			LANCE_BIG_MASK_PMAP_ANIM,
			LANCE_MED_MASK_PMAP_ANIM,
			LANCE_SML_MASK_PMAP_ANIM,
		},
		{
			NULL_RESOURCE,
			NULL_RESOURCE,
			NULL_RESOURCE,
		},
		{
			UTWIG_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		UTWIG_VICTORY_SONG,
		UTWIG_SHIP_SOUNDS,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		CLOSE_RANGE_WEAPON,
		NULL,
	},
	(UNINIT_FUNC *) NULL,
	(PREPROCESS_FUNC *) NULL,
	(POSTPROCESS_FUNC *) NULL,
	(INIT_WEAPON_FUNC *) NULL,
	0,
};

static COUNT
initialize_lance (ELEMENT *ShipPtr, HELEMENT WeaponArray[])
{
#define LAUNCH_XOFFS0 DISPLAY_TO_WORLD (5)
#define LAUNCH_YOFFS0 -DISPLAY_TO_WORLD (18)
#define LAUNCH_XOFFS1 DISPLAY_TO_WORLD (13)
#define LAUNCH_YOFFS1 -DISPLAY_TO_WORLD (9)
#define LAUNCH_XOFFS2 DISPLAY_TO_WORLD (17)
#define LAUNCH_YOFFS2 -DISPLAY_TO_WORLD (4)
#define MISSILE_HITS 1
#define MISSILE_DAMAGE 1
#define MISSILE_OFFSET 1
	COUNT i;
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.face = MissileBlock.index = StarShipPtr->ShipFacing;
	MissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
			| IGNORE_SIMILAR;
	MissileBlock.speed = MISSILE_SPEED;
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = NULL;
	MissileBlock.blast_offs = MISSILE_OFFSET;
	MissileBlock.pixoffs = 0;

	for (i = 0; i < 3; ++i)
	{
		COUNT angle;
		SIZE sin0, cos0;
		SIZE sin1sin0, cos1sin0, cos1cos0, sin1cos0;

		switch (i)
		{
			case 0:
				cos0 = LAUNCH_XOFFS0;
				sin0 = LAUNCH_YOFFS0;
				break;
			case 1:
				cos0 = LAUNCH_XOFFS1;
				sin0 = LAUNCH_YOFFS1;
				break;
			case 2:
				cos0 = LAUNCH_XOFFS2;
				sin0 = LAUNCH_YOFFS2;
				break;
		}
		angle = FACING_TO_ANGLE (MissileBlock.face) + QUADRANT;
		cos1cos0 = COSINE (angle, cos0);
		sin1sin0 = SINE (angle, sin0);
		sin1cos0 = SINE (angle, cos0);
		cos1sin0 = COSINE (angle, sin0);

		cos0 = cos1cos0 - sin1sin0;
		sin0 = sin1cos0 + cos1sin0;
		MissileBlock.cx = ShipPtr->next.location.x + cos0;
		MissileBlock.cy = ShipPtr->next.location.y + sin0;
		WeaponArray[(i << 1)] = initialize_missile (&MissileBlock);

		cos0 = -cos1cos0 - sin1sin0;
		sin0 = -sin1cos0 + cos1sin0;
		MissileBlock.cx = ShipPtr->next.location.x + cos0;
		MissileBlock.cy = ShipPtr->next.location.y + sin0;
		WeaponArray[(i << 1) + 1] = initialize_missile (&MissileBlock);
	}

	return (6);
}

static void
utwig_intelligence (ELEMENT *ShipPtr, EVALUATE_DESC *ObjectsOfConcern,
		COUNT ConcernCounter)
{
	SIZE ShieldStatus;
	STARSHIP *StarShipPtr;
	EVALUATE_DESC *lpEvalDesc;

	GetElementStarShip (ShipPtr, &StarShipPtr);

	lpEvalDesc = &ObjectsOfConcern[ENEMY_WEAPON_INDEX];
	if (StarShipPtr->RaceDescPtr->ship_info.energy_level == 0)
		ShieldStatus = 0;
	else
	{
		ShieldStatus = -1;
		if (lpEvalDesc->ObjectPtr && lpEvalDesc->MoveState == ENTICE)
		{
			STARSHIP *WeaponStarShipPtr;
		
			GetElementStarShip (lpEvalDesc->ObjectPtr, &WeaponStarShipPtr);

			ShieldStatus = 0;
			if (!(lpEvalDesc->ObjectPtr->state_flags & FINITE_LIFE)
				// Shiver: AI does not raise shields at Lurg oil blobs.		
				&& !(WeaponStarShipPtr && WeaponStarShipPtr->SpeciesID == LURG_ID
					&& lpEvalDesc->ObjectPtr->mass_points < 2))
			{
				lpEvalDesc->MoveState = PURSUE;
			}
			else if (lpEvalDesc->ObjectPtr->mass_points
				|| lpEvalDesc->ObjectPtr->state_flags & CREW_OBJECT)
			{
				if ((lpEvalDesc->which_turn >>= 1) == 0)
					lpEvalDesc->which_turn = 1;

				if (lpEvalDesc->ObjectPtr->mass_points)
					lpEvalDesc->ObjectPtr = 0;
				else
					lpEvalDesc->MoveState = PURSUE;
				ShieldStatus = 1;
			}
		}
	}

	if (StarShipPtr->special_counter == 0)
	{
		StarShipPtr->ship_input_state &= ~SPECIAL;
		if (ShieldStatus)
		{
			if ((ShieldStatus > 0 || lpEvalDesc->ObjectPtr)
				&& lpEvalDesc->which_turn <= 2
				&& (ShieldStatus > 0
				|| (lpEvalDesc->ObjectPtr->state_flags
						& PLAYER_SHIP) // means IMMEDIATE_WEAPON.
				|| PlotIntercept (lpEvalDesc->ObjectPtr, ShipPtr, 2, 0))
				&& (TFB_Random () & 3))
			{
				StarShipPtr->ship_input_state |= SPECIAL;
				StarShipPtr->ship_input_state &= ~WEAPON;
			}

			lpEvalDesc->ObjectPtr = 0;
		}
	}

	if ((lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX])->ObjectPtr)
	{
		STARSHIP *EnemyStarShipPtr;

		GetElementStarShip (lpEvalDesc->ObjectPtr, &EnemyStarShipPtr);
		
		if (!(EnemyStarShipPtr->RaceDescPtr->ship_info.ship_flags & IMMEDIATE_WEAPON)
				&& StarShipPtr->RaceDescPtr->ship_info.energy_level)
		{
			lpEvalDesc->MoveState = PURSUE;
		}
	}

	ship_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter);
}

// Shiver: I have applied several new conditions to Utwig's shield.
static void
utwig_collision (ELEMENT *ElementPtr0, POINT *pPt0,
		ELEMENT *ElementPtr1, POINT *pPt1)
{
	STARSHIP *EnemyStarShipPtr;

	GetElementStarShip (ElementPtr1, &EnemyStarShipPtr);

	if (ElementPtr0->life_span > NORMAL_LIFE
			&& (ElementPtr1->state_flags & FINITE_LIFE)
			&& ElementPtr1->mass_points
			// Prevent Chmmr satellites from charging up Utwig's battery during collision.
			&& !(EnemyStarShipPtr->SpeciesID == CHMMR_ID && ElementPtr1->mass_points == 10)
			// Do not gain energy from Lurg blobs; these inflict no damage against ships.
			&& !(EnemyStarShipPtr->SpeciesID == LURG_ID && ElementPtr1->mass_points < 2))
	{
		ElementPtr0->life_span += ElementPtr1->mass_points;
	}

	collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
}

static void
utwig_preprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;
	PRIMITIVE *lpPrim;

	if (ElementPtr->state_flags & APPEARING)
	{
		ElementPtr->collision_func = utwig_collision;
	}

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (ElementPtr->life_span > (NORMAL_LIFE + 1))
	{
		DeltaEnergy (ElementPtr,
				ElementPtr->life_span - (NORMAL_LIFE + 1));

		ProcessSound (SetAbsSoundIndex (
				StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);
	}

	if (!(StarShipPtr->cur_status_flags & SPECIAL))
		StarShipPtr->special_counter = 0;
	else if (StarShipPtr->special_counter % (SPECIAL_WAIT >> 1) == 0)
	{
		if (!DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST))
			StarShipPtr->RaceDescPtr->ship_info.ship_flags &=
					~(LIGHT_POINT_DEFENSE | SHIELD_DEFENSE);
		else if (StarShipPtr->special_counter == 0)
		{
			StarShipPtr->special_counter =
					StarShipPtr->RaceDescPtr->characteristics.special_wait;
			ProcessSound (SetAbsSoundIndex (
					StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 2), ElementPtr);
		}
	}

	lpPrim = &(GLOBAL (DisplayArray))[ElementPtr->PrimIndex];
	if (StarShipPtr->special_counter == 0)
	{
		SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1C, 0x00), 0x78));
		ElementPtr->life_span = NORMAL_LIFE;
		SetPrimType (lpPrim, STAMP_PRIM);
	}
	else
	{
		static const COLOR color_tab[] =
		{
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x00, 0x00), 0x2a),
			BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x00), 0x2b),
			BUILD_COLOR (MAKE_RGB15 (0x17, 0x00, 0x00), 0x2c),
			BUILD_COLOR (MAKE_RGB15 (0x13, 0x00, 0x00), 0x2d),
			BUILD_COLOR (MAKE_RGB15 (0x0F, 0x00, 0x00), 0x2e),
			BUILD_COLOR (MAKE_RGB15 (0x0B, 0x00, 0x00), 0x2f),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x15, 0x00), 0x7a),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x11, 0x00), 0x7b),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0E, 0x00), 0x7c),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0A, 0x00), 0x7d),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x07, 0x00), 0x7e),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x03, 0x00), 0x7f),
		};
					
#define NUM_TAB_COLORS (sizeof (color_tab) / sizeof (color_tab[0]))
			
		COUNT color_index = 0;
		COLOR c;

		if (StarShipPtr->weapon_counter == 0)
			++StarShipPtr->weapon_counter;

		c = COLOR_256 (GetPrimColor (lpPrim));
		if (c >= 0x78)
		{
			if ((c += 2) > 0x7E)
				c = 0x2A;
		}
		else if (c < 0x2E)
		{
			if (++c == 0x2B)
				c = 0x2C;
		}
		else
			c = 0x7a;
		if (c <= 0x2f && c >= 0x2a)
				color_index = (COUNT)c - 0x2a;
		else /* color is between 0x7a and 0x7f */
				color_index = (COUNT)(c - 0x7a) + (NUM_TAB_COLORS >> 1);
		SetPrimColor (lpPrim, color_tab[color_index]);

		ElementPtr->life_span = NORMAL_LIFE + 1;
		SetPrimType (lpPrim, STAMPFILL_PRIM);
	}
}

RACE_DESC*
init_utwig (void)
{
	RACE_DESC *RaceDescPtr;

	utwig_desc.preprocess_func = utwig_preprocess;
	utwig_desc.init_weapon_func = initialize_lance;
	utwig_desc.cyborg_control.intelligence_func = utwig_intelligence;

	RaceDescPtr = &utwig_desc;

	return (RaceDescPtr);
}
