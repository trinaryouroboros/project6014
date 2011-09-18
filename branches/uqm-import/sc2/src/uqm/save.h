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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _SAVE_H
#define _SAVE_H

#include "sis.h"
		// for SUMMARY_DESC
#include "libs/compiler.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern void SaveProblem (void);
extern BOOLEAN SaveGame (COUNT which_game, SUMMARY_DESC *summary_desc);

#if defined(__cplusplus)
}
#endif

#endif  /* _SAVE_H */

