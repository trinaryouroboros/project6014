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
#include "libs/sound/sound.h" // For StopSource
#include <math.h> // For sqrt

#define MAX_CREW 12
#define MAX_ENERGY 32
#define ENERGY_REGENERATION 1
#define WEAPON_ENERGY_COST 1 
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
#define NUM_SHIP_FACINGS 16
#define FOCUSBALL_OFFSET (7 << RESOLUTION_FACTOR)
#define FOCUSBALL_FRAME_STARTINDEX 16
#define NUM_FOCUSBALL_ANIMS 3
#define NUM_SABERS 5
#define DERVISH_DEGENERATION (-1)
#define DERVISH_COOLDOWN_TIME 36 
#define DERVISH_THRUST (80 << RESOLUTION_FACTOR) // JMS_GFX

static RACE_DESC foonfoon_desc =
{
	{ /* SHIP_INFO */
		FIRES_FORE | IMMEDIATE_WEAPON,
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
			FOONFOON_BEAM_BIG_MASK_PMAP_ANIM,
			FOONFOON_BEAM_MED_MASK_PMAP_ANIM,
			FOONFOON_BEAM_SML_MASK_PMAP_ANIM,
		},
		{
			FOONFOON_SABRE_BIG_MASK_PMAP_ANIM,
			FOONFOON_SABRE_MED_MASK_PMAP_ANIM,
			FOONFOON_SABRE_SML_MASK_PMAP_ANIM,
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
		FIRES_FORE | IMMEDIATE_WEAPON,
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
			FOONFOON_BEAM_BIG_MASK_PMAP_ANIM,
			FOONFOON_BEAM_MED_MASK_PMAP_ANIM,
			FOONFOON_BEAM_SML_MASK_PMAP_ANIM,
		},
		{
			FOONFOON_SABRE_BIG_MASK_PMAP_ANIM,
			FOONFOON_SABRE_MED_MASK_PMAP_ANIM,
			FOONFOON_SABRE_SML_MASK_PMAP_ANIM,
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
		FIRES_FORE | IMMEDIATE_WEAPON,
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
			FOONFOON_BEAM_BIG_MASK_PMAP_ANIM,
			FOONFOON_BEAM_MED_MASK_PMAP_ANIM,
			FOONFOON_BEAM_SML_MASK_PMAP_ANIM,
		},
		{
			FOONFOON_SABRE_BIG_MASK_PMAP_ANIM,
			FOONFOON_SABRE_MED_MASK_PMAP_ANIM,
			FOONFOON_SABRE_SML_MASK_PMAP_ANIM,
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
	
}

// Forward declarations.
static COUNT 
initialize_focusball (ELEMENT *ShipPtr, HELEMENT FocusArray[]);

static COUNT
initialize_beam (ELEMENT *ShipPtr, HELEMENT FocusArray[]);

// This animates the focusball between frames 0...2.
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
		COUNT frame_index;
		
		GetElementStarShip (ElementPtr, &StarShipPtr);
		LockElement (StarShipPtr->hShip, &ShipPtr);
		initialize_focusball (ShipPtr, &hFocusBall);
		if ((StarShipPtr->cur_status_flags | StarShipPtr->old_status_flags) & WEAPON)
			DeltaEnergy (ShipPtr, 0);
		UnlockElement (StarShipPtr->hShip);
		
		LockElement (hFocusBall, &EPtr);
		
		EPtr->current.image.frame = ElementPtr->current.image.frame;
		EPtr->turn_wait = ElementPtr->turn_wait;
		
		SetElementStarShip (EPtr, StarShipPtr);
		
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
		
		// When player releases WEAPON or SPECIAL key, kill the focusball.
		if (!(StarShipPtr->cur_status_flags & StarShipPtr->old_status_flags & WEAPON)
			&& !(StarShipPtr->cur_status_flags & StarShipPtr->old_status_flags & SPECIAL))
		{
			EPtr->life_span = 0;
			EPtr->preprocess_func = 0;
			EPtr->postprocess_func = 0;
		}
		
		UnlockElement (hFocusBall);
		PutElement (hFocusBall);
		
		SetPrimType (&(GLOBAL (DisplayArray))[ElementPtr->PrimIndex],  NO_PRIM);
		ElementPtr->state_flags |= NONSOLID;
	}
}

// Collision function for both the primary beam and the secondary saber.
static void
saber_beam_collision (ELEMENT *ElementPtr0, POINT *pPt0, ELEMENT *ElementPtr1, POINT *pPt1)
{
	HELEMENT hBlastElement;
	STARSHIP *StarShipPtr;	

	GetElementStarShip (ElementPtr0, &StarShipPtr);
	StarShipPtr->weapon_counter = 0;
	
	hBlastElement = weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
	if (hBlastElement)
	{
		RemoveElement (hBlastElement);
		FreeElement (hBlastElement);
		
		ElementPtr0->state_flags &= ~DISAPPEARING;
	}
}

// This makes the beam primary stay alive for an indefinite time.
static void
beam_postprocess (ELEMENT *ElementPtr)
{
	if (ElementPtr->state_flags & APPEARING)
		ZeroVelocityComponents (&ElementPtr->velocity);
	else
	{
		HELEMENT hBeam;
		ELEMENT *EPtr;
		ELEMENT *ShipPtr;
		STARSHIP *StarShipPtr;
		
		GetElementStarShip (ElementPtr, &StarShipPtr);
		LockElement (StarShipPtr->hShip, &ShipPtr);
		initialize_beam (ShipPtr, &hBeam);
		DeltaEnergy (ShipPtr, 0);
		UnlockElement (StarShipPtr->hShip);
		
		LockElement (hBeam, &EPtr);
		SetElementStarShip (EPtr, StarShipPtr);
		
		if (StarShipPtr->cur_status_flags & StarShipPtr->old_status_flags & WEAPON)
			StarShipPtr->weapon_counter = WEAPON_WAIT;
		else
		{
			EPtr->life_span = 0;
			EPtr->preprocess_func = 0;
			EPtr->postprocess_func = 0;
		}
		
		UnlockElement (hBeam);
		PutElement (hBeam);
		
		SetPrimType (&(GLOBAL (DisplayArray))[ElementPtr->PrimIndex],  NO_PRIM);
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

// This generates the narrow beam AND a focusball.
static COUNT
initialize_beam_and_focusball (ELEMENT *ShipPtr, HELEMENT BeamArray[])
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	static BYTE damage_amount[NUM_SIDES] = {0};
	BYTE num_of_created_elements = 1;

	// Do not deal damage on every frame. This makes the beam weaker.
	damage_amount[ShipPtr->playerNr] = (damage_amount[ShipPtr->playerNr] + 1) % 3;
	GetElementStarShip (ShipPtr, &StarShipPtr);
	
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.face = StarShipPtr->ShipFacing;
	MissileBlock.index = StarShipPtr->ShipFacing;
	MissileBlock.sender = ShipPtr->playerNr;
	MissileBlock.flags =  IGNORE_SIMILAR;
	MissileBlock.pixoffs = FOCUSBALL_OFFSET;
	MissileBlock.speed = DISPLAY_TO_WORLD (FOCUSBALL_OFFSET);
	MissileBlock.hit_points = 100;
	MissileBlock.damage = (damage_amount[ShipPtr->playerNr] == 0);
	MissileBlock.life = 2;
	MissileBlock.preprocess_func = 0;
	MissileBlock.blast_offs = 0;

	// Beam
	BeamArray[0] = initialize_missile (&MissileBlock);

	if (BeamArray[0])
	{
		ELEMENT *BeamPtr;
		
		LockElement (BeamArray[0], &BeamPtr);
		SetElementStarShip (BeamPtr, StarShipPtr);
		BeamPtr->collision_func = saber_beam_collision;
		BeamPtr->postprocess_func = beam_postprocess;
		InitIntersectStartPoint (BeamPtr);
		InitIntersectEndPoint (BeamPtr);
		BeamPtr->IntersectControl.IntersectStamp.frame = StarShipPtr->RaceDescPtr->ship_data.weapon[StarShipPtr->ShipFacing];
		UnlockElement (BeamArray[0]);
	}
	
	// Focusball
	if (!(StarShipPtr->old_status_flags & WEAPON))
	{
		MissileBlock.damage = 0;
		MissileBlock.index = FOCUSBALL_FRAME_STARTINDEX;
		MissileBlock.flags =  NONSOLID | IGNORE_SIMILAR;
		BeamArray[1] = initialize_missile (&MissileBlock);
	
		if (BeamArray[1])
		{
			ELEMENT *FocusPtr;
		
			LockElement (BeamArray[1], &FocusPtr);
			FocusPtr->postprocess_func = focusball_postprocess;
			UnlockElement (BeamArray[1]);
		
			num_of_created_elements++;
		}
	}
	
	// Beam is the first and (possible) focusball is the second.
	return (num_of_created_elements);
}

// This generates the narrow beam.
static COUNT
initialize_beam (ELEMENT *ShipPtr, HELEMENT BeamArray[])
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	static BYTE damage_amount[NUM_SIDES] = {0};
	
	// Do not deal damage on every frame. This makes the beam weaker.
	damage_amount[ShipPtr->playerNr] = (damage_amount[ShipPtr->playerNr] + 1) % 3;
	GetElementStarShip (ShipPtr, &StarShipPtr);
	
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.face = StarShipPtr->ShipFacing;
	MissileBlock.index = StarShipPtr->ShipFacing;
	MissileBlock.sender = ShipPtr->playerNr;
	MissileBlock.flags =  IGNORE_SIMILAR;
	MissileBlock.pixoffs = FOCUSBALL_OFFSET;
	MissileBlock.speed = DISPLAY_TO_WORLD (FOCUSBALL_OFFSET);
	MissileBlock.hit_points = 100;
	MissileBlock.damage = (damage_amount[ShipPtr->playerNr] == 0);
	MissileBlock.life = 2;
	MissileBlock.preprocess_func = 0;
	MissileBlock.blast_offs = 0;
	
	BeamArray[0] = initialize_missile (&MissileBlock);
	
	if (BeamArray[0])
	{
		ELEMENT *BeamPtr;
		
		LockElement (BeamArray[0], &BeamPtr);
		SetElementStarShip (BeamPtr, StarShipPtr);
		BeamPtr->collision_func = saber_beam_collision;
		BeamPtr->postprocess_func = beam_postprocess;
		InitIntersectStartPoint (BeamPtr);
		InitIntersectEndPoint (BeamPtr);
		BeamPtr->IntersectControl.IntersectStamp.frame = StarShipPtr->RaceDescPtr->ship_data.weapon[StarShipPtr->ShipFacing];
		UnlockElement (BeamArray[0]);
	}
	
	return (1);
}

// This generates the wide saber.
static COUNT
initialize_saber (ELEMENT *ShipPtr, HELEMENT SaberArray[], COUNT facingfix)
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	COUNT facing; 
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	facing = (StarShipPtr->ShipFacing + NUM_SABERS - 1 - facingfix) % NUM_SHIP_FACINGS;
	
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray  = StarShipPtr->RaceDescPtr->ship_data.special;
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
		SaberPtr->collision_func = saber_beam_collision;
		InitIntersectStartPoint (SaberPtr);
		InitIntersectEndPoint (SaberPtr);
		SaberPtr->IntersectControl.IntersectStamp.frame = StarShipPtr->RaceDescPtr->ship_data.special[facing];
		UnlockElement (SaberArray[0]);
	}
	
	return (1);
}

static void
foonfoon_postprocess (ELEMENT *ElementPtr)
{
	RACE_DESC *RDPtr;
	STARSHIP *StarShipPtr;
	STATUS_FLAGS cur_status_flags;
	BYTE frame_index, i;
	static BYTE not_had_pause[NUM_SIDES]	= {0};
	static BYTE focusball_exists[NUM_SIDES]	= {0};
	
	// For keeping the VUX limpet effects after spinning blade.
	static BYTE basic_max_thrust[3 * NUM_SIDES] = {MAX_THRUST, MAX_THRUST, MAX_THRUST_2XRES, MAX_THRUST_2XRES, MAX_THRUST_4XRES, MAX_THRUST_4XRES};
	static BYTE basic_thrust_wait[NUM_SIDES]= {THRUST_WAIT, THRUST_WAIT};
	static BYTE basic_turn_wait[NUM_SIDES]	= {TURN_WAIT, TURN_WAIT};
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
	RDPtr = StarShipPtr->RaceDescPtr;
	
	cur_status_flags = StarShipPtr->cur_status_flags;
	
	frame_index = GetFrameIndex (ElementPtr->current.image.frame);
	
	// Store the original turning rate & max speed if not in dervish mode, or the slow state after dervish.
	if (StarShipPtr->special_counter == 0 
		&& RDPtr->characteristics.turn_wait > 0
		&& frame_index < NUM_SHIP_FACINGS
		&& !(StarShipPtr->RaceDescPtr->ship_info.damage_flags & DAMAGE_THRUST))
	{
		basic_max_thrust[2 * RESOLUTION_FACTOR + ElementPtr->playerNr]  = RDPtr->characteristics.max_thrust;
		basic_thrust_wait[ElementPtr->playerNr] = RDPtr->characteristics.thrust_wait;
		basic_turn_wait[ElementPtr->playerNr]	= RDPtr->characteristics.turn_wait;
	}
	
	// After using special: Thrust is sloooow for a while.
	if (StarShipPtr->special_counter > 0)
	{
		// Slooooow.
		if ((MAX_THRUST << RESOLUTION_FACTOR) / 4 < basic_max_thrust[2 * RESOLUTION_FACTOR + ElementPtr->playerNr])
			RDPtr->characteristics.max_thrust = (MAX_THRUST << RESOLUTION_FACTOR) / 4;
		else
			RDPtr->characteristics.max_thrust = basic_max_thrust[2 * RESOLUTION_FACTOR + ElementPtr->playerNr];
		
		if (basic_thrust_wait[ElementPtr->playerNr] > 3)
			RDPtr->characteristics.thrust_wait	= basic_thrust_wait[ElementPtr->playerNr];
		else
			RDPtr->characteristics.thrust_wait	= 3;

		// Blue ion trail tells the player when dervish mode is unusable.
		StarShipPtr->RaceDescPtr->ship_info.damage_flags |= DAMAGE_THRUST;
		
		// Turning just returns to normal.
		RDPtr->characteristics.turn_wait = basic_turn_wait[ElementPtr->playerNr];
		
		// Battery charges normally when not in dervish mode.
		RDPtr->characteristics.energy_regeneration = ENERGY_REGENERATION;
		ElementPtr->state_flags |= CHANGING;
	}
	
	// Special: The dervish mode.
	else if (StarShipPtr->cur_status_flags & SPECIAL
			 && RDPtr->ship_info.energy_level > 0
			 && frame_index >= NUM_SHIP_FACINGS
			 && !not_had_pause[ElementPtr->playerNr])
	{
		// Add sabre elements.
		// The more sabres there are, the wider it is (and the more damage it deals).
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
		
		// Add a nice little focusball.
		if (!focusball_exists[ElementPtr->playerNr])
		{
			HELEMENT Focusball;
			
			initialize_focusball (ElementPtr, &Focusball);
			if (Focusball)
			{
				ELEMENT *FBMissilePtr;
				LockElement (Focusball, &FBMissilePtr);
				
				SetElementStarShip (FBMissilePtr, StarShipPtr);
				
				UnlockElement (Focusball);
				PutElement (Focusball);
			}
			
			// Don't create multiple focusballs. This makes 
			// a) the game run better, b) focusball animation visible. 
			focusball_exists[ElementPtr->playerNr] = 1;
		}
		
		// Turning rate & max speed increase.
		RDPtr->characteristics.thrust_wait = 0;
		RDPtr->characteristics.turn_wait   = 0;
		RDPtr->characteristics.max_thrust  = DERVISH_THRUST;
		StarShipPtr->RaceDescPtr->ship_info.damage_flags &= ~(DAMAGE_THRUST);
		
		// Battery slowly drops.
		RDPtr->characteristics.energy_regeneration = (BYTE)DERVISH_DEGENERATION;
		ElementPtr->state_flags |= CHANGING;
	}
	// Not using special.
	else
	{
		// When dervish ends...
		if ((StarShipPtr->old_status_flags & SPECIAL || RDPtr->ship_info.energy_level == 0)
			//&& StarShipPtr->special_counter == 0
			&& frame_index >= NUM_SHIP_FACINGS)
		{
			COUNT i;
			
			// Set a cooldown period before next dervish.
			StarShipPtr->special_counter = DERVISH_COOLDOWN_TIME;
			
			// Using dervish always drains the battery totally.
			DeltaEnergy (ElementPtr, -StarShipPtr->RaceDescPtr->ship_info.energy_level);
			
			// Slow down.
			SetVelocityVector (&ElementPtr->velocity, ((MAX_THRUST << RESOLUTION_FACTOR) / 4), NORMALIZE_FACING (StarShipPtr->ShipFacing));
			
			// We're not past max speed any more.
			cur_status_flags &= ~(SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED);
			
			// Return to normal graphics.
			if ((GetFrameIndex (ElementPtr->current.image.frame)) >= NUM_SHIP_FACINGS)
				ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->next.image.frame, StarShipPtr->ShipFacing);
			
			// End the spinning blade sound.
			for (i = FIRST_SFX_CHANNEL; i <= LAST_SFX_CHANNEL; ++i)
			{
				ELEMENT *posobj;
				if (!ChannelPlaying(i))
					continue;
				
				posobj = GetPositionalObject (i);
				if (posobj == ElementPtr)
					StopSource (i);
			}
			
			// Play the ending sound.
			ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);			
			
			// The focusball dies with the dervish mode.
			focusball_exists[ElementPtr->playerNr] = 0;
			
			// Prevent spamming the special key.
			cur_status_flags &= ~(SPECIAL);
			StarShipPtr->cur_status_flags = cur_status_flags;
			not_had_pause[ElementPtr->playerNr] = 1;
		}
		
		// Turning rate & max speed are normal when not in dervish mode.
		RDPtr->characteristics.max_thrust	= basic_max_thrust[2 * RESOLUTION_FACTOR + ElementPtr->playerNr];
		RDPtr->characteristics.thrust_wait	= basic_thrust_wait[ElementPtr->playerNr];
		RDPtr->characteristics.turn_wait	= basic_turn_wait[ElementPtr->playerNr];
		StarShipPtr->RaceDescPtr->ship_info.damage_flags &= ~(DAMAGE_THRUST);
		
		// Battery charges normally when not in dervish mode.
		RDPtr->characteristics.energy_regeneration = ENERGY_REGENERATION;
		ElementPtr->state_flags |= CHANGING;
	}
	
	// The player must let go of the special key between dervishes.
	// This requirement eliminates all sorts of bugs.
	if (!(StarShipPtr->old_status_flags & SPECIAL) 
		&& !(StarShipPtr->cur_status_flags & SPECIAL)
		&& StarShipPtr->RaceDescPtr->ship_info.energy_level > SPECIAL_ENERGY_COST)
		not_had_pause[ElementPtr->playerNr] = 0;
}

static void
foonfoon_preprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;
	STATUS_FLAGS cur_status_flags;
	BYTE frame_index;
	static BYTE not_had_pause[NUM_SIDES] = {0};
	static BYTE turn_direction[NUM_SIDES] = {0};
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
	cur_status_flags = StarShipPtr->cur_status_flags;
	
	frame_index = GetFrameIndex (ElementPtr->current.image.frame);
	
	if (StarShipPtr->cur_status_flags & SPECIAL
		//&& StarShipPtr->special_counter == 0 
		&& !not_had_pause[ElementPtr->playerNr])
	{
		// Can't use primary weapon and special simultaneously.
		cur_status_flags &= ~(WEAPON);
		
		// Not enough juice for dervish mode.
		if ((StarShipPtr->RaceDescPtr->ship_info.energy_level < SPECIAL_ENERGY_COST && !(StarShipPtr->old_status_flags & SPECIAL))
			|| StarShipPtr->RaceDescPtr->ship_info.energy_level == 0)
		{
			// Flash the 'batt' text.
			DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST);
			
			// Prevent spamming the special key.
			cur_status_flags &= ~(SPECIAL);
			StarShipPtr->old_status_flags &= ~(SPECIAL);
			not_had_pause[ElementPtr->playerNr] = 1;
			turn_direction[ElementPtr->playerNr] = 0;
		}
		else if (!not_had_pause[ElementPtr->playerNr])
		{
			COUNT facing;
			SIZE  speedx, speedy;
			SDWORD totalspeed;
			
			// Find out the ship facing and speed.
			facing = StarShipPtr->ShipFacing;
			GetCurrentVelocityComponents (&ElementPtr->velocity, &speedx, &speedy);
			totalspeed = sqrt (speedx * speedx + speedy * speedy);

			// Upon pressing the special key, speed rapidly to the direction which the ship was heading to.
			if (//!(StarShipPtr->old_status_flags & SPECIAL)
				//|| (/*StarShipPtr->old_status_flags & SPECIAL 
				frame_index < NUM_SHIP_FACINGS
				&& StarShipPtr->RaceDescPtr->ship_info.energy_level >= SPECIAL_ENERGY_COST
				&& StarShipPtr->cur_status_flags & (LEFT | RIGHT)
				)//&& totalspeed < DERVISH_THRUST)
			{
				// Decrement battery.
				DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST);
				
				// Go FAAAAST!
				SetVelocityVector (&ElementPtr->velocity, DERVISH_THRUST, NORMALIZE_FACING (facing));
				
				// Dervish swoosh sound.
				ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);
				
				// Spinning blade sound.
				ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 2), ElementPtr);
				
				if (StarShipPtr->cur_status_flags & LEFT)
					turn_direction[ElementPtr->playerNr] = 1;
				else if (StarShipPtr->cur_status_flags & RIGHT)
					turn_direction[ElementPtr->playerNr] = 2;
			}
			

			// Can't turn or use primary weapon in dervish mode.
			cur_status_flags &= ~(THRUST | WEAPON | LEFT | RIGHT);
			
			// Turn ship around whilst in dervish mode.
			if (turn_direction[ElementPtr->playerNr] == 1)
				facing -= 2;
			else if (turn_direction[ElementPtr->playerNr] == 2)
				facing += 2;
			facing %= NUM_SHIP_FACINGS;
			StarShipPtr->ShipFacing = facing;
			
			// Change to blur graphics.
			if (turn_direction[ElementPtr->playerNr])
			{
				ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->current.image.frame, facing + NUM_SHIP_FACINGS);
				cur_status_flags |= SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED;
			}
		}
	}
	// The player must let go of the special key between dervishes.
	// This requirement eliminates all sorts of bugs.
	else if (!(StarShipPtr->old_status_flags & SPECIAL) 
		&& !(StarShipPtr->cur_status_flags & SPECIAL)
		&& StarShipPtr->RaceDescPtr->ship_info.energy_level > SPECIAL_ENERGY_COST)
	{
		not_had_pause[ElementPtr->playerNr] = 0;
		turn_direction[ElementPtr->playerNr] = 0;
	}
	
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
		foonfoon_desc.init_weapon_func = initialize_beam_and_focusball;
		foonfoon_desc.cyborg_control.intelligence_func = foonfoon_intelligence;
		RaceDescPtr = &foonfoon_desc;
	}
	else if (RESOLUTION_FACTOR == 1)
	{
		foonfoon_desc_2xres.postprocess_func = foonfoon_postprocess;
		foonfoon_desc_2xres.preprocess_func  = foonfoon_preprocess;
		foonfoon_desc_2xres.init_weapon_func = initialize_beam_and_focusball;
		foonfoon_desc_2xres.cyborg_control.intelligence_func = foonfoon_intelligence;
		RaceDescPtr = &foonfoon_desc_2xres;
	}
	else
	{
		foonfoon_desc_4xres.postprocess_func = foonfoon_postprocess;
		foonfoon_desc_4xres.preprocess_func  = foonfoon_preprocess;
		foonfoon_desc_4xres.init_weapon_func = initialize_beam_and_focusball;
		foonfoon_desc_4xres.cyborg_control.intelligence_func = foonfoon_intelligence;
		RaceDescPtr = &foonfoon_desc_4xres;
	}
	
	return (RaceDescPtr);
}
