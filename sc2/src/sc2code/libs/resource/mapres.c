/*  mapres.c, Copyright (c) 2005 Michael C. Martin */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope thta it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  Se the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "libs/reslib.h"
#include "libs/log.h"
#include "libs/uio/charhashtable.h"
#include "propfile.h"
#include "stringbank.h"

/* The CharHashTable owns its keys, but not its values. We will keep the 
   values in a StringBank. */

static CharHashTable_HashTable *map = NULL;
static stringbank *bank = NULL;

static void
check_map_init (void)
{
	if (map == NULL) {
		map = CharHashTable_newHashTable (NULL, NULL, NULL, NULL, 0, 0.85, 0.9);
	}
	if (bank == NULL) {
		bank = StringBank_Create ();
	}
}

void
res_ClearTables (void)
{
	if (map != NULL) {
		CharHashTable_deleteHashTable (map);
		map = NULL;
	}
	if (bank != NULL) {
		StringBank_Free (bank);
		bank = NULL;
	}
}

/* Type conversion routines. */
void
res_LoadFile (uio_Stream *s)
{
	check_map_init ();
	
	if (!map)
	{
		return;
	}
	
	PropFile_from_file (s, res_PutString, NULL);
}

void
res_LoadFilename (uio_DirHandle *path, const char *fname)
{
	check_map_init ();
	
	if (!map)
	{
		return;
	}
	
	PropFile_from_filename (path, fname, res_PutString, NULL);
}

void
res_SaveFile (uio_Stream *f, const char *prefix)
{
	CharHashTable_Iterator *i;
	int prefix_len = 0;
	
	if (prefix)
		prefix_len = strlen (prefix);
	
	check_map_init ();

	i = CharHashTable_getIterator (map);
	while (!CharHashTable_iteratorDone (i))
	{
		const char *key = CharHashTable_iteratorKey (i);
		const char *value = (const char *)CharHashTable_iteratorValue (i);
		if (!prefix || !strncmp (prefix, key, prefix_len)) {
			WriteResFile (key, 1, strlen (key), f);
			PutResFileChar(' ', f);
			PutResFileChar('=', f);
			PutResFileChar(' ', f);
			WriteResFile (value, 1, strlen (value), f);
			PutResFileNewline(f);
		}
		i = CharHashTable_iteratorNext (i);
	}
	CharHashTable_freeIterator (i);
}

void
res_SaveFilename (uio_DirHandle *path, const char *fname, const char *prefix)
{
	uio_Stream *f;
	
	check_map_init ();
	f = res_OpenResFile (path, fname, "wb");
	if (f) {
		res_SaveFile (f, prefix);
		res_CloseResFile (f);
	}
}
