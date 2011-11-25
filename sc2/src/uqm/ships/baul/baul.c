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
#define MAX_ENERGY 22
#define ENERGY_REGENERATION 3
#define WEAPON_ENERGY_COST 1
#define SPECIAL_ENERGY_COST 2
#define ENERGY_WAIT 24
#define MAX_THRUST 32
#define THRUST_INCREMENT 6
#define TURN_WAIT 3
#define THRUST_WAIT 5
#define WEAPON_WAIT 24
#define SPECIAL_WAIT 7

#define SHIP_MASS 9
#define BAUL_OFFSET (4 << RESOLUTION_FACTOR)
#define MISSILE_SPEED DISPLAY_TO_WORLD (30)
#define MISSILE_LIFE 5

static RACE_DESC baul_desc =
{
	{ /* SHIP_INFO */
		FIRES_FORE,
		14, /* Super Melee cost */
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
#define MISSILE_SPEED_2XRES DISPLAY_TO_WORLD (60)

// JMS_GFX
static RACE_DESC baul_desc_2xres =
{
	{ /* SHIP_INFO */
		FIRES_FORE,
		14, /* Super Melee cost */
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
#define MISSILE_SPEED_4XRES DISPLAY_TO_WORLD (120)

// JMS_GFX
static RACE_DESC baul_desc_4xres =
{
	{ /* SHIP_INFO */
		FIRES_FORE,
		14, /* Super Melee cost */
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

#define SHOCKWAVE_FRAMES 8
#define LAST_GAS_INDEX 8
#define LAST_SHOCKWAVE_INDEX (LAST_GAS_INDEX + SHOCKWAVE_FRAMES)
#define NUM_DISSOLVE_ANIMS 4
#define MAX_GASES 32

#define SHOCKWAVE_RANGE (180 << RESOLUTION_FACTOR) // JMS_GFX
#define MAX_DESTRUCTION ((SHOCKWAVE_RANGE >> RESOLUTION_FACTOR) / 30) // JMS_GFX

/* Since this was copied from Shofixti code, it had some of these orz checks there. 
 Not sure if these are relevant anymore. */
#include "../orz/orz.h"
#define ORZ_MARINE(ptr) (ptr->preprocess_func == intruder_preprocess && \
ptr->collision_func == marine_collision)

static void
shockwave_preprocess (ELEMENT *ElementPtr)
{
	ElementPtr->next.image.frame = IncFrameIndex (ElementPtr->current.image.frame);
	
	// This makes the shockwave animate even if the ships are not moving and the screen is stationary.
	ElementPtr->state_flags |= CHANGING;
}

static void
generate_shockwave (ELEMENT *ElementPtr, BYTE which_player)
{
	STARSHIP *StarShipPtr;
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
	
	// Gas is still 'solid' when it's hit by the spray. Let's make a shockwave and kill the gas cloud. 
	if (!(ElementPtr->state_flags & NONSOLID))
	{
		HELEMENT hShockwave;
				
		hShockwave = AllocElement ();
		if (hShockwave)
		{
			ELEMENT *ShockwavePtr;
			STARSHIP *StarShipPtr;
			
			GetElementStarShip (ElementPtr, &StarShipPtr);
			
			PutElement (hShockwave);
			LockElement (hShockwave, &ShockwavePtr);
			SetElementStarShip (ShockwavePtr, StarShipPtr);
			ShockwavePtr->hit_points = ShockwavePtr->mass_points = 0;
			ShockwavePtr->playerNr = which_player; // Don't damage self.
			ShockwavePtr->state_flags = APPEARING | FINITE_LIFE | NONSOLID | IGNORE_SIMILAR;
			ShockwavePtr->life_span = SHOCKWAVE_FRAMES;
			SetPrimType (&(GLOBAL (DisplayArray))[ShockwavePtr->PrimIndex], STAMP_PRIM);
			ShockwavePtr->current.image.farray = StarShipPtr->RaceDescPtr->ship_data.special;
			ShockwavePtr->current.image.frame = SetAbsFrameIndex(StarShipPtr->RaceDescPtr->ship_data.special[0], LAST_GAS_INDEX);
			ShockwavePtr->next.image.frame = SetAbsFrameIndex(ElementPtr->current.image.frame, LAST_GAS_INDEX);
			ShockwavePtr->current.location = ElementPtr->current.location;
			ShockwavePtr->preprocess_func = shockwave_preprocess;
			ShockwavePtr->postprocess_func = NULL;
			ShockwavePtr->death_func = NULL;
			ZeroVelocityComponents (&ShockwavePtr->velocity);
			UnlockElement (hShockwave);
		}
		
		// Gas dies on the next turn.
		ElementPtr->state_flags |= NONSOLID;
		
		// Explosion sounds.
		ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 2), ElementPtr);
		ProcessSound (SetAbsSoundIndex (GameSounds, TARGET_DAMAGED_FOR_6_PLUS_PT), ElementPtr);
	}

	{
		// This is called during PostProcessQueue(), close to or at the end,
		// for the temporary shockwave element to apply the damage.
		// The effects are not seen until the next frame.
		HELEMENT hElement, hNextElement;
		
		for (hElement = GetHeadElement (); hElement != 0; hElement = hNextElement)
		{
			ELEMENT *ObjPtr;
			
			LockElement (hElement, &ObjPtr);
			hNextElement = GetSuccElement (ObjPtr);
			
			if (CollidingElement (ObjPtr) || ORZ_MARINE (ObjPtr))
			{
				SIZE delta_x, delta_y;
				DWORD dist;
				
				if ((delta_x = ObjPtr->next.location.x - ElementPtr->next.location.x) < 0) delta_x = -delta_x;
				if ((delta_y = ObjPtr->next.location.y - ElementPtr->next.location.y) < 0) delta_y = -delta_y;
				
				delta_x = WORLD_TO_DISPLAY (delta_x);
				delta_y = WORLD_TO_DISPLAY (delta_y);
				
				if (delta_x <= SHOCKWAVE_RANGE && delta_y <= SHOCKWAVE_RANGE
					&& (dist = (DWORD)(delta_x * delta_x) + (DWORD)(delta_y * delta_y)) <= (DWORD)(SHOCKWAVE_RANGE * SHOCKWAVE_RANGE))
				{
					SIZE destruction;
					
					destruction = ((MAX_DESTRUCTION * (SHOCKWAVE_RANGE - square_root (dist))) / SHOCKWAVE_RANGE) + 1;
					
					if (ObjPtr->state_flags & PLAYER_SHIP && ObjPtr->playerNr != which_player)
					{
						if (!DeltaCrew (ObjPtr, -destruction))
							ObjPtr->life_span = 0;
					}
					else if (!GRAVITY_MASS (ObjPtr->mass_points) && ObjPtr->playerNr != which_player)
					{
						if ((BYTE)destruction < ObjPtr->hit_points)
							ObjPtr->hit_points -= (BYTE)destruction;
						else
						{
							ObjPtr->hit_points = 0;
							ObjPtr->life_span = 0;
						}
					}
				}
			}
			
			UnlockElement (hElement);
		}
	}
}

static BYTE
count_gases (STARSHIP *StarShipPtr)
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

	return (num_gases);
}

static void
gas_collision (ELEMENT *ElementPtr0, POINT *pPt0, ELEMENT *ElementPtr1, POINT *pPt1);

static void
gas_death_animation (ELEMENT *ElementPtr)
{
	ElementPtr->next.image.frame = IncFrameIndex (ElementPtr->current.image.frame);
	ElementPtr->state_flags |= CHANGING;
}

static void
gas_death (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
	
	if (StarShipPtr->hShip)
	{
		HELEMENT hIonSpots;
		ELEMENT *ShipPtr;
		
		LockElement (StarShipPtr->hShip, &ShipPtr);
	
		if ((hIonSpots = AllocElement ()))
		{
			ELEMENT *IonSpotsPtr;
			
			LockElement (hIonSpots, &IonSpotsPtr);
			IonSpotsPtr->playerNr = ElementPtr->playerNr;
			IonSpotsPtr->state_flags = FINITE_LIFE | NONSOLID | IGNORE_SIMILAR | APPEARING;
			IonSpotsPtr->turn_wait = 0;
			IonSpotsPtr->life_span = NUM_DISSOLVE_ANIMS;
			IonSpotsPtr->current.location.x = ElementPtr->current.location.x;
			IonSpotsPtr->current.location.y = ElementPtr->current.location.y;
			IonSpotsPtr->current.image.farray = StarShipPtr->RaceDescPtr->ship_data.special;
			IonSpotsPtr->current.image.frame = SetAbsFrameIndex (ElementPtr->current.image.frame, LAST_SHOCKWAVE_INDEX);
			IonSpotsPtr->preprocess_func = gas_death_animation;
			SetElementStarShip (IonSpotsPtr, StarShipPtr);
			SetPrimType (&(GLOBAL (DisplayArray))[IonSpotsPtr->PrimIndex], STAMP_PRIM);
			
			UnlockElement (hIonSpots);
			PutElement (hIonSpots);
		}
		
		UnlockElement (StarShipPtr->hShip);
	}
}

static void
gas_preprocess (ELEMENT *ElementPtr)
{
	// Move to next image frame.
	if (GetFrameIndex (ElementPtr->current.image.frame) < LAST_GAS_INDEX - 1)
		ElementPtr->next.image.frame = IncFrameIndex (ElementPtr->current.image.frame);
	else
		ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->current.image.frame, 0);
	
	// This makes the gas animate even if the ships are not moving and the screen is stationary.
	ElementPtr->state_flags |= CHANGING;
	
	// If enemy ship dies, remove the gas (this prevents game crashing upon enemy ship dying with gas on it).
	if (ElementPtr->state_flags & IGNORE_SHIP && ElementPtr->hTarget == 0)
	{
		ElementPtr->life_span = 0;
		ElementPtr->state_flags |= DISAPPEARING;
	}
	// When the gas has collided with enemy ship, it sticks to the ship until expires.
	else if (ElementPtr->state_flags & IGNORE_SHIP)
	{
		ELEMENT *eptr;
		STARSHIP *StarShipPtr;
		SIZE offs_x, offs_y;
		SBYTE leftOrRight, upOrDown;
		COUNT angle, angleCorrect;
		static BYTE alignment[NUM_SIDES]={0,0};
		
		// eptr points to enemy ship now.
		LockElement (ElementPtr->hTarget, &eptr);
		
		// Make gas's location the same as the enemy ship's.
		ElementPtr->next.location = eptr->next.location;
		
		// Randomize the gas's location so every gas cloud doesn't stick to the same place on the enemy ship.
		GetElementStarShip (eptr, &StarShipPtr);
		angle = (ElementPtr->weapon_element_index) % 16;
		alignment[ElementPtr->playerNr] = ElementPtr->weapon_element_index % 4;
		if (alignment[ElementPtr->playerNr] == 0)
		{
			leftOrRight = -1;
			upOrDown = 1;
			angleCorrect = 0;
		}
		else if (alignment[ElementPtr->playerNr] == 1)
		{
			leftOrRight = 1;
			upOrDown = -1;
			angleCorrect = 0;
		}
		else if (alignment[ElementPtr->playerNr] == 2)
		{
			leftOrRight = -1;
			upOrDown = 1;
			angleCorrect = HALF_CIRCLE / 2;
		}
		else
		{
			leftOrRight = 1;
			upOrDown = -1;
			angleCorrect = HALF_CIRCLE / 2;
		}
		offs_x = SINE (angle - angleCorrect, (ElementPtr->weapon_element_index % 16) * (5 << RESOLUTION_FACTOR));
		offs_y = COSINE (angle - angleCorrect, (ElementPtr->weapon_element_index % 16) * (5 << RESOLUTION_FACTOR));
		ElementPtr->next.location.x = ElementPtr->next.location.x + leftOrRight * offs_x;
		ElementPtr->next.location.y = ElementPtr->next.location.y + upOrDown * offs_y;
	
		if (ElementPtr->turn_wait)
		{
			HELEMENT hEffect;
			
			hEffect = AllocElement ();
			if (hEffect)
			{
				// eptr points to the new gas element now.
				LockElement (hEffect, &eptr);
				eptr->playerNr = ElementPtr->playerNr;
				eptr->state_flags = FINITE_LIFE | IGNORE_SHIP | CHANGING;
				eptr->life_span = 1;
				eptr->weapon_element_index = ElementPtr->weapon_element_index;
				eptr->current = eptr->next = ElementPtr->next;
				eptr->preprocess_func = gas_preprocess;
				eptr->collision_func = gas_collision;
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
	STARSHIP *StarShipPtr;
	STARSHIP *EnemyStarShipPtr;
	BYTE	 enemyShipIsBaul = 0;
	
	// The ship this gas cloud belongs to.
	GetElementStarShip (ElementPtr0, &StarShipPtr);
	
	// Check if the colliding element is a ship. If it is not, check if it's a projectile from Baul ship.
	if (!elementsOfSamePlayer(ElementPtr0, ElementPtr1) && !(ElementPtr1->state_flags & PLAYER_SHIP) 
		&& ElementPtr0->life_span > 1 && ElementPtr1->life_span > 1
		&& ElementPtr1->playerNr > -1)
	{
		GetElementStarShip (ElementPtr1, &EnemyStarShipPtr);
		if (EnemyStarShipPtr->SpeciesID == BAUL_ID)
			enemyShipIsBaul = 1;
	}
	
	// If colliding with Baul's spray weapon, EXPLODE!!!
	if (ElementPtr1->current.image.farray == StarShipPtr->RaceDescPtr->ship_data.weapon
		|| (enemyShipIsBaul && ElementPtr1->current.image.farray == EnemyStarShipPtr->RaceDescPtr->ship_data.weapon))
	{
		// Move to shockwave graphics.
		ElementPtr0->current.image.frame = SetAbsFrameIndex (ElementPtr0->current.image.frame, LAST_GAS_INDEX);
		ElementPtr0->next.image.frame = SetAbsFrameIndex (ElementPtr0->current.image.frame, LAST_GAS_INDEX);
		
		// Remove the lock on enemy ship and make the gas die on next turn.
		ElementPtr0->hTarget = 0;
		ElementPtr0->life_span = 1;
		
		// Don't do the gas dissolve anim now that the shockwave appears.
		ElementPtr0->death_func = NULL;
		
		// Generate the actual shockwave.
		generate_shockwave (ElementPtr0, ElementPtr1->playerNr);
	}
	// If colliding with enemy ship, stick to the ship.
	else if (!(ElementPtr0->state_flags & IGNORE_SHIP)
			 && ElementPtr1->state_flags & PLAYER_SHIP 
			 && ElementPtr1->playerNr != ElementPtr0->playerNr)
	{
		HELEMENT hGasElement;
		ELEMENT *GasPtr;
		
		// Create a new gas element which is sticking to the enemy ship.
		if ((hGasElement = AllocElement ()))
		{
			LockElement (hGasElement, &GasPtr);
			
			if (GasPtr->state_flags == 0) /* not allocated before */
			{
				InsertElement (hGasElement, GetHeadElement ());
				
				GasPtr->current = ElementPtr0->next;
				GasPtr->next = GasPtr->current;
				GasPtr->playerNr = ElementPtr0->playerNr;
				GasPtr->state_flags = FINITE_LIFE | IGNORE_SHIP | CHANGING;
				GasPtr->preprocess_func = gas_preprocess;
				GasPtr->collision_func = gas_collision;
				SetPrimType (&(GLOBAL (DisplayArray))[GasPtr->PrimIndex], NO_PRIM);
				
				SetElementStarShip (GasPtr, StarShipPtr);
				GetElementStarShip (ElementPtr1, &StarShipPtr);
				GasPtr->hTarget = StarShipPtr->hShip;
			}
			
			GasPtr->life_span = ElementPtr0->life_span;
			GasPtr->weapon_element_index = ElementPtr0->weapon_element_index;
			GasPtr->turn_wait = (BYTE)(1 << ((BYTE)TFB_Random () & 1)); /* LEFT or RIGHT */
			
			UnlockElement (hGasElement);
		}
		
		// Erase the original gas element.
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
#define GAS_OFFSET (25 << RESOLUTION_FACTOR)
#define GAS_INIT_SPEED 0
#define GAS_HORZ_OFFSET (DISPLAY_TO_WORLD(5 << RESOLUTION_FACTOR))  // JMS_GFX
#define GAS_HORZ_OFFSET_2 (DISPLAY_TO_WORLD((-5) << RESOLUTION_FACTOR)) // JMS_GFX

static void spawn_gas (ELEMENT *ShipPtr)
{
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;
	HELEMENT Missile;
	SIZE offs_x, offs_y;
	COUNT angle;
	static COUNT gas_side[NUM_SIDES]   = {0, 0};
	static COUNT gas_number[NUM_SIDES] = {0, 0};
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	
	gas_number[ShipPtr->playerNr] = (gas_number[ShipPtr->playerNr] + 1) % 32;
	gas_side[ShipPtr->playerNr] = (gas_side[ShipPtr->playerNr] + 1) % 2;
	angle = FACING_TO_ANGLE (StarShipPtr->ShipFacing);
	
	if(gas_side[ShipPtr->playerNr])
	{
		offs_x = -SINE (angle, GAS_HORZ_OFFSET);
		offs_y = COSINE (angle, GAS_HORZ_OFFSET);
	}
	else
	{
		offs_x = -SINE (angle, GAS_HORZ_OFFSET_2);
		offs_y = COSINE (angle, GAS_HORZ_OFFSET_2);
	}
		
	
	MissileBlock.cx = ShipPtr->next.location.x + offs_x;
	MissileBlock.cy = ShipPtr->next.location.y + offs_y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
	MissileBlock.face = (StarShipPtr->ShipFacing - 8) % 16;
	MissileBlock.index = 0;
	MissileBlock.sender = ShipPtr->playerNr;
	MissileBlock.flags = 0;
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
		GasPtr->death_func = gas_death;
		GasPtr->weapon_element_index = gas_number[ShipPtr->playerNr];
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
	SIZE offs_x, offs_y;
	COUNT i, angle;
	static COUNT spray_side[NUM_SIDES]={0,0};
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	angle = FACING_TO_ANGLE (StarShipPtr->ShipFacing);
	
	for (i = 0; i < 2; i++)
	{
		spray_side[ShipPtr->playerNr] = (spray_side[ShipPtr->playerNr] + 1) % 2;
		if(spray_side[ShipPtr->playerNr])
		{
			offs_x = -SINE (angle, GAS_HORZ_OFFSET);
			offs_y = COSINE (angle, GAS_HORZ_OFFSET);
		}
		else
		{
			offs_x = -SINE (angle, GAS_HORZ_OFFSET_2);
			offs_y = COSINE (angle, GAS_HORZ_OFFSET_2);
		}
	
		MissileBlock.cx = ShipPtr->next.location.x + offs_x;
		MissileBlock.cy = ShipPtr->next.location.y + offs_y;
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
		SprayArray[i] = initialize_missile (&MissileBlock);
	}
	
	return (2);
}

#define GAS_BATCH_SIZE 1
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
