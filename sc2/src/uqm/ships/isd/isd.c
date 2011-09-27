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

#include "../ship.h"
#include "resinst.h"
#include "uqm/colors.h"
#include "uqm/globdata.h"
#include "libs/mathlib.h"
#include <stdlib.h>


#define MAX_CREW MAX_CREW_SIZE
#define MAX_ENERGY MAX_ENERGY_SIZE
#define ENERGY_REGENERATION 3
#define ENERGY_WAIT 10
#define MAX_THRUST 30
#define THRUST_INCREMENT 6
#define TURN_WAIT 6
#define THRUST_WAIT 6
#define SPECIAL_WAIT 9
#define SHIP_MASS 10

#define WEAPON_ENERGY_COST 1
#define WEAPON_WAIT 2
#define MISSILE_SPEED DISPLAY_TO_WORLD (25)
#define MISSILE_LIFE 15
#define MISSILE_HITS 2
#define MISSILE_DAMAGE 2
#define MISSILE_OFFSET 8
#define ISD_OFFSET 42

#define SPECIAL_ENERGY_COST 8
#define TRACK_THRESHOLD 6
#define ONE_WAY_FLIGHT 125
#define FIGHTER_LIFE (ONE_WAY_FLIGHT + ONE_WAY_FLIGHT + 150)
#define FIGHTER_SPEED DISPLAY_TO_WORLD (8)
#define FIGHTER_WEAPON_WAIT 8
#define FIGHTER_LASER_SPEED DISPLAY_TO_WORLD (20)
#define FIGHTER_LASER_HITS 1
#define FIGHTER_LASER_DAMAGE 1
#define FIGHTER_LASER_LIFE 10
#define FIGHTER_LASER_OFFSET 2
#define FIGHTER_LASER_BLAST_OFFSET 4
#define FIGHTER_LASER_RANGE DISPLAY_TO_WORLD (120 + FIGHTER_LASER_OFFSET)

#define AUXILIARY_ENERGY_COST 1
#define AUTOTURRET_RANGE (MISSILE_SPEED * MISSILE_LIFE)
#define AUTOTURRET_WAIT 11
#define AUTOTURRET_OFFSET 20

static RACE_DESC isd_desc =
{
	{ /* SHIP_INFO */
		FIRES_FORE | SEEKING_SPECIAL,
		30, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		ISD_RACE_STRINGS,
		ISD_ICON_MASK_PMAP_ANIM,
		ISD_MICON_MASK_PMAP_ANIM,
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
			ISD_BIG_MASK_PMAP_ANIM,
			ISD_MED_MASK_PMAP_ANIM,
			ISD_SML_MASK_PMAP_ANIM,
		},
		{
			ISDLASER_BIG_MASK_PMAP_ANIM,
			ISDLASER_MED_MASK_PMAP_ANIM,
			ISDLASER_SML_MASK_PMAP_ANIM,
		},
		{
			ISDFIGHTER_BIG_MASK_PMAP_ANIM,
			ISDFIGHTER_MED_MASK_PMAP_ANIM,
			ISDFIGHTER_SML_MASK_PMAP_ANIM,
		},
		{
			ISD_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		ISD_VICTORY_SONG,
		ISD_SHIP_SOUNDS,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		MISSILE_SPEED * MISSILE_LIFE,
		NULL,
	},
	(UNINIT_FUNC *) NULL,
	(PREPROCESS_FUNC *) NULL,
	(POSTPROCESS_FUNC *) NULL,
	(INIT_WEAPON_FUNC *) NULL,
	0,
};

static COUNT
initialize_turbolaser (ELEMENT *ShipPtr, HELEMENT MissileArray[])
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	int turret;
	// COUNT shot_facing;
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.face = MissileBlock.index = NORMALIZE_FACING (StarShipPtr->ShipFacing);
	MissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY)) | IGNORE_SIMILAR;
	MissileBlock.speed = MISSILE_SPEED;
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = NULL;
	MissileBlock.blast_offs = MISSILE_OFFSET;
	MissileBlock.pixoffs = ISD_OFFSET;

	turret = TFB_Random () % 7;

	if (turret == 0)
	{
		MissileBlock.cx = ShipPtr->next.location.x;
		MissileBlock.cy = ShipPtr->next.location.y;
	}
	else if (turret == 1)
	{
		MissileBlock.cx = ShipPtr->next.location.x + COSINE(FACING_TO_ANGLE (MissileBlock.face + 4), 32)
			+ COSINE(FACING_TO_ANGLE(StarShipPtr->ShipFacing), -52);
		MissileBlock.cy = ShipPtr->next.location.y + SINE(FACING_TO_ANGLE (MissileBlock.face + 4), 32)
			+ SINE(FACING_TO_ANGLE(StarShipPtr->ShipFacing), -52);
	}
	else if (turret == 2)
	{
		MissileBlock.cx = ShipPtr->next.location.x + COSINE(FACING_TO_ANGLE (MissileBlock.face + 4), -32)
			+ COSINE(FACING_TO_ANGLE(StarShipPtr->ShipFacing), -52);
		MissileBlock.cy = ShipPtr->next.location.y + SINE(FACING_TO_ANGLE (MissileBlock.face + 4), -32)
			+ SINE(FACING_TO_ANGLE(StarShipPtr->ShipFacing), -52);
	}
	else if (turret == 3)
	{
		MissileBlock.cx = ShipPtr->next.location.x + COSINE(FACING_TO_ANGLE (MissileBlock.face + 4), 48)
			+ COSINE(FACING_TO_ANGLE(StarShipPtr->ShipFacing), -120);
		MissileBlock.cy = ShipPtr->next.location.y + SINE(FACING_TO_ANGLE (MissileBlock.face + 4), 48)
			+ SINE(FACING_TO_ANGLE(StarShipPtr->ShipFacing), -120);
	}
	else if (turret == 4)
	{
		MissileBlock.cx = ShipPtr->next.location.x + COSINE(FACING_TO_ANGLE (MissileBlock.face + 4), -48)
			+ COSINE(FACING_TO_ANGLE(StarShipPtr->ShipFacing), -136);
		MissileBlock.cy = ShipPtr->next.location.y + SINE(FACING_TO_ANGLE (MissileBlock.face + 4), -48)
			+ SINE(FACING_TO_ANGLE(StarShipPtr->ShipFacing), -136);
	}
	else if (turret == 5)
	{
		MissileBlock.cx = ShipPtr->next.location.x + COSINE(FACING_TO_ANGLE (MissileBlock.face + 4), 64)
			+ COSINE(FACING_TO_ANGLE(StarShipPtr->ShipFacing), -156);
		MissileBlock.cy = ShipPtr->next.location.y + SINE(FACING_TO_ANGLE (MissileBlock.face + 4), 64)
			+ SINE(FACING_TO_ANGLE(StarShipPtr->ShipFacing), -156);
	}
	else if (turret == 6)
	{
		MissileBlock.cx = ShipPtr->next.location.x + COSINE(FACING_TO_ANGLE (MissileBlock.face + 4), -64)
			+ COSINE(FACING_TO_ANGLE(StarShipPtr->ShipFacing), -156);
		MissileBlock.cy = ShipPtr->next.location.y + SINE(FACING_TO_ANGLE (MissileBlock.face + 4), -64)
			+ SINE(FACING_TO_ANGLE(StarShipPtr->ShipFacing), -156);
	}
	
	MissileArray[0] = initialize_missile (&MissileBlock);
	return (1);
}

static COUNT
initialize_fighterlaser (ELEMENT *ElementPtr, HELEMENT LaserArray[])
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
	
	MissileBlock.cx = ElementPtr->next.location.x;
	MissileBlock.cy = ElementPtr->next.location.y;
	MissileBlock.face = ElementPtr->thrust_wait;
	MissileBlock.sender = (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY)) | IGNORE_SIMILAR;
	MissileBlock.pixoffs = FIGHTER_LASER_OFFSET;
	// The fighter laser graphics are after the first 16 frames (which are the fighter hull graphics).
	MissileBlock.index = MissileBlock.face + 16;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
	MissileBlock.speed = FIGHTER_LASER_SPEED;
	MissileBlock.hit_points = FIGHTER_LASER_HITS;
	MissileBlock.damage = FIGHTER_LASER_DAMAGE;
	MissileBlock.life = FIGHTER_LASER_LIFE;

	MissileBlock.preprocess_func = NULL;
	MissileBlock.blast_offs = FIGHTER_LASER_BLAST_OFFSET;
	LaserArray[0] = initialize_missile (&MissileBlock);
	
	return (1);
}

/*static void
fighter_postprocess (ELEMENT *ElementPtr)
{
	HELEMENT Laser;
	STARSHIP *StarShipPtr;
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
	initialize_fighterlaser (ElementPtr, &Laser);
	
	if (Laser)
	{
		ELEMENT *LaserPtr;

		LockElement (Laser, &LaserPtr);
		SetElementStarShip (LaserPtr, StarShipPtr);

		// FIGHTER ZAP
		ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 2), LaserPtr);

		UnlockElement (Laser);
		PutElement (Laser);
	}
	
	ElementPtr->postprocess_func = 0;
	ElementPtr->thrust_wait = FIGHTER_WEAPON_WAIT;
}*/

static void
fighter_preprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;
	GetElementStarShip (ElementPtr, &StarShipPtr);

	++StarShipPtr->RaceDescPtr->characteristics.special_wait;
	
	if (FIGHTER_LIFE - ElementPtr->life_span > TRACK_THRESHOLD && !(ElementPtr->state_flags & CHANGING))
	{
		BOOLEAN Enroute;
		COUNT orig_facing, facing;
		SIZE delta_x, delta_y;
		ELEMENT *eptr;

		Enroute = TRUE;

		delta_x = StarShipPtr->RaceDescPtr->ship_info.crew_level;
		delta_y = ElementPtr->life_span;

		orig_facing = facing = GetFrameIndex (ElementPtr->current.image.frame);
		if (((delta_y & 1) || ElementPtr->hTarget || TrackShip (ElementPtr, &facing) >= 0)
				&& (delta_x == 0 || delta_y >= ONE_WAY_FLIGHT))
			ElementPtr->state_flags |= IGNORE_SIMILAR;
		else if (delta_x)
		{
			LockElement (StarShipPtr->hShip, &eptr);
			delta_x = eptr->current.location.x - ElementPtr->current.location.x;
			delta_y = eptr->current.location.y - ElementPtr->current.location.y;
			UnlockElement (StarShipPtr->hShip);
			delta_x = WRAP_DELTA_X (delta_x);
			delta_y = WRAP_DELTA_Y (delta_y);
			facing = NORMALIZE_FACING (ANGLE_TO_FACING (ARCTAN (delta_x, delta_y)));

			ElementPtr->state_flags &= ~IGNORE_SIMILAR;

			Enroute = FALSE;
		}

		if (ElementPtr->thrust_wait > 0)
			--ElementPtr->thrust_wait;

		if (ElementPtr->hTarget)
		{
			LockElement (ElementPtr->hTarget, &eptr);
			delta_x = eptr->current.location.x - ElementPtr->current.location.x;
			delta_y = eptr->current.location.y - ElementPtr->current.location.y;
			UnlockElement (ElementPtr->hTarget);
			delta_x = WRAP_DELTA_X (delta_x);
			delta_y = WRAP_DELTA_Y (delta_y);

			if (ElementPtr->thrust_wait == 0
					&& abs (delta_x) < FIGHTER_LASER_RANGE * 3 / 4
					&& abs (delta_y) < FIGHTER_LASER_RANGE * 3 / 4
					&& delta_x * delta_x + delta_y * delta_y < 
						(FIGHTER_LASER_RANGE * 3 / 4) * (FIGHTER_LASER_RANGE * 3 / 4))
			{
				HELEMENT Laser;

				ElementPtr->thrust_wait =
					(BYTE)NORMALIZE_FACING (ANGLE_TO_FACING (ARCTAN (delta_x, delta_y)));

				initialize_fighterlaser (ElementPtr, &Laser);
				if (Laser)
				{
					ELEMENT *LaserPtr;

					LockElement (Laser, &LaserPtr);
					SetElementStarShip (LaserPtr, StarShipPtr);

					// FIGHTER ZAP
					ProcessSound (SetAbsSoundIndex
						(StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 2), LaserPtr);

					UnlockElement (Laser);
					PutElement (Laser);
				}
				
				ElementPtr->thrust_wait = FIGHTER_WEAPON_WAIT;
			}

			if (Enroute)
			{
				facing = GetFrameIndex (eptr->current.image.frame);
				if (ElementPtr->turn_wait & LEFT)
				{
					delta_x += COSINE (FACING_TO_ANGLE (facing - 4), DISPLAY_TO_WORLD (30));
					delta_y += SINE (FACING_TO_ANGLE (facing - 4), DISPLAY_TO_WORLD (30));
				}
				else
				{
					delta_x += COSINE (FACING_TO_ANGLE (facing + 4), DISPLAY_TO_WORLD (30));
					delta_y += SINE (FACING_TO_ANGLE (facing + 4), DISPLAY_TO_WORLD (30));
				}
				facing = NORMALIZE_FACING (ANGLE_TO_FACING (ARCTAN (delta_x, delta_y)));
			}
		}
		ElementPtr->state_flags |= CHANGING;

		if (facing != orig_facing)
			ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->next.image.frame, facing);
		
		SetVelocityVector (&ElementPtr->velocity, FIGHTER_SPEED, facing);
	}
}

static void
fighter_collision (ELEMENT *ElementPtr0, POINT *pPt0,
		ELEMENT *ElementPtr1, POINT *pPt1)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr0, &StarShipPtr);
	if (GRAVITY_MASS (ElementPtr1->mass_points))
	{
		HELEMENT hFighterElement;

		hFighterElement = AllocElement ();
		if (hFighterElement)
		{
			COUNT primIndex, travel_facing;
			SIZE delta_facing;
			ELEMENT *FighterElementPtr;

			LockElement (hFighterElement, &FighterElementPtr);
			primIndex = FighterElementPtr->PrimIndex;
			*FighterElementPtr = *ElementPtr0;
			FighterElementPtr->PrimIndex = primIndex;
			(GLOBAL (DisplayArray))[primIndex] = (GLOBAL (DisplayArray))[ElementPtr0->PrimIndex];
			FighterElementPtr->state_flags &= ~PRE_PROCESS;
			FighterElementPtr->state_flags |= CHANGING;
			FighterElementPtr->next = FighterElementPtr->current;
			travel_facing = GetVelocityTravelAngle (&FighterElementPtr->velocity);
			delta_facing = NORMALIZE_ANGLE (ARCTAN (pPt1->x - pPt0->x, pPt1->y - pPt0->y)- travel_facing);
		
			if (delta_facing == 0)
			{
				if (FighterElementPtr->turn_wait & LEFT)
					travel_facing -= QUADRANT;
				else
					travel_facing += QUADRANT;
			}
			else if (delta_facing <= HALF_CIRCLE)
				travel_facing -= QUADRANT;
			else
				travel_facing += QUADRANT;

			travel_facing = NORMALIZE_FACING (ANGLE_TO_FACING (NORMALIZE_ANGLE (travel_facing)));
			FighterElementPtr->next.image.frame =
					SetAbsFrameIndex (FighterElementPtr->next.image.frame,travel_facing);
			SetVelocityVector (&FighterElementPtr->velocity, FIGHTER_SPEED, travel_facing);
			UnlockElement (hFighterElement);

			PutElement (hFighterElement);
		}

		ElementPtr0->state_flags |= DISAPPEARING | COLLISION;
	}
	else if (ElementPtr0->pParent != ElementPtr1->pParent)
	{
		ElementPtr0->blast_offset = 0;
		weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
		ElementPtr0->state_flags |= DISAPPEARING | COLLISION;
	}
	else if (ElementPtr1->state_flags & PLAYER_SHIP)
	{
		ProcessSound (SetAbsSoundIndex (
						/* FIGHTERS_RETURN */
				StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 3), ElementPtr1);
		DeltaCrew (ElementPtr1, 1);
		ElementPtr0->state_flags |= DISAPPEARING | COLLISION;
	}

	if (ElementPtr0->state_flags & DISAPPEARING)
	{
		ElementPtr0->state_flags &= ~DISAPPEARING;

		ElementPtr0->hit_points = 0;
		ElementPtr0->life_span = 0;
		ElementPtr0->state_flags |= NONSOLID;

		--StarShipPtr->RaceDescPtr->characteristics.special_wait;
	}
}

static void
spawn_fighters (ELEMENT *ElementPtr)
{
	SIZE i;
	COUNT facing;
	SIZE delta_x, delta_y;
	HELEMENT hFighterElement;
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	facing = StarShipPtr->ShipFacing + ANGLE_TO_FACING (HALF_CIRCLE);
	delta_x = COSINE (FACING_TO_ANGLE (facing), DISPLAY_TO_WORLD (14));
	delta_y = SINE (FACING_TO_ANGLE (facing), DISPLAY_TO_WORLD (14));

	i = ElementPtr->crew_level > 2 ? 2 : 1;
	while (i-- && (hFighterElement = AllocElement ()))
	{
		SIZE sx, sy;
		COUNT fighter_facing;
		ELEMENT *FighterElementPtr;

		DeltaCrew (ElementPtr, -1);

		PutElement (hFighterElement);
		LockElement (hFighterElement, &FighterElementPtr);
		FighterElementPtr->hit_points = 1;
		FighterElementPtr->mass_points = 0;
		FighterElementPtr->thrust_wait = TRACK_THRESHOLD + 1;
		FighterElementPtr->state_flags = APPEARING
				| FINITE_LIFE | CREW_OBJECT | IGNORE_SIMILAR
				| (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));
		FighterElementPtr->life_span = FIGHTER_LIFE;
		SetPrimType (&(GLOBAL (DisplayArray))[FighterElementPtr->PrimIndex], STAMP_PRIM);
		{
			FighterElementPtr->preprocess_func = fighter_preprocess;
			FighterElementPtr->postprocess_func = 0;
			FighterElementPtr->collision_func = fighter_collision;
			FighterElementPtr->death_func = NULL;
		}

		FighterElementPtr->current.location = ElementPtr->next.location;
		if (i == 1)
		{
			FighterElementPtr->turn_wait = LEFT;
			fighter_facing = NORMALIZE_FACING (facing + 2);
			FighterElementPtr->current.location.x += delta_x - delta_y;
			FighterElementPtr->current.location.y += delta_y + delta_x;
		}
		else
		{
			FighterElementPtr->turn_wait = RIGHT;
			fighter_facing = NORMALIZE_FACING (facing - 2);
			FighterElementPtr->current.location.x += delta_x + delta_y;
			FighterElementPtr->current.location.y += delta_y - delta_x;
		}
		
		sx = COSINE (FACING_TO_ANGLE (fighter_facing), WORLD_TO_VELOCITY (FIGHTER_SPEED));
		sy = SINE (FACING_TO_ANGLE (fighter_facing), WORLD_TO_VELOCITY (FIGHTER_SPEED));
		SetVelocityComponents (&FighterElementPtr->velocity, sx, sy);
		FighterElementPtr->current.location.x -= VELOCITY_TO_WORLD (sx);
		FighterElementPtr->current.location.y -= VELOCITY_TO_WORLD (sy);

		FighterElementPtr->current.image.farray = StarShipPtr->RaceDescPtr->ship_data.special;
		FighterElementPtr->current.image.frame = SetAbsFrameIndex (StarShipPtr->RaceDescPtr->ship_data.special[0], fighter_facing);
		SetElementStarShip (FighterElementPtr, StarShipPtr);
		UnlockElement (hFighterElement);
	}
}

static void
isd_intelligence (ELEMENT *ShipPtr, EVALUATE_DESC *ObjectsOfConcern, COUNT ConcernCounter)
{
	STARSHIP *StarShipPtr;
	EVALUATE_DESC *lpEvalDesc;

	GetElementStarShip (ShipPtr, &StarShipPtr);

	ObjectsOfConcern[ENEMY_SHIP_INDEX].MoveState = PURSUE;
	lpEvalDesc = &ObjectsOfConcern[ENEMY_WEAPON_INDEX];

	if (lpEvalDesc->ObjectPtr
			&& lpEvalDesc->MoveState == ENTICE
			&& (!(lpEvalDesc->ObjectPtr->state_flags & CREW_OBJECT)
			|| lpEvalDesc->which_turn <= 8)
			&& (!(lpEvalDesc->ObjectPtr->state_flags & FINITE_LIFE)
			|| (lpEvalDesc->ObjectPtr->mass_points >= 4
			&& lpEvalDesc->which_turn == 2
			&& ObjectsOfConcern[ENEMY_SHIP_INDEX].which_turn > 16)))
		lpEvalDesc->MoveState = PURSUE;

	// Basic ship intelligence
	ship_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter);

	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	{
		STARSHIP *EnemyStarShipPtr;

		if (lpEvalDesc->ObjectPtr)
		{
			GetElementStarShip (lpEvalDesc->ObjectPtr, &EnemyStarShipPtr);

			// ISD will attack if the enemy is within approximate reach.
			if (ship_weapons (ShipPtr, ObjectsOfConcern->ObjectPtr, DISPLAY_TO_WORLD (25)))
				StarShipPtr->ship_input_state |= WEAPON;
		}
		if (StarShipPtr->special_counter == 0
				&& lpEvalDesc->ObjectPtr
				&& StarShipPtr->RaceDescPtr->ship_info.crew_level >
				(StarShipPtr->RaceDescPtr->ship_info.max_crew >> 2)
				&& !(EnemyStarShipPtr->RaceDescPtr->ship_info.ship_flags
					& (LIGHT_POINT_DEFENSE | HEAVY_POINT_DEFENSE))
				&& (StarShipPtr->RaceDescPtr->characteristics.special_wait < 6
				|| (MANEUVERABILITY (&EnemyStarShipPtr->RaceDescPtr->cyborg_control) <= SLOW_SHIP
				&& !(EnemyStarShipPtr->cur_status_flags & SHIP_BEYOND_MAX_SPEED))
				|| (lpEvalDesc->which_turn <= 12
				&& (StarShipPtr->ship_input_state & (LEFT | RIGHT))
				&& StarShipPtr->RaceDescPtr->ship_info.energy_level >=
				(BYTE)(StarShipPtr->RaceDescPtr->ship_info.max_energy >> 1))))
			StarShipPtr->ship_input_state |= SPECIAL;
		else
			StarShipPtr->ship_input_state &= ~SPECIAL;
	}

	StarShipPtr->RaceDescPtr->characteristics.special_wait = 0;
}

static void
initialize_autoturret (ELEMENT *ElementPtr)
{
	BYTE weakest;
	STARSHIP *StarShipPtr;
	HELEMENT hObject, hNextObject, hBestObject;
	ELEMENT *ShipPtr, *ObjectPtr;
	SIZE delta_x, delta_y;
	long dist, best_dist;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	hBestObject = 0;
	best_dist = AUTOTURRET_RANGE + 1;
	weakest = 255;
	LockElement (StarShipPtr->hShip, &ShipPtr);

	// Is an enemy object within range?
	for (hObject = GetPredElement (ElementPtr);
			hObject; hObject = hNextObject)
	{
		LockElement (hObject, &ObjectPtr);
		hNextObject = GetPredElement (ObjectPtr);
		if (((ObjectPtr->state_flags | ShipPtr->state_flags)
			& (GOOD_GUY | BAD_GUY)) == (GOOD_GUY | BAD_GUY)
			&& CollisionPossible (ObjectPtr, ShipPtr)
			&& !OBJECT_CLOAKED (ObjectPtr))
		{
			delta_x = ObjectPtr->next.location.x
					- ShipPtr->next.location.x;
			delta_y = ObjectPtr->next.location.y
					- ShipPtr->next.location.y;
			if (delta_x < 0)
				delta_x = -delta_x;
			if (delta_y < 0)
				delta_y = -delta_y;

			// Range check.
			if (delta_x <= AUTOTURRET_RANGE
				&& delta_y <= AUTOTURRET_RANGE
				&& (dist = (long)delta_x * delta_x + (long)delta_y * delta_y)
				<= (long)AUTOTURRET_RANGE * AUTOTURRET_RANGE)
			{
				// The enemy ship is the highest priority target.
				if (ObjectPtr->state_flags & PLAYER_SHIP)
				{
					hBestObject = hObject;
					best_dist = 0;
					weakest = 0;
				}
				// Otherwise target an enemy projectile. Lower hitpoints and closer proximity are preferable.
				else if (ObjectPtr->hit_points < weakest
					|| (ObjectPtr->hit_points == weakest
					&& dist < best_dist))
				{
					hBestObject = hObject;
					best_dist = dist;
					weakest = ObjectPtr->hit_points;
				}
			}
		}
		UnlockElement (hObject);
	}

	// Fire autoturret.
	if (hBestObject)
	{
		COUNT num_frames;
		HELEMENT hAutoLaser;
		MISSILE_BLOCK MissileBlock;

		LockElement (hBestObject, &ObjectPtr);

		delta_x = ObjectPtr->current.location.x
				- ShipPtr->current.location.x;
		delta_x = WRAP_DELTA_X (delta_x);
		delta_y = ObjectPtr->current.location.y
				- ShipPtr->current.location.y;
		delta_y = WRAP_DELTA_Y (delta_y);
	
		num_frames = (square_root ((long)delta_x * delta_x
				+ (long)delta_y * delta_y)) / MISSILE_SPEED;
		if (num_frames == 0)
			num_frames = 1;
	
		GetNextVelocityComponents (&ObjectPtr->velocity,
				&delta_x, &delta_y, num_frames);
	
		// This calculation causes the autoturret to lead its target.
		delta_x = (ObjectPtr->current.location.x + (delta_x / 2))
				- ShipPtr->current.location.x;
		delta_y = (ObjectPtr->current.location.y + (delta_y / 2))
				- ShipPtr->current.location.y;

		MissileBlock.cx = ShipPtr->next.location.x;
		MissileBlock.cy = ShipPtr->next.location.y;
		MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
		MissileBlock.face = MissileBlock.index = NORMALIZE_FACING (ANGLE_TO_FACING (ARCTAN (delta_x, delta_y)));
		MissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY)) | IGNORE_SIMILAR;
		MissileBlock.pixoffs = AUTOTURRET_OFFSET;
		MissileBlock.speed = MISSILE_SPEED;
		MissileBlock.hit_points = MISSILE_HITS;
		MissileBlock.damage = MISSILE_DAMAGE;
		MissileBlock.life = MISSILE_LIFE;
		MissileBlock.preprocess_func = NULL;
		MissileBlock.blast_offs = MISSILE_OFFSET;
		
		hAutoLaser = initialize_missile (&MissileBlock);
		if (hAutoLaser)
		{
			ELEMENT *AutoLaserPtr;

			LockElement (hAutoLaser, &AutoLaserPtr);
			SetElementStarShip (AutoLaserPtr, StarShipPtr);
			AutoLaserPtr->hTarget = 0;
			ProcessSound (SetAbsSoundIndex (
					/* Primary weapon sound */
			StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 0), ElementPtr);
			UnlockElement (hAutoLaser);
			PutElement (hAutoLaser);

			DeltaEnergy (ElementPtr, -AUXILIARY_ENERGY_COST);
			StarShipPtr->auxiliary_counter = AUTOTURRET_WAIT;
		}

		UnlockElement (hBestObject);
	}

	UnlockElement (StarShipPtr->hShip);
}

static void
isd_postprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
		
	if ((StarShipPtr->cur_status_flags & SPECIAL)
			&& ElementPtr->crew_level > 1
			&& StarShipPtr->special_counter == 0
			&& DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST))
	{
		ProcessSound (SetAbsSoundIndex (
						/* LAUNCH_FIGHTERS */
				StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);
		spawn_fighters (ElementPtr);

		StarShipPtr->special_counter = SPECIAL_WAIT;
	}

	if (StarShipPtr->auxiliary_counter == 0
		&& StarShipPtr->RaceDescPtr->ship_info.energy_level >= AUXILIARY_ENERGY_COST)
	{
		HELEMENT hDefense;

		hDefense = AllocElement ();
		if (hDefense)
		{
			ELEMENT *DefensePtr;
			
			PutElement (hDefense);

			LockElement (hDefense, &DefensePtr);
			DefensePtr->state_flags = APPEARING | NONSOLID | FINITE_LIFE
					| (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));

			{
				ELEMENT *SuccPtr;

				LockElement (GetSuccElement (ElementPtr), &SuccPtr);
				DefensePtr->hTarget = GetPredElement (SuccPtr);
				UnlockElement (GetSuccElement (ElementPtr));

				DefensePtr->death_func = initialize_autoturret;
			}

			SetElementStarShip (DefensePtr, StarShipPtr);
			
			UnlockElement (hDefense);
		}
	}
}

RACE_DESC*
init_isd (void)
{
	RACE_DESC *RaceDescPtr;

	// isd_desc.preprocess_func = isd_preprocess;
	isd_desc.postprocess_func = isd_postprocess;
	isd_desc.init_weapon_func = initialize_turbolaser;
	isd_desc.cyborg_control.intelligence_func = isd_intelligence;

	RaceDescPtr = &isd_desc;

	return (RaceDescPtr);
}
