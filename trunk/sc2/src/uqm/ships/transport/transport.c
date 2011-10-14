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

#define MAX_CREW 10
#define MAX_ENERGY 10
#define ENERGY_REGENERATION 1
#define WEAPON_ENERGY_COST 5
#define SPECIAL_ENERGY_COST 5
#define ENERGY_WAIT 2
#define MAX_THRUST 5
#define THRUST_INCREMENT 1
#define TURN_WAIT 2
#define THRUST_WAIT 5
#define WEAPON_WAIT 4
#define SPECIAL_WAIT 4

#define SHIP_MASS 5

static RACE_DESC transport_desc =
{
	{ /* SHIP_INFO */
		0,
		0, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		0,
		0,
		TRANSPORT_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		INFINITE_RADIUS, /* Initial sphere of influence radius */
		{ /* Known location (center of SoI) */
			MAX_X_UNIVERSE >> 1, MAX_Y_UNIVERSE >> 1,
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
			NULL_RESOURCE,
			NULL_RESOURCE,
			NULL_RESOURCE,
		},
		{
			NULL_RESOURCE,
			NULL_RESOURCE,
			NULL_RESOURCE,
		},
		{
			NULL_RESOURCE,
			NULL_RESOURCE,
			NULL_RESOURCE,
		},
		{
			NULL_RESOURCE,
			NULL, NULL, NULL, NULL, NULL
		},
		NULL_RESOURCE,
		NULL_RESOURCE,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		0,
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
#define MAX_THRUST_2XRES 10
#define THRUST_INCREMENT_2XRES 2

// JMS_GFX
static RACE_DESC transport_desc_2xres =
{
	{ /* SHIP_INFO */
		0,
		0, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		0,
		0,
		TRANSPORT_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		INFINITE_RADIUS, /* Initial sphere of influence radius */
		{ /* Known location (center of SoI) */
			MAX_X_UNIVERSE >> 1, MAX_Y_UNIVERSE >> 1,
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
			NULL_RESOURCE,
			NULL_RESOURCE,
			NULL_RESOURCE,
		},
		{
			NULL_RESOURCE,
			NULL_RESOURCE,
			NULL_RESOURCE,
		},
		{
			NULL_RESOURCE,
			NULL_RESOURCE,
			NULL_RESOURCE,
		},
		{
			NULL_RESOURCE,
			NULL, NULL, NULL, NULL, NULL
		},
		NULL_RESOURCE,
		NULL_RESOURCE,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		0,
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
#define MAX_THRUST_4XRES 20
#define THRUST_INCREMENT_4XRES 4

// JMS_GFX
static RACE_DESC transport_desc_4xres =
{
	{ /* SHIP_INFO */
		0,
		0, /* Super Melee cost */
		MAX_CREW, MAX_CREW,
		MAX_ENERGY, MAX_ENERGY,
		0,
		0,
		TRANSPORT_MICON_MASK_PMAP_ANIM,
		NULL, NULL, NULL, SHIP_IS_NOT_DAMAGED
	},
	{ /* FLEET_STUFF */
		INFINITE_RADIUS, /* Initial sphere of influence radius */
		{ /* Known location (center of SoI) */
			MAX_X_UNIVERSE >> 1, MAX_Y_UNIVERSE >> 1,
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
			NULL_RESOURCE,
			NULL_RESOURCE,
			NULL_RESOURCE,
		},
		{
			NULL_RESOURCE,
			NULL_RESOURCE,
			NULL_RESOURCE,
		},
		{
			NULL_RESOURCE,
			NULL_RESOURCE,
			NULL_RESOURCE,
		},
		{
			NULL_RESOURCE,
			NULL, NULL, NULL, NULL, NULL
		},
		NULL_RESOURCE,
		NULL_RESOURCE,
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		{ NULL, NULL, NULL },
		NULL, NULL
	},
	{
		0,
		0,
		NULL,
	},
	(UNINIT_FUNC *) NULL,
	(PREPROCESS_FUNC *) NULL,
	(POSTPROCESS_FUNC *) NULL,
	(INIT_WEAPON_FUNC *) NULL,
	0,
	0, /* CodeRef */
};

RACE_DESC*
init_transport (void)
{
	RACE_DESC *RaceDescPtr;

	if (RESOLUTION_FACTOR == 0)
		RaceDescPtr = &transport_desc;
	else if (RESOLUTION_FACTOR == 1)
		RaceDescPtr = &transport_desc_2xres;
	else
		RaceDescPtr = &transport_desc_4xres;

	return (RaceDescPtr);
}

