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
#include "foonfoon.h"
#include "resinst.h"

#include "uqm/globdata.h"
#include "libs/mathlib.h"
#include <math.h>

#define MAX_CREW 10
#define MAX_ENERGY 32
#define ENERGY_REGENERATION 1
#define WEAPON_ENERGY_COST 5
#define SPECIAL_ENERGY_COST 10
#define ENERGY_WAIT 2
#define MAX_THRUST 52
#define THRUST_INCREMENT 12
#define TURN_WAIT 1
#define THRUST_WAIT 1
#define WEAPON_WAIT 1
#define SPECIAL_WAIT 0

#define SHIP_MASS 2
#define MISSILE_SPEED DISPLAY_TO_WORLD (30)
#define MISSILE_LIFE 10
#define MISSILE_RANGE (MISSILE_SPEED * MISSILE_LIFE)

// Weapon specific
#define REVERSE_DIR (BYTE)(1 << 7)
#define FOCUSBALL_OFFSET (7 << RESOLUTION_FACTOR)
#define FOCUSBALL_FRAME_STARTINDEX 16
#define NUM_FOCUSBALL_ANIMS 3
#define NUM_SABERS 3
#define DERVISH_DEGENERATION (-1)
#define DERVISH_COOLDOWN_TIME 36
#define DERVISH_THRUST (80 << RESOLUTION_FACTOR) // JMS_GFX

static RACE_DESC foonfoon_desc =
{
	{ /* SHIP_INFO */
		FIRES_FORE | FIRES_AFT | SEEKING_SPECIAL | DONT_CHASE,
		18, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		FOONFOON_RACE_STRINGS,
		FOONFOON_ICON_MASK_PMAP_ANIM,
		FOONFOON_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		0, /* Initial SoI radius */
		{ /* Known location (center of SoI) */
			0, 0,
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
			FOONFOON_BIG_MASK_PMAP_ANIM,
			FOONFOON_MED_MASK_PMAP_ANIM,
			FOONFOON_SML_MASK_PMAP_ANIM,
		},
		{
			FOONFOON_PRIMARY_BIG_MASK_PMAP_ANIM,
			FOONFOON_PRIMARY_MED_MASK_PMAP_ANIM,
			FOONFOON_PRIMARY_SML_MASK_PMAP_ANIM,
		},
		{
			FOONFOON_SECONDARY_BIG_MASK_PMAP_ANIM,
			FOONFOON_SECONDARY_MED_MASK_PMAP_ANIM,
			FOONFOON_SECONDARY_SML_MASK_PMAP_ANIM,
		},
		{
			FOONFOON_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		FOONFOON_VICTORY_SONG,
		FOONFOON_SHIP_SOUNDS,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		MISSILE_RANGE,
		NULL,
	},
	(UNINIT_FUNC *) NULL,
	(PREPROCESS_FUNC *) NULL,
	(POSTPROCESS_FUNC *) NULL,
	(INIT_WEAPON_FUNC *) NULL,
	0,
	0, /* CodeRef */
};

// JMS_GFX
#define MAX_THRUST_2XRES 104
#define THRUST_INCREMENT_2XRES 24
#define MISSILE_SPEED_2XRES DISPLAY_TO_WORLD (60)
#define MISSILE_RANGE_2XRES (MISSILE_SPEED_2XRES * MISSILE_LIFE)

// JMS_GFX
static RACE_DESC foonfoon_desc_2xres =
{
	{ /* SHIP_INFO */
		FIRES_FORE | FIRES_AFT | SEEKING_SPECIAL | DONT_CHASE,
		18, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		FOONFOON_RACE_STRINGS,
		FOONFOON_ICON_MASK_PMAP_ANIM,
		FOONFOON_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		50 / SPHERE_RADIUS_INCREMENT * 2, /* Initial SoI radius */
		{ /* Known location (center of SoI) */
			7416, 9687,
		},
	},
	{
		MAX_THRUST_2XRES,
		THRUST_INCREMENT_2XRES,
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
			FOONFOON_BIG_MASK_PMAP_ANIM,
			FOONFOON_MED_MASK_PMAP_ANIM,
			FOONFOON_SML_MASK_PMAP_ANIM,
		},
		{
			FOONFOON_PRIMARY_BIG_MASK_PMAP_ANIM,
			FOONFOON_PRIMARY_MED_MASK_PMAP_ANIM,
			FOONFOON_PRIMARY_SML_MASK_PMAP_ANIM,
		},
		{
			FOONFOON_SECONDARY_BIG_MASK_PMAP_ANIM,
			FOONFOON_SECONDARY_MED_MASK_PMAP_ANIM,
			FOONFOON_SECONDARY_SML_MASK_PMAP_ANIM,
		},
		{
			FOONFOON_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		FOONFOON_VICTORY_SONG,
		FOONFOON_SHIP_SOUNDS,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		MISSILE_RANGE_2XRES,
		NULL,
	},
	(UNINIT_FUNC *) NULL,
	(PREPROCESS_FUNC *) NULL,
	(POSTPROCESS_FUNC *) NULL,
	(INIT_WEAPON_FUNC *) NULL,
	0,
	0, /* CodeRef */
};

// JMS_GFX
#define MAX_THRUST_4XRES 208
#define THRUST_INCREMENT_4XRES 48
#define MISSILE_SPEED_4XRES DISPLAY_TO_WORLD (120)
#define MISSILE_RANGE_4XRES (MISSILE_SPEED_2XRES * MISSILE_LIFE)

// JMS_GFX
static RACE_DESC foonfoon_desc_4xres =
{
	{ /* SHIP_INFO */
		FIRES_FORE | FIRES_AFT | SEEKING_SPECIAL | DONT_CHASE,
		18, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		FOONFOON_RACE_STRINGS,
		FOONFOON_ICON_MASK_PMAP_ANIM,
		FOONFOON_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		50 / SPHERE_RADIUS_INCREMENT * 2, /* Initial SoI radius */
		{ /* Known location (center of SoI) */
			7416, 9687,
		},
	},
	{
		MAX_THRUST_4XRES,
		THRUST_INCREMENT_4XRES,
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
			FOONFOON_BIG_MASK_PMAP_ANIM,
			FOONFOON_MED_MASK_PMAP_ANIM,
			FOONFOON_SML_MASK_PMAP_ANIM,
		},
		{
			FOONFOON_PRIMARY_BIG_MASK_PMAP_ANIM,
			FOONFOON_PRIMARY_MED_MASK_PMAP_ANIM,
			FOONFOON_PRIMARY_SML_MASK_PMAP_ANIM,
		},
		{
			FOONFOON_SECONDARY_BIG_MASK_PMAP_ANIM,
			FOONFOON_SECONDARY_MED_MASK_PMAP_ANIM,
			FOONFOON_SECONDARY_SML_MASK_PMAP_ANIM,
		},
		{
			FOONFOON_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		FOONFOON_VICTORY_SONG,
		FOONFOON_SHIP_SOUNDS,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		MISSILE_RANGE_4XRES,
		NULL,
	},
	(UNINIT_FUNC *) NULL,
	(PREPROCESS_FUNC *) NULL,
	(POSTPROCESS_FUNC *) NULL,
	(INIT_WEAPON_FUNC *) NULL,
	0,
	0, /* CodeRef */
};

static void
foonfoon_intelligence (ELEMENT *ShipPtr, EVALUATE_DESC *ObjectsOfConcern, COUNT ConcernCounter)
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
		
		travel_facing = NORMALIZE_FACING (ANGLE_TO_FACING (GetVelocityTravelAngle (&ShipPtr->velocity) + HALF_CIRCLE) );
		delta_x = lpEvalDesc->ObjectPtr->current.location.x - ShipPtr->current.location.x;
		delta_y = lpEvalDesc->ObjectPtr->current.location.y - ShipPtr->current.location.y;
		direction_facing = NORMALIZE_FACING (ANGLE_TO_FACING (ARCTAN (delta_x, delta_y)));
		
		if (NORMALIZE_FACING (direction_facing
							  - (StarShipPtr->ShipFacing + ANGLE_TO_FACING (HALF_CIRCLE))
							  + ANGLE_TO_FACING (QUADRANT))
			<= ANGLE_TO_FACING (HALF_CIRCLE)
			&& (lpEvalDesc->which_turn <= 8
				|| NORMALIZE_FACING (direction_facing
					+ ANGLE_TO_FACING (HALF_CIRCLE)
					- ANGLE_TO_FACING (GetVelocityTravelAngle (&lpEvalDesc->ObjectPtr->velocity))
					+ ANGLE_TO_FACING (QUADRANT))
				<= ANGLE_TO_FACING (HALF_CIRCLE))
			&& (!(StarShipPtr->cur_status_flags &
				  (SHIP_BEYOND_MAX_SPEED | SHIP_IN_GRAVITY_WELL))
				|| NORMALIZE_FACING (direction_facing - travel_facing + ANGLE_TO_FACING (QUADRANT))
				<= ANGLE_TO_FACING (HALF_CIRCLE)))
			StarShipPtr->ship_input_state |= SPECIAL;
	}
}

// Forward declaration.
static COUNT 
initialize_focusball (ELEMENT *ShipPtr, HELEMENT FocusArray[]);

// This animates the focusball between frames 0...2.
static void
focusball_preprocess (ELEMENT *ElementPtr)
{
	COUNT frame_index;

	frame_index = GetFrameIndex (ElementPtr->current.image.frame) - FOCUSBALL_FRAME_STARTINDEX;
	if (    ((ElementPtr->turn_wait & REVERSE_DIR) && (frame_index % NUM_FOCUSBALL_ANIMS) != 0)
		|| (!(ElementPtr->turn_wait & REVERSE_DIR) && ((frame_index + 1) % NUM_FOCUSBALL_ANIMS) == 0))
	{
		--frame_index;
		ElementPtr->turn_wait |= REVERSE_DIR;
	}
	else
	{
		++frame_index;
		ElementPtr->turn_wait &= ~REVERSE_DIR;
	}
		
	ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->current.image.frame, FOCUSBALL_FRAME_STARTINDEX + frame_index);
	ElementPtr->state_flags |= CHANGING;
}

// This animates the focusball between frames 0...2 and creates sound for activating the focusball.
static void
focusball_postprocess (ELEMENT *ElementPtr)
{
	if (ElementPtr->state_flags & APPEARING)
		ZeroVelocityComponents (&ElementPtr->velocity);
	else
	{
		HELEMENT hFocusBall;
		ELEMENT *EPtr;
		ELEMENT *ShipPtr;
		STARSHIP *StarShipPtr;
		
		GetElementStarShip (ElementPtr, &StarShipPtr);
		LockElement (StarShipPtr->hShip, &ShipPtr);
		initialize_focusball (ShipPtr, &hFocusBall);
		DeltaEnergy (ShipPtr, 0);
		UnlockElement (StarShipPtr->hShip);
		
		LockElement (hFocusBall, &EPtr);
		
		EPtr->current.image.frame = ElementPtr->current.image.frame;
		EPtr->turn_wait = ElementPtr->turn_wait;
		
		SetElementStarShip (EPtr, StarShipPtr);
		
			COUNT frame_index;
			
			frame_index = GetFrameIndex (EPtr->current.image.frame) - FOCUSBALL_FRAME_STARTINDEX;
			if		(((EPtr->turn_wait & REVERSE_DIR) && (frame_index % NUM_FOCUSBALL_ANIMS) != 0)
				||  (!(EPtr->turn_wait & REVERSE_DIR) && ((frame_index + 1) % NUM_FOCUSBALL_ANIMS) == 0))
		{
			--frame_index;
			EPtr->turn_wait |= REVERSE_DIR;
		}
		else
		{
			++frame_index;
			EPtr->turn_wait &= ~REVERSE_DIR;
		}
			
		EPtr->current.image.frame = SetAbsFrameIndex (EPtr->current.image.frame, FOCUSBALL_FRAME_STARTINDEX + frame_index);
		
		if (StarShipPtr->cur_status_flags & StarShipPtr->old_status_flags & WEAPON)
			StarShipPtr->weapon_counter = WEAPON_WAIT;
		else if (!(StarShipPtr->cur_status_flags & StarShipPtr->old_status_flags & SPECIAL))
		{
			EPtr->life_span = 0;
			EPtr->preprocess_func = focusball_preprocess;
			EPtr->postprocess_func = 0;
			
			ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 0), EPtr);
		}
		
		UnlockElement (hFocusBall);
		PutElement (hFocusBall);
		
		SetPrimType (&(GLOBAL (DisplayArray))[ElementPtr->PrimIndex],  NO_PRIM);
		ElementPtr->state_flags |= NONSOLID;
	}
}

// This generates the focus ball.
static COUNT
initialize_focusball (ELEMENT *ShipPtr, HELEMENT FocusArray[])
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.face = StarShipPtr->ShipFacing;
	MissileBlock.index = FOCUSBALL_FRAME_STARTINDEX;
	MissileBlock.sender = ShipPtr->playerNr;
	MissileBlock.flags =  NONSOLID | IGNORE_SIMILAR;
	MissileBlock.pixoffs = FOCUSBALL_OFFSET;
	MissileBlock.speed = DISPLAY_TO_WORLD (FOCUSBALL_OFFSET);
	MissileBlock.hit_points = 1;
	MissileBlock.damage = 0;
	MissileBlock.life = 2;
	MissileBlock.preprocess_func = 0;
	MissileBlock.blast_offs = 0;
	FocusArray[0] = initialize_missile (&MissileBlock);
	
	if (FocusArray[0])
	{
		ELEMENT *FocusPtr;
		
		LockElement (FocusArray[0], &FocusPtr);
		FocusPtr->postprocess_func = focusball_postprocess;
		UnlockElement (FocusArray[0]);
	}
	
	return (1);
}

static void
saber_collision (ELEMENT *ElementPtr0, POINT *pPt0, ELEMENT *ElementPtr1, POINT *pPt1)
{
	HELEMENT hBlastElement;
	
	hBlastElement = weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
	if (hBlastElement)
	{
		RemoveElement (hBlastElement);
		FreeElement (hBlastElement);
		
		ElementPtr0->state_flags &= ~DISAPPEARING;
	}
}

// This generates the wide saber.
static COUNT
initialize_saber (ELEMENT *ShipPtr, HELEMENT SaberArray[], COUNT facingfix)
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	COUNT facing; 
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	facing = (StarShipPtr->ShipFacing + NUM_SABERS - facingfix) % 16;
	
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray  = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.index   = MissileBlock.face = facing;
	MissileBlock.sender  = ShipPtr->playerNr;
	MissileBlock.flags   = IGNORE_SIMILAR;
	MissileBlock.pixoffs = FOCUSBALL_OFFSET;
	MissileBlock.speed   = DERVISH_THRUST;
	MissileBlock.damage  = 1;
	MissileBlock.life    = 1;
	MissileBlock.hit_points = 100;
	MissileBlock.preprocess_func = 0;
	MissileBlock.blast_offs      = 0;
			
	if ((SaberArray[0] = initialize_missile (&MissileBlock)))
	{
		ELEMENT *SaberPtr;
		
		LockElement (SaberArray[0], &SaberPtr);
		SetElementStarShip (SaberPtr, StarShipPtr);
		SaberPtr->collision_func = saber_collision;
		InitIntersectStartPoint (SaberPtr);
		InitIntersectEndPoint (SaberPtr);
		SaberPtr->IntersectControl.IntersectStamp.frame = StarShipPtr->RaceDescPtr->ship_data.weapon[facing];
		UnlockElement (SaberArray[0]);
	}
	
	return (1);
}

// This generates the focus ball in conjunction with the special key.
// The only difference is that this focusball is not postprocessed, but preprocessed.
// (For some reason, postprocessing crashes when initialize_focusball is called from foonfoon_postprocess)
static COUNT
initialize_special_focusball (ELEMENT *ShipPtr, HELEMENT FocusArray[])
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.face = StarShipPtr->ShipFacing;
	MissileBlock.index = FOCUSBALL_FRAME_STARTINDEX;
	MissileBlock.sender = ShipPtr->playerNr;
	MissileBlock.flags =  NONSOLID | IGNORE_SIMILAR;
	MissileBlock.pixoffs = FOCUSBALL_OFFSET;
	MissileBlock.speed = DERVISH_THRUST;
	MissileBlock.hit_points = 1;
	MissileBlock.damage = 0;
	MissileBlock.life = 2;
	MissileBlock.preprocess_func = 0;
	MissileBlock.blast_offs = 0;
	FocusArray[0] = initialize_missile (&MissileBlock);
	
	if (FocusArray[0])
	{
		ELEMENT *FocusPtr;
		
		LockElement (FocusArray[0], &FocusPtr);
		FocusPtr->preprocess_func = focusball_preprocess;
		UnlockElement (FocusArray[0]);
	}
	
	return (1);
}

static void
foonfoon_postprocess (ELEMENT *ElementPtr)
{
	RACE_DESC *RDPtr;
	STARSHIP *StarShipPtr;
	STATUS_FLAGS cur_status_flags;
	BYTE i;
	static BYTE had_pause = 1;
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
	RDPtr = StarShipPtr->RaceDescPtr;
	cur_status_flags = StarShipPtr->cur_status_flags;
	
	// After using special: Thrust is sloooow for a while.
	if (StarShipPtr->special_counter > 0)
	{
		RDPtr->characteristics.max_thrust   = (MAX_THRUST << RESOLUTION_FACTOR) / 4;
		RDPtr->characteristics.thrust_wait	= THRUST_WAIT;
		RDPtr->characteristics.turn_wait	= TURN_WAIT;
		
		// Battery charges normally when not in dervish mode.
		RDPtr->characteristics.energy_regeneration = ENERGY_REGENERATION;
	}
	
	// Special: The dervish mode.
	else if (StarShipPtr->cur_status_flags & SPECIAL
		&& StarShipPtr->special_counter == 0
		&& RDPtr->ship_info.energy_level > 0
		&& had_pause)
	{
		HELEMENT FocusBall;
		
		// Graphical nicety: Let's add a focusball to the mouth of the ship.
		initialize_special_focusball (ElementPtr, &FocusBall);
		if (FocusBall)
		{
			ELEMENT *FBMissilePtr;
			LockElement (FocusBall, &FBMissilePtr);
			
			UnlockElement (FocusBall);
			PutElement (FocusBall);
		}
		
		// The more sabre elements there are, the wider it is (and the more damage it deals).
		for (i = 0; i < NUM_SABERS; i++)
		{
			HELEMENT Saber;
			
			initialize_saber (ElementPtr, &Saber, i);
			if (Saber)
			{
				ELEMENT *SMissilePtr;
				LockElement (Saber, &SMissilePtr);
			
				UnlockElement (Saber);
				PutElement (Saber);
			}
		}
		
		// Turning rate & max speed increase.
		RDPtr->characteristics.thrust_wait = 0;
		RDPtr->characteristics.turn_wait   = 0;
		RDPtr->characteristics.max_thrust  = DERVISH_THRUST;
		
		// Battery slowly drops.
		RDPtr->characteristics.energy_regeneration = (BYTE)DERVISH_DEGENERATION;
	}
	// Not using special.
	else
	{
		// When dervish ends...
		if (StarShipPtr->old_status_flags & SPECIAL && StarShipPtr->special_counter == 0 && had_pause)
		{
			// Set a cooldown period before next dervish.
			StarShipPtr->special_counter = DERVISH_COOLDOWN_TIME;
			
			// Play the ending sound.
			ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);
			
			// Slow down.
			SetVelocityVector (&ElementPtr->velocity, ((MAX_THRUST << RESOLUTION_FACTOR) / 4), NORMALIZE_FACING (StarShipPtr->ShipFacing));
			
			// Prevent spamming the special key.
			cur_status_flags &= ~(SPECIAL);
			StarShipPtr->cur_status_flags = cur_status_flags;
			had_pause = 0;
		}
		
		// Turning rate & max speed are normal when not in dervish mode.
		RDPtr->characteristics.thrust_wait	= THRUST_WAIT;
		RDPtr->characteristics.turn_wait	= TURN_WAIT;
		RDPtr->characteristics.max_thrust   = MAX_THRUST << RESOLUTION_FACTOR;
		
		// Battery charges normally when not in dervish mode.
		RDPtr->characteristics.energy_regeneration = ENERGY_REGENERATION;
	}
	
	// The player must let go of the special key between dervishes.
	// This requirement eliminates all sorts of bugs.
	if (!(StarShipPtr->old_status_flags & SPECIAL) 
		&& !(StarShipPtr->cur_status_flags & SPECIAL)
		&& StarShipPtr->RaceDescPtr->ship_info.energy_level > SPECIAL_ENERGY_COST)
		had_pause = 1;
}

static void
foonfoon_preprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;
	STATUS_FLAGS cur_status_flags;
	static BYTE turning_left = 1;
	static BYTE released_special_since_last = 1;
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
	cur_status_flags = StarShipPtr->cur_status_flags;
	
	if (StarShipPtr->cur_status_flags & SPECIAL 
		&& StarShipPtr->special_counter == 0 
		&& released_special_since_last)
	{
		// Not enough juice for dervish mode.
		if ((StarShipPtr->RaceDescPtr->ship_info.energy_level < SPECIAL_ENERGY_COST && !(StarShipPtr->old_status_flags & SPECIAL))
			|| StarShipPtr->RaceDescPtr->ship_info.energy_level == 0)
		{
			// Flash the 'batt' text.
			DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST);
			
			// Prevent spamming the special key.
			cur_status_flags &= ~(SPECIAL);
			StarShipPtr->old_status_flags &= ~(SPECIAL);
			released_special_since_last = 0;
		}
		else if (released_special_since_last)
		{
			COUNT facing;
			SIZE  speedx, speedy;
			SDWORD totalspeed;
			
			// Can't turn or use primary weapon in dervish mode.
			cur_status_flags &= ~(THRUST | WEAPON | LEFT | RIGHT);
			
			// Find out the ship facing and speed.
			facing = StarShipPtr->ShipFacing;
			GetCurrentVelocityComponents (&ElementPtr->velocity, &speedx, &speedy);
			totalspeed = sqrt (speedx * speedx + speedy * speedy);

			// Upon pressing the special key, speed rapidly to the direction which the ship was heading.
			if (!(StarShipPtr->old_status_flags & SPECIAL)
				|| (StarShipPtr->old_status_flags & SPECIAL && StarShipPtr->RaceDescPtr->ship_info.energy_level >= SPECIAL_ENERGY_COST
					&& totalspeed < DERVISH_THRUST))
			{
				DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST);
				SetVelocityVector (&ElementPtr->velocity, DERVISH_THRUST, NORMALIZE_FACING (facing));
				
				// Dervish swoosh sound.
				ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);
				
				// Next time, turn into another direction.
				turning_left++;
				turning_left %=2;
			}
			
			// Turn ship around whilst in dervish mode.
			if (turning_left)
				facing += 2;
			else
				facing -= 2;
			facing %= 16;
			StarShipPtr->ShipFacing = facing;
			ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->current.image.frame, facing);
			
			cur_status_flags |= SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED;
		}
	}
	else if (!(StarShipPtr->old_status_flags & SPECIAL) 
			 && !(StarShipPtr->cur_status_flags & SPECIAL)
			 && StarShipPtr->RaceDescPtr->ship_info.energy_level > SPECIAL_ENERGY_COST)
		released_special_since_last = 1;
	
	StarShipPtr->cur_status_flags = cur_status_flags;
}

RACE_DESC*
init_foonfoon (void)
{
	RACE_DESC *RaceDescPtr;
	
	if (RESOLUTION_FACTOR == 0)
	{
		foonfoon_desc.postprocess_func = foonfoon_postprocess;
		foonfoon_desc.preprocess_func  = foonfoon_preprocess;
		foonfoon_desc.init_weapon_func = initialize_focusball;
		foonfoon_desc.cyborg_control.intelligence_func = foonfoon_intelligence;
		RaceDescPtr = &foonfoon_desc;
	}
	else if (RESOLUTION_FACTOR == 1)
	{
		foonfoon_desc_2xres.postprocess_func = foonfoon_postprocess;
		foonfoon_desc_2xres.preprocess_func  = foonfoon_preprocess;
		foonfoon_desc_2xres.init_weapon_func = initialize_focusball;
		foonfoon_desc_2xres.cyborg_control.intelligence_func = foonfoon_intelligence;
		RaceDescPtr = &foonfoon_desc_2xres;
	}
	else
	{
		foonfoon_desc_4xres.postprocess_func = foonfoon_postprocess;
		foonfoon_desc_4xres.preprocess_func  = foonfoon_preprocess;
		foonfoon_desc_4xres.init_weapon_func = initialize_focusball;
		foonfoon_desc_4xres.cyborg_control.intelligence_func = foonfoon_intelligence;
		RaceDescPtr = &foonfoon_desc_4xres;
	}
	
	return (RaceDescPtr);
}
