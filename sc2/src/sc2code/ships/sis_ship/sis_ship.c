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

// JMS 2009: -Added Orz space state check to Sis_hyper_preprocess
// JMS 2010: -Red ship gfx in hyperspace, blue in Orz space, green in quasispace
//			 -Added separate weaponry function for Chmmr Explorer
//			 -Added some nuts and bolts to make Chmmr Explorer weapons work
//			 -Modded Chmmr Explorer main weapon as dual alternate side firing ion bolt cannon
//			 -Chmmr Explorer secondary: Shard mines
//			 -Autopilot now engages only after coming to full stop first
//			 -Reduced weapon range (Blaster life) from 12 to 9
//			 -Changed special weapon to confuse banger. Retained the shardmine code still, but it is disabled. Look for JMS_CONFUSER

#include "ships/ship.h"
#include "ships/sis_ship/resinst.h"

#include "colors.h"
#include "controls.h"
#include "globdata.h"
#include "libs/mathlib.h"

#include "libs/log.h"

#define MAX_TRACKING 3
#define MAX_DEFENSE 8

#define MAX_CREW MAX_CREW_SIZE
#define MAX_ENERGY MAX_ENERGY_SIZE
#define ENERGY_REGENERATION 1
#define WEAPON_ENERGY_COST 1
#define SPECIAL_ENERGY_COST 0
#define ENERGY_WAIT 10
#define MAX_THRUST 10
#define THRUST_INCREMENT 4
#define TURN_WAIT 17
#define THRUST_WAIT 6
#define WEAPON_WAIT 6
#define SPECIAL_WAIT 9

#define SHIP_MASS MAX_SHIP_MASS

#define BLASTER_SPEED DISPLAY_TO_WORLD (24)
#define BLASTER_LIFE 7 // JMS: WAS 12

// JMS: These affect CHmmr Explorer in Super Melee
#define EXPLORER_MAX_THRUST 34
#define EXPLORER_THRUST_INCREMENT 9
#define EXPLORER_TURN_WAIT 1
#define EXPLORER_THRUST_WAIT 1

// JMS: Chmmr Explorer has smaller weapon delay and largish special delay
#define EXPLORER_WEAPON_WAIT 2
#define EXPLORER_SPECIAL_WAIT 22
#define EXPLORER_WEAPON_ENERGY_COST 1

static RACE_DESC sis_desc =
{
	{ /* SHIP_INFO */
		0,
		16, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		NULL_RESOURCE,
		SIS_ICON_MASK_PMAP_ANIM,
		NULL_RESOURCE,
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
			SIS_BIG_MASK_PMAP_ANIM,
			SIS_MED_MASK_PMAP_ANIM,
			SIS_SML_MASK_PMAP_ANIM,
		},
		{
			BLASTER_BIG_MASK_PMAP_ANIM,
			BLASTER_MED_MASK_PMAP_ANIM,
			BLASTER_SML_MASK_PMAP_ANIM,
		},
		{
			NULL_RESOURCE,
			NULL_RESOURCE,
			NULL_RESOURCE,
		},
		{
			SIS_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		SIS_VICTORY_SONG,
		SIS_SHIP_SOUNDS,
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

// JMS: Explorer ship has its own description here.
static RACE_DESC exp_desc =
{
	{ /* SHIP_INFO */
		0,
		30, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		EXP_RACE_STRINGS,
		EXP_ICON_MASK_PMAP_ANIM,	// JMS: Explorer icons
		EXP_MICON_MASK_PMAP_ANIM,	// JMS: Explorer melee icons
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
		THRUST_INCREMENT,
		ENERGY_REGENERATION,
		EXPLORER_WEAPON_ENERGY_COST,
		SPECIAL_ENERGY_COST,
		ENERGY_WAIT,
		TURN_WAIT,
		THRUST_WAIT,
		EXPLORER_WEAPON_WAIT,
		0,  // JMS: Special weapon wait is abused here to keep track of number of mines.
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
			// JMS_CONFUSER: Replaced old special gfx with new ones
			/*SPARK_ONLY_BIG_MASK_PMAP_ANIM,
			SPARK_ONLY_MED_MASK_PMAP_ANIM,
			SPARK_ONLY_SML_MASK_PMAP_ANIM,*/
			CONFUSE_BANGER_BIG_MASK_PMAP_ANIM,
			CONFUSE_BANGER_MED_MASK_PMAP_ANIM,
			CONFUSE_BANGER_SML_MASK_PMAP_ANIM,
		},
		{
			SIS_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		SIS_VICTORY_SONG,
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


static void InitModuleSlots (RACE_DESC *RaceDescPtr,
		const BYTE *ModuleSlots);
static void InitDriveSlots (RACE_DESC *RaceDescPtr,
		const BYTE *DriveSlots);
static void InitJetSlots (RACE_DESC *RaceDescPtr,
		const BYTE *JetSlots);
void uninit_sis (RACE_DESC *pRaceDesc);

static BYTE num_trackers = 0;

static void
sis_hyper_preprocess (ELEMENT *ElementPtr)
{
	SIZE udx, udy, dx, dy;
	SIZE AccelerateDirection;
	STARSHIP *StarShipPtr;
	
	// JMS_GFX: These babies help to make the hyperspace speed calculations not overflow in 640x480.
	SDWORD dtempx, dtempy;

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
			GetCurrentVelocityComponents (&ElementPtr->velocity,
					&dx, &dy);
			
			// JMS: Engage autopilot only after coming to full stop
			if (dx==0 && dy==0)
				SET_GAME_STATE (AUTOPILOT_OK, 1);
			else
				SET_GAME_STATE (AUTOPILOT_OK, 0);
			
			// JMS_GFX: Since the sis_ship max speed with max thrusters in 640x480 mode is 2048, 
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
			if (NORMALIZE_FACING (StarShipPtr->ShipFacing
					+ ANGLE_TO_FACING (QUADRANT)
					- facing) > ANGLE_TO_FACING (HALF_CIRCLE))
				goto LeaveAutoPilot;

			facing = StarShipPtr->ShipFacing;
		}
		else if ((int)facing != (int)StarShipPtr->ShipFacing
				&& ElementPtr->turn_wait == 0)
		{
			if (NORMALIZE_FACING (
					StarShipPtr->ShipFacing - facing
					) >= ANGLE_TO_FACING (HALF_CIRCLE))
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
			{
				ZeroVelocityComponents (&ElementPtr->velocity);
			}
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

			max_velocity = WORLD_TO_VELOCITY (
					StarShipPtr->RaceDescPtr->characteristics.max_thrust);

			dy = (speed / velocity_increment + 1)
					* velocity_increment;
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

		ElementPtr->thrust_wait =
				StarShipPtr->RaceDescPtr->characteristics.thrust_wait;
	}
}

static void
sis_hyper_postprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GLOBAL (velocity) = ElementPtr->velocity;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (((StarShipPtr->cur_status_flags & WEAPON) ||
			PulsedInputState.menu[KEY_MENU_CANCEL])
			&& StarShipPtr->special_counter == 0)
	{
#define MENU_DELAY 10
		DoMenuOptions ();
		StarShipPtr->cur_status_flags &= ~SHIP_AT_MAX_SPEED;
		StarShipPtr->special_counter = MENU_DELAY;
	}
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// Precursor vessel special//////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static void
spawn_point_defense (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (ElementPtr->state_flags & PLAYER_SHIP)
	{
		HELEMENT hDefense;

		hDefense = AllocElement ();
		if (hDefense)
		{
			ELEMENT *DefensePtr;

			LockElement (hDefense, &DefensePtr);
			DefensePtr->state_flags = APPEARING | NONSOLID | FINITE_LIFE |
					(ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));
			{
				DefensePtr->death_func = spawn_point_defense;
			}
			GetElementStarShip (ElementPtr, &StarShipPtr);
			SetElementStarShip (DefensePtr, StarShipPtr);
			UnlockElement (hDefense);

			PutElement (hDefense);
		}
	}
	else
	{
		BOOLEAN PaidFor;
		HELEMENT hObject, hNextObject;
		ELEMENT *ShipPtr;
		COLOR LaserColor;
		static const COLOR ColorRange[] =
		{
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x03, 0x00), 0x7F),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x07, 0x00), 0x7E),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0A, 0x00), 0x7D),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0E, 0x00), 0x7C),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x11, 0x00), 0x7B),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x15, 0x00), 0x7A),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x18, 0x00), 0x79),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1C, 0x00), 0x78),
		};

		PaidFor = FALSE;

		LaserColor = ColorRange[
				StarShipPtr->RaceDescPtr->characteristics.special_energy_cost
				];
		LockElement (StarShipPtr->hShip, &ShipPtr);
		for (hObject = GetTailElement (); hObject; hObject = hNextObject)
		{
			ELEMENT *ObjectPtr;

			LockElement (hObject, &ObjectPtr);
			hNextObject = GetPredElement (ObjectPtr);
			if (ObjectPtr != ShipPtr && CollidingElement (ObjectPtr) &&
					!OBJECT_CLOAKED (ObjectPtr))
			{
#define LASER_RANGE (UWORD)100
				SIZE delta_x, delta_y;

				delta_x = ObjectPtr->next.location.x -
						ShipPtr->next.location.x;
				delta_y = ObjectPtr->next.location.y -
						ShipPtr->next.location.y;
				if (delta_x < 0)
					delta_x = -delta_x;
				if (delta_y < 0)
					delta_y = -delta_y;
				delta_x = WORLD_TO_DISPLAY (delta_x);
				delta_y = WORLD_TO_DISPLAY (delta_y);
				if ((UWORD)delta_x <= LASER_RANGE &&
						(UWORD)delta_y <= LASER_RANGE &&
						(UWORD)delta_x * (UWORD)delta_x +
						(UWORD)delta_y * (UWORD)delta_y <=
						LASER_RANGE * LASER_RANGE)
				{
					HELEMENT hPointDefense;
					LASER_BLOCK LaserBlock;

					if (!PaidFor)
					{
						if (!DeltaEnergy (ShipPtr,
								-(StarShipPtr->RaceDescPtr->characteristics.special_energy_cost
								<< 2)))
							break;

						ProcessSound (SetAbsSoundIndex (
										/* POINT_DEFENSE_LASER */
								StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);
						StarShipPtr->special_counter =
								StarShipPtr->RaceDescPtr->characteristics.special_wait;
						PaidFor = TRUE;
					}

					LaserBlock.cx = ShipPtr->next.location.x;
					LaserBlock.cy = ShipPtr->next.location.y;
					LaserBlock.face = 0;
					LaserBlock.ex = ObjectPtr->next.location.x
							- ShipPtr->next.location.x;
					LaserBlock.ey = ObjectPtr->next.location.y
							- ShipPtr->next.location.y;
					LaserBlock.sender = (ShipPtr->state_flags &
							(GOOD_GUY | BAD_GUY)) | IGNORE_SIMILAR;
					LaserBlock.pixoffs = 0;
					LaserBlock.color = LaserColor;
					hPointDefense = initialize_laser (&LaserBlock);
					if (hPointDefense)
					{
						ELEMENT *PDPtr;

						LockElement (hPointDefense, &PDPtr);
						PDPtr->mass_points =
								StarShipPtr->RaceDescPtr->characteristics.special_energy_cost;
						SetElementStarShip (PDPtr, StarShipPtr);
						PDPtr->hTarget = 0;
						UnlockElement (hPointDefense);

						PutElement (hPointDefense);
					}
				}
			}
			UnlockElement (hObject);
		}
		UnlockElement (StarShipPtr->hShip);
	}
}


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// Shardmine special ////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static void
shardmine_splinter_preprocess (ELEMENT *ElementPtr)
{
	ElementPtr->next.image.frame =
	IncFrameIndex (ElementPtr->current.image.frame);
	ElementPtr->state_flags |= CHANGING;
}
static void
shardmine_collision (ELEMENT *ElementPtr0, POINT *pPt0,
				   ELEMENT *ElementPtr1, POINT *pPt1)
{
	weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
	
	if (ElementPtr0->state_flags & DISAPPEARING)
	{
		ElementPtr0->state_flags &= ~DISAPPEARING;
		ElementPtr0->state_flags |= NONSOLID | CHANGING;
		ElementPtr0->life_span = 8;
		ElementPtr0->next.image.frame =
		SetAbsFrameIndex (ElementPtr0->current.image.frame, 0);
		
		ElementPtr0->preprocess_func = shardmine_splinter_preprocess;
	}
}
// JMS: Chmmr Explorer special: animation and limiting the number of mines in arena
#define MAX_MINES 8
static void
shardmine_spin_preprocess (ELEMENT *ElementPtr)
{
	ELEMENT *ShipPtr;
	STARSHIP *StarShipPtr;
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
	LockElement (StarShipPtr->hShip, &ShipPtr);
	
	if (ShipPtr->crew_level
		&& ++StarShipPtr->RaceDescPtr->characteristics.special_wait > MAX_MINES)
	{		
		ElementPtr->life_span = 1;
		ElementPtr->state_flags |= DISAPPEARING;
	}
	else
	{
		++ElementPtr->life_span;

// JMS: This number should be be the same as the number of PNGs in the ani file.
// For chrissakes, don't put too big a number here!
#define LAST_SPIN_INDEX 15
		if (GetFrameIndex (ElementPtr->current.image.frame) < LAST_SPIN_INDEX)
			ElementPtr->next.image.frame = IncFrameIndex (ElementPtr->current.image.frame);
		else
			ElementPtr->next.image.frame = SetAbsFrameIndex (ElementPtr->current.image.frame, 0);
		
		ElementPtr->state_flags |= CHANGING;
		ElementPtr->turn_wait = 0;
	}
	UnlockElement (StarShipPtr->hShip);
}
#define TRACK_WAIT 4
static void
shardminetrack_preprocess (ELEMENT *ElementPtr)
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
//#define ACTIVATE_RANGE (224 * RESOLUTION_FACTOR) /* Originally SPACE_WIDTH */ // JMS_GFX
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
			if(0)
			{
				ZeroVelocityComponents (&ElementPtr->velocity);
			}
			else
			{
				ElementPtr->thrust_wait = TRACK_WAIT;
				SetVelocityVector (&ElementPtr->velocity,
								   DISPLAY_TO_WORLD (2 * RESOLUTION_FACTOR), facing); // JMS_GFX
			}
		}
	}
	
	shardmine_spin_preprocess (ElementPtr);
}
static void
shardmine_decelerate_preprocess (ELEMENT *ElementPtr)
{
	SIZE dx, dy;
	
	GetCurrentVelocityComponents (&ElementPtr->velocity, &dx, &dy);
	dx /= 2;
	dy /= 2;
	SetVelocityComponents (&ElementPtr->velocity, dx, dy);
	if (dx == 0 && dy == 0)
	{
		ElementPtr->preprocess_func = shardminetrack_preprocess;
	}
	
	shardmine_spin_preprocess (ElementPtr);
}
static void
shardmine_missile_preprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (!(StarShipPtr->cur_status_flags & WEAPON))
	{
		ElementPtr->life_span >>= 1;
		ElementPtr->preprocess_func = shardmine_decelerate_preprocess;
	}
	
	shardmine_spin_preprocess (ElementPtr);
}
// JMS: Chmmr Explorer special: This puts the newest mine on the arena to
// the last place in the list of mine elements. 
//
// -> When new mines are deployed, oldest ones are eradicated from their way.
static void
shardmine_missile_postprocess (ELEMENT *ElementPtr)
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
		ListElementPtr->state_flags = (ListElementPtr->state_flags & ~(PRE_PROCESS | CHANGING | APPEARING)) | POST_PROCESS;
		UnlockElement (hElement);
		
		GetElementStarShip (ElementPtr, &StarShipPtr);
		LockElement (StarShipPtr->hShip, &ListElementPtr);
		InsertElement (hElement, GetSuccElement (ListElementPtr));
		UnlockElement (StarShipPtr->hShip);
		
		ElementPtr->life_span = 0;
	}
}
// JMS: Chmmr Explorer special: Main function
static void
spawn_shardmine_missile (ELEMENT *ShipPtr)
{
#define SPATHI_REAR_OFFSET 20
#define DISCRIMINATOR_SPEED 18
#define DISCRIMINATOR_LIFE 64
#define DISCRIMINATOR_HITS 10
#define DISCRIMINATOR_DAMAGE 4
#define DISCRIMINATOR_OFFSET 0
	HELEMENT ButtMissile;
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK ButtMissileBlock;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	ButtMissileBlock.cx = ShipPtr->next.location.x;
	ButtMissileBlock.cy = ShipPtr->next.location.y;
	ButtMissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
	ButtMissileBlock.face = ButtMissileBlock.index =
	NORMALIZE_FACING (StarShipPtr->ShipFacing
					  + ANGLE_TO_FACING (HALF_CIRCLE));
	ButtMissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY)) | IGNORE_SIMILAR;
	ButtMissileBlock.pixoffs = SPATHI_REAR_OFFSET;
	ButtMissileBlock.speed = DISCRIMINATOR_SPEED;
	ButtMissileBlock.hit_points = DISCRIMINATOR_HITS;
	ButtMissileBlock.damage = DISCRIMINATOR_DAMAGE;
	ButtMissileBlock.life = DISCRIMINATOR_LIFE;
	ButtMissileBlock.preprocess_func = shardmine_missile_preprocess;
	ButtMissileBlock.blast_offs = DISCRIMINATOR_OFFSET;
	ButtMissile = initialize_missile (&ButtMissileBlock);
	if (ButtMissile)
	{
		ELEMENT *ButtPtr;
		
		LockElement (ButtMissile, &ButtPtr);
		ButtPtr->turn_wait = 0;
		ButtPtr->thrust_wait = 0;
		SetElementStarShip (ButtPtr, StarShipPtr);
		ButtPtr->postprocess_func = shardmine_missile_postprocess;
		ButtPtr->collision_func = shardmine_collision;
		UnlockElement (ButtMissile);
		PutElement (ButtMissile);

	}
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// Confuser special /////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static void
confuse_banger_preprocess (ELEMENT *ElementPtr)
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
				ElementPtr->next.image.frame =
				SetRelFrameIndex (ElementPtr->next.image.frame, -8);
			
			GetElementStarShip (eptr, &StarShipPtr);
			StarShipPtr->ship_input_state = (StarShipPtr->ship_input_state & ~(LEFT | RIGHT | THRUST)) | ElementPtr->turn_wait;
			
			hEffect = AllocElement ();
			if (hEffect)
			{
				LockElement (hEffect, &eptr);
				
				eptr->state_flags = FINITE_LIFE | NONSOLID | CHANGING | (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));
				eptr->life_span = 1;
				eptr->current = eptr->next = ElementPtr->next;
				eptr->preprocess_func = confuse_banger_preprocess;
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

#define RECOIL_VELOCITY WORLD_TO_VELOCITY (DISPLAY_TO_WORLD (2))
#define MAX_RECOIL_VELOCITY (RECOIL_VELOCITY * 3)
#define CONFUSE_DURATION 20

static void
confuse_banger_collision (ELEMENT *ElementPtr0, POINT *pPt0, ELEMENT *ElementPtr1, POINT *pPt1)
{	
	if ((ElementPtr1->state_flags & PLAYER_SHIP)
		&& ElementPtr1->crew_level
		&& !GRAVITY_MASS (ElementPtr1->mass_points + 1))
	{
		COUNT angle;
		SIZE cur_delta_x, cur_delta_y;
		STARSHIP *StarShipPtr;
		
		GetElementStarShip (ElementPtr1, &StarShipPtr);
		StarShipPtr->cur_status_flags &= ~(SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED);
		
		angle = FACING_TO_ANGLE (GetFrameIndex (ElementPtr0->next.image.frame) );
		DeltaVelocityComponents (&ElementPtr1->velocity,
								 COSINE (angle, RECOIL_VELOCITY),
								 SINE (angle, RECOIL_VELOCITY));
		GetCurrentVelocityComponents (&ElementPtr1->velocity,&cur_delta_x, &cur_delta_y);
	
		if ((long)cur_delta_x * (long)cur_delta_x
			+ (long)cur_delta_y * (long)cur_delta_y
			> (long)MAX_RECOIL_VELOCITY * (long)MAX_RECOIL_VELOCITY)
		{
			angle = ARCTAN (cur_delta_x, cur_delta_y);
			SetVelocityComponents (&ElementPtr1->velocity,
								   COSINE (angle, MAX_RECOIL_VELOCITY),
								   SINE (angle, MAX_RECOIL_VELOCITY));
		}
	}
	
	if (ElementPtr1->state_flags & PLAYER_SHIP)
	{
		HELEMENT hConfusionElement, hNextElement;
		ELEMENT *ConfusionPtr;
		STARSHIP *StarShipPtr;
		
		GetElementStarShip (ElementPtr0, &StarShipPtr);
		for (hConfusionElement = GetHeadElement ();
			 hConfusionElement; hConfusionElement = hNextElement)
		{
			LockElement (hConfusionElement, &ConfusionPtr);
			if ((ConfusionPtr->state_flags & (GOOD_GUY | BAD_GUY)) ==
				(ElementPtr0->state_flags & (GOOD_GUY | BAD_GUY))
				&& ConfusionPtr->current.image.farray ==
				StarShipPtr->RaceDescPtr->ship_data.special
				&& (ConfusionPtr->state_flags & NONSOLID))
			{
				UnlockElement (hConfusionElement);
				break;
			}
			hNextElement = GetSuccElement (ConfusionPtr);
			UnlockElement (hConfusionElement);
		}
		
		if (hConfusionElement || (hConfusionElement = AllocElement ()))
		{
			LockElement (hConfusionElement, &ConfusionPtr);
			
			if (ConfusionPtr->state_flags == 0) /* not allocated before */
			{
				InsertElement (hConfusionElement, GetHeadElement ());
				
				ConfusionPtr->current = ElementPtr0->next;
				ConfusionPtr->current.image.frame = SetAbsFrameIndex (ConfusionPtr->current.image.frame, 16);
				ConfusionPtr->next = ConfusionPtr->current;
				ConfusionPtr->state_flags = FINITE_LIFE | NONSOLID | CHANGING
				| (ElementPtr0->state_flags & (GOOD_GUY | BAD_GUY));
				ConfusionPtr->preprocess_func = confuse_banger_preprocess;
				SetPrimType ( &(GLOBAL (DisplayArray))[ConfusionPtr->PrimIndex], NO_PRIM);
				
				SetElementStarShip (ConfusionPtr, StarShipPtr);
				GetElementStarShip (ElementPtr1, &StarShipPtr);
				ConfusionPtr->hTarget = StarShipPtr->hShip;
			}
			
			ConfusionPtr->life_span = CONFUSE_DURATION;
			ConfusionPtr->turn_wait =
			(BYTE)(1 << ((BYTE)TFB_Random () & 1)); /* LEFT or RIGHT */
			
			UnlockElement (hConfusionElement);
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
initialize_confuse_banger (ELEMENT *ShipPtr, HELEMENT ConfusionArray[])
{
#define CMISSILE_SPEED DISPLAY_TO_WORLD (24)
#define CMISSILE_HITS 4
#define CMISSILE_DAMAGE 3
#define CMISSILE_LIFE 10
#define CMISSILE_OFFSET 4
#define CMISSILE_START_OFFSET 28
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK ConfusionBlock;
	
	GetElementStarShip (ShipPtr, &StarShipPtr);
	ConfusionBlock.cx = ShipPtr->next.location.x;
	ConfusionBlock.cy = ShipPtr->next.location.y;
	ConfusionBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
	ConfusionBlock.face = StarShipPtr->ShipFacing;
	ConfusionBlock.index = ConfusionBlock.face;
	ConfusionBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
	| IGNORE_SIMILAR;
	ConfusionBlock.pixoffs = CMISSILE_START_OFFSET;
	ConfusionBlock.speed = CMISSILE_SPEED;
	ConfusionBlock.hit_points = CMISSILE_HITS;
	ConfusionBlock.damage = CMISSILE_DAMAGE;
	ConfusionBlock.life = CMISSILE_LIFE;
	ConfusionBlock.preprocess_func = confuse_banger_preprocess;
	ConfusionBlock.blast_offs = CMISSILE_OFFSET;
	ConfusionArray[0] = initialize_missile (&ConfusionBlock);
	
	if (ConfusionArray[0])
	{
		ELEMENT *CMissilePtr;
		
		LockElement (ConfusionArray[0], &CMissilePtr);
		CMissilePtr->collision_func = confuse_banger_collision;
		SetElementStarShip (CMissilePtr, StarShipPtr);
		UnlockElement (ConfusionArray[0]);
	}
	return (1);
}


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// SPECIALS END /////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static void
sis_battle_preprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	
	// JMS_CONFUSER: Commented these out for the confuse_banger special weapon to work correctly
	//
	// JMS: This is needed to keep track of the number of shard mines with Chmmr Explorer
	//if (GET_GAME_STATE (WHICH_SHIP_PLAYER_HAS) == 0)
	//	StarShipPtr->RaceDescPtr->characteristics.special_wait = 0;
	
	if (StarShipPtr->RaceDescPtr->characteristics.special_energy_cost == 0
		&&(GET_GAME_STATE (WHICH_SHIP_PLAYER_HAS) == 1))
	{
		StarShipPtr->cur_status_flags &= ~SPECIAL;
		StarShipPtr->special_counter = 2;
	}
	if (!(StarShipPtr->RaceDescPtr->ship_info.ship_flags
			& (FIRES_FORE | FIRES_RIGHT | FIRES_AFT | FIRES_LEFT)))
	{
		StarShipPtr->cur_status_flags &= ~WEAPON;
		StarShipPtr->weapon_counter = 2;
	}
}

static void
sis_battle_postprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;
	GetElementStarShip (ElementPtr, &StarShipPtr);
	
	if ((GET_GAME_STATE (WHICH_SHIP_PLAYER_HAS) == 0)
		&& StarShipPtr->special_counter == 0
		&& (StarShipPtr->cur_status_flags & SPECIAL)
		&& (DeltaEnergy (ElementPtr, -(StarShipPtr->RaceDescPtr->characteristics.special_energy_cost)))
		)
		
	{
		// JMS_CONFUSER: Disabled the shard mine related lines...
		/*spawn_shardmine_missile(ElementPtr);
		ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), ElementPtr);
		StarShipPtr->special_counter = EXPLORER_SPECIAL_WAIT;*/ // JMS: Disabled shardmines and introduced confuser banger
		
		// JMS_CONFUSER:... and added these instead!
		HELEMENT ConfuseBanger;
		initialize_confuse_banger (ElementPtr, &ConfuseBanger);
		if (ConfuseBanger)
		{
			ELEMENT *CMissilePtr;
			LockElement (ConfuseBanger, &CMissilePtr);
			
			ProcessSound (SetAbsSoundIndex (StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1), CMissilePtr);
			
			UnlockElement (ConfuseBanger);
			PutElement (ConfuseBanger);
			StarShipPtr->special_counter = EXPLORER_SPECIAL_WAIT;
		}
		
	}
	
	else if ((StarShipPtr->cur_status_flags & SPECIAL)
			&& (GET_GAME_STATE (WHICH_SHIP_PLAYER_HAS) == 1)
			&& StarShipPtr->special_counter == 0
			&& StarShipPtr->RaceDescPtr->characteristics.special_energy_cost)
	{
			spawn_point_defense (ElementPtr);
	}
}

#define BLASTER_DAMAGE 2

static void
blaster_collision (ELEMENT *ElementPtr0, POINT *pPt0,
		ELEMENT *ElementPtr1, POINT *pPt1)
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
				BlastElementPtr->current.image.frame =
						SetAbsFrameIndex (ElementPtr0->current.image.frame, 0);
				BlastElementPtr->preprocess_func = NULL;
				break;
			case BLASTER_DAMAGE * 2:
				BlastElementPtr->life_span = 6;
				BlastElementPtr->current.image.frame =
						IncFrameIndex (ElementPtr0->current.image.frame);
				break;
			case BLASTER_DAMAGE * 3:
				BlastElementPtr->life_span = 7;
				BlastElementPtr->current.image.frame =
						SetAbsFrameIndex (ElementPtr0->current.image.frame, 20);
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
				ElementPtr->next.image.frame =
						IncFrameIndex (ElementPtr->current.image.frame);
				ElementPtr->state_flags |= CHANGING;
			}
			break;
		case BLASTER_DAMAGE * 3:
			if (GetFrameIndex (ElementPtr->current.image.frame) < 19)
				ElementPtr->next.image.frame =
						IncFrameIndex (ElementPtr->current.image.frame);
			else
				ElementPtr->next.image.frame =
						SetAbsFrameIndex (ElementPtr->current.image.frame, 16);
			ElementPtr->state_flags |= CHANGING;
			break;
	}

	if (LONIBBLE (ElementPtr->turn_wait))
		--ElementPtr->turn_wait;
	else if ((wait = HINIBBLE (ElementPtr->turn_wait)))
	{
		COUNT facing;

		facing = NORMALIZE_FACING (ANGLE_TO_FACING (
				GetVelocityTravelAngle (&ElementPtr->velocity)));
		if (TrackShip (ElementPtr, &facing) > 0)
			SetVelocityVector (&ElementPtr->velocity, BLASTER_SPEED, facing);

		ElementPtr->turn_wait = MAKE_BYTE (wait, wait);
	}
}

static COUNT
initialize_blasters (ELEMENT *ShipPtr, HELEMENT BlasterArray[])
{
#define SIS_VERT_OFFSET 28
#define SIS_HORZ_OFFSET 20
#define BLASTER_HITS 2
#define BLASTER_OFFSET 8
	COUNT num_blasters;

	BYTE nt;
	COUNT i;
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock[6];
	MISSILE_BLOCK *lpMB;
	
	GetElementStarShip (ShipPtr, &StarShipPtr);

	num_blasters = 0;
	for (i = 0, lpMB = &MissileBlock[0]; i < NUM_MODULE_SLOTS; ++i)
	{
		BYTE which_gun;

		if (i == 3)
			i = NUM_MODULE_SLOTS - 1;
		which_gun = GLOBAL_SIS (ModuleSlots[(NUM_MODULE_SLOTS - 1) - i]);
		if (which_gun >= GUN_WEAPON && which_gun <= CANNON_WEAPON)
		{
			which_gun -= GUN_WEAPON - 1;
			lpMB->cx = ShipPtr->next.location.x;
			lpMB->cy = ShipPtr->next.location.y;
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
					lpMB->pixoffs = SIS_VERT_OFFSET;
					lpMB->face = StarShipPtr->ShipFacing;
					break;
				case 1: /* SPREAD WEAPON */
					lpMB->pixoffs = SIS_VERT_OFFSET;
					lpMB->face = NORMALIZE_FACING (
							StarShipPtr->ShipFacing + 1);
					lpMB[1] = lpMB[0];
					++lpMB;
					lpMB->face = NORMALIZE_FACING (
							StarShipPtr->ShipFacing - 1);
					break;
				case 2: /* SIDE WEAPON */
					lpMB->pixoffs = SIS_HORZ_OFFSET;
					lpMB->face = NORMALIZE_FACING (
							StarShipPtr->ShipFacing
							+ ANGLE_TO_FACING (QUADRANT));
					lpMB[1] = lpMB[0];
					++lpMB;
					lpMB->face = NORMALIZE_FACING (
							StarShipPtr->ShipFacing
							- ANGLE_TO_FACING (QUADRANT));
					break;
					// BW: No tail weapon in Explorer
					//				case NUM_MODULE_SLOTS - 1: /* TAIL WEAPON */
					//					lpMB->pixoffs = SIS_VERT_OFFSET;
					//					lpMB->face = NORMALIZE_FACING (
					//							StarShipPtr->ShipFacing
					//							+ ANGLE_TO_FACING (HALF_CIRCLE));
					//					break;
			}

			++lpMB;
		}
	}


	nt = (BYTE)((4 - num_trackers) & 3);
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

// JMS: Chmmr Explorer weapons initing function be here.
static COUNT
initialize_explorer_weaponry (ELEMENT *ShipPtr, HELEMENT BlasterArray[])
{
#define SIS_VERT_OFFSET 28
#define SIS_HORZ_OFFSET 20
#define SIS_HORZ_OFFSET_2 (DISPLAY_TO_WORLD(5 * RESOLUTION_FACTOR))
#define SIS_HORZ_OFFSET_3 (DISPLAY_TO_WORLD(-5 * RESOLUTION_FACTOR))
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
		offs_x = -SINE (angle, SIS_HORZ_OFFSET_2);
		offs_y = COSINE (angle, SIS_HORZ_OFFSET_2);
	}
	else
	{
		offs_x = -SINE (angle, SIS_HORZ_OFFSET_3);
		offs_y = COSINE (angle, SIS_HORZ_OFFSET_3);
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
					lpMB->pixoffs = SIS_VERT_OFFSET;
					lpMB->face = StarShipPtr->ShipFacing;
					break;
				case 1: /* SPREAD WEAPON */
					lpMB->pixoffs = SIS_VERT_OFFSET;
					lpMB->face = NORMALIZE_FACING (
						StarShipPtr->ShipFacing + 1);
					lpMB[1] = lpMB[0];
					++lpMB;
					lpMB->face = NORMALIZE_FACING (
						StarShipPtr->ShipFacing - 1);
					break;
				case 2: /* SIDE WEAPON */
					lpMB->pixoffs = SIS_HORZ_OFFSET;
					lpMB->face = NORMALIZE_FACING (
						StarShipPtr->ShipFacing
						+ ANGLE_TO_FACING (QUADRANT));
					lpMB[1] = lpMB[0];
					++lpMB;
					lpMB->face = NORMALIZE_FACING (
						StarShipPtr->ShipFacing
						- ANGLE_TO_FACING (QUADRANT));
					break;
					// BW: No tail weapon in Explorer
					//				case NUM_MODULE_SLOTS - 1: /* TAIL WEAPON */
					//					lpMB->pixoffs = SIS_VERT_OFFSET;
					//					lpMB->face = NORMALIZE_FACING (
					//						StarShipPtr->ShipFacing
					//						+ ANGLE_TO_FACING (HALF_CIRCLE));
					//					break;
			}
			
			++lpMB;
		}
	}
	
	
	nt = (BYTE)((4 - num_trackers) & 3);
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


static void
sis_intelligence (ELEMENT *ShipPtr, EVALUATE_DESC *ObjectsOfConcern,
		COUNT ConcernCounter)
{
	EVALUATE_DESC *lpEvalDesc;
	STARSHIP *StarShipPtr;

	GetElementStarShip (ShipPtr, &StarShipPtr);

	lpEvalDesc = &ObjectsOfConcern[ENEMY_WEAPON_INDEX];
	if (lpEvalDesc->ObjectPtr)
	{
		if (StarShipPtr->RaceDescPtr->characteristics.special_energy_cost)
		{
			if (StarShipPtr->special_counter == 0
					&& ((lpEvalDesc->ObjectPtr
					&& lpEvalDesc->which_turn <= 2)
					|| (ObjectsOfConcern[ENEMY_SHIP_INDEX].ObjectPtr != NULL
					&& ObjectsOfConcern[ENEMY_SHIP_INDEX].which_turn <= 4)))
				StarShipPtr->ship_input_state |= SPECIAL;
			else
				StarShipPtr->ship_input_state &= ~SPECIAL;
			lpEvalDesc->ObjectPtr = NULL;
		}
		else if (MANEUVERABILITY (&StarShipPtr->RaceDescPtr->cyborg_control)
				< MEDIUM_SHIP
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

	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	if (num_trackers
			&& StarShipPtr->weapon_counter == 0
			&& !(StarShipPtr->ship_input_state & WEAPON)
			&& lpEvalDesc->ObjectPtr
			&& lpEvalDesc->which_turn <= 16)
	{
		COUNT direction_facing;
		SIZE delta_x, delta_y;
		UWORD fire_flags, ship_flags;
		COUNT facing;

		delta_x = lpEvalDesc->ObjectPtr->current.location.x
				- ShipPtr->current.location.x;
		delta_y = lpEvalDesc->ObjectPtr->current.location.y
				- ShipPtr->current.location.y;
		direction_facing = NORMALIZE_FACING (
				ANGLE_TO_FACING (ARCTAN (delta_x, delta_y)));

		ship_flags = StarShipPtr->RaceDescPtr->ship_info.ship_flags;
		for (fire_flags = FIRES_FORE, facing = StarShipPtr->ShipFacing;
				fire_flags <= FIRES_LEFT;
				fire_flags <<= 1, facing += QUADRANT)
		{
			if ((ship_flags & fire_flags) && NORMALIZE_FACING (
					direction_facing - facing + ANGLE_TO_FACING (OCTANT)
					) <= ANGLE_TO_FACING (QUADRANT))
			{
				StarShipPtr->ship_input_state |= WEAPON;
				break;
			}
		}
	}
}

static void
InitModuleSlots (RACE_DESC *RaceDescPtr, const BYTE *ModuleSlots)
{
	COUNT i;
	
	// JMS: Chmmr Explorer has 50 men max crew complement without modules, precursor vessel 0 as usual
	//		Also: Chmmr Explorer has one front firing gun equipped as default.
	if ((GET_GAME_STATE (WHICH_SHIP_PLAYER_HAS) == 0))
	{
		RaceDescPtr->ship_info.max_crew = EXPLORER_CREW_CAPACITY;
		RaceDescPtr->ship_info.ship_flags |= FIRES_FORE;
		RaceDescPtr->characteristics.weapon_energy_cost += 2;
		RaceDescPtr->characteristics.special_energy_cost = 4;
	}
	else if ((GET_GAME_STATE (WHICH_SHIP_PLAYER_HAS) == 1))
	{
		RaceDescPtr->ship_info.max_crew = 0;
	}

	num_trackers = 0;
	for (i = 0; i < NUM_MODULE_SLOTS; ++i)
	{
		BYTE which_mod;

		which_mod = ModuleSlots[(NUM_MODULE_SLOTS - 1) - i];
		switch (which_mod)
		{
			case CREW_POD:
				RaceDescPtr->ship_info.max_crew += CREW_POD_CAPACITY;
				break;
			case GUN_WEAPON:
			case BLASTER_WEAPON:
			case CANNON_WEAPON:
				RaceDescPtr->characteristics.weapon_energy_cost +=
						(which_mod - GUN_WEAPON + 1) * 2;
				if (i <= 1)
					RaceDescPtr->ship_info.ship_flags |= FIRES_FORE;
				else if (i == 2)
					RaceDescPtr->ship_info.ship_flags |= FIRES_LEFT | FIRES_RIGHT;
				else
					RaceDescPtr->ship_info.ship_flags |= FIRES_AFT;
				break;
			case TRACKING_SYSTEM:
				++num_trackers;
				break;
			case ANTIMISSILE_DEFENSE:
				++RaceDescPtr->characteristics.special_energy_cost;
				break;
			case SHIVA_FURNACE:
				++RaceDescPtr->characteristics.energy_regeneration;
				break;
			case DYNAMO_UNIT:
				RaceDescPtr->characteristics.energy_wait -= 2;
				if (RaceDescPtr->characteristics.energy_wait < 4)
					RaceDescPtr->characteristics.energy_wait = 4;
				break;
		}
	}

	if (num_trackers > MAX_TRACKING)
		num_trackers = MAX_TRACKING;
	RaceDescPtr->characteristics.weapon_energy_cost += num_trackers * 3;
	if (RaceDescPtr->characteristics.special_energy_cost)
	{
		RaceDescPtr->ship_info.ship_flags |= POINT_DEFENSE;
		if (RaceDescPtr->characteristics.special_energy_cost > MAX_DEFENSE)
			RaceDescPtr->characteristics.special_energy_cost = MAX_DEFENSE;
	}
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
init_sis (void)
{
	RACE_DESC *RaceDescPtr;

	COUNT i;
	static RACE_DESC new_sis_desc;

	/* copy initial ship settings to new_sis_desc */
	// JMS: Ship selection. If player has Explorer, copy Explorer description to new_sis_desc.
	if ((GET_GAME_STATE (WHICH_SHIP_PLAYER_HAS) == 0))
		new_sis_desc = exp_desc;
	else if ((GET_GAME_STATE (WHICH_SHIP_PLAYER_HAS) == 1))
		new_sis_desc = sis_desc;
	
	new_sis_desc.uninit_func = uninit_sis;

	if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
	{
		for (i = 0; i < NUM_VIEWS; ++i)
		{
			new_sis_desc.ship_data.ship_rsc[i] = NULL_RESOURCE;
			new_sis_desc.ship_data.weapon_rsc[i] = NULL_RESOURCE;
			new_sis_desc.ship_data.special_rsc[i] = NULL_RESOURCE;
		}
		new_sis_desc.ship_info.icons_rsc = NULL_RESOURCE;
		new_sis_desc.ship_data.captain_control.captain_rsc = NULL_RESOURCE;
		new_sis_desc.ship_data.victory_ditty_rsc = NULL_RESOURCE;
		new_sis_desc.ship_data.ship_sounds_rsc = NULL_RESOURCE;

		// JMS: Hack: Blue ship in Orz space, green in quasi, red in hyperspace...
		// Need to use separate set of frames for different colors since
		// frame PNGs are in RGB mode instead of indexed color.
		if (GET_GAME_STATE (ORZ_SPACE_SIDE) > 1)
			new_sis_desc.ship_data.ship_rsc[0] = SIS_ORZ_MASK_PMAP_ANIM;
		else if (GET_GAME_STATE (ARILOU_SPACE_SIDE) > 1)
			new_sis_desc.ship_data.ship_rsc[0] = SIS_QUASI_MASK_PMAP_ANIM;
		else
			new_sis_desc.ship_data.ship_rsc[0] = SIS_HYPER_MASK_PMAP_ANIM;

		new_sis_desc.preprocess_func = sis_hyper_preprocess;
		new_sis_desc.postprocess_func = sis_hyper_postprocess;

		 // JMS_GFX
		if (RESOLUTION_FACTOR > 1)
		{
			new_sis_desc.characteristics.max_thrust = (10 * RESOLUTION_FACTOR) - (4 * RESOLUTION_FACTOR); // JMS_GFX
			new_sis_desc.characteristics.thrust_increment *= RESOLUTION_FACTOR;
		}
		else
			new_sis_desc.characteristics.max_thrust -= 4;
	}
	else
	{
		new_sis_desc.preprocess_func = sis_battle_preprocess;
		new_sis_desc.postprocess_func = sis_battle_postprocess;
		
		// JMS: Chmmr Explorer uses different function for its weapons than Precursor vessel.
		if ((GET_GAME_STATE (WHICH_SHIP_PLAYER_HAS) == 0))
			new_sis_desc.init_weapon_func = initialize_explorer_weaponry;
		else if ((GET_GAME_STATE (WHICH_SHIP_PLAYER_HAS) == 1))
			new_sis_desc.init_weapon_func = initialize_blasters;
		
		new_sis_desc.cyborg_control.intelligence_func = sis_intelligence;

		if (GET_GAME_STATE (CHMMR_BOMB_STATE) == 3)
			SET_GAME_STATE (BOMB_CARRIER, 1);
	}

	InitModuleSlots(&new_sis_desc, GLOBAL_SIS (ModuleSlots));
	InitDriveSlots(&new_sis_desc, GLOBAL_SIS (DriveSlots));
	InitJetSlots(&new_sis_desc, GLOBAL_SIS (JetSlots));
	
	// JMS: Give the explorer some stats so it won't be slow as fuck in supermelee
	// and does not depend on the thruster/jet numbers in adventure mode
	if((GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS))==0)
	{
		new_sis_desc.characteristics.max_thrust = EXPLORER_MAX_THRUST * RESOLUTION_FACTOR;
		new_sis_desc.characteristics.thrust_wait = EXPLORER_THRUST_WAIT;
		new_sis_desc.characteristics.thrust_increment = EXPLORER_THRUST_INCREMENT * RESOLUTION_FACTOR;
		new_sis_desc.characteristics.turn_wait = EXPLORER_TURN_WAIT;
	}
	
	if (LOBYTE (GLOBAL (CurrentActivity)) == SUPER_MELEE)
	{
		new_sis_desc.ship_info.crew_level = new_sis_desc.ship_info.max_crew;
	}
	else
	{
		// Count the captain too.
		new_sis_desc.ship_info.max_crew++;
		new_sis_desc.ship_info.crew_level = GLOBAL_SIS (CrewEnlisted) + 1;
		new_sis_desc.ship_info.ship_flags |= PLAYER_CAPTAIN;
	}
	
	new_sis_desc.ship_info.energy_level = new_sis_desc.ship_info.max_energy;

	RaceDescPtr = &new_sis_desc;

	return (RaceDescPtr);
}

void
uninit_sis (RACE_DESC *pRaceDesc)
{
	if (LOBYTE (GLOBAL (CurrentActivity)) != IN_HYPERSPACE)
	{
		GLOBAL_SIS (CrewEnlisted) = pRaceDesc->ship_info.crew_level;
		if (pRaceDesc->ship_info.ship_flags & PLAYER_CAPTAIN)
			GLOBAL_SIS (CrewEnlisted)--;
	}
}