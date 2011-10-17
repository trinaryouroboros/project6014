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
#include "slykohr.h"
#include "resinst.h"

#include "uqm/battle.h"
#include "uqm/globdata.h"


#define MAX_CREW MAX_CREW_SIZE
#define MAX_ENERGY MAX_ENERGY_SIZE
#define ENERGY_REGENERATION 1
#define WEAPON_ENERGY_COST 6
#define SPECIAL_ENERGY_COST (MAX_ENERGY_SIZE / 2)
#define ENERGY_WAIT 4
#define MAX_THRUST 30
#define THRUST_INCREMENT 6
#define TURN_WAIT 4
#define THRUST_WAIT 6
#define WEAPON_WAIT 6
#define SPECIAL_WAIT 9

#define SHIP_MASS 10
#define MISSILE_SPEED (64 << RESOLUTION_FACTOR) // JMS_GFX
#define MISSILE_LIFE 64 /* actually, it's as long as you
										 * hold the button down.
										 */
static RACE_DESC slylandro_kohrah_desc =
{
	{ /* SHIP_INFO */
		FIRES_FORE,
		30, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		SLYLANDRO_KOHRAH_RACE_STRINGS,
		KOHR_AH_ICON_MASK_PMAP_ANIM,
		KOHR_AH_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		4000 / SPHERE_RADIUS_INCREMENT * 2, /* Initial SoI radius */
		{ /* Known location (center of SoI) */
			9999,9999,
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
			KOHR_AH_BIG_MASK_PMAP_ANIM,
			KOHR_AH_MED_MASK_PMAP_ANIM,
			KOHR_AH_SML_MASK_PMAP_ANIM,
		},
		{
			BUZZSAW_BIG_MASK_PMAP_ANIM,
			BUZZSAW_MED_MASK_PMAP_ANIM,
			BUZZSAW_SML_MASK_PMAP_ANIM,
		},
		{
			GAS_BIG_MASK_PMAP_ANIM,
			GAS_MED_MASK_PMAP_ANIM,
			GAS_SML_MASK_PMAP_ANIM,
		},
		{
			SLYLANDRO_KOHRAH_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		KOHR_AH_VICTORY_SONG,
		KOHR_AH_SHIP_SOUNDS,
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
	0, /* CodeRef */
};

// JMS_GFX
#define MAX_THRUST_2XRES 60
#define THRUST_INCREMENT_2XRES 12

// JMS_GFX
static RACE_DESC slylandro_kohrah_desc_2xres =
{
	{ /* SHIP_INFO */
		FIRES_FORE,
		30, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		SLYLANDRO_KOHRAH_RACE_STRINGS,
		KOHR_AH_ICON_MASK_PMAP_ANIM,
		KOHR_AH_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		4000 / SPHERE_RADIUS_INCREMENT * 2, /* Initial SoI radius */
		{ /* Known location (center of SoI) */
			9999,9999,
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
			KOHR_AH_BIG_MASK_PMAP_ANIM,
			KOHR_AH_MED_MASK_PMAP_ANIM,
			KOHR_AH_SML_MASK_PMAP_ANIM,
		},
		{
			BUZZSAW_BIG_MASK_PMAP_ANIM,
			BUZZSAW_MED_MASK_PMAP_ANIM,
			BUZZSAW_SML_MASK_PMAP_ANIM,
		},
		{
			GAS_BIG_MASK_PMAP_ANIM,
			GAS_MED_MASK_PMAP_ANIM,
			GAS_SML_MASK_PMAP_ANIM,
		},
		{
			SLYLANDRO_KOHRAH_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		KOHR_AH_VICTORY_SONG,
		KOHR_AH_SHIP_SOUNDS,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		CLOSE_RANGE_WEAPON_2XRES,
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
#define MAX_THRUST_4XRES 120
#define THRUST_INCREMENT_4XRES 24

// JMS_GFX
static RACE_DESC slylandro_kohrah_desc_4xres =
{
	{ /* SHIP_INFO */
		FIRES_FORE,
		30, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		SLYLANDRO_KOHRAH_RACE_STRINGS,
		KOHR_AH_ICON_MASK_PMAP_ANIM,
		KOHR_AH_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		4000 / SPHERE_RADIUS_INCREMENT * 2, /* Initial SoI radius */
		{ /* Known location (center of SoI) */
			9999,9999,
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
			KOHR_AH_BIG_MASK_PMAP_ANIM,
			KOHR_AH_MED_MASK_PMAP_ANIM,
			KOHR_AH_SML_MASK_PMAP_ANIM,
		},
		{
			BUZZSAW_BIG_MASK_PMAP_ANIM,
			BUZZSAW_MED_MASK_PMAP_ANIM,
			BUZZSAW_SML_MASK_PMAP_ANIM,
		},
		{
			GAS_BIG_MASK_PMAP_ANIM,
			GAS_MED_MASK_PMAP_ANIM,
			GAS_SML_MASK_PMAP_ANIM,
		},
		{
			SLYLANDRO_KOHRAH_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		KOHR_AH_VICTORY_SONG,
		KOHR_AH_SHIP_SOUNDS,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		CLOSE_RANGE_WEAPON_4XRES,
		NULL,
	},
	(UNINIT_FUNC *) NULL,
	(PREPROCESS_FUNC *) NULL,
	(POSTPROCESS_FUNC *) NULL,
	(INIT_WEAPON_FUNC *) NULL,
	0,
	0, /* CodeRef */
};

#define SAW_RATE 0
#define MAX_SAWS 8

static void
spin_preprocess (ELEMENT *ElementPtr)
{
	ELEMENT *ShipPtr;
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	LockElement (StarShipPtr->hShip, &ShipPtr);
	if (ShipPtr->crew_level && ++StarShipPtr->RaceDescPtr->characteristics.special_wait > MAX_SAWS)
	{
		ElementPtr->life_span = 1;
		ElementPtr->state_flags |= DISAPPEARING;
	}
	else
	{
		++ElementPtr->life_span;
		if (ElementPtr->turn_wait)
			--ElementPtr->turn_wait;
		else
		{
#define LAST_SPIN_INDEX 1
			if (GetFrameIndex (ElementPtr->current.image.frame) < LAST_SPIN_INDEX)
				ElementPtr->next.image.frame = IncFrameIndex (ElementPtr->current.image.frame);
			else
				ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->current.image.frame, 0);
			
			ElementPtr->state_flags |= CHANGING;
			ElementPtr->turn_wait = SAW_RATE;
		}
	}
	UnlockElement (StarShipPtr->hShip);
}

#define TRACK_WAIT 4

static void
buzztrack_preprocess (ELEMENT *ElementPtr)
{
	if (ElementPtr->thrust_wait)
		--ElementPtr->thrust_wait;
	else
	{
		COUNT facing = 0;

		if (ElementPtr->hTarget == 0
				&& TrackShip (ElementPtr, &facing) < 0)
		{
			ZeroVelocityComponents (&ElementPtr->velocity);
		}
		else
		{
#define ACTIVATE_RANGE (224 << RESOLUTION_FACTOR) // JMS_GFX /* Originally SPACE_WIDTH */
			SIZE delta_x, delta_y;
			ELEMENT *eptr;

			LockElement (ElementPtr->hTarget, &eptr);
			delta_x = eptr->current.location.x
					- ElementPtr->current.location.x;
			delta_y = eptr->current.location.y
					- ElementPtr->current.location.y;
			UnlockElement (ElementPtr->hTarget);
			delta_x = WRAP_DELTA_X (delta_x);
			delta_y = WRAP_DELTA_Y (delta_y);
			facing = NORMALIZE_FACING (
					ANGLE_TO_FACING (ARCTAN (delta_x, delta_y))
					);

			if (delta_x < 0)
				delta_x = -delta_x;
			if (delta_y < 0)
				delta_y = -delta_y;
			delta_x = WORLD_TO_DISPLAY (delta_x);
			delta_y = WORLD_TO_DISPLAY (delta_y);
			if (delta_x >= ACTIVATE_RANGE
					|| delta_y >= ACTIVATE_RANGE
					|| (DWORD)((UWORD)delta_x * delta_x)
					+ (DWORD)((UWORD)delta_y * delta_y) >=
					(DWORD)ACTIVATE_RANGE * ACTIVATE_RANGE)
			{
				ZeroVelocityComponents (&ElementPtr->velocity);
			}
			else
			{
				ElementPtr->thrust_wait = TRACK_WAIT;
				SetVelocityVector (&ElementPtr->velocity, DISPLAY_TO_WORLD (2 << RESOLUTION_FACTOR), facing);
			}
		}
	}

	spin_preprocess (ElementPtr);
}

static void
decelerate_preprocess (ELEMENT *ElementPtr)
{
	SIZE dx, dy;

	GetCurrentVelocityComponents (&ElementPtr->velocity, &dx, &dy);
	dx /= 2;
	dy /= 2;
	SetVelocityComponents (&ElementPtr->velocity, dx, dy);
	if (dx == 0 && dy == 0)
	{
		ElementPtr->preprocess_func = buzztrack_preprocess;
	}

	spin_preprocess (ElementPtr);
}

static void
splinter_preprocess (ELEMENT *ElementPtr)
{
	ElementPtr->next.image.frame =
			IncFrameIndex (ElementPtr->current.image.frame);
	ElementPtr->state_flags |= CHANGING;
}

static void
buzzsaw_collision (ELEMENT *ElementPtr0, POINT *pPt0,
		ELEMENT *ElementPtr1, POINT *pPt1)
{
	weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);

	if (ElementPtr0->state_flags & DISAPPEARING)
	{
		ElementPtr0->state_flags &= ~DISAPPEARING;
		ElementPtr0->state_flags |= NONSOLID | CHANGING;
		ElementPtr0->life_span = 5;
		ElementPtr0->next.image.frame =
				SetAbsFrameIndex (ElementPtr0->current.image.frame, 2);

		ElementPtr0->preprocess_func = splinter_preprocess;
	}
}

static void
buzzsaw_preprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (!(StarShipPtr->cur_status_flags & WEAPON))
	{
		ElementPtr->life_span >>= 1;
		ElementPtr->preprocess_func = decelerate_preprocess;
	}

	spin_preprocess (ElementPtr);
}

static void
buzzsaw_postprocess (ELEMENT *ElementPtr)
{
	HELEMENT hElement;

	ElementPtr->postprocess_func = 0;
	hElement = AllocElement ();
	if (hElement)
	{
		COUNT primIndex;
		ELEMENT *ListElementPtr;
		STARSHIP *StarShipPtr;

		LockElement (hElement, &ListElementPtr);
		primIndex = ListElementPtr->PrimIndex;
		*ListElementPtr = *ElementPtr;
		ListElementPtr->PrimIndex = primIndex;
		(GLOBAL (DisplayArray))[primIndex] =
				(GLOBAL (DisplayArray))[ElementPtr->PrimIndex];
		ListElementPtr->current = ListElementPtr->next;
		InitIntersectStartPoint (ListElementPtr);
		InitIntersectEndPoint (ListElementPtr);
		ListElementPtr->state_flags = (ListElementPtr->state_flags
				& ~(PRE_PROCESS | CHANGING | APPEARING))
				| POST_PROCESS;
		UnlockElement (hElement);

		GetElementStarShip (ElementPtr, &StarShipPtr);
		LockElement (StarShipPtr->hShip, &ListElementPtr);
		InsertElement (hElement, GetSuccElement (ListElementPtr));
		UnlockElement (StarShipPtr->hShip);

		ElementPtr->life_span = 0;
	}
}

static COUNT
initialize_buzzsaw (ELEMENT *ShipPtr, HELEMENT SawArray[])
{
#define MISSILE_HITS 10
#define MISSILE_DAMAGE 4
#define MISSILE_OFFSET (9 << RESOLUTION_FACTOR) // JMS_GFX
#define KOHR_AH_OFFSET (28 << RESOLUTION_FACTOR) // JMS_GFX
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
	MissileBlock.pixoffs = KOHR_AH_OFFSET;
	MissileBlock.speed = MISSILE_SPEED; // JMS_GFX
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = buzzsaw_preprocess;
	MissileBlock.blast_offs = MISSILE_OFFSET;
	SawArray[0] = initialize_missile (&MissileBlock);

	if (SawArray[0])
	{
		ELEMENT *SawPtr;

		LockElement (SawArray[0], &SawPtr);
		SawPtr->turn_wait = SAW_RATE;
		SawPtr->thrust_wait = 0;
		SawPtr->postprocess_func = buzzsaw_postprocess;
		SawPtr->collision_func = buzzsaw_collision;
		UnlockElement (SawArray[0]);
	}

	return (1);
}

static void
slylandro_kohrah_escape (STARSHIP *StarShipPtr)
{
	ELEMENT *ElementPtr;
	
	SET_GAME_STATE(SLYLANDRO_KOHRAH_PANIC, 1);
	
	LockElement (StarShipPtr->hShip, &ElementPtr);
	if (GetPrimType (&DisplayArray[ElementPtr->PrimIndex]) == STAMP_PRIM
		&& ElementPtr->life_span == NORMAL_LIFE
		&& !(ElementPtr->state_flags & FINITE_LIFE)
		&& ElementPtr->mass_points != MAX_SHIP_MASS * 10
		&& !(ElementPtr->state_flags & APPEARING))
	{
		extern void flee_preprocess (ELEMENT *);
		
		// JMS: battle_counter 1 instead of 0, since we're dealing with computer's fleet here, not the player's.
		battle_counter[1]--;
		
		// JMS: This is an ugly way of telling the game that enemy ship escaped, not exploded...
		SET_GAME_STATE (ENEMY_ESCAPE_OCCURRED, 1);
		
		ElementPtr->turn_wait = 3;
		ElementPtr->thrust_wait = MAKE_BYTE (4, 0);
		ElementPtr->preprocess_func = flee_preprocess;
		ElementPtr->mass_points = MAX_SHIP_MASS * 10;
		ZeroVelocityComponents (&ElementPtr->velocity);
		StarShipPtr->cur_status_flags &=
		~(SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED);
		
		SetPrimColor (&DisplayArray[ElementPtr->PrimIndex],
					  BUILD_COLOR (MAKE_RGB15 (0x0B, 0x00, 0x00), 0x2E));
		SetPrimType (&DisplayArray[ElementPtr->PrimIndex], STAMPFILL_PRIM);
		
		StarShipPtr->ship_input_state = 0;
	}
	UnlockElement (StarShipPtr->hShip);
}

static void
slylandro_kohrah_intelligence (ELEMENT *ShipPtr, EVALUATE_DESC *ObjectsOfConcern,
		COUNT ConcernCounter)
{
	EVALUATE_DESC *lpEvalDesc;
	STARSHIP *StarShipPtr;

	lpEvalDesc = &ObjectsOfConcern[ENEMY_WEAPON_INDEX];
	if (lpEvalDesc->ObjectPtr
			&& lpEvalDesc->MoveState == ENTICE
			&& (lpEvalDesc->ObjectPtr->state_flags & CREW_OBJECT)
			&& lpEvalDesc->which_turn <= 8)
		lpEvalDesc->MoveState = PURSUE;

	ship_intelligence (ShipPtr,
			ObjectsOfConcern, ConcernCounter);

	GetElementStarShip (ShipPtr, &StarShipPtr);
	StarShipPtr->ship_input_state &= ~SPECIAL;

	// JMS: Slylandro-kohrahs will escape from battle ASAP.
	slylandro_kohrah_escape(StarShipPtr);	
	
	if (StarShipPtr->special_counter == 0
			&& StarShipPtr->RaceDescPtr->ship_info.energy_level >= SPECIAL_ENERGY_COST
			&& lpEvalDesc->ObjectPtr
			&& lpEvalDesc->which_turn <= 8)
		StarShipPtr->ship_input_state |= SPECIAL;

	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	if (lpEvalDesc->ObjectPtr)
	{
#define FRAGMENT_LIFE 10
#define FRAGMENT_SPEED MISSILE_SPEED
#define FRAGMENT_RANGE (FRAGMENT_LIFE * FRAGMENT_SPEED)
		HELEMENT h, hNext;
		ELEMENT *BuzzSawPtr;

		h = (StarShipPtr->old_status_flags & WEAPON) ?
				GetSuccElement (ShipPtr) : (HELEMENT)0;
		for (; h; h = hNext)
		{
			LockElement (h, &BuzzSawPtr);
			hNext = GetSuccElement (BuzzSawPtr);
			if (!(BuzzSawPtr->state_flags & NONSOLID)
					&& BuzzSawPtr->next.image.farray == StarShipPtr->RaceDescPtr->ship_data.weapon
					&& BuzzSawPtr->life_span > MISSILE_LIFE * 3 / 4
			    && elementsOfSamePlayer(BuzzSawPtr, ShipPtr))
			{
				{
					//COUNT which_turn;

					if (!PlotIntercept (BuzzSawPtr,
							lpEvalDesc->ObjectPtr, BuzzSawPtr->life_span,
							FRAGMENT_RANGE / 2))
						StarShipPtr->ship_input_state &= ~WEAPON;
					else if (StarShipPtr->weapon_counter == 0)
						StarShipPtr->ship_input_state |= WEAPON;

					UnlockElement (h);
					break;
				}
				hNext = 0;
			}
			UnlockElement (h);
		}

		if (h == 0)
		{
			if (StarShipPtr->old_status_flags & WEAPON)
				StarShipPtr->ship_input_state &= ~WEAPON;
			else if (StarShipPtr->weapon_counter == 0
					&& ship_weapons (ShipPtr, lpEvalDesc->ObjectPtr, FRAGMENT_RANGE / 2))
				StarShipPtr->ship_input_state |= WEAPON;

			if (StarShipPtr->special_counter == 0
					&& !(StarShipPtr->ship_input_state & WEAPON)
					&& lpEvalDesc->which_turn <= 8
					&& (StarShipPtr->ship_input_state & (LEFT | RIGHT))
					&& StarShipPtr->RaceDescPtr->ship_info.energy_level >=
					SPECIAL_ENERGY_COST)
				StarShipPtr->ship_input_state |= SPECIAL;
		}
	}
}

#define GAS_RATE 2

static void
gas_cloud_preprocess (ELEMENT *ElementPtr)
{
	if (ElementPtr->turn_wait)
		--ElementPtr->turn_wait;
	else
	{
		ElementPtr->next.image.frame =
				IncFrameIndex (ElementPtr->current.image.frame);
		ElementPtr->state_flags |= CHANGING;

		ElementPtr->turn_wait = GAS_RATE;
	}
}

#define GAS_DAMAGE 3

static void
gas_cloud_collision (ELEMENT *ElementPtr0, POINT *pPt0,
		ELEMENT *ElementPtr1, POINT *pPt1)
{
	if (ElementPtr1->state_flags & PLAYER_SHIP)
		ElementPtr0->mass_points = GAS_DAMAGE;
	else
		ElementPtr0->mass_points = 50;

	weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
}

static void
spawn_gas_cloud (ELEMENT *ElementPtr)
{
#define GAS_SPEED (16 << RESOLUTION_FACTOR) // JMS_GFX
#define GAS_HITS 100
#define GAS_OFFSET (2 << RESOLUTION_FACTOR) // JMS_GFX
#define NUM_GAS_CLOUDS 16
	SIZE dx, dy;
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	MissileBlock.cx = ElementPtr->next.location.x;
	MissileBlock.cy = ElementPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
	MissileBlock.index = 0;
	MissileBlock.sender = ElementPtr->playerNr;
	MissileBlock.flags = IGNORE_SIMILAR;
	MissileBlock.pixoffs = 20 << RESOLUTION_FACTOR; // JMS_GFX
	MissileBlock.speed = GAS_SPEED;
	MissileBlock.hit_points = GAS_HITS;
	MissileBlock.damage = GAS_DAMAGE;
	MissileBlock.life =
			GetFrameCount (MissileBlock.farray[0]) * (GAS_RATE + 1) - 1;
	MissileBlock.preprocess_func = gas_cloud_preprocess;
	MissileBlock.blast_offs = GAS_OFFSET;

	GetCurrentVelocityComponents (&ElementPtr->velocity, &dx, &dy);
	for (MissileBlock.face = 0;
			MissileBlock.face < ANGLE_TO_FACING (FULL_CIRCLE);
			MissileBlock.face +=
			(ANGLE_TO_FACING (FULL_CIRCLE) / NUM_GAS_CLOUDS))
	{
		HELEMENT hGasCloud;

		hGasCloud = initialize_missile (&MissileBlock);
		if (hGasCloud)
		{
			ELEMENT *GasCloudPtr;

			LockElement (hGasCloud, &GasCloudPtr);
			SetElementStarShip (GasCloudPtr, StarShipPtr);
			GasCloudPtr->hTarget = 0;
			GasCloudPtr->turn_wait = GAS_RATE - 1;
			GasCloudPtr->collision_func = gas_cloud_collision;
			DeltaVelocityComponents (&GasCloudPtr->velocity, dx, dy);
			UnlockElement (hGasCloud);
			PutElement (hGasCloud);
		}
	}
}

static void
slylandro_kohrah_postprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if ((StarShipPtr->cur_status_flags & SPECIAL)
			&& StarShipPtr->special_counter == 0
			&& DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST))
	{
		spawn_gas_cloud (ElementPtr);

		ProcessSound (SetAbsSoundIndex (
				StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);

		StarShipPtr->special_counter = SPECIAL_WAIT;
	}
}

static void
slylandro_kohrah_preprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
			/* no spinning disks */
	StarShipPtr->RaceDescPtr->characteristics.special_wait = 0;
	if (StarShipPtr->weapon_counter == 0
			&& (StarShipPtr->cur_status_flags
			& StarShipPtr->old_status_flags & WEAPON))
		++StarShipPtr->weapon_counter;
}

RACE_DESC*
init_slylandro_kohrah (void)
{
	RACE_DESC *RaceDescPtr;

	if (RESOLUTION_FACTOR == 0)
	{
		slylandro_kohrah_desc.preprocess_func = slylandro_kohrah_preprocess;
		slylandro_kohrah_desc.postprocess_func = slylandro_kohrah_postprocess;
		slylandro_kohrah_desc.init_weapon_func = initialize_buzzsaw;
		slylandro_kohrah_desc.cyborg_control.intelligence_func = slylandro_kohrah_intelligence;
		RaceDescPtr = &slylandro_kohrah_desc;
	}
	else if (RESOLUTION_FACTOR == 1)
	{
		slylandro_kohrah_desc_2xres.preprocess_func = slylandro_kohrah_preprocess;
		slylandro_kohrah_desc_2xres.postprocess_func = slylandro_kohrah_postprocess;
		slylandro_kohrah_desc_2xres.init_weapon_func = initialize_buzzsaw;
		slylandro_kohrah_desc_2xres.cyborg_control.intelligence_func = slylandro_kohrah_intelligence;
		RaceDescPtr = &slylandro_kohrah_desc_2xres;
	}
	else
	{
		slylandro_kohrah_desc_4xres.preprocess_func = slylandro_kohrah_preprocess;
		slylandro_kohrah_desc_4xres.postprocess_func = slylandro_kohrah_postprocess;
		slylandro_kohrah_desc_4xres.init_weapon_func = initialize_buzzsaw;
		slylandro_kohrah_desc_4xres.cyborg_control.intelligence_func = slylandro_kohrah_intelligence;
		RaceDescPtr = &slylandro_kohrah_desc_4xres;
	}

	return (RaceDescPtr);
}
