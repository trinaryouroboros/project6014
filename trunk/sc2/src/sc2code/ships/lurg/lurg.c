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
//			- Gave Lurg some more aggressivenes, when enemy ship is close
//			- Lurg can now use oil as a defense against projectiles, most notably earthling missiles.
//			- Lurg also now sprouts oil whenever battery is full and enemy ship far
//			- Increased OIL_BATCH from 4 to 5 for shits and giggles.

#include "ships/ship.h"
#include "ships/lurg/resinst.h"

#include "libs/mathlib.h"


#define MAX_CREW 20
#define MAX_ENERGY 16
#define ENERGY_REGENERATION 1
#define ENERGY_WAIT 5
#define MAX_THRUST 20
#define THRUST_INCREMENT 7
#define THRUST_WAIT 1
#define TURN_WAIT 1
#define SHIP_MASS 6

#define WEAPON_ENERGY_COST 3
#define WEAPON_WAIT 9
#define MISSILE_SPEED DISPLAY_TO_WORLD (18)
#define MISSILE_LIFE 25
#define MISSILE_HITS 4
#define MISSILE_DAMAGE 4

#define SPECIAL_ENERGY_COST 2
#define SPECIAL_WAIT 3
#define OIL_HITS 2
#define OIL_DAMAGE 1 // Shiver: Oil inflicts damage only in specific circumstances.
#define OIL_SPEED DISPLAY_TO_WORLD (2*RESOLUTION_FACTOR) // JMS_GFX
#define OIL_INIT_SPEED (OIL_SPEED*3)
#define OIL_LIFE 350
#define OIL_BATCH_SIZE 5 // JMS: Was 4
#define OIL_DELAY 5
#define OIL_DELAY_MAX 40
#define OIL_SNARE WORLD_TO_VELOCITY (-1)

#define REPAIR_WAIT 216

static RACE_DESC lurg_desc =
{
	{ /* SHIP_INFO */
		FIRES_FORE | SEEKING_SPECIAL,
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
lurg_intelligence (ELEMENT *ShipPtr, EVALUATE_DESC *ObjectsOfConcern, COUNT ConcernCounter)
{
	STARSHIP *StarShipPtr;
	EVALUATE_DESC *lpEvalDesc;
	SIZE OilStatus;
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];

	// JMS: Movement behavior additions
	if (lpEvalDesc->ObjectPtr)
	{
		STARSHIP *EnemyStarShipPtr;
		GetElementStarShip (lpEvalDesc->ObjectPtr, &EnemyStarShipPtr);
		
		// JMS: Avoid ships with seeking weapons when they're far away (Modified from Chenjesu AI).
		if ((MANEUVERABILITY (&EnemyStarShipPtr->RaceDescPtr->cyborg_control ) <= SLOW_SHIP
				&& WEAPON_RANGE (&EnemyStarShipPtr->RaceDescPtr->cyborg_control ) >= LONG_RANGE_WEAPON * 3 / 4
				&& (EnemyStarShipPtr->RaceDescPtr->ship_info.ship_flags & SEEKING_WEAPON)
				&& lpEvalDesc->which_turn > 22))
			lpEvalDesc->MoveState = AVOID;
		
		// JMS: don't pay attention to enemy projectiles when the enemy ship is close enough. This makes Lurg fire a bit more.
		if (lpEvalDesc->which_turn <= 6)
			ObjectsOfConcern[ENEMY_WEAPON_INDEX].ObjectPtr = 0;
	}
	
	// Basic ship intelligence is done here
	ship_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter);
	StarShipPtr->ship_input_state &= ~SPECIAL;
	
	// JMS: Weapon behavior additions: If enemy ship is close, keep firing even if it isn't perfectly lined up 
	if (lpEvalDesc->which_turn < 12)
		StarShipPtr->ship_input_state |= WEAPON;
	
	// Special(oil) behavior additions
	if (StarShipPtr->special_counter == 0 && lpEvalDesc->ObjectPtr && lpEvalDesc->which_turn <= 24)
	{
		COUNT travel_facing, direction_facing;
		SIZE delta_x, delta_y;

		travel_facing = NORMALIZE_FACING (ANGLE_TO_FACING (GetVelocityTravelAngle (&ShipPtr->velocity) + HALF_CIRCLE) );
		delta_x = lpEvalDesc->ObjectPtr->current.location.x - ShipPtr->current.location.x;
		delta_y = lpEvalDesc->ObjectPtr->current.location.y - ShipPtr->current.location.y;
		direction_facing = NORMALIZE_FACING (ANGLE_TO_FACING (ARCTAN (delta_x, delta_y)));

		if (NORMALIZE_FACING (direction_facing - (StarShipPtr->ShipFacing 
				+ ANGLE_TO_FACING (HALF_CIRCLE)) 
				+ ANGLE_TO_FACING (QUADRANT))
				<= ANGLE_TO_FACING (HALF_CIRCLE)
				&& (lpEvalDesc->which_turn <= 8
				|| NORMALIZE_FACING (direction_facing
				+ ANGLE_TO_FACING (HALF_CIRCLE)
				- ANGLE_TO_FACING (GetVelocityTravelAngle ( &lpEvalDesc->ObjectPtr->velocity))
				+ ANGLE_TO_FACING (QUADRANT))
				<= ANGLE_TO_FACING (HALF_CIRCLE))
				&& (!(StarShipPtr->cur_status_flags &
				(SHIP_BEYOND_MAX_SPEED | SHIP_IN_GRAVITY_WELL))
				|| NORMALIZE_FACING (direction_facing
				- travel_facing + ANGLE_TO_FACING (QUADRANT))
				<= ANGLE_TO_FACING (HALF_CIRCLE)))
			StarShipPtr->ship_input_state |= SPECIAL;
	}
	
	// JMS: Defensive measure 1: Release oil when battery full and enemy ship is not too close.
	if(StarShipPtr->RaceDescPtr->ship_info.energy_level == StarShipPtr->RaceDescPtr->ship_info.max_energy
	   && lpEvalDesc->ObjectPtr
	   && lpEvalDesc->which_turn > 12)
		StarShipPtr->ship_input_state |= SPECIAL;
	
	// JMS: Defensive measure 2: Lurg leaves oil in the way of projectiles closing in. (stolen from yehat.c shield code.)
	lpEvalDesc = &ObjectsOfConcern[ENEMY_WEAPON_INDEX];
	OilStatus = -1;
	if (lpEvalDesc->ObjectPtr)//&& lpEvalDesc->MoveState == ENTICE)
	{
		OilStatus = 0;
		if (!(lpEvalDesc->ObjectPtr->state_flags & (FINITE_LIFE | CREW_OBJECT)))
			lpEvalDesc->MoveState = PURSUE;
		else if (lpEvalDesc->ObjectPtr->mass_points || (lpEvalDesc->ObjectPtr->state_flags & CREW_OBJECT))
		{
			if (!(lpEvalDesc->ObjectPtr->state_flags & FINITE_LIFE))
				lpEvalDesc->which_turn <<= 1;
			else
			{
				if ((lpEvalDesc->which_turn >>= 1) == 0)
					lpEvalDesc->which_turn = 1;
				
				if (lpEvalDesc->ObjectPtr->mass_points)
					lpEvalDesc->ObjectPtr = 0;
				else
					lpEvalDesc->MoveState = PURSUE;
			}
			OilStatus = 1;
		}
	}
	if (StarShipPtr->special_counter == 0) // JMS: Defensive measure 2 continues...
	{
		if (OilStatus)
		{
			if (ShipPtr->life_span <= NORMAL_LIFE + 1
				&& (OilStatus > 0 || lpEvalDesc->ObjectPtr)
				&& lpEvalDesc->which_turn <= 12
				&& (OilStatus > 0
					|| (lpEvalDesc->ObjectPtr->state_flags & PLAYER_SHIP) // means IMMEDIATE WEAPON
					|| PlotIntercept (lpEvalDesc->ObjectPtr, ShipPtr, 12, 0))
				//&& (TFB_Random () & 3)
				)
				StarShipPtr->ship_input_state |= SPECIAL;
			
			if (lpEvalDesc->ObjectPtr && !(lpEvalDesc->ObjectPtr->state_flags & CREW_OBJECT))
				lpEvalDesc->ObjectPtr = 0;
		}
	}
}

static void
acid_preprocess (ELEMENT *ElementPtr)
{
	COUNT facing;

	facing = (GetFrameIndex (ElementPtr->next.image.frame)) % 16; // JMS: Modulo 16 here ensures that the explosion frames are not used in wrong place.
	
	if (ElementPtr->thrust_wait == 1) // Left start.
	{
		if (ElementPtr->turn_wait < 2)
		{
			--facing;
			ElementPtr->turn_wait += 1;
		}
		else
		{
			ElementPtr->thrust_wait = 0;
			ElementPtr->turn_wait = 6;
		}
	}

	if (ElementPtr->thrust_wait == 2) // Right start.
	{
		if (ElementPtr->turn_wait < 2)
		{
			++facing;
			ElementPtr->turn_wait += 1;
		}
		else
		{
			ElementPtr->thrust_wait = 0;
			ElementPtr->turn_wait = 0;
		}
	}

	if (ElementPtr->thrust_wait == 0) // Main loop.
	{
		if (ElementPtr->turn_wait < 4) // Turn left.
		{
			--facing;
			ElementPtr->turn_wait += 1;
		} 
		else if (ElementPtr->turn_wait < 6) // Wait.
		{
			ElementPtr->turn_wait += 1;
		}
		else if (ElementPtr->turn_wait < 10) // Turn right.
		{
			++facing;
			ElementPtr->turn_wait += 1;
		}
		else if (ElementPtr->turn_wait < 12) // Wait.
		{
			ElementPtr->turn_wait += 1;
		}
		else if (ElementPtr->turn_wait == 12) // Wait and reset the loop.
		{
			ElementPtr->turn_wait = 0;
		}
	}

	ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->next.image.frame, facing);
	ElementPtr->state_flags |= CHANGING;
	
	SetVelocityVector (&ElementPtr->velocity, MISSILE_SPEED, facing);
}

static COUNT
initialize_acid (ELEMENT *ShipPtr, HELEMENT AcidArray[])
{
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
	MissileBlock.blast_offs = MISSILE_OFFSET;
	MissileBlock.preprocess_func = acid_preprocess;

	AcidArray[0] = initialize_missile (&MissileBlock);
	
	if (AcidArray[0])
	{
		ELEMENT *AcidPtr;

		LockElement (AcidArray[0], &AcidPtr);
		if ((BYTE)TFB_Random () & 1) // Left or right?
			AcidPtr->thrust_wait = 1;
		else
			AcidPtr->thrust_wait = 2;
		UnlockElement (AcidArray[0]);
	}
	
	return (1);
}

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
oil_collision (ELEMENT *ElementPtr0, POINT *pPt0, ELEMENT *ElementPtr1, POINT *pPt1)
{
	if (!(ElementPtr1->state_flags
				& (APPEARING | GOOD_GUY | BAD_GUY
				| PLAYER_SHIP | FINITE_LIFE))
				&& !GRAVITY_MASS (ElementPtr1->mass_points))
	{
		ElementPtr0->mass_points = 0;
		// Shiver: Oil does no damage against asteroids.
	}
	else if ((ElementPtr0->state_flags
			& (GOOD_GUY | BAD_GUY)) !=
			(ElementPtr1->state_flags
			& (GOOD_GUY | BAD_GUY)))
	{
		STARSHIP *StarShipPtr;
		STARSHIP *EnemyStarShipPtr;

		GetElementStarShip (ElementPtr0, &StarShipPtr);
		GetElementStarShip (ElementPtr1, &EnemyStarShipPtr);
		
		if (ElementPtr1->state_flags & PLAYER_SHIP)
		{
			COUNT facing;

			ElementPtr1->thrust_wait += OIL_DELAY;
			if (ElementPtr1->thrust_wait > OIL_DELAY_MAX)
				ElementPtr1->thrust_wait = OIL_DELAY_MAX;
			ElementPtr1->turn_wait += OIL_DELAY;
			if (ElementPtr1->turn_wait > OIL_DELAY_MAX)
				ElementPtr1->turn_wait = OIL_DELAY_MAX;

			if(EnemyStarShipPtr->SpeciesID == SUPOX_ID)
			{
				EnemyStarShipPtr->special_counter += OIL_DELAY;
				if (EnemyStarShipPtr->special_counter > OIL_DELAY_MAX)
					EnemyStarShipPtr->special_counter = OIL_DELAY_MAX;
			}
				
			
			facing = NORMALIZE_FACING (ANGLE_TO_FACING (GetVelocityTravelAngle (&ElementPtr1->velocity) + HALF_CIRCLE));
		
			DeltaVelocityComponents (&ElementPtr1->velocity,
				COSINE (facing, OIL_SNARE),
				SINE (facing, OIL_SNARE));

			ElementPtr0->mass_points = 0;

			ProcessSound (SetAbsSoundIndex (
					/* Sound effect?? */
			StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 2), ElementPtr0);
		}
		else
		{
			if (ElementPtr1->hit_points > 3)
			{
				SIZE xHits;
				xHits = ElementPtr1->hit_points;
				ElementPtr0->mass_points = (TFB_Random () & 3) == 1;
				// Shiver: Oil is ineffective against large projectiles.
			}
			else
				ElementPtr0->mass_points = 1;
				// Shiver: Do damage full, guaranteed against projectiles otherwise.
		}
	}

	if (ElementPtr0->thrust_wait <= COLLISION_THRUST_WAIT)
		ElementPtr0->thrust_wait += COLLISION_THRUST_WAIT << 1;

	weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
}

static void spill_oil (ELEMENT *ShipPtr)
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	HELEMENT Missile;

#define OIL_OFFSET (3 * RESOLUTION_FACTOR) // JMS_GFX
#define SHIP_OFFSET (15 * RESOLUTION_FACTOR) // JMS_GFX

	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
	MissileBlock.face = (COUNT)TFB_Random (); // Shiver: Deploy at random in every direction.
	MissileBlock.index = 0;
	MissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
			| IGNORE_SIMILAR;
	MissileBlock.pixoffs = SHIP_OFFSET;
	MissileBlock.speed = OIL_INIT_SPEED;
	MissileBlock.hit_points = OIL_HITS;
	MissileBlock.damage = OIL_DAMAGE;
	MissileBlock.life = OIL_LIFE;
	MissileBlock.preprocess_func = oil_preprocess;
	MissileBlock.blast_offs = OIL_OFFSET;
	Missile = initialize_missile (&MissileBlock);

	if (Missile)
	{
		ELEMENT *OilPtr;

		LockElement (Missile, &OilPtr);
		/* Shiver: turn_wait here affects how long the projectile travels at
			a faster-than-usual speed when deployed. */
		OilPtr->turn_wait = (((COUNT)TFB_Random () & 5) + 5);
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
						/* Sound effect? */
				StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);
		
		for (i = 0; i < OIL_BATCH_SIZE; i++)
			spill_oil (ElementPtr);

		StarShipPtr->special_counter = SPECIAL_WAIT;
	}

	// Shiver: Start timer for passive regeneration when damaged.
	if (ElementPtr->hit_points < MAX_CREW
		&& StarShipPtr->auxiliary_counter == 0)
	{
		StarShipPtr->auxiliary_counter += REPAIR_WAIT;
	}

	// Shiver: Slowly regenerate crew.
	if (StarShipPtr->auxiliary_counter == 1
			&& ElementPtr->hit_points > 0)
	{
		DeltaCrew (ElementPtr, 1);

		ProcessSound (SetAbsSoundIndex (
				/* Sound effect? */
		StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 3), ElementPtr);
	}
}

RACE_DESC*
init_lurg (void)
{
	RACE_DESC *RaceDescPtr;

	lurg_desc.postprocess_func = lurg_postprocess;
	lurg_desc.init_weapon_func = initialize_acid;
	lurg_desc.cyborg_control.intelligence_func = lurg_intelligence;

	RaceDescPtr = &lurg_desc;

	return (RaceDescPtr);
}