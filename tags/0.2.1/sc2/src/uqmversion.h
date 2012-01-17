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

#ifndef UQMVERSION_H
#define UQMVERSION_H

#define P6014_MAJOR_VERSION     0
#define P6014_MAJOR_VERSION_S  "0"
#define P6014_MINOR_VERSION     2
#define P6014_MINOR_VERSION_S  "2"
#define P6014_PATCH_VERSION     1
#define P6014_PATCH_VERSION_S  "1"
#define P6014_EXTRA_VERSION    ""
/* The final version is interpreted as:
 * printf ("%d.%d.%d%s", P6014_MAJOR_VERSION, P6014_MINOR_VERSION,
 * 		P6014_PATCH_VERSION, P6014_EXTRA_VERSION);
 */

#define P6014_STRING_VERSION \
P6014_MAJOR_VERSION_S "." P6014_MINOR_VERSION_S "." P6014_PATCH_VERSION_S \
P6014_EXTRA_VERSION

#define P6014_SVN_REVISION "$Revision$"

#endif
