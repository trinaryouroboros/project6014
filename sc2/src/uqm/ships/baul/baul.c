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


#define MAX_CREW 16
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
#define MISSILE_LIFE 15

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
			BAULFIRE_BIG_MASK_PMAP_ANIM,
			BAULFIRE_MED_MASK_PMAP_ANIM,
			BAULFIRE_SML_MASK_PMAP_ANIM,
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
			BAULFIRE_BIG_MASK_PMAP_ANIM,
			BAULFIRE_MED_MASK_PMAP_ANIM,
			BAULFIRE_SML_MASK_PMAP_ANIM,
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
			BAULFIRE_BIG_MASK_PMAP_ANIM,
			BAULFIRE_MED_MASK_PMAP_ANIM,
			BAULFIRE_SML_MASK_PMAP_ANIM,
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
				|| (ShipPtr->turn_wait == 0
					&& !(StarShipPtr->ship_input_state & (LEFT | RIGHT)))
				|| NORMALIZE_FACING (ANGLE_TO_FACING (
													  GetVelocityTravelAngle (
																			  &ObjectsOfConcern[ENEMY_WEAPON_INDEX].ObjectPtr->velocity
																			  ) + HALF_CIRCLE + OCTANT)
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


#define GAS_SPEED DISPLAY_TO_WORLD (1 << RESOLUTION_FACTOR) // JMS_GFX
static void
gas_preprocess (ELEMENT *ElementPtr)
{
	BYTE thrust_wait, turn_wait;
	
	thrust_wait = HINIBBLE (ElementPtr->turn_wait);
	turn_wait = LONIBBLE (ElementPtr->turn_wait);
	
	if (turn_wait > 0)
		--turn_wait;
	else
	{
		COUNT facing;
		facing = (COUNT)TFB_Random ();
		SetVelocityVector (&ElementPtr->velocity, GAS_SPEED, facing);
		turn_wait = 4;
	}
	
	ElementPtr->turn_wait = MAKE_BYTE (turn_wait, thrust_wait);
	
	if (ElementPtr->state_flags & NONSOLID)
	{
		ELEMENT *eptr;
		
		LockElement (ElementPtr->hTarget, &eptr);
		ElementPtr->next.location = eptr->next.location;
		
		if (ElementPtr->turn_wait)
		{
			HELEMENT hEffect;
			STARSHIP *StarShipPtr;
			
			if (GetFrameIndex (ElementPtr->next.image.frame = IncFrameIndex (ElementPtr->current.image.frame)) == 0)
				ElementPtr->next.image.frame = SetRelFrameIndex (ElementPtr->next.image.frame, -8);
			
			GetElementStarShip (eptr, &StarShipPtr);
			
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
				
				GetElementStarShip (ElementPtr, &StarShipPtr);
				SetElementStarShip (eptr, StarShipPtr);
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
		for (hGasElement = GetHeadElement ();
			 hGasElement; hGasElement = hNextElement)
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
				GasPtr->current.image.frame = SetAbsFrameIndex (GasPtr->current.image.frame, 8);
				GasPtr->next = GasPtr->current;
				GasPtr->playerNr = ElementPtr0->playerNr;
				GasPtr->state_flags = FINITE_LIFE | NONSOLID | CHANGING;
				GasPtr->preprocess_func = gas_preprocess;
				SetPrimType (&(GLOBAL (DisplayArray))[GasPtr->PrimIndex],NO_PRIM);
				SetElementStarShip (GasPtr, StarShipPtr);
				GetElementStarShip (ElementPtr1, &StarShipPtr);
				GasPtr->hTarget = StarShipPtr->hShip;
			}
			
			GasPtr->life_span = ElementPtr0->life_span;
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
#define GAS_OFFSET 100
#define GAS_INIT_SPEED DISPLAY_TO_WORLD (4 << RESOLUTION_FACTOR) // JMS_GFX
#define GAS_SPREAD_MINIMUM 6
#define GAS_SPREAD_VARIATION 4
static void spawn_gas (ELEMENT *ShipPtr)
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	HELEMENT Missile;
		
	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
	MissileBlock.face = (StarShipPtr->ShipFacing - 7 - (((COUNT)TFB_Random ()) % 3)) % 16;
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
		// GasPtr->turn_wait affects how long the projectile travels at GAS_INIT_SPEED.
		GasPtr->turn_wait = GAS_SPREAD_MINIMUM + ((COUNT)TFB_Random () & GAS_SPREAD_VARIATION);
		GasPtr->collision_func = gas_collision;
		SetElementStarShip (GasPtr, StarShipPtr);
		ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), GasPtr);
		UnlockElement (Missile);
		PutElement (Missile);
	}
}



static COUNT
initialize_baulfire (ELEMENT *ShipPtr, HELEMENT BaulfireArray[])
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
	MissileBlock.face = MissileBlock.index = StarShipPtr->ShipFacing;
	MissileBlock.sender = ShipPtr->playerNr;
	MissileBlock.flags = IGNORE_SIMILAR;
	MissileBlock.pixoffs = BAUL_OFFSET;
	MissileBlock.speed = MISSILE_SPEED << RESOLUTION_FACTOR; // JMS_GFX
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = NULL;
	MissileBlock.blast_offs = MISSILE_OFFSET;
	BaulfireArray[0] = initialize_missile (&MissileBlock);
	
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
		baul_desc.init_weapon_func = initialize_baulfire;
		baul_desc.cyborg_control.intelligence_func = baul_intelligence;
		RaceDescPtr = &baul_desc;
	}
	else if (RESOLUTION_FACTOR == 1)
	{
		baul_desc_2xres.preprocess_func = baul_preprocess;
		baul_desc_2xres.postprocess_func = baul_postprocess;
		baul_desc_2xres.init_weapon_func = initialize_baulfire;
		baul_desc_2xres.cyborg_control.intelligence_func = baul_intelligence;
		RaceDescPtr = &baul_desc_2xres;
	}
	else
	{
		baul_desc_4xres.preprocess_func = baul_preprocess;
		baul_desc_4xres.postprocess_func = baul_postprocess;
		baul_desc_4xres.init_weapon_func = initialize_baulfire;
		baul_desc_4xres.cyborg_control.intelligence_func = baul_intelligence;
		RaceDescPtr = &baul_desc_4xres;
	}
	
	return (RaceDescPtr);
}
