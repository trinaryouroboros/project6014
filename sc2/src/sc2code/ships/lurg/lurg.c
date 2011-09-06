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
#include "ships/lurg/resinst.h"
#include "libs/mathlib.h"


#define MAX_CREW 20
#define MAX_ENERGY 20
#define ENERGY_REGENERATION 1
#define ENERGY_WAIT 6
#define MAX_THRUST 20
#define THRUST_INCREMENT 6
#define THRUST_WAIT 1
#define TURN_WAIT 1
#define SHIP_MASS 6

#define WEAPON_ENERGY_COST 4
#define WEAPON_WAIT 10
#define MISSILE_SPEED DISPLAY_TO_WORLD (18)
#define MISSILE_LIFE 25
#define MISSILE_HITS 5
#define MISSILE_DAMAGE 3
#define MISSILE_OFFSET 2
#define LURG_OFFSET 23

#define SPECIAL_ENERGY_COST 2
#define SPECIAL_WAIT 2
#define OIL_BATCH_SIZE 6
#define OIL_SPEED DISPLAY_TO_WORLD (2*RESOLUTION_FACTOR)
#define OIL_INIT_SPEED DISPLAY_TO_WORLD (6*RESOLUTION_FACTOR)
#define OIL_HITS 3
#define OIL_DAMAGE 1 // Oil inflicts damage only in specific circumstances.
#define OIL_SPREAD_MINIMUM 6
#define OIL_SPREAD_VARIATION 4
#define OIL_LIFE 300
#define OIL_LIFE_VARIATION 100
#define OIL_DELAY 6
#define OIL_DELAY_MAX 36
#define OIL_SNARE WORLD_TO_VELOCITY (-1)
#define OIL_OFFSET 3
#define LURG_OFFSET_2 16

#define REPAIR_WAIT 192 // Was 216.

static RACE_DESC lurg_desc =
{
	{ /* SHIP_INFO */
		FIRES_FORE | SEEKING_SPECIAL | LIGHT_POINT_DEFENSE,
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
		(MISSILE_SPEED * MISSILE_LIFE) *  4/5,
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
	STARSHIP *StarShipPtr, *EnemyStarShipPtr;
	EVALUATE_DESC *lpEvalDesc;
	
	GetElementStarShip (ShipPtr, &StarShipPtr);

	// Don't use the secondary unless specifically told to.
	StarShipPtr->ship_input_state &= ~SPECIAL;

	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	if (lpEvalDesc->ObjectPtr)
	{
		GetElementStarShip (lpEvalDesc->ObjectPtr, &EnemyStarShipPtr);

		// Be aggressive when the enemy is ensnared or at very close range.
		if (lpEvalDesc->which_turn <= 28
			&& lpEvalDesc->ObjectPtr->turn_wait >= OIL_DELAY
			&& lpEvalDesc->ObjectPtr->thrust_wait >= OIL_DELAY
			&& !(lpEvalDesc->ObjectPtr->state_flags & APPEARING)
			|| lpEvalDesc->which_turn <= 14)
		{
			lpEvalDesc->MoveState = PURSUE;

			// Disregard enemy weapons in these circumstances.
			ObjectsOfConcern[ENEMY_WEAPON_INDEX].ObjectPtr = NULL;
		}
		// Use caution if the enemy is nearby but not ensnared.
		else if (lpEvalDesc->which_turn <= 24
			// Approach enemies /w long range weapons while crew is full.
			|| (WEAPON_RANGE (&EnemyStarShipPtr->RaceDescPtr->cyborg_control)
					>= LONG_RANGE_WEAPON * 3/5
				&& StarShipPtr->RaceDescPtr->ship_info.crew_level == MAX_CREW))
		{
			lpEvalDesc->MoveState = ENTICE;
		}
		// Otherwise stall for time when the enemy is far away.
		else
		{
			lpEvalDesc->MoveState = AVOID;
		}

		// Sometimes take shots which don't line up with the opponent's current trajectory.
		if (ship_weapons (ShipPtr, lpEvalDesc->ObjectPtr, DISPLAY_TO_WORLD (5))
				&& (TFB_Random () & 7))
			StarShipPtr->ship_input_state |= WEAPON;
	}

	lpEvalDesc = &ObjectsOfConcern[ENEMY_WEAPON_INDEX];
	// Drop oil in the way of various incoming projectiles.
	if (lpEvalDesc->ObjectPtr
		&& StarShipPtr->RaceDescPtr->ship_info.energy_level >= 8
		&& lpEvalDesc->ObjectPtr->hit_points < 4 // Forget high HP projectiles.
		&& (lpEvalDesc->ObjectPtr->mass_points
			|| lpEvalDesc->ObjectPtr->state_flags & (CREW_OBJECT | FINITE_LIFE))
		&& lpEvalDesc->which_turn <= 10)
	{
		StarShipPtr->ship_input_state |= SPECIAL;
	}
	
	// Otherwise disregard all enemy weapons unless there's a crew weapon or huge weapon present.
	if (lpEvalDesc->ObjectPtr
			&& !(lpEvalDesc->ObjectPtr->state_flags & CREW_OBJECT)
			&& !(lpEvalDesc->ObjectPtr->hit_points >= 6)
			&& !(lpEvalDesc->ObjectPtr->mass_points >= 6))
	{
		ObjectsOfConcern[ENEMY_WEAPON_INDEX].ObjectPtr = NULL;
	}
	
	// Basic ship intelligence.
	ship_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter);

	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	// Cut thrust when cornered to prevent Lurg from accelerating into its opponent.
	if (lpEvalDesc->ObjectPtr && lpEvalDesc->which_turn <= 14
		// Prevent Lurg from sitting completely stationary.
		&& ShipPtr->next.location.x != ShipPtr->current.location.x
		&& ShipPtr->next.location.y != ShipPtr->current.location.y)
	{
		StarShipPtr->ship_input_state &= ~THRUST;
	}
		
	// Drop several batches of oil whenever the battery tops off.
	if (lpEvalDesc->ObjectPtr
		&& StarShipPtr->RaceDescPtr->ship_info.energy_level ==
			StarShipPtr->RaceDescPtr->ship_info.max_energy
		|| (StarShipPtr->RaceDescPtr->ship_info.energy_level >
			(StarShipPtr->RaceDescPtr->ship_info.max_energy - 6)
			&& StarShipPtr->old_status_flags & SPECIAL))
	{
		StarShipPtr->ship_input_state |= SPECIAL;
	}
}

static void
acid_preprocess (ELEMENT *ElementPtr)
{
	COUNT facing;

	facing = (GetFrameIndex (ElementPtr->next.image.frame)) % 16;
	// JMS: Modulo 16 here ensures that the explosion frames are not used in wrong place.
	
	if (ElementPtr->thrust_wait == 1) // Left start.
	{
		--facing;
		ElementPtr->turn_wait += 1;
		ElementPtr->thrust_wait = 0;
		ElementPtr->turn_wait = 3;
	}

	if (ElementPtr->thrust_wait == 2) // Right start.
	{
		++facing;
		ElementPtr->turn_wait += 1;
		ElementPtr->thrust_wait = 0;
		ElementPtr->turn_wait = 0;
	}

	if (ElementPtr->thrust_wait == 0) // Main loop.
	{
		if (ElementPtr->turn_wait < 2) // Turn left.
		{
			--facing;
			ElementPtr->turn_wait += 1;
		} 
		else if (ElementPtr->turn_wait < 3) // Wait.
		{
			ElementPtr->turn_wait += 1;
		}
		else if (ElementPtr->turn_wait < 5) // Turn right.
		{
			++facing;
			ElementPtr->turn_wait += 1;
		}
		else if (ElementPtr->turn_wait < 6) // Wait.
		{
			ElementPtr->turn_wait += 1;
		}
		else if (ElementPtr->turn_wait == 6) // Wait and reset the loop.
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

		facing = (COUNT)TFB_Random ();

		SetVelocityVector (&ElementPtr->velocity,
				OIL_SPEED, facing);

		turn_wait = 4;
	}

	ElementPtr->turn_wait = MAKE_BYTE (turn_wait, thrust_wait);
}

static void
oil_collision (ELEMENT *ElementPtr0, POINT *pPt0, ELEMENT *ElementPtr1, POINT *pPt1)
{
	if (!(ElementPtr1->state_flags & (APPEARING | GOOD_GUY | BAD_GUY | PLAYER_SHIP | FINITE_LIFE))
			&& !GRAVITY_MASS (ElementPtr1->mass_points))
	{
		ElementPtr0->mass_points = 0;
		// Oil does no damage against asteroids
	}
	else if ((ElementPtr0->state_flags & (GOOD_GUY | BAD_GUY))
			!= (ElementPtr1->state_flags & (GOOD_GUY | BAD_GUY)))
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
				
			
			facing = NORMALIZE_FACING (
				ANGLE_TO_FACING (GetVelocityTravelAngle (&ElementPtr1->velocity) + HALF_CIRCLE));
		
			DeltaVelocityComponents (&ElementPtr1->velocity,
				COSINE (facing, OIL_SNARE),
				SINE (facing, OIL_SNARE));

			ElementPtr0->mass_points = 0;

			// ProcessSound (SetAbsSoundIndex (
					/* Sound effect?? */
			// StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr0);
		}
		else
		{
			if (ElementPtr1->hit_points > 3)
			{
				SIZE xHits;
				xHits = ElementPtr1->hit_points;
				ElementPtr0->mass_points = (TFB_Random () & 3) == 1;
				// Oil is ineffective against large projectiles
			}
			else
				ElementPtr0->mass_points = 1;
				// Do full, guaranteed damage against small projectiles
		}
	}

	weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
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
	MissileBlock.face = (COUNT)TFB_Random (); // Deploy at random in every direction.
	MissileBlock.index = 0;
	MissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
			| IGNORE_SIMILAR;
	MissileBlock.pixoffs = LURG_OFFSET_2;
	MissileBlock.life = OIL_LIFE + (TFB_Random () & OIL_LIFE_VARIATION);
	MissileBlock.speed = OIL_INIT_SPEED;
	MissileBlock.hit_points = OIL_HITS;
	MissileBlock.damage = OIL_DAMAGE;
	MissileBlock.preprocess_func = oil_preprocess;
	MissileBlock.blast_offs = OIL_OFFSET;
	Missile = initialize_missile (&MissileBlock);

	if (Missile)
	{
		ELEMENT *OilPtr;

		LockElement (Missile, &OilPtr);
		// OilPtr->turn_wait affects how long the projectile travels at OIL_INIT_SPEED.
		OilPtr->turn_wait = OIL_SPREAD_MINIMUM + ((COUNT)TFB_Random () & OIL_SPREAD_VARIATION);
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

	GetElementStarShip (ElementPtr, &StarShipPtr);
	
	if ((StarShipPtr->cur_status_flags & SPECIAL)
			&& StarShipPtr->special_counter == 0
			&& DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST))
	{
		int i;

		ProcessSound (SetAbsSoundIndex // Spill oil.
			(StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);
		
		for (i = 0; i < OIL_BATCH_SIZE; i++)
			spill_oil (ElementPtr);

		StarShipPtr->special_counter = SPECIAL_WAIT;
	}

	// Start timer for passive regeneration when damaged.
	if (ElementPtr->hit_points < MAX_CREW
		&& StarShipPtr->auxiliary_counter == 0)
	{
		StarShipPtr->auxiliary_counter += REPAIR_WAIT;
	}

	// Slowly regenerate crew.
	if (StarShipPtr->auxiliary_counter == 1
			&& ElementPtr->hit_points > 0)
	{
		DeltaCrew (ElementPtr, 1);

		ProcessSound (SetAbsSoundIndex // Regenerate.
			(StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 2), ElementPtr);
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