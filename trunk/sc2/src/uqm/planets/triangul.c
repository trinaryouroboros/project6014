// Copyright Jaakko Markus Seppala 2010

#include "libs/strlib.h"
#include "libs/graphics/gfx_common.h"
#include "libs/mathlib.h"

#include "../setup.h"
#include "triangul.h"
#include "planets.h"
// for DrawOval

// Shamelessly copied from pstarmap.c and should be updated to reflect
// any changes on the original
static inline long
signedDivWithError (long val, long divisor)
{
	int invert = 0;
	if (val < 0)
	{
		invert = 1;
		val = -val;
	}
	val = (val + ROUNDING_ERROR (divisor)) / divisor;
	return invert ? -val : val;
}

#define MAP_FIT_X ((MAX_X_UNIVERSE + 1) / SIS_SCREEN_WIDTH + 1)

static inline COORD
universeToDispx (COORD ux, POINT mapOrigin, int zoomLevel)
{
	return signedDivWithError ((((long)ux - mapOrigin.x) << zoomLevel)
			* SIS_SCREEN_WIDTH, MAX_X_UNIVERSE + MAP_FIT_X)
			+ ((SIS_SCREEN_WIDTH - 1) >> 1);
}
#define UNIVERSE_TO_DISPX(ux, mapOrigin, zoomLevel)  universeToDispx(ux, mapOrigin, zoomLevel)

static inline COORD
universeToDispy (COORD uy, POINT mapOrigin, int zoomLevel)
{
	return signedDivWithError ((((long)mapOrigin.y - uy) << zoomLevel)
			* SIS_SCREEN_HEIGHT, MAX_Y_UNIVERSE + 2)
			+ ((SIS_SCREEN_HEIGHT - 1) >> 1);
}
#define UNIVERSE_TO_DISPY(uy, mapOrigin, zoomLevel)  universeToDispy(uy, mapOrigin, zoomLevel)

// JMS
static void
GetTriangulationSphereRect (COUNT radius, POINT coords, char sphereString[], RECT *pRect, RECT *pRepairRect, POINT mapOrigin, int zoomLevel)
{
	long diameter;
	
	diameter = (long)(radius * SPHERE_RADIUS_INCREMENT);
	pRect->extent.width = UNIVERSE_TO_DISPX (diameter, mapOrigin, zoomLevel) - UNIVERSE_TO_DISPX (0, mapOrigin, zoomLevel);
	if (pRect->extent.width < 0)
		pRect->extent.width = -pRect->extent.width;
	else if (pRect->extent.width == 0)
		pRect->extent.width = 1;
	pRect->extent.height = UNIVERSE_TO_DISPY (diameter, mapOrigin, zoomLevel) - UNIVERSE_TO_DISPY (0, mapOrigin, zoomLevel);
	if (pRect->extent.height < 0)
		pRect->extent.height = -pRect->extent.height;
	else if (pRect->extent.height == 0)
		pRect->extent.height = 1;
	
	pRect->corner.x = UNIVERSE_TO_DISPX (coords.x, mapOrigin, zoomLevel);
	pRect->corner.y = UNIVERSE_TO_DISPY (coords.y, mapOrigin, zoomLevel);
	pRect->corner.x -= pRect->extent.width >> 1;
	pRect->corner.y -= pRect->extent.height >> 1;
	
	{
		TEXT t;
		
		SetContextFont (TinyFont);
		
		t.baseline.x = pRect->corner.x + (pRect->extent.width >> 1);
		t.baseline.y = pRect->corner.y + (pRect->extent.height + (8 * RESOLUTION_FACTOR)) - 1; // JMS_GFX
		t.align = ALIGN_CENTER;
		t.CharCount =sizeof(sphereString);
		t.pStr = (UNICODE *)sphereString;
		TextRect (&t, pRepairRect, NULL);
		
		if (pRepairRect->corner.x <= 0)
			pRepairRect->corner.x = 1;
		else if (pRepairRect->corner.x + pRepairRect->extent.width >= SIS_SCREEN_WIDTH)
			pRepairRect->corner.x = SIS_SCREEN_WIDTH - pRepairRect->extent.width - 1;
		if (pRepairRect->corner.y <= 0)
			pRepairRect->corner.y = 1;
		else if (pRepairRect->corner.y + pRepairRect->extent.height >= SIS_SCREEN_HEIGHT)
			pRepairRect->corner.y = SIS_SCREEN_HEIGHT - pRepairRect->extent.height - 1;
		
		BoxUnion (pRepairRect, pRect, pRepairRect);
		pRepairRect->extent.width++;
		pRepairRect->extent.height++;
	}
}

// JMS: Draw Triangulation Spheres
void drawTriangulationSpheres (COUNT which_space, COUNT orz_space, RECT *pClipRect, POINT mapOrigin, int zoomLevel)
{	
	if (which_space <= 1 && orz_space <= 1) // JMS: Orz space check
	{
		COUNT index;
		COLOR c = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x00, 0x00), 0x2D);
		
		// JMS: Sphere_statuses, getgamestates defined at triangul.h
		BYTE sphere_statuses[] =
		  {
		    SPHERE_STATUSES
		  };
		
		// JMS: Sphere centre coordinates, defined at triangul.h
		static const POINT sphere_coords[] =
		  {
		    SPHERE_COORDS
		  };

	        // JMS: Sphere radiuses, defined at triangul.h
		static const COUNT sphere_radiuses[] =
		  {
		    SPHERE_RADIUSES
		  };

		// JMS: Sphere texts, defined at triangul.h
		char sphere_strings[][64] =
		  {
			  SPHERE_STRINGS
		  };
		
		// JMS: Loop through every sphere
		for (index = 0; index < sizeof(sphere_statuses) ; ++index)
		{
			// JMS: If the sphere is marked visible
			if (sphere_statuses[index])
			{
				RECT r, repair_r;
				COUNT radius = sphere_radiuses[index];
				POINT coords = sphere_coords[index];

				GetTriangulationSphereRect (radius, coords, sphere_strings[index], &r, &repair_r, mapOrigin, zoomLevel);

				if (r.corner.x < SIS_SCREEN_WIDTH
					&& r.corner.y < SIS_SCREEN_HEIGHT
					&& r.corner.x + r.extent.width > 0
					&& r.corner.y + r.extent.height > 0
					&& (pClipRect == 0
						|| (repair_r.corner.x < pClipRect->corner.x + pClipRect->extent.width
							&& repair_r.corner.y < pClipRect->corner.y + pClipRect->extent.height
							&& repair_r.corner.x + repair_r.extent.width > pClipRect->corner.x
							&& repair_r.corner.y + repair_r.extent.height > pClipRect->corner.y)))
				{
					TEXT t;

					SetContextForeGroundColor (c);
					DrawOval (&r, 1);

					SetContextFont (TinyFont);
					
					t.baseline.x = r.corner.x + (r.extent.width >> 1);
					t.baseline.y = r.corner.y + (r.extent.height + (8 * RESOLUTION_FACTOR)) - 1; // JMS_GFX
					t.align = ALIGN_CENTER;
					t.CharCount = sizeof(sphere_strings[index]);
					t.pStr = (UNICODE *)sphere_strings[index];
					TextRect (&t, &r, NULL);

					if (r.corner.x <= 0)
						t.baseline.x -= r.corner.x - 1;
					else if (r.corner.x + r.extent.width >= SIS_SCREEN_WIDTH)
						t.baseline.x -= (r.corner.x + r.extent.width) - SIS_SCREEN_WIDTH + 1;
					if (r.corner.y <= 0)
						t.baseline.y -= r.corner.y - 1;
					else if (r.corner.y + r.extent.height >= SIS_SCREEN_HEIGHT)
						t.baseline.y -= (r.corner.y + r.extent.height)- SIS_SCREEN_HEIGHT + 1;
					
					{
						BYTE r, g, b;
						COLOR c32k;
						
						c32k = COLOR_32k (c);
						r = (BYTE)((c32k >> (5 * 2)) & 0x1F);
						if ((r += 0x03) > 0x1F) r = 0x1F;
						g = (BYTE)((c32k >> (5 * 1)) & 0x1F);
						if ((g += 0x03) > 0x1F) g = 0x1F;
						b = (BYTE)((c32k >> (5 * 0)) & 0x1F);
						if ((b += 0x03) > 0x1F) b = 0x1F;
		
						SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (r, g, b), COLOR_256 (c) - 1));
					}
					font_DrawText (&t);
				}
			}
		}
	}
}
