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
#include "baul.h"
#include "resinst.h"

#include "uqm/globdata.h"
#include "libs/log.h"


#define MAX_CREW 20
#define MAX_ENERGY 12
#define ENERGY_REGENERATION 3
#define WEAPON_ENERGY_COST 1
#define SPECIAL_ENERGY_COST 4
#define ENERGY_WAIT 24
#define MAX_THRUST 32
#define THRUST_INCREMENT 6
#define TURN_WAIT 3
#define THRUST_WAIT 5
#define WEAPON_WAIT 1
#define SPECIAL_WAIT 0

#define SHIP_MASS 9
#define BAUL_OFFSET 9
#define MISSILE_SPEED DISPLAY_TO_WORLD (20)
#define MISSILE_LIFE 5

static RACE_DESC baul_desc =
{
	{ /* SHIP_INFO */
		FIRES_FORE,
		10, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		BAUL_RACE_STRINGS,
		BAUL_ICON_MASK_PMAP_ANIM,
		BAUL_MICON_MASK_PMAP_ANIM,
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
			BAUL_BIG_MASK_PMAP_ANIM,
			BAUL_MED_MASK_PMAP_ANIM,
			BAUL_SML_MASK_PMAP_ANIM,
		},
		{
			BAULSPRAY_BIG_MASK_PMAP_ANIM,
			BAULSPRAY_MED_MASK_PMAP_ANIM,
			BAULSPRAY_SML_MASK_PMAP_ANIM,
		},
		{
			BAULGAS_BIG_MASK_PMAP_ANIM,
			BAULGAS_MED_MASK_PMAP_ANIM,
			BAULGAS_SML_MASK_PMAP_ANIM,
		},
		{
			BAUL_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		BAUL_VICTORY_SONG,
		BAUL_SHIP_SOUNDS,
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
	0, /* CodeRef */
};

// JMS_GFX
#define MAX_THRUST_2XRES 64
#define THRUST_INCREMENT_2XRES 12
#define MISSILE_SPEED_2XRES DISPLAY_TO_WORLD (40)

// JMS_GFX
static RACE_DESC baul_desc_2xres =
{
	{ /* SHIP_INFO */
		FIRES_FORE,
		10, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		BAUL_RACE_STRINGS,
		BAUL_ICON_MASK_PMAP_ANIM,
		BAUL_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		0, /* Initial SoI radius */
		{ /* Known location (center of SoI) */
			0,0,
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
			BAUL_BIG_MASK_PMAP_ANIM,
			BAUL_MED_MASK_PMAP_ANIM,
			BAUL_SML_MASK_PMAP_ANIM,
		},
		{
			BAULSPRAY_BIG_MASK_PMAP_ANIM,
			BAULSPRAY_MED_MASK_PMAP_ANIM,
			BAULSPRAY_SML_MASK_PMAP_ANIM,
		},
		{
			BAULGAS_BIG_MASK_PMAP_ANIM,
			BAULGAS_MED_MASK_PMAP_ANIM,
			BAULGAS_SML_MASK_PMAP_ANIM,
		},
		{
			BAUL_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		BAUL_VICTORY_SONG,
		BAUL_SHIP_SOUNDS,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		(MISSILE_SPEED_2XRES * MISSILE_LIFE) >> 1,
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
#define MAX_THRUST_4XRES 128
#define THRUST_INCREMENT_4XRES 24
#define MISSILE_SPEED_4XRES DISPLAY_TO_WORLD (80)

// JMS_GFX
static RACE_DESC baul_desc_4xres =
{
	{ /* SHIP_INFO */
		FIRES_FORE,
		10, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		BAUL_RACE_STRINGS,
		BAUL_ICON_MASK_PMAP_ANIM,
		BAUL_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		0, /* Initial SoI radius */
		{ /* Known location (center of SoI) */
			0,0,
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
			BAUL_BIG_MASK_PMAP_ANIM,
			BAUL_MED_MASK_PMAP_ANIM,
			BAUL_SML_MASK_PMAP_ANIM,
		},
		{
			BAULSPRAY_BIG_MASK_PMAP_ANIM,
			BAULSPRAY_MED_MASK_PMAP_ANIM,
			BAULSPRAY_SML_MASK_PMAP_ANIM,
		},
		{
			BAULGAS_BIG_MASK_PMAP_ANIM,
			BAULGAS_MED_MASK_PMAP_ANIM,
			BAULGAS_SML_MASK_PMAP_ANIM,
		},
		{
			BAUL_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		BAUL_VICTORY_SONG,
		BAUL_SHIP_SOUNDS,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		(MISSILE_SPEED_4XRES * MISSILE_LIFE) >> 1,
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
baul_intelligence (ELEMENT *ShipPtr, EVALUATE_DESC *ObjectsOfConcern,
						COUNT ConcernCounter)
{
	
	STARSHIP *StarShipPtr;
	EVALUATE_DESC *lpEvalDesc;
	
	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	if (lpEvalDesc->ObjectPtr)
	{
#define STATIONARY_SPEED WORLD_TO_VELOCITY (DISPLAY_TO_WORLD (4 << RESOLUTION_FACTOR)) // JMS_GFX
		SIZE dx, dy;
		
		GetCurrentVelocityComponents (
									  &lpEvalDesc->ObjectPtr->velocity, &dx, &dy
									  );
		if (lpEvalDesc->which_turn > 8
			|| (long)dx * dx + (long)dy * dy <=
			(long)STATIONARY_SPEED * STATIONARY_SPEED)
			lpEvalDesc->MoveState = PURSUE;
		else
			lpEvalDesc->MoveState = ENTICE;
	}
	ship_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter);
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	if (StarShipPtr->special_counter == 0)
	{
		StarShipPtr->ship_input_state &= ~SPECIAL;
		if (ObjectsOfConcern[ENEMY_WEAPON_INDEX].ObjectPtr
			&& ObjectsOfConcern[ENEMY_WEAPON_INDEX].MoveState == ENTICE)
		{
			if ((StarShipPtr->ship_input_state & THRUST)
				|| (ShipPtr->turn_wait == 0 && !(StarShipPtr->ship_input_state & (LEFT | RIGHT)))
				|| NORMALIZE_FACING (ANGLE_TO_FACING (GetVelocityTravelAngle (
						&ObjectsOfConcern[ENEMY_WEAPON_INDEX].ObjectPtr->velocity) + HALF_CIRCLE + OCTANT)
						- StarShipPtr->ShipFacing) > ANGLE_TO_FACING (QUADRANT))
				StarShipPtr->ship_input_state |= SPECIAL;
		}
		else if (lpEvalDesc->ObjectPtr)
		{
			if (lpEvalDesc->MoveState == PURSUE)
			{
				if (StarShipPtr->RaceDescPtr->ship_info.energy_level >= WEAPON_ENERGY_COST
					+ SPECIAL_ENERGY_COST
					&& ShipPtr->turn_wait == 0
					&& !(StarShipPtr->ship_input_state & (LEFT | RIGHT))
					&& (!(StarShipPtr->cur_status_flags & SPECIAL)
						|| !(StarShipPtr->cur_status_flags
							 & (SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED))))
					StarShipPtr->ship_input_state |= SPECIAL;
			}
			else if (lpEvalDesc->MoveState == ENTICE)
			{
				COUNT direction_angle;
				SIZE delta_x, delta_y;
				
				delta_x = lpEvalDesc->ObjectPtr->next.location.x
				- ShipPtr->next.location.x;
				delta_y = lpEvalDesc->ObjectPtr->next.location.y
				- ShipPtr->next.location.y;
				direction_angle = ARCTAN (delta_x, delta_y);
				
				if ((lpEvalDesc->which_turn > 24
					 && !(StarShipPtr->ship_input_state & (LEFT | RIGHT)))
					|| (lpEvalDesc->which_turn <= 16
						&& NORMALIZE_ANGLE (direction_angle
											- (FACING_TO_ANGLE (StarShipPtr->ShipFacing) + HALF_CIRCLE)
											+ QUADRANT) <= HALF_CIRCLE
						&& (lpEvalDesc->which_turn < 12
							|| NORMALIZE_ANGLE (direction_angle
												- (GetVelocityTravelAngle (
																		   &lpEvalDesc->ObjectPtr->velocity
																		   ) + HALF_CIRCLE)
												+ (OCTANT + 2)) <= ((OCTANT + 2) << 1))))
					StarShipPtr->ship_input_state |= SPECIAL;
			}
		}
		
		if ((StarShipPtr->ship_input_state & SPECIAL)
			&& StarShipPtr->RaceDescPtr->ship_info.energy_level >=
			SPECIAL_ENERGY_COST)
			StarShipPtr->ship_input_state &= ~THRUST;
	}
}

#define MAX_GASES 64
#define LAST_GAS_INDEX 8

static BYTE
count_gases (STARSHIP *StarShipPtr, BOOLEAN FindSpot)
{
	BYTE num_gases, id_use[MAX_GASES];
	HELEMENT hElement, hNextElement;
	
	num_gases = MAX_GASES;
	while (num_gases--)
		id_use[num_gases] = 0;
	
	num_gases = 0;
	for (hElement = GetTailElement (); hElement; hElement = hNextElement)
	{
		ELEMENT *ElementPtr;
		
		LockElement (hElement, &ElementPtr);
		hNextElement = GetPredElement (ElementPtr);
		if (ElementPtr->current.image.farray == StarShipPtr->RaceDescPtr->ship_data.special
			&& GetFrameIndex (ElementPtr->current.image.frame) < LAST_GAS_INDEX
			&& ElementPtr->life_span)
		{
			if (++num_gases == MAX_GASES)
			{
				UnlockElement (hElement);
				hNextElement = 0;
			}
		}
		UnlockElement (hElement);
	}
	
	if (FindSpot)
	{
		num_gases = 0;
		while (id_use[num_gases])
			++num_gases;
	}
	
	log_add (log_User, "Num_gases %d\n", num_gases);
	
	return (num_gases);
}

static void
gas_preprocess (ELEMENT *ElementPtr)
{
	// Move to next image frame.
	if (GetFrameIndex (ElementPtr->current.image.frame) < LAST_GAS_INDEX)
		ElementPtr->next.image.frame = IncFrameIndex (ElementPtr->current.image.frame);
	else
		ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->current.image.frame, 0);
	
	// If enemy ship dies, remove the gas (this prevents game crashing upon enemy ship dying with gas on it).
	if (ElementPtr->state_flags & NONSOLID && ElementPtr->hTarget == 0)
	{
		ElementPtr->life_span = 0;
		ElementPtr->state_flags |= DISAPPEARING;
	}
	// When the gas has collided with enemy ship, it sticks to the ship until expires.
	else if (ElementPtr->state_flags & NONSOLID)
	{
		ELEMENT *eptr;
		STARSHIP *StarShipPtr;
		SBYTE LeftOrRight;
		
		GetElementStarShip (ElementPtr, &StarShipPtr);
		
		LockElement (ElementPtr->hTarget, &eptr);
		ElementPtr->next.location = eptr->next.location;
		
		if (ElementPtr->turn_wait)
		{
			HELEMENT hEffect;
			STARSHIP *StarShipPtr2;
			
			if (ElementPtr->turn_wait == 1)
				LeftOrRight = 1;
			else
				LeftOrRight = -1;
			
			ElementPtr->next.location.x += (LeftOrRight * count_gases (StarShipPtr, TRUE)) << RESOLUTION_FACTOR;
			ElementPtr->next.location.y += (LeftOrRight * count_gases (StarShipPtr, TRUE)) << RESOLUTION_FACTOR;
			
			log_add (log_Debug, "leftorright%d, countgases %d \n", LeftOrRight, count_gases (StarShipPtr, TRUE));

			GetElementStarShip (eptr, &StarShipPtr2);
			
			hEffect = AllocElement ();
			if (hEffect)
			{
				LockElement (hEffect, &eptr);
				eptr->playerNr = ElementPtr->playerNr;
				eptr->state_flags = FINITE_LIFE | NONSOLID | CHANGING;
				eptr->life_span = 1;
				eptr->current = eptr->next = ElementPtr->next;
				eptr->preprocess_func = gas_preprocess;
				SetPrimType (&(GLOBAL (DisplayArray))[eptr->PrimIndex], STAMP_PRIM);
				
				GetElementStarShip (ElementPtr, &StarShipPtr2);
				SetElementStarShip (eptr, StarShipPtr2);
				eptr->hTarget = ElementPtr->hTarget;
				
				UnlockElement (hEffect);
				PutElement (hEffect);
			}
		}
		
		UnlockElement (ElementPtr->hTarget);
	}
}

static void
gas_collision (ELEMENT *ElementPtr0, POINT *pPt0, ELEMENT *ElementPtr1, POINT *pPt1)
{
	if (ElementPtr1->state_flags & PLAYER_SHIP)
	{
		HELEMENT hGasElement, hNextElement;
		ELEMENT *GasPtr;
		STARSHIP *StarShipPtr;
		
		GetElementStarShip (ElementPtr0, &StarShipPtr);
		for (hGasElement = GetHeadElement ();hGasElement; hGasElement = hNextElement)
		{
			LockElement (hGasElement, &GasPtr);
			
			if (elementsOfSamePlayer (GasPtr, ElementPtr0)
				&& GasPtr->current.image.farray == StarShipPtr->RaceDescPtr->ship_data.special
				&& (GasPtr->state_flags & NONSOLID))
			{
				UnlockElement (hGasElement);
				break;
			}
			
			hNextElement = GetSuccElement (GasPtr);
			UnlockElement (hGasElement);
		}
		
		if (hGasElement || (hGasElement = AllocElement ()))
		{
			LockElement (hGasElement, &GasPtr);
			
			if (GasPtr->state_flags == 0) /* not allocated before */
			{
				InsertElement (hGasElement, GetHeadElement ());
				
				GasPtr->current = ElementPtr0->next;
				GasPtr->next = GasPtr->current;
				GasPtr->playerNr = ElementPtr0->playerNr;
				GasPtr->state_flags = FINITE_LIFE | NONSOLID | CHANGING;
				GasPtr->preprocess_func = gas_preprocess;
				SetPrimType (&(GLOBAL (DisplayArray))[GasPtr->PrimIndex], NO_PRIM);
				
				SetElementStarShip (GasPtr, StarShipPtr);
				GetElementStarShip (ElementPtr1, &StarShipPtr);
				GasPtr->hTarget = StarShipPtr->hShip;
			}
			
			GasPtr->life_span = ElementPtr0->life_span;
			GasPtr->turn_wait = (BYTE)(1 << ((BYTE)TFB_Random () & 1)); /* LEFT or RIGHT */
			
			UnlockElement (hGasElement);
		}
		
		ElementPtr0->hit_points = 0;
		ElementPtr0->life_span = 0;
		ElementPtr0->state_flags |= DISAPPEARING | COLLISION | NONSOLID;
	}
	(void) pPt0;  /* Satisfying compiler (unused parameter) */
	(void) pPt1;  /* Satisfying compiler (unused parameter) */
}

#define GAS_HITS 100
#define GAS_DAMAGE 1
#define GAS_LIFE 480
#define GAS_OFFSET (50 << RESOLUTION_FACTOR)
#define GAS_INIT_SPEED 0
static void spawn_gas (ELEMENT *ShipPtr)
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	HELEMENT Missile;
		
	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
	MissileBlock.face = (StarShipPtr->ShipFacing - 5 - (((COUNT)TFB_Random ()) % 5)) % 16;
	MissileBlock.index = GetFrameCount (StarShipPtr->RaceDescPtr->ship_data.special[0]) - 1;
	MissileBlock.sender = ShipPtr->playerNr;
	MissileBlock.flags = IGNORE_SIMILAR;
	MissileBlock.pixoffs = GAS_OFFSET;
	MissileBlock.speed = GAS_INIT_SPEED;
	MissileBlock.hit_points = GAS_HITS;
	MissileBlock.damage = GAS_DAMAGE;
	MissileBlock.life = GAS_LIFE;
	MissileBlock.preprocess_func = gas_preprocess;
	MissileBlock.blast_offs = GAS_OFFSET;
	Missile = initialize_missile (&MissileBlock);
	
	if (Missile)
	{
		ELEMENT *GasPtr;
		
		LockElement (Missile, &GasPtr);
		GasPtr->collision_func = gas_collision;
		SetElementStarShip (GasPtr, StarShipPtr);
		ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), GasPtr);
		UnlockElement (Missile);
		PutElement (Missile);
	}
}

#define LAST_SPRAY_INDEX 5

static void
spray_preprocess (ELEMENT *ElementPtr)
{
	// Move to next image frame.
	if (GetFrameIndex (ElementPtr->current.image.frame) < LAST_GAS_INDEX)
		ElementPtr->next.image.frame = IncFrameIndex (ElementPtr->current.image.frame);
	else
		ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->current.image.frame, 0);
}

static COUNT
initialize_spray (ELEMENT *ShipPtr, HELEMENT SprayArray[])
{
#define MISSILE_HITS 2
#define MISSILE_DAMAGE 1
#define MISSILE_OFFSET (3 << RESOLUTION_FACTOR) // JMS_GFX
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.face = StarShipPtr->ShipFacing;
	MissileBlock.index = 0;
	MissileBlock.sender = ShipPtr->playerNr;
	MissileBlock.flags = IGNORE_SIMILAR;
	MissileBlock.pixoffs = BAUL_OFFSET;
	MissileBlock.speed = MISSILE_SPEED << RESOLUTION_FACTOR; // JMS_GFX
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = spray_preprocess;
	MissileBlock.blast_offs = MISSILE_OFFSET;
	SprayArray[0] = initialize_missile (&MissileBlock);
	
	return (1);
}

#define GAS_BATCH_SIZE 3
static void
baul_postprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
	
	if ((StarShipPtr->cur_status_flags & SPECIAL)
		&& StarShipPtr->special_counter == 0
		&& DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST))
	{
		int i;
		
		ProcessSound (SetAbsSoundIndex // Spawn gas.
					  (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);
		
		for (i = 0; i < GAS_BATCH_SIZE; i++)
			spawn_gas (ElementPtr);
		
		StarShipPtr->special_counter = SPECIAL_WAIT;
	}
}

static void
baul_preprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
}

RACE_DESC*
init_baul (void)
{
	RACE_DESC *RaceDescPtr;
	
	if (RESOLUTION_FACTOR == 0)
	{
		baul_desc.preprocess_func = baul_preprocess;
		baul_desc.postprocess_func = baul_postprocess;
		baul_desc.init_weapon_func = initialize_spray;
		baul_desc.cyborg_control.intelligence_func = baul_intelligence;
		RaceDescPtr = &baul_desc;
	}
	else if (RESOLUTION_FACTOR == 1)
	{
		baul_desc_2xres.preprocess_func = baul_preprocess;
		baul_desc_2xres.postprocess_func = baul_postprocess;
		baul_desc_2xres.init_weapon_func = initialize_spray;
		baul_desc_2xres.cyborg_control.intelligence_func = baul_intelligence;
		RaceDescPtr = &baul_desc_2xres;
	}
	else
	{
		baul_desc_4xres.preprocess_func = baul_preprocess;
		baul_desc_4xres.postprocess_func = baul_postprocess;
		baul_desc_4xres.init_weapon_func = initialize_spray;
		baul_desc_4xres.cyborg_control.intelligence_func = baul_intelligence;
		RaceDescPtr = &baul_desc_4xres;
	}
	
	return (RaceDescPtr);
}
