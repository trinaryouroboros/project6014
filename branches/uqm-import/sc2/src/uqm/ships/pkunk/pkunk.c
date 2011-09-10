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

#include "uqm/globdata.h"
#include "libs/mathlib.h"


#define MAX_CREW 8
#define MAX_ENERGY 12
#define ENERGY_REGENERATION 0
#define WEAPON_ENERGY_COST 1
#define SPECIAL_ENERGY_COST 2
#define ENERGY_WAIT 0
#define MAX_THRUST 64
#define THRUST_INCREMENT 16
#define TURN_WAIT 0
#define THRUST_WAIT 0
#define WEAPON_WAIT 0
#define SPECIAL_WAIT 16

#define SHIP_MASS 1
#define MISSILE_SPEED DISPLAY_TO_WORLD (24)
#define MISSILE_LIFE 5

static RACE_DESC pkunk_desc =
{
	{ /* SHIP_INFO */
		FIRES_FORE | FIRES_LEFT | FIRES_RIGHT,
		20, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		PKUNK_RACE_STRINGS,
		PKUNK_ICON_MASK_PMAP_ANIM,
		PKUNK_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL
	},
	{ /* FLEET_STUFF */
		666 / SPHERE_RADIUS_INCREMENT * 2, /* Initial SoI radius */
		{ /* Known location (center of SoI) */
			502, 401,
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
		0, /* SPECIAL_WAIT */
		SHIP_MASS,
	},
	{
		{
			PKUNK_BIG_MASK_PMAP_ANIM,
			PKUNK_MED_MASK_PMAP_ANIM,
			PKUNK_SML_MASK_PMAP_ANIM,
		},
		{
			BUG_BIG_MASK_PMAP_ANIM,
			BUG_MED_MASK_PMAP_ANIM,
			BUG_SML_MASK_PMAP_ANIM,
		},
		{
			NULL_RESOURCE,
			NULL_RESOURCE,
			NULL_RESOURCE,
		},
		{
			PKUNK_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
		PKUNK_VICTORY_SONG,
		PKUNK_SHIP_SOUNDS,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		CLOSE_RANGE_WEAPON + 1,
		NULL,
	},
	(UNINIT_FUNC *) NULL,
	(PREPROCESS_FUNC *) NULL,
	(POSTPROCESS_FUNC *) NULL,
	(INIT_WEAPON_FUNC *) NULL,
	0,
};

static void
animate (ELEMENT *ElementPtr)
{
	if (ElementPtr->turn_wait > 0)
		--ElementPtr->turn_wait;
	else
	{
		ElementPtr->next.image.frame =
				IncFrameIndex (ElementPtr->current.image.frame);
		ElementPtr->state_flags |= CHANGING;

		ElementPtr->turn_wait = ElementPtr->next_turn;
	}
}

static COUNT
initialize_bug_missile (ELEMENT *ShipPtr, HELEMENT MissileArray[])
{
#define PKUNK_OFFSET 15
#define MISSILE_HITS 1
#define MISSILE_DAMAGE 1
#define MISSILE_OFFSET 1
	COUNT i;
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.index = 0;
	MissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
			| IGNORE_SIMILAR;
	MissileBlock.pixoffs = PKUNK_OFFSET;
	MissileBlock.speed = MISSILE_SPEED;
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = NULL;
	MissileBlock.blast_offs = MISSILE_OFFSET;

	for (i = 0; i < 3; ++i)
	{
		MissileBlock.face =
				StarShipPtr->ShipFacing
				+ (ANGLE_TO_FACING (QUADRANT) * i);
		if (i == 2)
			MissileBlock.face += ANGLE_TO_FACING (QUADRANT);
		MissileBlock.face = NORMALIZE_FACING (MissileBlock.face);

		if ((MissileArray[i] = initialize_missile (&MissileBlock)))
		{
			SIZE dx, dy;
			ELEMENT *MissilePtr;

			LockElement (MissileArray[i], &MissilePtr);
			GetCurrentVelocityComponents (&ShipPtr->velocity, &dx, &dy);
			DeltaVelocityComponents (&MissilePtr->velocity, dx, dy);
			MissilePtr->current.location.x -= VELOCITY_TO_WORLD (dx);
			MissilePtr->current.location.y -= VELOCITY_TO_WORLD (dy);

			MissilePtr->preprocess_func = animate;
			UnlockElement (MissileArray[i]);
		}
	}

	return (3);
}

static HELEMENT hPhoenix = 0;

static void
pkunk_intelligence (ELEMENT *ShipPtr, EVALUATE_DESC *ObjectsOfConcern,
		COUNT ConcernCounter)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	if (hPhoenix && StarShipPtr->special_counter)
	{
		RemoveElement (hPhoenix);
		FreeElement (hPhoenix);
		hPhoenix = 0;
	}

	if (StarShipPtr->RaceDescPtr->ship_info.energy_level <
			StarShipPtr->RaceDescPtr->ship_info.max_energy
			&& (StarShipPtr->special_counter == 0
			|| (BYTE)TFB_Random () < 20))
		StarShipPtr->ship_input_state |= SPECIAL;
	else
		StarShipPtr->ship_input_state &= ~SPECIAL;
	ship_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter);
}

static void pkunk_preprocess (ELEMENT *ElementPtr);
static void pkunk_postprocess (ELEMENT *ElementPtr);

static void
new_pkunk (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (!(ElementPtr->state_flags & PLAYER_SHIP))
	{
		ELEMENT *ShipPtr;

		LockElement (StarShipPtr->hShip, &ShipPtr);
		ShipPtr->death_func = new_pkunk;
		UnlockElement (StarShipPtr->hShip);
	}
	else
	{
		ElementPtr->state_flags = APPEARING | PLAYER_SHIP | IGNORE_SIMILAR
				| (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));

		ElementPtr->mass_points = SHIP_MASS;
		ElementPtr->preprocess_func = StarShipPtr->RaceDescPtr->preprocess_func;
		ElementPtr->postprocess_func = StarShipPtr->RaceDescPtr->postprocess_func;
		ElementPtr->death_func =
				(void (*) (ELEMENT *ElementPtr))
						StarShipPtr->RaceDescPtr->init_weapon_func;
		StarShipPtr->RaceDescPtr->preprocess_func = pkunk_preprocess;
		StarShipPtr->RaceDescPtr->postprocess_func = pkunk_postprocess;
		StarShipPtr->RaceDescPtr->init_weapon_func = initialize_bug_missile;
		StarShipPtr->RaceDescPtr->ship_info.crew_level = MAX_CREW;
		StarShipPtr->RaceDescPtr->ship_info.energy_level = MAX_ENERGY;
					/* fix vux impairment */
		StarShipPtr->RaceDescPtr->characteristics.max_thrust = MAX_THRUST;
		StarShipPtr->RaceDescPtr->characteristics.thrust_increment = THRUST_INCREMENT;
		StarShipPtr->RaceDescPtr->characteristics.turn_wait = TURN_WAIT;
		StarShipPtr->RaceDescPtr->characteristics.thrust_wait = THRUST_WAIT;
		StarShipPtr->RaceDescPtr->characteristics.special_wait = 0;

		StarShipPtr->ship_input_state = 0;
		StarShipPtr->cur_status_flags = 0;
		StarShipPtr->old_status_flags = 0;
		StarShipPtr->energy_counter = 0;
		StarShipPtr->weapon_counter = 0;
		StarShipPtr->special_counter = 0;
		ElementPtr->crew_level = 0;
		ElementPtr->turn_wait = 0;
		ElementPtr->thrust_wait = 0;
		ElementPtr->life_span = NORMAL_LIFE;

		StarShipPtr->ShipFacing = NORMALIZE_FACING (TFB_Random ());
		ElementPtr->current.image.farray = StarShipPtr->RaceDescPtr->ship_data.ship;
		ElementPtr->current.image.frame =
				SetAbsFrameIndex (StarShipPtr->RaceDescPtr->ship_data.ship[0],
				StarShipPtr->ShipFacing);
		SetPrimType (&(GLOBAL (DisplayArray))[
				ElementPtr->PrimIndex
				], STAMP_PRIM);

		do
		{
			ElementPtr->current.location.x =
					WRAP_X (DISPLAY_ALIGN_X (TFB_Random ()));
			ElementPtr->current.location.y =
					WRAP_Y (DISPLAY_ALIGN_Y (TFB_Random ()));
		} while (CalculateGravity (ElementPtr)
				|| TimeSpaceMatterConflict (ElementPtr));

		ElementPtr->hTarget = StarShipPtr->hShip;
	}
}

static void
intercept_pkunk_death (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	ElementPtr->state_flags &= ~DISAPPEARING;
	ElementPtr->life_span = 1;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (StarShipPtr->RaceDescPtr->ship_info.crew_level == 0)
	{
		ELEMENT *ShipPtr;

		LockElement (StarShipPtr->hShip, &ShipPtr);
		if (GRAVITY_MASS (ShipPtr->mass_points + 1))
		{
			ElementPtr->state_flags |= DISAPPEARING;
			ElementPtr->life_span = 0;
		}
		else
		{
			ShipPtr->mass_points = MAX_SHIP_MASS + 1;
			StarShipPtr->RaceDescPtr->preprocess_func = ShipPtr->preprocess_func;
			StarShipPtr->RaceDescPtr->postprocess_func = ShipPtr->postprocess_func;
			StarShipPtr->RaceDescPtr->init_weapon_func =
					(COUNT (*) (ELEMENT *ElementPtr, HELEMENT Weapon[]))
							ShipPtr->death_func;

			ElementPtr->death_func = new_pkunk;
		}
		UnlockElement (StarShipPtr->hShip);
	}
}

#define START_PHOENIX_COLOR BUILD_COLOR (MAKE_RGB15 (0x1F, 0x15, 0x00), 0x7A)
#define TRANSITION_LIFE 1

void
spawn_phoenix_trail (ELEMENT *ElementPtr)
{
	static const COLOR color_tab[] =
	{
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x00, 0x00), 0x2a),
		BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x00), 0x2b),
		BUILD_COLOR (MAKE_RGB15 (0x17, 0x00, 0x00), 0x2c),
		BUILD_COLOR (MAKE_RGB15 (0x13, 0x00, 0x00), 0x2d),
		BUILD_COLOR (MAKE_RGB15 (0x0F, 0x00, 0x00), 0x2e),
		BUILD_COLOR (MAKE_RGB15 (0x0B, 0x00, 0x00), 0x2f),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x15, 0x00), 0x7a),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x11, 0x00), 0x7b),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0E, 0x00), 0x7c),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0A, 0x00), 0x7d),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x07, 0x00), 0x7e),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x03, 0x00), 0x7f),
	};
	
#define NUM_TAB_COLORS (sizeof (color_tab) / sizeof (color_tab[0]))

	COUNT color_index = 0;
	COLOR Color;

	Color = COLOR_256 (
			GetPrimColor (&(GLOBAL (DisplayArray))[ElementPtr->PrimIndex])
			);
	if (Color != 0x2F)
	{
		ElementPtr->life_span = TRANSITION_LIFE;

		++Color;
		if (Color > 0x7F)
			Color = 0x2A;
		if (Color <= 0x2f && Color >= 0x2a)
				color_index = (COUNT)Color - 0x2a;
		else /* color is between 0x7a and 0x7f */
				color_index = (COUNT)(Color - 0x7a) + (NUM_TAB_COLORS >> 1);
		SetPrimColor (
				&(GLOBAL (DisplayArray))[ElementPtr->PrimIndex],
				color_tab[color_index]
				);

		ElementPtr->state_flags &= ~DISAPPEARING;
		ElementPtr->state_flags |= CHANGING;
	}
}

#define PHOENIX_LIFE 12

void
phoenix_transition (ELEMENT *ElementPtr)
{
	HELEMENT hShipImage;
	ELEMENT *ShipImagePtr;
	STARSHIP *StarShipPtr;
	
	GetElementStarShip (ElementPtr, &StarShipPtr);
	LockElement (StarShipPtr->hShip, &ShipImagePtr);

	if (!(ShipImagePtr->state_flags & NONSOLID))
	{
		ElementPtr->preprocess_func = NULL;
	}
	else if ((hShipImage = AllocElement ()))
	{
#define TRANSITION_SPEED DISPLAY_TO_WORLD (20)
		COUNT angle;

		PutElement (hShipImage);

		LockElement (hShipImage, &ShipImagePtr);
		ShipImagePtr->state_flags = APPEARING | FINITE_LIFE | NONSOLID;
		ShipImagePtr->life_span = TRANSITION_LIFE;
		SetPrimType (&(GLOBAL (DisplayArray))[ShipImagePtr->PrimIndex],
				STAMPFILL_PRIM);
		SetPrimColor (
				&(GLOBAL (DisplayArray))[ShipImagePtr->PrimIndex],
				START_PHOENIX_COLOR
				);
		ShipImagePtr->current.image = ElementPtr->current.image;
		ShipImagePtr->current.location = ElementPtr->current.location;
		if (!(ElementPtr->state_flags & PLAYER_SHIP))
		{
			angle = ElementPtr->mass_points;

			ShipImagePtr->current.location.x +=
					COSINE (angle, TRANSITION_SPEED);
			ShipImagePtr->current.location.y +=
					SINE (angle, TRANSITION_SPEED);
			ElementPtr->preprocess_func = NULL;
		}
		else
		{
			angle = FACING_TO_ANGLE (StarShipPtr->ShipFacing);

			ShipImagePtr->current.location.x -=
					COSINE (angle, TRANSITION_SPEED)
					* (ElementPtr->life_span - 1);
			ShipImagePtr->current.location.y -=
					SINE (angle, TRANSITION_SPEED)
					* (ElementPtr->life_span - 1);

			ShipImagePtr->current.location.x =
					WRAP_X (ShipImagePtr->current.location.x);
			ShipImagePtr->current.location.y =
					WRAP_Y (ShipImagePtr->current.location.y);
		}

		ShipImagePtr->mass_points = (BYTE)angle;
		ShipImagePtr->preprocess_func = phoenix_transition;
		ShipImagePtr->death_func = spawn_phoenix_trail;
		SetElementStarShip (ShipImagePtr, StarShipPtr);

		UnlockElement (hShipImage);
	}

	UnlockElement (StarShipPtr->hShip);
}

static void
pkunk_preprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (ElementPtr->state_flags & APPEARING)
	{
		ELEMENT *PhoenixPtr;

		if (((BYTE)TFB_Random () & 1)
				&& (hPhoenix = AllocElement ()))
		{

			LockElement (hPhoenix, &PhoenixPtr);
			PhoenixPtr->state_flags =
					FINITE_LIFE | NONSOLID | IGNORE_SIMILAR
					| (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));
			PhoenixPtr->life_span = 1;

			PhoenixPtr->death_func = intercept_pkunk_death;

			SetElementStarShip (PhoenixPtr, StarShipPtr);

			UnlockElement (hPhoenix);
			InsertElement (hPhoenix, GetHeadElement ());
		}

		if (ElementPtr->hTarget == 0)
			StarShipPtr->RaceDescPtr->preprocess_func = 0;
		else
		{
			COUNT angle, facing;

			ProcessSound (SetAbsSoundIndex (
					StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1
					), ElementPtr);

			ElementPtr->life_span = PHOENIX_LIFE;
			SetPrimType (&(GLOBAL (DisplayArray))[ElementPtr->PrimIndex],
					NO_PRIM);
			ElementPtr->state_flags |= NONSOLID | FINITE_LIFE | CHANGING;

			facing = StarShipPtr->ShipFacing;
			for (angle = OCTANT; angle < FULL_CIRCLE; angle += QUADRANT)
			{
				StarShipPtr->ShipFacing = NORMALIZE_FACING (
						facing + ANGLE_TO_FACING (angle)
						);
				phoenix_transition (ElementPtr);
			}
			StarShipPtr->ShipFacing = facing;
		}
	}

	if (StarShipPtr->RaceDescPtr->preprocess_func)
	{
		StarShipPtr->cur_status_flags &=
				~(LEFT | RIGHT | THRUST | WEAPON | SPECIAL);

		if (ElementPtr->life_span == NORMAL_LIFE)
		{
			ElementPtr->current.image.frame =
					ElementPtr->next.image.frame =
					SetEquFrameIndex (
					ElementPtr->current.image.farray[0],
					ElementPtr->current.image.frame);
			SetPrimType (&(GLOBAL (DisplayArray))[ElementPtr->PrimIndex],
					STAMP_PRIM);
			InitIntersectStartPoint (ElementPtr);
			InitIntersectEndPoint (ElementPtr);
			InitIntersectFrame (ElementPtr);
			ZeroVelocityComponents (&ElementPtr->velocity);
			ElementPtr->state_flags &= ~(NONSOLID | FINITE_LIFE);
			ElementPtr->state_flags |= CHANGING;

			StarShipPtr->RaceDescPtr->preprocess_func = 0;
		}
	}
}

static void
pkunk_postprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (StarShipPtr->RaceDescPtr->characteristics.special_wait)
		--StarShipPtr->RaceDescPtr->characteristics.special_wait;
	else if ((StarShipPtr->cur_status_flags & SPECIAL)
			&& StarShipPtr->RaceDescPtr->ship_info.energy_level <
			StarShipPtr->RaceDescPtr->ship_info.max_energy)
	{
		COUNT CurSound;
		static COUNT LastSound = 0;

		do
		{
			CurSound =
					2 + ((COUNT)TFB_Random ()
					% (GetSoundCount (StarShipPtr->RaceDescPtr->ship_data.ship_sounds) - 2));
		} while (CurSound == LastSound);
		ProcessSound (SetAbsSoundIndex (
				StarShipPtr->RaceDescPtr->ship_data.ship_sounds, CurSound
				), ElementPtr);
		LastSound = CurSound;

		DeltaEnergy (ElementPtr, SPECIAL_ENERGY_COST);

		StarShipPtr->RaceDescPtr->characteristics.special_wait = SPECIAL_WAIT;
	}
}

RACE_DESC*
init_pkunk (void)
{
	RACE_DESC *RaceDescPtr;

	pkunk_desc.preprocess_func = pkunk_preprocess;
	pkunk_desc.postprocess_func = pkunk_postprocess;
	pkunk_desc.init_weapon_func = initialize_bug_missile;
	pkunk_desc.cyborg_control.intelligence_func = pkunk_intelligence;

	RaceDescPtr = &pkunk_desc;

	return (RaceDescPtr);
}

