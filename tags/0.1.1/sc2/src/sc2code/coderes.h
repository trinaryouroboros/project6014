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

#ifndef _CODERES_H
#define _CODERES_H

#include "reslib.h"

extern BOOLEAN InstallCodeResType (void);
extern void *LoadCodeResInstance (RESOURCE res);
extern void *CaptureCodeRes (void *hCode, void *pData, void **ppLocData);
extern void *ReleaseCodeRes (void *CodeRef);
extern BOOLEAN DestroyCodeRes (void *hCode);

typedef struct
{
	UWORD size;
} CODE_REF;

#endif /* _CODERES_H */
