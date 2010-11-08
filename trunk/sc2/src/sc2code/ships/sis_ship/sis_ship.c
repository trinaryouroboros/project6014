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

// JMS 2009: -Added Orz space state check to exp_hyper_preprocess
// JMS 2010: -Red ship gfx in hyperspace, blue in Orz space, green in quasispace
//			 -Added separate weaponry function for Chmmr Explorer
//			 -Added some nuts and bolts to make Chmmr Explorer weapons work
//			 -Modded Chmmr Explorer main weapon as dual alternate side firing ion bolt cannon
//			 -Chmmr Explorer secondary: Shard mines
//			 -Autopilot now engages only after coming to full stop first
//			 -Reduced weapon range (Blaster life) from 12 to 9
//			 -Changed special weapon to confuse banger. Retained the shardmine code still, but it is disabled. Look for JMS_CONFUSER
//			 -Removed all the old and unnecessary shit not related to current Explorer configuration

#include "ships/ship.h"
#include "ships/sis_ship/resinst.h"

#include "colors.h"
#include "controls.h"
#include "globdata.h"
#include "libs/mathlib.h"
#include "libs/log.h"

#define MAX_CREW MAX_CREW_SIZE
#define MAX_ENERGY 42
#define ENERGY_REGENERATION 1
#define ENERGY_WAIT 6

#define SHIP_MASS MAX_SHIP_MASS
#define BLASTER_SPEED DISPLAY_TO_WORLD (24)
#define BLASTER_LIFE 9 // JMS: WAS 12

// These affect Chmmr Explorer in Super Melee and adventure mode
#define EXPLORER_MAX_THRUST 36
#define EXPLORER_THRUST_INCREMENT 4
#define EXPLORER_TURN_WAIT 1
#define EXPLORER_THRUST_WAIT 1

// Chmmr Explorer has smaller weapon delay and largish special delay
#define EXPLORER_WEAPON_WAIT 2
#define EXPLORER_SPECIAL_WAIT 18
#define EXPLORER_WEAPON_ENERGY_COST 0 // Weapon cost is set at "+3" in initModuleSlots.
#define EXPLORER_SPECIAL_ENERGY_COST 0

#define MAX_THRUST 10		//
#define TURN_WAIT 17		// JMS: Kept these defines for now since they might have some effect
#define THRUST_WAIT 6		// in the speed calculations in some point. Not sure though, got to check in some point!

static RACE_DESC exp_desc =
{
	{ /* SHIP_INFO */
		0,
		25, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		EXP_RACE_STRINGS,
		EXP_ICON_MASK_PMAP_ANIM,	// Explorer icons
		EXP_MICON_MASK_PMAP_ANIM,	// Explorer melee icons
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		0, /* Initial sphere of influence radius */
		{ /* Known location (center of SoI) */
			0, 0,
		},
	},
	{
		MAX_THRUST,
		EXPLORER_THRUST_INCREMENT,
		ENERGY_REGENERATION,
		EXPLORER_WEAPON_ENERGY_COST,
		EXPLORER_SPECIAL_ENERGY_COST,
		ENERGY_WAIT,
		TURN_WAIT,
		THRUST_WAIT,
		EXPLORER_WEAPON_WAIT,
		0,  // Special weapon wait
		SHIP_MASS,
	},
	{
		{
			EXP_BIG_MASK_PMAP_ANIM,
			EXP_MED_MASK_PMAP_ANIM,
			EXP_SML_MASK_PMAP_ANIM,
		},
		{
			BLASTER_BIG_MASK_PMAP_ANIM,
			BLASTER_MED_MASK_PMAP_ANIM,
			BLASTER_SML_MASK_PMAP_ANIM,
		},
		{
			CONFUSE_BANGER_BIG_MASK_PMAP_ANIM,
			CONFUSE_BANGER_MED_MASK_PMAP_ANIM,
			CONFUSE_BANGER_SML_MASK_PMAP_ANIM,
		},
		{
			SIS_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		EXP_VICTORY_SONG,
		EXP_SHIP_SOUNDS,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		BLASTER_SPEED * BLASTER_LIFE,
		NULL,
	},
	(UNINIT_FUNC *) NULL,
	(PREPROCESS_FUNC *) NULL,
	(POSTPROCESS_FUNC *) NULL,
	(INIT_WEAPON_FUNC *) NULL,
	0,
};

static void InitModuleSlots (RACE_DESC *RaceDescPtr);
static void InitDriveSlots (RACE_DESC *RaceDescPtr, const BYTE *DriveSlots);
static void InitJetSlots (RACE_DESC *RaceDescPtr, const BYTE *JetSlots);
void uninit_exp (RACE_DESC *pRaceDesc);

/* Hyperspace movement */
static void
exp_hyper_preprocess (ELEMENT *ElementPtr)
{
	SIZE udx, udy, dx, dy;
	SIZE AccelerateDirection;
	STARSHIP *StarShipPtr;
	SDWORD dtempx, dtempy;	// JMS_GFX: These babies help to make the hyperspace speed calculations not overflow in 640x480.

	if (ElementPtr->state_flags & APPEARING)
		ElementPtr->velocity = GLOBAL (velocity);

	AccelerateDirection = 0;
	GetElementStarShip (ElementPtr, &StarShipPtr);
	++StarShipPtr->weapon_counter; /* no shooting in hyperspace! */
	
	if ((GLOBAL (autopilot)).x == ~0
			|| (GLOBAL (autopilot)).y == ~0
			|| (StarShipPtr->cur_status_flags & (LEFT | RIGHT | THRUST))
			|| !(GET_GAME_STATE(AUTOPILOT_OK))) // JMS: This check makes autopilot engage only after coming to full stop
	{
LeaveAutoPilot:
		
		// JMS: This re-check is now needed because of the added autopilot_ok variable to previous check
		if ((GLOBAL (autopilot)).x == ~0 || (GLOBAL (autopilot)).y == ~0 || (StarShipPtr->cur_status_flags & (LEFT | RIGHT | THRUST)))
				(GLOBAL (autopilot)).x = (GLOBAL (autopilot)).y = ~0;
		
		if (!(StarShipPtr->cur_status_flags & THRUST)
				|| (GLOBAL_SIS (FuelOnBoard) == 0
				&& (GET_GAME_STATE (ARILOU_SPACE_SIDE) <= 1) && GET_GAME_STATE (ORZ_SPACE_SIDE) <= 1)) // JMS
		{
			AccelerateDirection = -1;
			GetCurrentVelocityComponents (&ElementPtr->velocity, &dx, &dy);
			
			// JMS: Engage autopilot only after coming to full stop
			if (dx==0 && dy==0)
				SET_GAME_STATE (AUTOPILOT_OK, 1);
			else
				SET_GAME_STATE (AUTOPILOT_OK, 0);
			
			// JMS_GFX: Since the max speed with max thrusters in 640x480 mode is 2048, 
			// SIZE variables dx and dy would overflow when leftshifted by 4. (The speed would
			// be then 32768 which would wrap around to -32768 instead).
			// If speed would surpass the wraparound limit, the speed is limited to max of 32767.
			// This introduces very slight error to the speed, but who cares - it is negligible.
			dtempx = (SDWORD)dx;
			dtempy = (SDWORD)dy;
			if((dtempx << 4) > 32767)
				udx = 32767;
			else
				udx = dtempx;
			if((dtempy << 4) > 32767)
				udy = 32767;
			else
				udy = dtempy;

			StarShipPtr->cur_status_flags &= ~THRUST;
		}
	}
	else
	{
		SIZE facing;
		POINT universe;

		universe.x = LOGX_TO_UNIVERSE (GLOBAL_SIS (log_x));
		universe.y = LOGY_TO_UNIVERSE (GLOBAL_SIS (log_y));
		udx = (GLOBAL (autopilot)).x - universe.x;
		udy = -((GLOBAL (autopilot)).y - universe.y);
		if ((dx = udx) < 0)
			dx = -dx;
		if ((dy = udy) < 0)
			dy = -dy;
		if (dx <= 1 && dy <= 1)
			goto LeaveAutoPilot;

		facing = NORMALIZE_FACING (ANGLE_TO_FACING (ARCTAN (udx, udy)));

		/* This prevents ship from flying backwards on auto-pilot.
		 * It could also theoretically abort autopilot in a bad savegame */
		if ((StarShipPtr->cur_status_flags & SHIP_AT_MAX_SPEED)
				/*|| (ElementPtr->state_flags & APPEARING)*/ )
		{
			if (NORMALIZE_FACING (StarShipPtr->ShipFacing + ANGLE_TO_FACING (QUADRANT) - facing) > ANGLE_TO_FACING (HALF_CIRCLE))
				goto LeaveAutoPilot;

			facing = StarShipPtr->ShipFacing;
		}
		else if ((int)facing != (int)StarShipPtr->ShipFacing
				&& ElementPtr->turn_wait == 0)
		{
			if (NORMALIZE_FACING (StarShipPtr->ShipFacing - facing) >= ANGLE_TO_FACING (HALF_CIRCLE))
			{
				facing = NORMALIZE_FACING (facing - 1);
				StarShipPtr->cur_status_flags |= RIGHT;
			}
			else if ((int)StarShipPtr->ShipFacing != (int)facing)
			{
				facing = NORMALIZE_FACING (facing + 1);
				StarShipPtr->cur_status_flags |= LEFT;
			}

			if ((int)facing == (int)StarShipPtr->ShipFacing)
				ZeroVelocityComponents (&ElementPtr->velocity);
		}

		GetCurrentVelocityComponents (&ElementPtr->velocity, &dx, &dy);
		
		if ((GLOBAL_SIS (FuelOnBoard)
				|| GET_GAME_STATE (ARILOU_SPACE_SIDE) > 1 || GET_GAME_STATE (ORZ_SPACE_SIDE) > 1) // JMS: Orz space check.
				&& (int)facing == (int)StarShipPtr->ShipFacing)
		{
			StarShipPtr->cur_status_flags |= SHIP_AT_MAX_SPEED;
			AccelerateDirection = 1;
		}
		else
		{
			AccelerateDirection = -1;
			udx = dx << 4;
			udy = dy << 4;
		}
	}

	if (ElementPtr->thrust_wait == 0 && AccelerateDirection)
	{
		COUNT dist;
		SIZE speed, velocity_increment;

		velocity_increment = WORLD_TO_VELOCITY (StarShipPtr->RaceDescPtr->characteristics.thrust_increment);

		if ((dist = square_root ((long)udx * udx + (long)udy * udy)) == 0)
			dist = 1; /* prevent divide by zero */

		speed = square_root ((long)dx * dx + (long)dy * dy);
		
		if (AccelerateDirection < 0)
		{
			dy = (speed / velocity_increment - 1) * velocity_increment;
			
			if (dy < speed - velocity_increment)
				dy = speed - velocity_increment;
			if ((speed = dy) < 0)
				speed = 0;

			StarShipPtr->cur_status_flags &= ~SHIP_AT_MAX_SPEED;
		}
		else
		{
			SIZE max_velocity;

			AccelerateDirection = 0;

			max_velocity = WORLD_TO_VELOCITY (StarShipPtr->RaceDescPtr->characteristics.max_thrust);

			dy = (speed / velocity_increment + 1) * velocity_increment;
			
			if (dy < speed + velocity_increment)
				dy = speed + velocity_increment;
			
			if ((speed = dy) > max_velocity)
			{
				speed = max_velocity;
				StarShipPtr->cur_status_flags |= SHIP_AT_MAX_SPEED;
			}
		}
		
		dx = (SIZE)((long)udx * speed / (long)dist);
		dy = (SIZE)((long)udy * speed / (long)dist);
		
		SetVelocityComponents (&ElementPtr->velocity, dx, dy);
		ElementPtr->thrust_wait =StarShipPtr->RaceDescPtr->characteristics.thrust_wait;
	}
}

static void
exp_hyper_postprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GLOBAL (velocity) = ElementPtr->velocity;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if ( ((StarShipPtr->cur_status_flags & WEAPON) || PulsedInputState.menu[KEY_MENU_CANCEL])
			&& StarShipPtr->special_counter == 0 )
	{
#define MENU_DELAY 10
		DoMenuOptions ();
		StarShipPtr->cur_status_flags &= ~SHIP_AT_MAX_SPEED;
		StarShipPtr->special_counter = MENU_DELAY;
	}
}

/* Weapons: Stunner special */
static void
stunner_preprocess (ELEMENT *ElementPtr)
{
	if (!(ElementPtr->state_flags & NONSOLID))
	{
		/*ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->current.image.frame,
			(GetFrameIndex (ElementPtr->current.image.frame) + 1) & 7);*/
		ElementPtr->state_flags |= CHANGING;
	}
	else if (ElementPtr->hTarget == 0)
	{
		ElementPtr->life_span = 0;
		ElementPtr->state_flags |= DISAPPEARING;
	}
	else
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

			// Shiver: This if statement below prevents the Androsynth Blazer from losing its energy degeneration.
			if (!(eptr->current.image.farray == StarShipPtr->RaceDescPtr->ship_data.special))
				StarShipPtr->energy_counter += 1;

			StarShipPtr->ship_input_state = (StarShipPtr->ship_input_state & ~(LEFT | RIGHT | THRUST | WEAPON | SPECIAL));
			
			hEffect = AllocElement ();
			if (hEffect)
			{
				LockElement (hEffect, &eptr);
				
				eptr->state_flags = FINITE_LIFE | NONSOLID | CHANGING | (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));
				eptr->life_span = 1;
				eptr->current = eptr->next = ElementPtr->next;
				eptr->preprocess_func = stunner_preprocess;
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

#define STUN_DURATION 72

static void
stunner_collision (ELEMENT *ElementPtr0, POINT *pPt0, ELEMENT *ElementPtr1, POINT *pPt1)
{	
	if (ElementPtr1->state_flags & PLAYER_SHIP)
	{
		HELEMENT hStunElement, hNextElement;
		ELEMENT *StunPtr;
		STARSHIP *StarShipPtr;
		
		GetElementStarShip (ElementPtr0, &StarShipPtr);
		for (hStunElement = GetHeadElement ();
			 hStunElement; hStunElement = hNextElement)
		{
			LockElement (hStunElement, &StunPtr);
			if ((StunPtr->state_flags & (GOOD_GUY | BAD_GUY)) ==
				(ElementPtr0->state_flags & (GOOD_GUY | BAD_GUY))
				&& StunPtr->current.image.farray ==
				StarShipPtr->RaceDescPtr->ship_data.special
				&& (StunPtr->state_flags & NONSOLID))
			{
				UnlockElement (hStunElement);
				break;
			}
			hNextElement = GetSuccElement (StunPtr);
			UnlockElement (hStunElement);
		}
		
		if (hStunElement || (hStunElement = AllocElement ()))
		{
			LockElement (hStunElement, &StunPtr);
			
			if (StunPtr->state_flags == 0) /* not allocated before */
			{
				InsertElement (hStunElement, GetHeadElement ());
				
				StunPtr->current = ElementPtr0->next;
				StunPtr->current.image.frame = SetAbsFrameIndex (StunPtr->current.image.frame, 16);
				StunPtr->next = StunPtr->current;
				StunPtr->state_flags = FINITE_LIFE | NONSOLID | CHANGING
				| (ElementPtr0->state_flags & (GOOD_GUY | BAD_GUY));
				StunPtr->preprocess_func = stunner_preprocess;
				SetPrimType ( &(GLOBAL (DisplayArray))[StunPtr->PrimIndex], NO_PRIM);
				
				SetElementStarShip (StunPtr, StarShipPtr);
				GetElementStarShip (ElementPtr1, &StarShipPtr);
				StunPtr->hTarget = StarShipPtr->hShip;
			}
			
			StunPtr->life_span = STUN_DURATION;
			StunPtr->turn_wait = (BYTE)(1 << ((BYTE)TFB_Random () & 1)); /* LEFT or RIGHT */
			
			UnlockElement (hStunElement);
		}
		
		weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
		
		ElementPtr0->hit_points = 0;
		ElementPtr0->life_span = 0;
		ElementPtr0->state_flags |= DISAPPEARING | COLLISION | NONSOLID;
		
	}
	(void) pPt0;  /* Satisfying compiler (unused parameter) */
	(void) pPt1;  /* Satisfying compiler (unused parameter) */
}

static COUNT
initialize_stunner (ELEMENT *ShipPtr, HELEMENT StunArray[])
{
#define SMISSILE_SPEED DISPLAY_TO_WORLD (16)
#define SMISSILE_HITS 50
#define SMISSILE_DAMAGE 0
#define SMISSILE_LIFE 20
#define SMISSILE_OFFSET 4
#define SMISSILE_START_OFFSET 28
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK StunBlock;
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	StunBlock.cx = ShipPtr->next.location.x;
	StunBlock.cy = ShipPtr->next.location.y;
	StunBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
	StunBlock.face = StarShipPtr->ShipFacing;
	StunBlock.index = StunBlock.face;
	StunBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY)) | IGNORE_SIMILAR;
	StunBlock.pixoffs = SMISSILE_START_OFFSET;
	StunBlock.speed = SMISSILE_SPEED;
	StunBlock.hit_points = SMISSILE_HITS;
	StunBlock.damage = SMISSILE_DAMAGE;
	StunBlock.life = SMISSILE_LIFE;
	StunBlock.preprocess_func = stunner_preprocess;
	StunBlock.blast_offs = SMISSILE_OFFSET;
	StunArray[0] = initialize_missile (&StunBlock);
	
	if (StunArray[0])
	{
		ELEMENT *SMissilePtr;
		SIZE dx, dy;
		
		LockElement (StunArray[0], &SMissilePtr);

		// Shiver: This pushes the SMissile toward the direction the Explorer is moving.
		GetCurrentVelocityComponents (&ShipPtr->velocity, &dx, &dy);
		dx = dx * 3/4;
		dy = dy * 3/4;
		DeltaVelocityComponents (&SMissilePtr->velocity, dx, dy);
		SMissilePtr->current.location.x -= VELOCITY_TO_WORLD (dx);
		SMissilePtr->current.location.y -= VELOCITY_TO_WORLD (dy);

		SMissilePtr->collision_func = stunner_collision;
		SetElementStarShip (SMissilePtr, StarShipPtr);
		UnlockElement (StunArray[0]);
	}
	return (1);
}

/* Melee related pre/postprocess */
static void
exp_battle_preprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);

	if (!(StarShipPtr->RaceDescPtr->ship_info.ship_flags & (FIRES_FORE | FIRES_RIGHT | FIRES_AFT | FIRES_LEFT)))
	{
		StarShipPtr->cur_status_flags &= ~WEAPON;
		StarShipPtr->weapon_counter = 2;
	}
}

static void
exp_battle_postprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;
	GetElementStarShip (ElementPtr, &StarShipPtr);
	
	if (StarShipPtr->special_counter == 0
		&& (StarShipPtr->cur_status_flags & SPECIAL)
		&& (DeltaEnergy (ElementPtr, -(StarShipPtr->RaceDescPtr->characteristics.special_energy_cost)))
		)
	{
		HELEMENT Stunner;
		initialize_stunner (ElementPtr, &Stunner);
		if (Stunner)
		{
			ELEMENT *SMISSILEPtr;
			LockElement (Stunner, &SMISSILEPtr);
			
			ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), SMISSILEPtr);
			
			UnlockElement (Stunner);
			PutElement (Stunner);
			StarShipPtr->special_counter = EXPLORER_SPECIAL_WAIT;
		}
	}
}

/* Weapons: Main weapon */
#define BLASTER_DAMAGE 2
static void
blaster_collision (ELEMENT *ElementPtr0, POINT *pPt0, ELEMENT *ElementPtr1, POINT *pPt1)
{
	HELEMENT hBlastElement;

	hBlastElement = weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
	if (hBlastElement)
	{
		ELEMENT *BlastElementPtr;

		LockElement (hBlastElement, &BlastElementPtr);
		switch (ElementPtr0->mass_points)
		{
			case BLASTER_DAMAGE * 1:
				BlastElementPtr->life_span = 2;
				BlastElementPtr->current.image.frame = SetAbsFrameIndex (ElementPtr0->current.image.frame, 0);
				BlastElementPtr->preprocess_func = NULL;
				break;
			case BLASTER_DAMAGE * 2:
				BlastElementPtr->life_span = 6;
				BlastElementPtr->current.image.frame = IncFrameIndex (ElementPtr0->current.image.frame);
				break;
			case BLASTER_DAMAGE * 3:
				BlastElementPtr->life_span = 7;
				BlastElementPtr->current.image.frame = SetAbsFrameIndex (ElementPtr0->current.image.frame, 20);
				break;
		}
		UnlockElement (hBlastElement);
	}
}

static void
blaster_preprocess (ELEMENT *ElementPtr)
{
	BYTE wait;

	switch (ElementPtr->mass_points)
	{
		case BLASTER_DAMAGE * 1:
			if (GetFrameIndex (ElementPtr->current.image.frame) < 8)
			{
				ElementPtr->next.image.frame = IncFrameIndex (ElementPtr->current.image.frame);
				ElementPtr->state_flags |= CHANGING;
			}
			break;
		case BLASTER_DAMAGE * 3:
			if (GetFrameIndex (ElementPtr->current.image.frame) < 19)
				ElementPtr->next.image.frame = IncFrameIndex (ElementPtr->current.image.frame);
			else
				ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->current.image.frame, 16);
			ElementPtr->state_flags |= CHANGING;
			break;
	}

	if (LONIBBLE (ElementPtr->turn_wait))
		--ElementPtr->turn_wait;
	else if ((wait = HINIBBLE (ElementPtr->turn_wait)))
	{
		COUNT facing;

		facing = NORMALIZE_FACING (ANGLE_TO_FACING (GetVelocityTravelAngle (&ElementPtr->velocity)));
		if (TrackShip (ElementPtr, &facing) > 0)
			SetVelocityVector (&ElementPtr->velocity, BLASTER_SPEED, facing);

		ElementPtr->turn_wait = MAKE_BYTE (wait, wait);
	}
}

static COUNT
initialize_explorer_weaponry (ELEMENT *ShipPtr, HELEMENT BlasterArray[])
{
#define EXP_VERT_OFFSET 28
#define EXP_HORZ_OFFSET 20
#define EXP_HORZ_OFFSET_2 (DISPLAY_TO_WORLD(5 * RESOLUTION_FACTOR))
#define EXP_HORZ_OFFSET_3 (DISPLAY_TO_WORLD(-5 * RESOLUTION_FACTOR))
#define BLASTER_HITS 2
#define BLASTER_OFFSET 8
	COUNT num_blasters;
	
	BYTE nt;
	COUNT i;
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock[6];
	MISSILE_BLOCK *lpMB;
	
	COORD cx, cy;
	static COUNT blaster_side[2]={0,0};
	COUNT facing, angle;
	SIZE offs_x, offs_y;
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	
	num_blasters = 0;
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	facing = StarShipPtr->ShipFacing;
	angle = FACING_TO_ANGLE (facing);

	blaster_side[(ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))] = 
		(blaster_side[(ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))] + 1) % 2;
	
	cx = ShipPtr->next.location.x;
	cy = ShipPtr->next.location.y;
	
	if(blaster_side[(ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))])
	{
		offs_x = -SINE (angle, EXP_HORZ_OFFSET_2);
		offs_y = COSINE (angle, EXP_HORZ_OFFSET_2);
	}
	else
	{
		offs_x = -SINE (angle, EXP_HORZ_OFFSET_3);
		offs_y = COSINE (angle, EXP_HORZ_OFFSET_3);
	}
		
	for (i = 0, lpMB = &MissileBlock[0]; i < 1; ++i)
	{
		BYTE which_gun;

		// JMS: Chmmr Explorer always has weapon in nose weapon slot.
		if(i == 0)
			which_gun = GUN_WEAPON;
		else
			which_gun = 0;
		
		if (which_gun >= GUN_WEAPON && which_gun <= CANNON_WEAPON)
		{
			which_gun -= GUN_WEAPON - 1;
			lpMB->cx = cx + offs_x;
			lpMB->cy = cy + offs_y;
			lpMB->farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
			lpMB->sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
			| IGNORE_SIMILAR;
			lpMB->blast_offs = BLASTER_OFFSET;
			lpMB->speed = BLASTER_SPEED;
			lpMB->preprocess_func = blaster_preprocess;
			lpMB->hit_points = BLASTER_HITS * which_gun;
			lpMB->damage = BLASTER_DAMAGE * which_gun;
			lpMB->life = BLASTER_LIFE
			+ ((BLASTER_LIFE >> 2) * (which_gun - 1));

			// Which weapon graphics to use
			if (which_gun == 1)
				lpMB->index = 0;
			else if (which_gun == 2)
				lpMB->index = 9;
			else
				lpMB->index = 16;
			
			switch (i)
			{
				case 0: /* NOSE WEAPON */
					lpMB->pixoffs = EXP_VERT_OFFSET;
					lpMB->face = StarShipPtr->ShipFacing;
					break;
				case 1: /* SPREAD WEAPON */
					lpMB->pixoffs = EXP_VERT_OFFSET;
					lpMB->face = NORMALIZE_FACING (StarShipPtr->ShipFacing + 1);
					lpMB[1] = lpMB[0];
					++lpMB;
					lpMB->face = NORMALIZE_FACING (StarShipPtr->ShipFacing - 1);
					break;
				case 2: /* SIDE WEAPON */
					lpMB->pixoffs = EXP_HORZ_OFFSET;
					lpMB->face = NORMALIZE_FACING (StarShipPtr->ShipFacing + ANGLE_TO_FACING (QUADRANT));
					lpMB[1] = lpMB[0];
					++lpMB;
					lpMB->face = NORMALIZE_FACING (StarShipPtr->ShipFacing - ANGLE_TO_FACING (QUADRANT));
					break;
			}
			
			++lpMB;
		}
	}
	
	nt = (BYTE)((4 - 0) & 3); // Was (BYTE)((4 - num_trackers) & 3);
	num_blasters = lpMB - &MissileBlock[0];
	for (i = 0, lpMB = &MissileBlock[0]; i < num_blasters; ++i, ++lpMB)
	{
		if ((BlasterArray[i] = initialize_missile (lpMB)))
		{
			ELEMENT *BlasterPtr;
			
			LockElement (BlasterArray[i], &BlasterPtr);
			BlasterPtr->collision_func = blaster_collision;
			BlasterPtr->turn_wait = MAKE_BYTE (nt, nt);
			UnlockElement (BlasterArray[i]);
		}
	}
	return (num_blasters);
}

/* AI */
static void
exp_intelligence (ELEMENT *ShipPtr, EVALUATE_DESC *ObjectsOfConcern,
		COUNT ConcernCounter)
{
	EVALUATE_DESC *lpEvalDesc;
	STARSHIP *StarShipPtr;

	GetElementStarShip (ShipPtr, &StarShipPtr);

	lpEvalDesc = &ObjectsOfConcern[ENEMY_WEAPON_INDEX];
	if (lpEvalDesc->ObjectPtr)
	{
		if (MANEUVERABILITY (&StarShipPtr->RaceDescPtr->cyborg_control) < MEDIUM_SHIP
				&& lpEvalDesc->MoveState == ENTICE
				&& (!(lpEvalDesc->ObjectPtr->state_flags & CREW_OBJECT)
				|| lpEvalDesc->which_turn <= 8)
				&& (!(lpEvalDesc->ObjectPtr->state_flags & FINITE_LIFE)
				|| (lpEvalDesc->ObjectPtr->mass_points >= 4
				&& lpEvalDesc->which_turn == 2
				&& ObjectsOfConcern[ENEMY_SHIP_INDEX].which_turn > 16)))
			lpEvalDesc->MoveState = PURSUE;
	}

	ship_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter);
	
	// JMS: Use special like Melnorme uses its own special (This could use a little refinement...)
	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	StarShipPtr->ship_input_state &= ~SPECIAL;
	if (StarShipPtr->special_counter == 0 
		&& StarShipPtr->RaceDescPtr->ship_info.energy_level >= StarShipPtr->RaceDescPtr->characteristics.special_energy_cost)
	{
		BYTE old_input_state;
		
		old_input_state = StarShipPtr->ship_input_state;
		
		StarShipPtr->RaceDescPtr->init_weapon_func = initialize_stunner;
		
		++ShipPtr->turn_wait;
		++ShipPtr->thrust_wait;
		ship_intelligence (ShipPtr, ObjectsOfConcern, ENEMY_SHIP_INDEX + 1);
		--ShipPtr->thrust_wait;
		--ShipPtr->turn_wait;
		
		if (StarShipPtr->ship_input_state & WEAPON)
		{
			StarShipPtr->ship_input_state &= ~WEAPON;
			StarShipPtr->ship_input_state |= SPECIAL;
		}
		
		StarShipPtr->ship_input_state = (unsigned char)(old_input_state | (StarShipPtr->ship_input_state & SPECIAL));
		StarShipPtr->RaceDescPtr->init_weapon_func = initialize_explorer_weaponry;
	}
}

/* Ship configuration */
static void
InitModuleSlots (RACE_DESC *RaceDescPtr)
{	
	// JMS: Chmmr Explorer has 50 men max crew complement without modules, precursor vessel 0 as usual
	// Also: Chmmr Explorer has one front firing gun equipped as default.
	RaceDescPtr->ship_info.max_crew = EXPLORER_CREW_CAPACITY;
	RaceDescPtr->ship_info.ship_flags |= FIRES_FORE;
	RaceDescPtr->characteristics.weapon_energy_cost += 3;
	RaceDescPtr->characteristics.special_energy_cost = 12;
}

static void
InitDriveSlots (RACE_DESC *RaceDescPtr, const BYTE *DriveSlots)
{
	COUNT i;

	// NB. RaceDescPtr->characteristics.max_thrust is already initialised.
	RaceDescPtr->characteristics.thrust_wait = 0;
	for (i = 0; i < NUM_DRIVE_SLOTS; ++i)
	{
		switch (DriveSlots[i])
		{
			case FUSION_THRUSTER:
				RaceDescPtr->characteristics.max_thrust += (2 * RESOLUTION_FACTOR); // JMS_GFX
				++RaceDescPtr->characteristics.thrust_wait;
				break;
		}
	}
	
	RaceDescPtr->characteristics.thrust_wait = (BYTE)(THRUST_WAIT - (RaceDescPtr->characteristics.thrust_wait >> 1));
	RaceDescPtr->characteristics.max_thrust =
			((RaceDescPtr->characteristics.max_thrust /
			RaceDescPtr->characteristics.thrust_increment) + 1)
			* RaceDescPtr->characteristics.thrust_increment;
}

static void
InitJetSlots (RACE_DESC *RaceDescPtr, const BYTE *JetSlots)
{
	COUNT i;

	for (i = 0; i < NUM_JET_SLOTS; ++i)
	{
		switch (JetSlots[i])
		{
			case TURNING_JETS:
				RaceDescPtr->characteristics.turn_wait -= 2;
				break;
		}
	}
}

RACE_DESC*
init_exp (void)
{
	RACE_DESC *RaceDescPtr;

	COUNT i;
	static RACE_DESC new_exp_desc;

	new_exp_desc = exp_desc;	
	new_exp_desc.uninit_func = uninit_exp;

	/* hyperspace */
	if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
	{
		for (i = 0; i < NUM_VIEWS; ++i)
		{
			new_exp_desc.ship_data.ship_rsc[i] = NULL_RESOURCE;
			new_exp_desc.ship_data.weapon_rsc[i] = NULL_RESOURCE;
			new_exp_desc.ship_data.special_rsc[i] = NULL_RESOURCE;
		}
		
		new_exp_desc.ship_info.icons_rsc = NULL_RESOURCE;
		new_exp_desc.ship_data.captain_control.captain_rsc = NULL_RESOURCE;
		new_exp_desc.ship_data.victory_ditty_rsc = NULL_RESOURCE;
		new_exp_desc.ship_data.ship_sounds_rsc = NULL_RESOURCE;

		// JMS: Hack: Blue ship in Orz space, green in quasi, red in hyperspace...
		// Need to use separate set of frames for different colors since
		// frame PNGs are in RGB mode instead of indexed color.
		if (GET_GAME_STATE (ORZ_SPACE_SIDE) > 1)
			new_exp_desc.ship_data.ship_rsc[0] = SIS_ORZ_MASK_PMAP_ANIM;
		else if (GET_GAME_STATE (ARILOU_SPACE_SIDE) > 1)
			new_exp_desc.ship_data.ship_rsc[0] = SIS_QUASI_MASK_PMAP_ANIM;
		else
			new_exp_desc.ship_data.ship_rsc[0] = SIS_HYPER_MASK_PMAP_ANIM;

		new_exp_desc.preprocess_func = exp_hyper_preprocess;
		new_exp_desc.postprocess_func = exp_hyper_postprocess;

		 // JMS_GFX
		if (RESOLUTION_FACTOR > 1)
		{
			new_exp_desc.characteristics.max_thrust = (10 * RESOLUTION_FACTOR) - (4 * RESOLUTION_FACTOR); // JMS_GFX
			new_exp_desc.characteristics.thrust_increment *= RESOLUTION_FACTOR;
		}
		else
			new_exp_desc.characteristics.max_thrust -= 4;
	}
	/* Melee fight */
	else 
	{
		new_exp_desc.preprocess_func = exp_battle_preprocess;
		new_exp_desc.postprocess_func = exp_battle_postprocess;
		new_exp_desc.init_weapon_func = initialize_explorer_weaponry;
		new_exp_desc.cyborg_control.intelligence_func = exp_intelligence;

	}

	InitModuleSlots(&new_exp_desc);
	InitDriveSlots(&new_exp_desc, GLOBAL_SIS (DriveSlots));
	InitJetSlots(&new_exp_desc, GLOBAL_SIS (JetSlots));
	
	// JMS: Give the explorer some stats so it won't be slow as fuck in supermelee
	// and does not depend on the thruster/jet numbers in adventure mode
	if((GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS))==0)
	{
		new_exp_desc.characteristics.max_thrust = EXPLORER_MAX_THRUST * RESOLUTION_FACTOR;
		new_exp_desc.characteristics.thrust_wait = EXPLORER_THRUST_WAIT;
		new_exp_desc.characteristics.thrust_increment = EXPLORER_THRUST_INCREMENT * RESOLUTION_FACTOR;
		new_exp_desc.characteristics.turn_wait = EXPLORER_TURN_WAIT;
	}
	
	if (LOBYTE (GLOBAL (CurrentActivity)) == SUPER_MELEE)
	{
		new_exp_desc.ship_info.crew_level = new_exp_desc.ship_info.max_crew;
	}
	else
	{
		// Count the captain too.
		new_exp_desc.ship_info.max_crew++;
		new_exp_desc.ship_info.crew_level = GLOBAL_SIS (CrewEnlisted) + 1;
		new_exp_desc.ship_info.ship_flags |= PLAYER_CAPTAIN;
	}
	
	new_exp_desc.ship_info.energy_level = new_exp_desc.ship_info.max_energy;

	RaceDescPtr = &new_exp_desc;

	return (RaceDescPtr);
}

void
uninit_exp (RACE_DESC *pRaceDesc)
{
	if (LOBYTE (GLOBAL (CurrentActivity)) != IN_HYPERSPACE)
	{
		GLOBAL_SIS (CrewEnlisted) = pRaceDesc->ship_info.crew_level;
		if (pRaceDesc->ship_info.ship_flags & PLAYER_CAPTAIN)
			GLOBAL_SIS (CrewEnlisted)--;
	}
}