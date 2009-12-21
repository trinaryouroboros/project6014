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

#include "build.h"
#include "encount.h"
#include "globdata.h"
#include "lander.h"
#include "nameref.h"
#include "resinst.h"
#include "setup.h"
#include "state.h"
#include "sounds.h"
#include "planets/genall.h"
#include "libs/mathlib.h"


void
GenerateMasterrace (BYTE control)
{
	switch (control)
	{
		case GENERATE_MOONS:
			GenerateRandomIP (GENERATE_MOONS);
			break;
		case GENERATE_PLANETS:
			GenerateRandomIP (GENERATE_PLANETS);
			break;
		case GENERATE_ORBITAL:
			GenerateRandomIP (GENERATE_ORBITAL);
			break;
		default:
			GenerateRandomIP (control);
			break;
	}
}