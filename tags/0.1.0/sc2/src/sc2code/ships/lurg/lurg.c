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

// JMS 2010 - Totally new file: Lurg ship

#include "ships/ship.h"
#include "ships/lurg/resinst.h"

#include "libs/mathlib.h"


#define MAX_CREW 20
#define MAX_ENERGY 16
#define ENERGY_REGENERATION 1
#define WEAPON_ENERGY_COST 3
#define SPECIAL_ENERGY_COST 1
#define ENERGY_WAIT 4
#define MAX_THRUST 20
#define THRUST_INCREMENT 8
#define TURN_WAIT 1
#define THRUST_WAIT 0
#define WEAPON_WAIT 10
#define SPECIAL_WAIT 0

#define SHIP_MASS 4
#define MISSILE_SPEED DISPLAY_TO_WORLD (30)
#define MISSILE_LIFE 10

#define OIL_DELAY 5
#define OIL_DELAY_MAX 50

static RACE_DESC lurg_desc =
{
	{ /* SHIP_INFO */
		FIRES_FORE | FIRES_AFT | SEEKING_SPECIAL,
		20, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		LURG_RACE_STRINGS,
		LURG_ICON_MASK_PMAP_ANIM,
		LURG_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		0, //1525 / SPHERE_RADIUS_INCREMENT * 2, /* Initial SoI radius */
		{ /* Known location (center of SoI) */
			3710, 4389,
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
			LURG_BIG_MASK_PMAP_ANIM,
			LURG_MED_MASK_PMAP_ANIM,
			LURG_SML_MASK_PMAP_ANIM,
		},
		{
			LURGGOB_BIG_MASK_PMAP_ANIM,
			LURGGOB_MED_MASK_PMAP_ANIM,
			LURGGOB_SML_MASK_PMAP_ANIM,
		},
		{
			OIL_BIG_MASK_PMAP_ANIM,
			OIL_MED_MASK_PMAP_ANIM,
			OIL_SML_MASK_PMAP_ANIM,
		},
		{
			LURG_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		LURG_VICTORY_SONG,
		LURG_SHIP_SOUNDS,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		(MISSILE_SPEED * MISSILE_LIFE) >> 1,
		NULL,
	},
	(UNINIT_FUNC *) NULL,
	(PREPROCESS_FUNC *) NULL,
	(POSTPROCESS_FUNC *) NULL,
	(INIT_WEAPON_FUNC *) NULL,
	0,
};

static void
lurg_intelligence (ELEMENT *ShipPtr, EVALUATE_DESC *ObjectsOfConcern,
		COUNT ConcernCounter)
{
	STARSHIP *StarShipPtr;
	EVALUATE_DESC *lpEvalDesc;

	ship_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter);

	GetElementStarShip (ShipPtr, &StarShipPtr);
	StarShipPtr->ship_input_state &= ~SPECIAL;

	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	if (StarShipPtr->special_counter == 0
			&& lpEvalDesc->ObjectPtr
			&& lpEvalDesc->which_turn <= 24)
	{
		COUNT travel_facing, direction_facing;
		SIZE delta_x, delta_y;

		travel_facing = NORMALIZE_FACING (
				ANGLE_TO_FACING (GetVelocityTravelAngle (&ShipPtr->velocity)
				+ HALF_CIRCLE)
				);
		delta_x = lpEvalDesc->ObjectPtr->current.location.x
				- ShipPtr->current.location.x;
		delta_y = lpEvalDesc->ObjectPtr->current.location.y
				- ShipPtr->current.location.y;
		direction_facing = NORMALIZE_FACING (
				ANGLE_TO_FACING (ARCTAN (delta_x, delta_y))
				);

		if (NORMALIZE_FACING (direction_facing
				- (StarShipPtr->ShipFacing + ANGLE_TO_FACING (HALF_CIRCLE))
				+ ANGLE_TO_FACING (QUADRANT))
				<= ANGLE_TO_FACING (HALF_CIRCLE)
				&& (lpEvalDesc->which_turn <= 8
				|| NORMALIZE_FACING (direction_facing
				+ ANGLE_TO_FACING (HALF_CIRCLE)
				- ANGLE_TO_FACING (GetVelocityTravelAngle (
						&lpEvalDesc->ObjectPtr->velocity
						))
				+ ANGLE_TO_FACING (QUADRANT))
				<= ANGLE_TO_FACING (HALF_CIRCLE))
				&& (!(StarShipPtr->cur_status_flags &
				(SHIP_BEYOND_MAX_SPEED | SHIP_IN_GRAVITY_WELL))
				|| NORMALIZE_FACING (direction_facing
				- travel_facing + ANGLE_TO_FACING (QUADRANT))
				<= ANGLE_TO_FACING (HALF_CIRCLE)))
			StarShipPtr->ship_input_state |= SPECIAL;
	}
}

static COUNT
initialize_horn (ELEMENT *ShipPtr, HELEMENT HornArray[])
{
#define MISSILE_HITS 6
#define MISSILE_DAMAGE 4
#define MISSILE_OFFSET 2
#define LURG_OFFSET 23
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.face = MissileBlock.index = StarShipPtr->ShipFacing;
	MissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
			| IGNORE_SIMILAR;
	MissileBlock.pixoffs = LURG_OFFSET;
	MissileBlock.speed = MISSILE_SPEED;
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = NULL;
	MissileBlock.blast_offs = MISSILE_OFFSET;
	HornArray[0] = initialize_missile (&MissileBlock);
	return (1);
}

#define SHIP_OFFSET (15 * RESOLUTION_FACTOR) // JMS_GFX
#define OIL_OFFSET (3 * RESOLUTION_FACTOR) // JMS_GFX
#define OIL_HITS 2
#define OIL_DAMAGE 1
#define OIL_SPEED DISPLAY_TO_WORLD (2*RESOLUTION_FACTOR) // JMS_GFX
#define OIL_INIT_SPEED (OIL_SPEED*10)
#define OIL_LIFE 250

static void
oil_preprocess (ELEMENT *ElementPtr)
{
	BYTE thrust_wait, turn_wait;

	thrust_wait = HINIBBLE (ElementPtr->turn_wait);
	turn_wait = LONIBBLE (ElementPtr->turn_wait);
	if (thrust_wait > 0)
		--thrust_wait;
	else
	{
		ElementPtr->next.image.frame =
				IncFrameIndex (ElementPtr->current.image.frame);
		ElementPtr->state_flags |= CHANGING;

		thrust_wait = (BYTE)((COUNT)TFB_Random () & 7);
	}

	if (turn_wait > 0)
		--turn_wait;
	else
	{
		COUNT facing;
		SIZE delta_facing;

		facing = NORMALIZE_FACING (ANGLE_TO_FACING (
				GetVelocityTravelAngle (&ElementPtr->velocity)));
		if ((delta_facing = TrackShip (ElementPtr, &facing)) == -1)
			facing = (COUNT)TFB_Random ();
		else if (delta_facing <= ANGLE_TO_FACING (HALF_CIRCLE))
			facing += (COUNT)TFB_Random () & (ANGLE_TO_FACING (HALF_CIRCLE) - 1);
		else
			facing -= (COUNT)TFB_Random () & (ANGLE_TO_FACING (HALF_CIRCLE) - 1);
		SetVelocityVector (&ElementPtr->velocity,
				OIL_SPEED, facing);

#define TRACK_WAIT 4
		turn_wait = TRACK_WAIT;
	}

	ElementPtr->turn_wait = MAKE_BYTE (turn_wait, thrust_wait);
}

static void
oil_collision (ELEMENT *ElementPtr0, POINT *pPt0,
		ELEMENT *ElementPtr1, POINT *pPt1)
{
#define ENERGY_DRAIN 10
	collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
	if ((ElementPtr0->state_flags
			& (GOOD_GUY | BAD_GUY)) !=
			(ElementPtr1->state_flags
			& (GOOD_GUY | BAD_GUY)))
	{
		STARSHIP *StarShipPtr;

		GetElementStarShip (ElementPtr0, &StarShipPtr);
		ProcessSound (SetAbsSoundIndex (
						/* DOGGY_STEALS_ENERGY */
				StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 2), ElementPtr0);
		ElementPtr1->thrust_wait += OIL_DELAY;
		if (ElementPtr1->thrust_wait > OIL_DELAY_MAX)
			ElementPtr1->thrust_wait = OIL_DELAY_MAX;
		ElementPtr1->turn_wait += OIL_DELAY;
		if (ElementPtr1->turn_wait > OIL_DELAY_MAX)
			ElementPtr1->turn_wait = OIL_DELAY_MAX;
	}
	if (ElementPtr0->thrust_wait <= COLLISION_THRUST_WAIT)
		ElementPtr0->thrust_wait += COLLISION_THRUST_WAIT << 1;
}

static void spill_oil (ELEMENT *ShipPtr)
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	HELEMENT Missile;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
	MissileBlock.face = NORMALIZE_FACING (StarShipPtr->ShipFacing
			+ ANGLE_TO_FACING (HALF_CIRCLE));
	MissileBlock.index = 0;
	MissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
			| IGNORE_SIMILAR;
	MissileBlock.pixoffs = SHIP_OFFSET;
	MissileBlock.speed = OIL_INIT_SPEED;
	MissileBlock.hit_points = OIL_HITS;
	MissileBlock.damage = ((COUNT)TFB_Random () & 7) == 1;
	MissileBlock.life = OIL_LIFE;
	MissileBlock.preprocess_func = oil_preprocess;
	MissileBlock.blast_offs = OIL_OFFSET;
	Missile = initialize_missile (&MissileBlock);

	if (Missile)
	{
		ELEMENT *OilPtr;

		LockElement (Missile, &OilPtr);
		OilPtr->turn_wait = 0;
		SetElementStarShip (OilPtr, StarShipPtr);
		OilPtr->collision_func = oil_collision;
		UnlockElement (Missile);
		PutElement (Missile);
	}
}

static void
lurg_postprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;
	SHIP_INFO *ShipInfoPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	ShipInfoPtr = &StarShipPtr->RaceDescPtr->ship_info;
	if ((StarShipPtr->cur_status_flags & SPECIAL)
			&& ShipInfoPtr->energy_level > 1
			&& StarShipPtr->special_counter == 0
			&& DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST))
	{
		int i;
		ProcessSound (SetAbsSoundIndex (
						/* LAUNCH_FIGHTERS */
				StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);
		for (i = 0; i < 5; i++)
			spill_oil (ElementPtr);

		StarShipPtr->special_counter = SPECIAL_WAIT;
	}
}

RACE_DESC*
init_lurg (void)
{
	RACE_DESC *RaceDescPtr;

	lurg_desc.postprocess_func = lurg_postprocess;
	lurg_desc.init_weapon_func = initialize_horn;
	lurg_desc.cyborg_control.intelligence_func = lurg_intelligence;

	RaceDescPtr = &lurg_desc;

	return (RaceDescPtr);
}