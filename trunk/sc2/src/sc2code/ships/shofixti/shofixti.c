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

// JMS 2010: Disabled Tanaka/Katana from the game.
//			 (Left the code there for reference though.)

#include "ships/ship.h"
#include "ships/shofixti/resinst.h"

#include "globdata.h"
#include "libs/mathlib.h"


#define MAX_CREW 6
#define MAX_ENERGY 4
#define ENERGY_REGENERATION 1
#define WEAPON_ENERGY_COST 1
#define SPECIAL_ENERGY_COST 0
#define ENERGY_WAIT 9
#define MAX_THRUST 35
#define THRUST_INCREMENT 5
#define TURN_WAIT 1
#define THRUST_WAIT 0
#define WEAPON_WAIT 3
#define SPECIAL_WAIT 0

#define SHIP_MASS 1
#define MISSILE_SPEED DISPLAY_TO_WORLD (24)
#define MISSILE_LIFE 10

static RACE_DESC shofixti_desc =
{
	{ /* SHIP_INFO */
		FIRES_FORE,
		5, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		SHOFIXTI_RACE_STRINGS,
		SHOFIXTI_ICON_MASK_PMAP_ANIM,
		SHOFIXTI_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		250 / SPHERE_RADIUS_INCREMENT * 2, /* Initial sphere of influence radius */
		{ /* Known location (center of SoI) */
			7908,6269,
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
			SHOFIXTI_BIG_MASK_PMAP_ANIM,
			SHOFIXTI_MED_MASK_PMAP_ANIM,
			SHOFIXTI_SML_MASK_PMAP_ANIM,
		},
		{
			DART_BIG_MASK_PMAP_ANIM,
			DART_MED_MASK_PMAP_ANIM,
			DART_SML_MASK_PMAP_ANIM,
		},
		{
			DESTRUCT_BIG_MASK_ANIM,
			DESTRUCT_MED_MASK_ANIM,
			DESTRUCT_SML_MASK_ANIM,
		},
		{
			SHOFIXTI_CAPTAIN_MASK_PMAP_ANIM,
			NULL, NULL, NULL, NULL, NULL
		},
	        SHOFIXTI_VICTORY_SONG,
		SHOFIXTI_SHIP_SOUNDS,
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
initialize_standard_missile (ELEMENT *ShipPtr, HELEMENT MissileArray[])
{
#define SHOFIXTI_OFFSET 15
#define MISSILE_HITS 1
#define MISSILE_DAMAGE 1
#define MISSILE_OFFSET 1
	STARSHIP *StarShipPtr;
	MISSILE_BLOCK MissileBlock;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.face = MissileBlock.index = StarShipPtr->ShipFacing;
	MissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
			| IGNORE_SIMILAR;
	MissileBlock.pixoffs = SHOFIXTI_OFFSET;
	MissileBlock.speed = MISSILE_SPEED;
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = NULL;
	MissileBlock.blast_offs = MISSILE_OFFSET;
	MissileArray[0] = initialize_missile (&MissileBlock);

	return (1);
}

static void
destruct_preprocess (ELEMENT *ElementPtr)
{
#define DESTRUCT_SWITCH ((NUM_EXPLOSION_FRAMES * 3) - 3)
	PRIMITIVE *lpPrim;

	lpPrim = &(GLOBAL (DisplayArray))[ElementPtr->PrimIndex];
	ElementPtr->state_flags |= CHANGING;
	if (ElementPtr->life_span > DESTRUCT_SWITCH)
	{
		SetPrimType (lpPrim, STAMPFILL_PRIM);
		if (ElementPtr->life_span == DESTRUCT_SWITCH + 2)
			SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x0A), 0x0E));
		else
			SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F));
	}
	else if (ElementPtr->life_span < DESTRUCT_SWITCH)
	{
		ElementPtr->next.image.frame =
				IncFrameIndex (ElementPtr->current.image.frame);
		if (GetPrimColor (lpPrim) == BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F))
			SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x0A), 0x0E));
		else if (GetPrimColor (lpPrim) == BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x0A), 0x0E))
			SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0A, 0x0A), 0x0C));
		else if (GetPrimColor (lpPrim) == BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0A, 0x0A), 0x0C))
			SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x14, 0x0A, 0x00), 0x06));
		else if (GetPrimColor (lpPrim) == BUILD_COLOR (MAKE_RGB15 (0x14, 0x0A, 0x00), 0x06))
			SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x14, 0x00, 0x00), 0x04));
	}
	else
	{
		HELEMENT hDestruct;

		SetPrimType (lpPrim, NO_PRIM);
		ElementPtr->preprocess_func = NULL;

		hDestruct = AllocElement ();
		if (hDestruct)
		{
			ELEMENT *DestructPtr;
			STARSHIP *StarShipPtr;

			GetElementStarShip (ElementPtr, &StarShipPtr);

			PutElement (hDestruct);
			LockElement (hDestruct, &DestructPtr);
			SetElementStarShip (DestructPtr, StarShipPtr);
			DestructPtr->hit_points = DestructPtr->mass_points = 0;
			DestructPtr->state_flags = APPEARING | FINITE_LIFE | NONSOLID;
			DestructPtr->life_span = (NUM_EXPLOSION_FRAMES - 3) - 1;
			SetPrimType (
					&(GLOBAL (DisplayArray))[DestructPtr->PrimIndex],
					STAMPFILL_PRIM
					);
			SetPrimColor (
					&(GLOBAL (DisplayArray))[DestructPtr->PrimIndex],
					BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F)
					);
			DestructPtr->current.image.farray = StarShipPtr->RaceDescPtr->ship_data.special;
			DestructPtr->current.image.frame = StarShipPtr->RaceDescPtr->ship_data.special[0];
			DestructPtr->current.location = ElementPtr->current.location;
			{
				DestructPtr->preprocess_func = destruct_preprocess;
			}
			DestructPtr->postprocess_func =
					DestructPtr->death_func = NULL;
			ZeroVelocityComponents (&DestructPtr->velocity);
			UnlockElement (hDestruct);
		}
	}
}

/* In order to detect any Orz Marines that have boarded the ship
   when it self-destructs, we'll need to see these Orz functions */
extern void intruder_preprocess (ELEMENT*);
extern void marine_collision (ELEMENT*, POINT*, ELEMENT*, POINT*);
#define ORZ_MARINE(ptr) (ptr->preprocess_func == intruder_preprocess && \
		ptr->collision_func == marine_collision)

static void
self_destruct (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (ElementPtr->state_flags & PLAYER_SHIP)
	{
		HELEMENT hDestruct;
		
		hDestruct = AllocElement ();
		if (hDestruct)
		{
			ELEMENT *DestructPtr;

			LockElement (hDestruct, &DestructPtr);
			DestructPtr->state_flags = APPEARING | NONSOLID | FINITE_LIFE |
					(ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));
			DestructPtr->next.location = ElementPtr->next.location;
			DestructPtr->life_span = 0;
			DestructPtr->pParent = ElementPtr->pParent;
			DestructPtr->hTarget = 0;

			DestructPtr->death_func = self_destruct;

			UnlockElement (hDestruct);

			PutElement (hDestruct);
		}

		ElementPtr->state_flags |= NONSOLID;
		ElementPtr->life_span = 0;

		ElementPtr->preprocess_func = destruct_preprocess;
	}
	else
	{
		HELEMENT hElement, hNextElement;

		for (hElement = GetHeadElement ();
				hElement != 0; hElement = hNextElement)
		{
			ELEMENT *ObjPtr;

			LockElement (hElement, &ObjPtr);
			hNextElement = GetSuccElement (ObjPtr);

			if (CollidingElement (ObjPtr) || ORZ_MARINE (ObjPtr))
			{
#define DESTRUCT_RANGE 180
				SIZE delta_x, delta_y;
				DWORD dist;

				if ((delta_x = ObjPtr->next.location.x
						- ElementPtr->next.location.x) < 0)
					delta_x = -delta_x;
				if ((delta_y = ObjPtr->next.location.y
						- ElementPtr->next.location.y) < 0)
					delta_y = -delta_y;
				delta_x = WORLD_TO_DISPLAY (delta_x);
				delta_y = WORLD_TO_DISPLAY (delta_y);
				if (delta_x <= DESTRUCT_RANGE && delta_y <= DESTRUCT_RANGE
						&& (dist = (DWORD)(delta_x * delta_x)
						+ (DWORD)(delta_y * delta_y)) <=
						(DWORD)(DESTRUCT_RANGE * DESTRUCT_RANGE))
				{
#define MAX_DESTRUCTION (DESTRUCT_RANGE / 10)
					SIZE destruction;

					destruction = ((MAX_DESTRUCTION
							* (DESTRUCT_RANGE - square_root (dist)))
							/ DESTRUCT_RANGE) + 1;

					if (ObjPtr->state_flags & PLAYER_SHIP)
					{
						if (!DeltaCrew (ObjPtr, -destruction))
							ObjPtr->life_span = 0;
					}
					else if (!GRAVITY_MASS (ObjPtr->mass_points))
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

static void
shofixti_intelligence (ELEMENT *ShipPtr, EVALUATE_DESC *ObjectsOfConcern,
		COUNT ConcernCounter)
{
	STARSHIP *StarShipPtr;

	ship_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter);

	GetElementStarShip (ShipPtr, &StarShipPtr);
	if (StarShipPtr->special_counter != 0)
		return;

	if (StarShipPtr->ship_input_state & SPECIAL)
		StarShipPtr->ship_input_state &= ~SPECIAL;
	else
	{
		EVALUATE_DESC *lpWeaponEvalDesc;
		EVALUATE_DESC *lpShipEvalDesc;

		lpWeaponEvalDesc = &ObjectsOfConcern[ENEMY_WEAPON_INDEX];
		lpShipEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
		if (StarShipPtr->RaceDescPtr->ship_data.special[0]
				&& (GetFrameCount (StarShipPtr->RaceDescPtr->ship_data.
				captain_control.special)
				- GetFrameIndex (StarShipPtr->RaceDescPtr->ship_data.
				captain_control.special) > 5
				|| (lpShipEvalDesc->ObjectPtr != NULL
				&& lpShipEvalDesc->which_turn <= 4)
				|| (lpWeaponEvalDesc->ObjectPtr != NULL
							/* means IMMEDIATE WEAPON */
				&& (((lpWeaponEvalDesc->ObjectPtr->state_flags & PLAYER_SHIP)
				&& ShipPtr->crew_level == 1)
				|| (PlotIntercept (lpWeaponEvalDesc->ObjectPtr, ShipPtr, 2, 0)
				&& lpWeaponEvalDesc->ObjectPtr->mass_points >=
				ShipPtr->crew_level
				&& (TFB_Random () & 1))))))
			StarShipPtr->ship_input_state |= SPECIAL;
	}
}

static void
shofixti_postprocess (ELEMENT *ElementPtr)
{
	STARSHIP *StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if ((StarShipPtr->cur_status_flags
			^ StarShipPtr->old_status_flags) & SPECIAL)
	{
		StarShipPtr->RaceDescPtr->ship_data.captain_control.special =
				IncFrameIndex (StarShipPtr->RaceDescPtr->ship_data.
				captain_control.special);
		if (GetFrameCount (StarShipPtr->RaceDescPtr->ship_data.
				captain_control.special)
				- GetFrameIndex (StarShipPtr->RaceDescPtr->ship_data.
				captain_control.special) == 3)
			self_destruct (ElementPtr);
	}
}

RACE_DESC*
init_shofixti (void)
{
	RACE_DESC *RaceDescPtr;

	static RACE_DESC new_shofixti_desc;

	shofixti_desc.postprocess_func = shofixti_postprocess;
	shofixti_desc.init_weapon_func = initialize_standard_missile;
	shofixti_desc.cyborg_control.intelligence_func = shofixti_intelligence;

	new_shofixti_desc = shofixti_desc;
	
	// JMS: No more Tanaka/Katana!!
	//if (LOBYTE (GLOBAL (CurrentActivity)) == IN_ENCOUNTER
	//		&& !GET_GAME_STATE (SHOFIXTI_RECRUITED))
	if(0)
	{
		// Tanaka/Katana flies in a damaged ship.
#define NUM_LIMPETS 3
		COUNT i;

		new_shofixti_desc.ship_data.ship_rsc[0] = OLDSHOF_BIG_MASK_PMAP_ANIM;
		new_shofixti_desc.ship_data.ship_rsc[1] = OLDSHOF_MED_MASK_PMAP_ANIM;
		new_shofixti_desc.ship_data.ship_rsc[2] = OLDSHOF_SML_MASK_PMAP_ANIM;
		new_shofixti_desc.ship_data.special_rsc[0] = NULL_RESOURCE;
		new_shofixti_desc.ship_data.special_rsc[1] = NULL_RESOURCE;
		new_shofixti_desc.ship_data.special_rsc[2] = NULL_RESOURCE;
		new_shofixti_desc.ship_data.captain_control.captain_rsc =
				OLDSHOF_CAPTAIN_MASK_PMAP_ANIM;

		/* Weapon doesn't work as well */
		new_shofixti_desc.characteristics.weapon_wait = 10;
		
		/* Simulate VUX limpets */
		for (i = 0; i < NUM_LIMPETS; ++i)
		{
			if (++new_shofixti_desc.characteristics.turn_wait == 0)
				--new_shofixti_desc.characteristics.turn_wait;
			if (++new_shofixti_desc.characteristics.thrust_wait == 0)
				--new_shofixti_desc.characteristics.thrust_wait;
#define MIN_THRUST_INCREMENT DISPLAY_TO_WORLD (1)
			if (new_shofixti_desc.characteristics.thrust_increment <=
					MIN_THRUST_INCREMENT)
			{
				new_shofixti_desc.characteristics.max_thrust =
						new_shofixti_desc.characteristics.thrust_increment << 1;
			}
			else
			{
				COUNT num_thrusts;

				num_thrusts = new_shofixti_desc.characteristics.max_thrust /
						new_shofixti_desc.characteristics.thrust_increment;
				--new_shofixti_desc.characteristics.thrust_increment;
				new_shofixti_desc.characteristics.max_thrust =
						new_shofixti_desc.characteristics.thrust_increment *
						num_thrusts;
			}
		}
	}

	RaceDescPtr = &new_shofixti_desc;

	return (RaceDescPtr);
}

