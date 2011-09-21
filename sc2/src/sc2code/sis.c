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

// JMS 2009: Shows *Below* in orz space in the SIS info screen

// JMS_GFX 2011: Merged the resolution Factor stuff from UQM-HD.

#include "colors.h"
#include "commglue.h"
#include "encount.h"
#include "gamestr.h"
#include "options.h"
#include "starbase.h"
#include "setup.h"
#include "state.h"
#include "libs/graphics/gfx_common.h"
#include "libs/tasklib.h"
#include "libs/log.h"

#include <stdio.h>

/**** 1/1-Scale blueprint
static const COUNT crew_lines_sizes[16] =
{
  4,4,3,3,2,1,1,1,2,2,8,7,7,1,2,2
};

static const COORD crew_lines_x_coords[16] = 
{
  93,92,91,90,90,89,90,89,86,85,72,73,73,68,67,67
};

static const COORD crew_lines_y_coords[16] = 
{
  29,27,25,23,21,19,31,29,27,25,23,21,19,23,21,19
};
****/

/**** Enlarged blueprint ****/
static const COUNT crew_lines_sizes[16] =
{
  4,3,3,2,2,2,1,1,1,2,9,8,7,2,2,1
};

static const COORD crew_lines_x_coords[16] = 
{
  90,89,88,87,86,85,84,84,83,81,66,67,68,60,61,63
};

static const COORD crew_lines_y_coords[16] = 
{
  20,18,16,14,12,10,8,18,16,14,12,10,8,12,10,8
};

static const UNICODE *describeWeapon (BYTE moduleType);

void
RepairSISBorder (void)
{
	RECT r;
	CONTEXT OldContext;

	OldContext = SetContext (ScreenContext);

	BatchGraphics ();
	r.corner.x = SIS_ORG_X - 1;
	r.corner.y = SIS_ORG_Y - 1;
	r.extent.width = 1;
	r.extent.height = SIS_SCREEN_HEIGHT + 2;
	SetContextForeGroundColor (
			BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19));
	DrawFilledRectangle (&r);

	SetContextForeGroundColor (
			BUILD_COLOR (MAKE_RGB15 (0x08, 0x08, 0x08), 0x1F));
	r.corner.x += (SIS_SCREEN_WIDTH + 2) - 1;
	DrawFilledRectangle (&r);

	r.corner.x = SIS_ORG_X - 1;
	r.corner.y += (SIS_SCREEN_HEIGHT + 2) - 1;
	r.extent.width = SIS_SCREEN_WIDTH + 2;
	r.extent.height = 1;
	DrawFilledRectangle (&r);
	UnbatchGraphics ();

	SetContext (OldContext);
}

void
ClearSISRect (BYTE ClearFlags)
{
	RECT r;
	COLOR OldColor;
	CONTEXT OldContext;

	OldContext = SetContext (StatusContext);
	OldColor = SetContextForeGroundColor (
			BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x0A), 0x08));

	r.corner.x = 2;
	r.extent.width = STATUS_WIDTH - 4;

	BatchGraphics ();
	if (ClearFlags & DRAW_SIS_DISPLAY)
	{
		DeltaSISGauges (UNDEFINED_DELTA, UNDEFINED_DELTA, UNDEFINED_DELTA);
	}

	if (ClearFlags & CLEAR_SIS_RADAR)
	{
		UnlockMutex (GraphicsLock);
		DrawMenuStateStrings ((BYTE)~0, 1);
		LockMutex (GraphicsLock);
#ifdef NEVER
		r.corner.x = RADAR_X - 1;
		r.corner.y = RADAR_Y - 1;
		r.extent.width = RADAR_WIDTH + 2;
		r.extent.height = RADAR_HEIGHT + 2;

		DrawStarConBox (&r, 1,
				BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19),
				BUILD_COLOR (MAKE_RGB15 (0x08, 0x08, 0x08), 0x1F),
				TRUE, BUILD_COLOR (MAKE_RGB15 (0x00, 0x0E, 0x00), 0x6C));
#endif /* NEVER */
	}
	UnbatchGraphics ();

	SetContextForeGroundColor (OldColor);
	SetContext (OldContext);
}

// The small blue box in the upper edge of the screen
void
DrawSISTitle (UNICODE *pStr)
{
	TEXT t;
	CONTEXT OldContext;
	RECT r;
	
	t.baseline.x = SIS_TITLE_WIDTH >> 1;
	t.baseline.y = SIS_TITLE_HEIGHT - RES_STAT_SCALE(2); // JMS_GFX
	t.align = ALIGN_CENTER;
	t.pStr = pStr;
	t.CharCount = (COUNT)~0;

	OldContext = SetContext (OffScreenContext);

	r.corner.x = SIS_ORG_X + SIS_SCREEN_WIDTH - SIS_TITLE_BOX_WIDTH + (1 << RESOLUTION_FACTOR); // JMS_GFX
	r.corner.y = SIS_ORG_Y - SIS_TITLE_HEIGHT;
	r.extent.width = SIS_TITLE_WIDTH;
	r.extent.height = SIS_TITLE_HEIGHT - RES_STAT_SCALE(1); // JMS_GFX
	SetContextFGFrame (Screen);
	SetContextClipRect (&r);
	
	SetContextFont (TinyFont);

	BatchGraphics ();
	SetContextBackGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
	ClearDrawable ();
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x1B), 0x33));
	font_DrawText (&t);
	UnbatchGraphics ();

	SetContextClipRect (NULL);

	SetContext (OldContext);
}

void
DrawHyperCoords (POINT universe)
{
	UNICODE buf[100];

	sprintf (buf, "%03u.%01u : %03u.%01u",
			universe.x / 10, universe.x % 10,
			universe.y / 10, universe.y % 10);

	DrawSISTitle (buf);
}

// The large blue box in the upper edge of the screen
void
DrawSISMessage (const UNICODE *pStr)
{
	DrawSISMessageEx (pStr, -1, -1, DSME_NONE);
}

BOOLEAN
DrawSISMessageEx (const UNICODE *pStr, SIZE CurPos, SIZE ExPos, COUNT flags)
{
	UNICODE buf[256];
	CONTEXT OldContext;
	TEXT t;
	RECT r;

	OldContext = SetContext (OffScreenContext);
	// prepare the context
	r.corner.x = SIS_ORG_X + 1;
	r.corner.y = SIS_ORG_Y - SIS_MESSAGE_HEIGHT;
	r.extent.width = SIS_MESSAGE_WIDTH;
	r.extent.height = SIS_MESSAGE_HEIGHT - RES_STAT_SCALE(1);
	SetContextFGFrame (Screen);
	SetContextClipRect (&r);
	
	BatchGraphics ();
	SetContextBackGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));

	if (pStr == (UNICODE *)~0L)
	{
		if (GLOBAL_SIS (FuelOnBoard) == 0)
			pStr = GAME_STRING (NAVIGATION_STRING_BASE + 2);
		else
			pStr = GAME_STRING (NAVIGATION_STRING_BASE + 3);
	}
	else
	{
		if (pStr == 0)
		{
			switch (LOBYTE (GLOBAL (CurrentActivity)))
			{
				default:
				case IN_ENCOUNTER:
					buf[0] = '\0';
					break;
				case IN_LAST_BATTLE:
				case IN_INTERPLANETARY:
					GetClusterName (CurStarDescPtr, buf);
					break;
				case IN_HYPERSPACE:
					if (GET_GAME_STATE (ORZ_SPACE_SIDE) > 1)
					{
						utf8StringCopy (buf, sizeof (buf), 
								GAME_STRING (NAVIGATION_STRING_BASE + 6));
						// JMS: "ORZ Space = * Below *"
					}
					else 
					{
						utf8StringCopy (buf, sizeof (buf),
							GAME_STRING (NAVIGATION_STRING_BASE
								+ (GET_GAME_STATE (ARILOU_SPACE_SIDE) <= 1
									? 0 : 1)));
					}
					break;
			}

			pStr = buf;
		}

		SetContextForeGroundColor (
				BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x1B), 0x33));
	}

	t.baseline.y = SIS_MESSAGE_HEIGHT - RES_STAT_SCALE(2); // JMS_GFX
	t.pStr = pStr;
	t.CharCount = utf8StringCount(pStr);
	SetContextFont (TinyFont);

	if (flags & DSME_CLEARFR)
	{
		SetFlashRect (NULL, (FRAME)0);
	}

	if (CurPos < 0 && ExPos < 0)
	{	// normal state
		ClearDrawable ();
		t.baseline.x = SIS_MESSAGE_WIDTH >> 1;
		t.align = ALIGN_CENTER;
		font_DrawText (&t);
	}
	else
	{	// editing state
		int i;
		RECT text_r;
		BYTE char_deltas[MAX_DESC_CHARS];
		BYTE *pchar_deltas;

		t.baseline.x = RES_STAT_SCALE(3);
		t.align = ALIGN_LEFT;

		TextRect (&t, &text_r, char_deltas);
		if (text_r.extent.width + t.baseline.x + 2 >= r.extent.width)
		{	// the text does not fit the input box size and so
			// will not fit when displayed later
			// disallow the change
			UnbatchGraphics ();
			SetContextClipRect (NULL);
			SetContext (OldContext);
			return (FALSE);
		}

		ClearDrawable ();

		if (CurPos >= 0 && CurPos <= t.CharCount)
		{	// calc and draw the cursor
			RECT cur_r = text_r;

			for (i = CurPos, pchar_deltas = char_deltas; i > 0; --i)
				cur_r.corner.x += (SIZE)*pchar_deltas++;
			if (CurPos < t.CharCount) /* end of line */
				--cur_r.corner.x;
			
			if (flags & DSME_BLOCKCUR)
			{	// Use block cursor for keyboardless systems
				if (CurPos == t.CharCount)
				{	// cursor at end-line -- use insertion point
					cur_r.extent.width = 1;
				}
				else if (CurPos + 1 == t.CharCount)
				{	// extra pixel for last char margin
					cur_r.extent.width = (SIZE)*pchar_deltas + 2;
				}
				else
				{	// normal mid-line char
					cur_r.extent.width = (SIZE)*pchar_deltas + 1;
				}
			}
			else
			{	// Insertion point cursor
				cur_r.extent.width = 1;
			}
			
			cur_r.corner.y = 0;
			cur_r.extent.height = r.extent.height;
			SetContextForeGroundColor (BLACK_COLOR);
			DrawFilledRectangle (&cur_r);
		}

		SetContextForeGroundColor (
				BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x1B), 0x33));

		if (ExPos >= 0 && ExPos < t.CharCount)
		{	// handle extra characters
			t.CharCount = ExPos;
			font_DrawText (&t);

			// print extra chars
			SetContextForeGroundColor (
					BUILD_COLOR (MAKE_RGB15 (0x12, 0x00, 0x12), 0x33));
			for (i = ExPos, pchar_deltas = char_deltas; i > 0; --i)
				t.baseline.x += (SIZE)*pchar_deltas++;
			t.pStr = skipUTF8Chars (t.pStr, ExPos);
			t.CharCount = (COUNT)~0;
			font_DrawText (&t);
		}
		else
		{	// just print the text
			font_DrawText (&t);
		}
	}

	if (flags & DSME_SETFR)
	{
		r.corner.x = r.corner.y = 0;
		SetFlashRect (&r, (FRAME)0);
	}

	UnbatchGraphics ();

	SetContextClipRect (NULL);
	SetContext (OldContext);

	return (TRUE);
}

void
DateToString (unsigned char *buf, size_t bufLen,
		BYTE month_index, BYTE day_index, COUNT year_index)
{
	snprintf (buf, bufLen, "%s %02d" STR_MIDDLE_DOT "%04d",
			GAME_STRING (MONTHS_STRING_BASE + month_index - 1),
			day_index, year_index);
}

// The green screen which contains date etc.
void
DrawStatusMessage (const UNICODE *pStr)
{
	RECT r;
	TEXT t;
	UNICODE buf[128];
	CONTEXT OldContext;

	OldContext = SetContext (StatusContext);
	GetContextClipRect (&r);
	SetContext (OffScreenContext);
	SetContextFGFrame (Screen);
	r.corner.x += RES_STAT_SCALE(2); // JMS_GFX
	r.corner.y += RES_STAT_SCALE(130); // JMS_GFX
	r.extent.width = STATUS_MESSAGE_WIDTH;
	r.extent.height = STATUS_MESSAGE_HEIGHT;
	SetContextClipRect (&r);

	BatchGraphics ();
	SetContextBackGroundColor (
			BUILD_COLOR (MAKE_RGB15 (0x00, 0x08, 0x00), 0x6E));
	ClearDrawable ();

	if (pStr == (UNICODE *)~0)
	{
		if (pMenuState == 0
				&& CommData.ConversationPhrases /* Melnorme shenanigans */
				&& cur_comm == MELNORME_CONVERSATION)
			sprintf (buf, "%u %s", MAKE_WORD (
					GET_GAME_STATE (MELNORME_CREDIT0),
					GET_GAME_STATE (MELNORME_CREDIT1)
					), GAME_STRING (STATUS_STRING_BASE + 0)); // "Cr"
		else if (GET_GAME_STATE (CHMMR_BOMB_STATE) < 2)
			sprintf (buf, "%u %s", GLOBAL_SIS (ResUnits),
					GAME_STRING (STATUS_STRING_BASE + 1)); // "RU"
		else
			sprintf (buf, "%s %s",
					(optWhichMenu == OPT_PC) ?
						GAME_STRING (STATUS_STRING_BASE + 2)
						: STR_INFINITY_SIGN, // "UNLIMITED"
					GAME_STRING (STATUS_STRING_BASE + 1)); // "RU"
		pStr = buf;
	}
	else if (pStr == 0)
	{
		DateToString (buf, sizeof buf,
				GLOBAL (GameClock.month_index),
				GLOBAL (GameClock.day_index),
				GLOBAL (GameClock.year_index));
		pStr = buf;
	}

	t.baseline.x = STATUS_MESSAGE_WIDTH >> 1;
	t.baseline.y = STATUS_MESSAGE_HEIGHT - (1 << RESOLUTION_FACTOR) - RESOLUTION_FACTOR; // JMS_GFX
	t.align = ALIGN_CENTER;
	t.pStr = pStr;
	t.CharCount = (COUNT)~0;

	SetContextFont (TinyFont);
	SetContextForeGroundColor (
			BUILD_COLOR (MAKE_RGB15 (0x00, 0x10, 0x00), 0x6B));
	font_DrawText (&t);
	UnbatchGraphics ();

	SetContextClipRect (NULL);

	SetContext (OldContext);
}

void
DrawCaptainsName (void)
{
	RECT r;
	TEXT t;
	CONTEXT OldContext;
	FONT OldFont;
	COLOR OldColor;

	OldContext = SetContext (StatusContext);
	OldFont = SetContextFont (TinyFont);
	OldColor = SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));

	// Background of captain's name.	
	r.corner.x = RES_STAT_SCALE(2 + 1);						// JMS_GFX;
	r.corner.y = RES_STAT_SCALE(10);						// JMS_GFX
	r.extent.width = SHIP_NAME_WIDTH - RES_STAT_SCALE(2);	// JMS_GFX
	r.extent.height = SHIP_NAME_HEIGHT;						// JMS_GFX
	DrawFilledRectangle (&r);

	// The name itself.
	t.baseline.x = (STATUS_WIDTH >> 1) - RES_STAT_SCALE(1);
	t.baseline.y = r.corner.y + RES_STAT_SCALE(6); // JMS_GFX
	t.align = ALIGN_CENTER;
	t.pStr = GLOBAL_SIS (CommanderName);
	t.CharCount = (COUNT)~0;
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x16, 0x0B, 0x1F), 0x38));
	font_DrawText (&t);

	SetContextForeGroundColor (OldColor);
	SetContextFont (OldFont);
	SetContext (OldContext);
}

void
DrawFlagshipName (BOOLEAN InStatusArea)
{
	RECT r;
	TEXT t;
	FONT OldFont;
	COLOR OldColor;
	CONTEXT OldContext;
	FRAME OldFontEffect;
	UNICODE buf[250];

	if (InStatusArea)
	{
		OldContext = SetContext (StatusContext);
		OldFont = SetContextFont (StarConFont);

		r.corner.x = RES_STAT_SCALE(2);		// JMS_GFX
		r.corner.y = RES_STAT_SCALE(20);	// JMS_GFX
		r.extent.width = SHIP_NAME_WIDTH;	// JMS_GFX
		r.extent.height = SHIP_NAME_HEIGHT;

		t.pStr = GLOBAL_SIS (ShipName);
	}
	else
	{
		OldContext = SetContext (SpaceContext);
		OldFont = SetContextFont (MicroFont);

		r.corner.x = 0;
		r.corner.y = 1;
		r.extent.width = SIS_SCREEN_WIDTH;
		r.extent.height = SHIP_NAME_HEIGHT;

		t.pStr = buf;
		sprintf (buf, "%s %s", GAME_STRING (NAMING_STRING_BASE + 1), GLOBAL_SIS (ShipName));
		// XXX: this will not work with UTF-8 strings
		strupr (buf);
	}
	OldFontEffect = SetContextFontEffect (NULL);
	OldColor = SetContextForeGroundColor (BLACK_COLOR);
	DrawFilledRectangle (&r);

	t.baseline.x = r.corner.x + (r.extent.width >> 1);
	t.baseline.y = r.corner.y + (SHIP_NAME_HEIGHT - (InStatusArea ? RES_STAT_SCALE(1) : 0)); // JMS_GFX
	t.align = ALIGN_CENTER;
	t.CharCount = (COUNT)~0;
	if (optWhichFonts == OPT_PC)
		SetContextFontEffect (SetAbsFrameIndex (FontGradFrame,
				InStatusArea ? 0 : 3));
	else
		SetContextForeGroundColor (
				BUILD_COLOR (MAKE_RGB15 (0x14, 0x0A, 0x00), 0x0C));

	font_DrawText (&t);

	SetContextFontEffect (OldFontEffect);
	SetContextForeGroundColor (OldColor);
	SetContextFont (OldFont);
	SetContext (OldContext);
}

void
DrawFlagshipStats (void)
{
	RECT r;
	TEXT t;
	FONT OldFont;
	COLOR OldColor;
	FRAME OldFontEffect;
	CONTEXT OldContext;
	UNICODE buf[128];
	SIZE leading;
	BYTE i;
	BYTE energy_regeneration, energy_wait, turn_wait;
	COUNT max_thrust;
	DWORD fuel;

	/* collect stats */
#define ENERGY_REGENERATION 1
#define ENERGY_WAIT 10
#define MAX_THRUST 10
#define TURN_WAIT 17
	energy_regeneration = ENERGY_REGENERATION;
	energy_wait = ENERGY_WAIT;
	max_thrust = MAX_THRUST;
	turn_wait = TURN_WAIT;


	if (GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS) == CHMMR_EXPLORER_SHIP)
	{
	    fuel = EXPLORER_FUEL_CAPACITY;
	}
	else
	{
	    fuel = 10 * FUEL_TANK_SCALE;
	    
	    for (i = 0; i < NUM_MODULE_SLOTS; i++)
		{
			switch (GLOBAL_SIS (ModuleSlots[i])) 
			{
				case FUEL_TANK:
					fuel += FUEL_TANK_CAPACITY;
					break;
				case HIGHEFF_FUELSYS:
					fuel += HEFUEL_TANK_CAPACITY;
					break;
				case DYNAMO_UNIT:
					energy_wait -= 2;
					if (energy_wait < 4)
						energy_wait = 4;
					break;
				case SHIVA_FURNACE:
					energy_regeneration++;
					break;
			}
		}
	}

	for (i = 0; i < NUM_DRIVE_SLOTS; ++i)
		if (GLOBAL_SIS (DriveSlots[i]) == FUSION_THRUSTER)
			max_thrust += 2;

	for (i = 0; i < NUM_JET_SLOTS; ++i)
		if (GLOBAL_SIS (JetSlots[i]) == TURNING_JETS)
			turn_wait -= 2;
	/* END collect stats */

	OldContext = SetContext (SpaceContext);
	OldFont = SetContextFont (StarConFont);
	OldFontEffect = SetContextFontEffect (NULL);
	GetContextFontLeading (&leading);

	/* we need room to play.  full screen width, 4 lines tall */
	r.corner.x = 0;
	r.corner.y = SIS_SCREEN_HEIGHT - (4 * leading);
	r.extent.width = SIS_SCREEN_WIDTH;
	r.extent.height = (4 * leading);

	OldColor = SetContextForeGroundColor (BLACK_COLOR);
	DrawFilledRectangle (&r);

	/*
	   now that we've cleared out our playground, compensate for the
	   fact that the leading is way more than is generally needed.
	*/
	leading -= 3;
	t.baseline.x = SIS_SCREEN_WIDTH / 6; //wild-assed guess, but it worked
	t.baseline.y = r.corner.y + leading + 3;
	t.align = ALIGN_RIGHT;
	t.CharCount = (COUNT)~0;

	SetContextFontEffect (SetAbsFrameIndex (FontGradFrame, 4));

	t.pStr = GAME_STRING (FLAGSHIP_STRING_BASE + 0); // "nose:"
	font_DrawText (&t);
	t.baseline.y += leading;
	t.pStr = GAME_STRING (FLAGSHIP_STRING_BASE + 1); // "spread:"
	font_DrawText (&t);
	t.baseline.y += leading;
	t.pStr = GAME_STRING (FLAGSHIP_STRING_BASE + 2); // "side:"
	font_DrawText (&t);
	t.baseline.y += leading;
	t.pStr = GAME_STRING (FLAGSHIP_STRING_BASE + 3); // "tail:"
	font_DrawText (&t);

	t.baseline.x += 5;
	t.baseline.y = r.corner.y + leading + 3;
	t.align = ALIGN_LEFT;
	t.pStr = buf;

	sprintf (buf, "%-7.7s", describeWeapon (GLOBAL_SIS (ModuleSlots[15])));
	font_DrawText (&t);
	t.baseline.y += leading;
	sprintf (buf, "%-7.7s", describeWeapon (GLOBAL_SIS (ModuleSlots[14])));
	font_DrawText (&t);
	t.baseline.y += leading;
	sprintf (buf, "%-7.7s", describeWeapon (GLOBAL_SIS (ModuleSlots[13])));
	font_DrawText (&t);
	t.baseline.y += leading;
	sprintf (buf, "%-7.7s", describeWeapon (GLOBAL_SIS (ModuleSlots[0])));
	font_DrawText (&t);

	t.baseline.x = r.extent.width - 25;
	t.baseline.y = r.corner.y + leading + 3;
	t.align = ALIGN_RIGHT;

	SetContextFontEffect (SetAbsFrameIndex (FontGradFrame, 5));

	t.pStr = GAME_STRING (FLAGSHIP_STRING_BASE + 4); // "maximum velocity:"
	font_DrawText (&t);
	t.baseline.y += leading;
	t.pStr = GAME_STRING (FLAGSHIP_STRING_BASE + 5); // "turning rate:"
	font_DrawText (&t);
	t.baseline.y += leading;
	t.pStr = GAME_STRING (FLAGSHIP_STRING_BASE + 6); // "combat energy:"
	font_DrawText (&t);
	t.baseline.y += leading;
	t.pStr = GAME_STRING (FLAGSHIP_STRING_BASE + 7); // "maximum fuel:"
	font_DrawText (&t);

	t.baseline.x = r.extent.width - 2;
	t.baseline.y = r.corner.y + leading + 3;
	t.pStr = buf;

	sprintf (buf, "%4u", max_thrust * 4);
	font_DrawText (&t);
	t.baseline.y += leading;
	sprintf (buf, "%4u", 1 + TURN_WAIT - turn_wait);
	font_DrawText (&t);
	t.baseline.y += leading;
	{
		unsigned int energy_per_10_sec =
				(((100 * ONE_SECOND * energy_regeneration) /
				((1 + energy_wait) * BATTLE_FRAME_RATE)) + 5) / 10;
		sprintf (buf, "%2u.%1u",
				energy_per_10_sec / 10,
				energy_per_10_sec % 10);
	}
	font_DrawText (&t);
	t.baseline.y += leading;
	sprintf (buf, "%4u", (fuel / FUEL_TANK_SCALE));
	font_DrawText (&t);

	SetContextFontEffect (OldFontEffect);
	SetContextForeGroundColor (OldColor);
	SetContextFont (OldFont);
	SetContext (OldContext);
}

static const UNICODE *
describeWeapon (BYTE moduleType)
{
	switch (moduleType)
	{
		case GUN_WEAPON:
			return GAME_STRING (FLAGSHIP_STRING_BASE + 8); // "gun"
		case BLASTER_WEAPON:
			return GAME_STRING (FLAGSHIP_STRING_BASE + 9); // "blaster"
		case CANNON_WEAPON:
			return GAME_STRING (FLAGSHIP_STRING_BASE + 10); // "cannon"
		case BOMB_MODULE_0:
		case BOMB_MODULE_1:
		case BOMB_MODULE_2:
		case BOMB_MODULE_3:
		case BOMB_MODULE_4:
		case BOMB_MODULE_5:
			return GAME_STRING (FLAGSHIP_STRING_BASE + 11); // "n/a"
		default:
			return GAME_STRING (FLAGSHIP_STRING_BASE + 12); // "none"
	}
}

void
DrawLanders (void)
{
	BYTE i;
	SIZE width;
	RECT r;
	STAMP s;
	CONTEXT OldContext;

	OldContext = SetContext (StatusContext);

	s.frame = IncFrameIndex (FlagStatFrame);
	GetFrameRect (s.frame, &r);

	i = GLOBAL_SIS (NumLanders);
	r.corner.x = (STATUS_WIDTH >> 1) - r.corner.x;
	s.origin.x = r.corner.x - (((r.extent.width * i) + (2 * (i - 1))) >> 1);
	s.origin.y = RES_STAT_SCALE(29); // JMS_GFX

	width = r.extent.width + 2;
	r.extent.width = (r.extent.width * MAX_LANDERS) + (2 * (MAX_LANDERS - 1)) + 2;
	r.corner.x -= r.extent.width >> 1;
	r.corner.y += s.origin.y;
	SetContextForeGroundColor (BLACK_COLOR);
	DrawFilledRectangle (&r);
	while (i--)
	{
		DrawStamp (&s);
		s.origin.x += width;
	}

	SetContext (OldContext);
}

void
DrawStorageBays (BOOLEAN Refresh)
{
	BYTE i;
	RECT r;
	CONTEXT OldContext;

	OldContext = SetContext (StatusContext);

	r.extent.width = RES_STAT_SCALE(2); // JMS_GFX
	r.extent.height = RES_STAT_SCALE(4); // JMS_GFX
	r.corner.y = RES_STAT_SCALE(123); // JMS_GFX
	if (Refresh)
	{
		r.extent.width = NUM_MODULE_SLOTS * (r.extent.width + 1);
		r.corner.x = (STATUS_WIDTH >> 1) - (r.extent.width >> 1);

		SetContextForeGroundColor (BLACK_COLOR);
		DrawFilledRectangle (&r);
		r.extent.width = RES_STAT_SCALE(2); // JMS_GFX
	}

	i = (BYTE)CountSISPieces (STORAGE_BAY);
	if (i)
	{
		COUNT j;

		r.corner.x = (STATUS_WIDTH >> 1)
				- ((i * (r.extent.width + 1)) >> 1);
		SetContextForeGroundColor (
				BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x0A), 0x08));
		for (j = GLOBAL_SIS (TotalElementMass);
				j >= STORAGE_BAY_CAPACITY; j -= STORAGE_BAY_CAPACITY)
		{
			DrawFilledRectangle (&r);
			r.corner.x += r.extent.width + RES_STAT_SCALE(1); // JMS_GFX;

			--i;
		}

		r.extent.height = (4 * j + (STORAGE_BAY_CAPACITY - 1)) / STORAGE_BAY_CAPACITY;
		if (r.extent.height)
		{
			r.corner.y += RES_STAT_SCALE(4 - r.extent.height);
			DrawFilledRectangle (&r);
			r.extent.height = RES_STAT_SCALE(4 - r.extent.height);
			if (r.extent.height)
			{
				r.corner.y = RES_STAT_SCALE(123);
				SetContextForeGroundColor (
						BUILD_COLOR (MAKE_RGB15 (0x06, 0x06, 0x06), 0x20));
				DrawFilledRectangle (&r);
			}
			r.corner.x += r.extent.width + 1;

			--i;
		}
		r.extent.height = RES_STAT_SCALE(4);

		SetContextForeGroundColor (
				BUILD_COLOR (MAKE_RGB15 (0x06, 0x06, 0x06), 0x20));
		while (i--)
		{
			DrawFilledRectangle (&r);
			r.corner.x += r.extent.width + 1;
		}
	}

	SetContext (OldContext);
}

void
GetGaugeRect (RECT *pRect, BOOLEAN IsCrewRect)
{
	pRect->extent.width = RES_STAT_SCALE(24); // JMS_GFX
	pRect->corner.x = (STATUS_WIDTH >> 1) - (pRect->extent.width >> 1);
	pRect->extent.height = RES_STAT_SCALE(5); // JMS_GFX
	pRect->corner.y = RES_STAT_SCALE((IsCrewRect ? 117 : 38) + RESOLUTION_FACTOR); // JMS_GFX
}

static void
DrawPC_SIS (void)
{
	TEXT t;
	RECT r;

	GetGaugeRect (&r, FALSE);
	t.baseline.x = STATUS_WIDTH >> 1;
	t.baseline.y = r.corner.y - 1; // JMS_GFX
	t.align = ALIGN_CENTER;
	t.CharCount = (COUNT)~0;
	SetContextFont (TinyFont);
	SetContextForeGroundColor (BLACK_COLOR);

	r.corner.y -= RES_STAT_SCALE(6); // JMS_GFX
	r.corner.x -= RES_STAT_SCALE(1); // JMS_GFX
	r.extent.width += RES_STAT_SCALE(2);
	DrawFilledRectangle (&r);

	SetContextFontEffect (SetAbsFrameIndex (FontGradFrame, 1));
	t.pStr = GAME_STRING (STATUS_STRING_BASE + 3); // "FUEL"
	font_DrawText (&t);

	r.corner.y += RES_STAT_SCALE(79); // JMS_GFX
	t.baseline.y += RES_STAT_SCALE(79); // JMS_GFX
	DrawFilledRectangle (&r);

	SetContextFontEffect (SetAbsFrameIndex (FontGradFrame, 2));
	t.pStr = GAME_STRING (STATUS_STRING_BASE + 4); // "CREW"
	font_DrawText (&t);
	SetContextFontEffect (NULL);

	// Background of text "CAPTAIN".
	r.corner.x = (2 + 1) << RESOLUTION_FACTOR; // JMS_GFX;
	r.corner.y = RES_STAT_SCALE(3); // JMS_GFX
	r.extent.width = RES_STAT_SCALE(58); // JMS_GFX
	r.extent.height = RES_STAT_SCALE(7); // JMS_GFX
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
	DrawFilledRectangle (&r);
	
	// Text "CAPTAIN".
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x02, 0x04, 0x1E), 0x38));
	t.baseline.y = r.corner.y + RES_STAT_SCALE(6); // JMS_GFX
	t.pStr = GAME_STRING (STATUS_STRING_BASE + 5); // "CAPTAIN"
	font_DrawText (&t);
}

void
DeltaSISGauges (SIZE crew_delta, SIZE fuel_delta, int resunit_delta)
{
	STAMP s;
	RECT r;
	TEXT t;
	UNICODE buf[60];
	CONTEXT OldContext;

	if (crew_delta == 0 && fuel_delta == 0 && resunit_delta == 0)
		return;

	OldContext = SetContext (StatusContext);

	BatchGraphics ();
	if (crew_delta == UNDEFINED_DELTA)
	{
		COUNT i;

		s.origin.x = s.origin.y = 0;
		s.frame = FlagStatFrame;
		DrawStamp (&s);
		if (optWhichFonts == OPT_PC)
			DrawPC_SIS();

	    if (GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS) == CHMMR_EXPLORER_SHIP)
		{
		}
	    else
		{
			s.origin.x = 1;
			s.origin.y = 0;
			for (i = 0; i < NUM_DRIVE_SLOTS; ++i)
			{
				BYTE which_piece;

				if ((which_piece = GLOBAL_SIS (DriveSlots[i])) < EMPTY_SLOT)
				{
					s.frame = SetAbsFrameIndex (FlagStatFrame, which_piece + 1 + 0);
					DrawStamp (&s);
					s.frame = IncFrameIndex (s.frame);
					DrawStamp (&s);
				}

				s.origin.y -= RES_STAT_SCALE(3); // JMS_GFX
			}
			s.origin.y = 0;
			for (i = 0; i < NUM_JET_SLOTS; ++i)
			{
				BYTE which_piece;

				if ((which_piece = GLOBAL_SIS (JetSlots[i])) < EMPTY_SLOT)
				{
					s.frame = SetAbsFrameIndex (FlagStatFrame, which_piece + 1 + 1);
					DrawStamp (&s);
					s.frame = IncFrameIndex (s.frame);
					DrawStamp (&s);
				}

				s.origin.y -= RES_STAT_SCALE(3); // JMS_GFX
			}
			s.origin.y = 1;
			s.origin.x = 1; // This properly centers the modules.
			  
			for (i = 0; i < NUM_MODULE_SLOTS; ++i)
			{
				BYTE which_piece;

				if ((which_piece = GLOBAL_SIS (ModuleSlots[i])) < EMPTY_SLOT)
				{
					s.frame = SetAbsFrameIndex (FlagStatFrame, which_piece + 1 + 2);
					DrawStamp (&s);
				}

				s.origin.y -= RES_STAT_SCALE(3); // JMS_GFX
			}
		}

		{
			HSHIPFRAG hStarShip, hNextShip;
			POINT *pship_pos;
			POINT ship_pos[MAX_COMBAT_SHIPS] =
			{
				SUPPORT_SHIP_PTS
			};

			UnlockMutex (GraphicsLock);
			for (hStarShip = GetHeadLink (&GLOBAL (built_ship_q)),
					pship_pos = ship_pos;
					hStarShip; hStarShip = hNextShip, ++pship_pos)
			{
				SHIP_FRAGMENT *StarShipPtr;

				StarShipPtr = LockShipFrag (&GLOBAL (built_ship_q), hStarShip);
				hNextShip = _GetSuccLink (StarShipPtr);

				s.origin.x = pship_pos->x;
				s.origin.y = pship_pos->y;
				s.frame = StarShipPtr->icons;
				LockMutex (GraphicsLock);
				DrawStamp (&s);
				UnlockMutex (GraphicsLock);

				UnlockShipFrag (&GLOBAL (built_ship_q), hStarShip);
			}
			LockMutex (GraphicsLock);
		}
	}

	t.baseline.x = STATUS_WIDTH >> 1;
	t.align = ALIGN_CENTER;
	t.pStr = buf;
	SetContextFont (TinyFont);

	if (crew_delta != 0)
	{
		if (crew_delta != UNDEFINED_DELTA)
		{
			COUNT CrewCapacity;

			if (crew_delta < 0 && GLOBAL_SIS (CrewEnlisted) <= (COUNT)-crew_delta)
				GLOBAL_SIS (CrewEnlisted) = 0;
			else if ((GLOBAL_SIS (CrewEnlisted) += crew_delta) > (CrewCapacity = GetCPodCapacity (NULL)))
				GLOBAL_SIS (CrewEnlisted) = CrewCapacity;
		}

		sprintf (buf, "%u", GLOBAL_SIS (CrewEnlisted));
		GetGaugeRect (&r, TRUE);
		t.baseline.y = r.corner.y + r.extent.height; // JMS_GFX
		t.CharCount = (COUNT)~0;
		SetContextForeGroundColor (BLACK_COLOR);
		DrawFilledRectangle (&r);
		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x0E, 0x00), 0x6C));
		font_DrawText (&t);
	}

	if (fuel_delta != 0)
	{
		COUNT old_coarse_fuel, new_coarse_fuel;

		if (fuel_delta == UNDEFINED_DELTA)
			old_coarse_fuel = (COUNT)~0;
		else
		{
			DWORD FuelCapacity;

			old_coarse_fuel = (COUNT)(GLOBAL_SIS (FuelOnBoard) / FUEL_TANK_SCALE);
			if (fuel_delta < 0
					&& GLOBAL_SIS (FuelOnBoard) <= (DWORD)-fuel_delta)
				GLOBAL_SIS (FuelOnBoard) = 0;
			else if ((GLOBAL_SIS (FuelOnBoard) += fuel_delta) >
					(FuelCapacity = GetFTankCapacity (NULL)))
				GLOBAL_SIS (FuelOnBoard) = FuelCapacity;
		}

		new_coarse_fuel = (COUNT)(
				GLOBAL_SIS (FuelOnBoard) / FUEL_TANK_SCALE
				);
		if (new_coarse_fuel != old_coarse_fuel)
		{
			sprintf (buf, "%u", new_coarse_fuel);
			GetGaugeRect (&r, FALSE);
			t.baseline.y = r.corner.y + r.extent.height; // JMS_GFX
			t.CharCount = (COUNT)~0;
			SetContextForeGroundColor (BLACK_COLOR);
			DrawFilledRectangle (&r);
			SetContextForeGroundColor (
					BUILD_COLOR (MAKE_RGB15 (0x13, 0x00, 0x00), 0x2C));
			font_DrawText (&t);
		}
	}

	if (crew_delta == UNDEFINED_DELTA)
	{
		DrawFlagshipName (TRUE);
		DrawCaptainsName ();
		DrawLanders ();
		DrawStorageBays (FALSE);
	}

	if (resunit_delta != 0)
	{
		if (resunit_delta != UNDEFINED_DELTA)
		{
			if (resunit_delta < 0 && GLOBAL_SIS (ResUnits) <= (DWORD)-resunit_delta)
				GLOBAL_SIS (ResUnits) = 0;
			else
				GLOBAL_SIS (ResUnits) += resunit_delta;

			DrawStatusMessage ((UNICODE *)~0);
		}
		else
		{
			r.corner.x = RES_STAT_SCALE(2); // JMS_GFX
			r.corner.y = RES_STAT_SCALE(130); // JMS_GFX
			r.extent.width = STATUS_MESSAGE_WIDTH;
			r.extent.height = STATUS_MESSAGE_HEIGHT;
			SetContextForeGroundColor (
					BUILD_COLOR (MAKE_RGB15 (0x00, 0x08, 0x00), 0x6E));
			DrawFilledRectangle (&r);

			if ((pMenuState == 0
					&& CommData.ConversationPhrases /* Melnorme shenanigans */
					&& cur_comm == MELNORME_CONVERSATION)
					|| (pMenuState
					&& (pMenuState->InputFunc == DoStarBase
					|| pMenuState->InputFunc == DoOutfit
					|| pMenuState->InputFunc == DoShipyard)))
				DrawStatusMessage ((UNICODE *)~0);
			else
				DrawStatusMessage (NULL);
		}
	}
	UnbatchGraphics ();

	SetContext (OldContext);
}

COUNT
GetCrewCount (void)
{
	return (GLOBAL_SIS (CrewEnlisted));
}


// BW: New function for populating the Explorer
COUNT
GetCPodCapacity (POINT *ppt)
{
  int i;
  COUNT line_remainder;
  COORD x,y;

  line_remainder = GLOBAL_SIS (CrewEnlisted) + 1;
  i=0;
  
  while (line_remainder > crew_lines_sizes[i])
    {
      line_remainder -= crew_lines_sizes[i];
      i++;
    }

  x = crew_lines_x_coords[i];
  y = crew_lines_y_coords[i];
  
  if (ppt)
    {
      static const COLOR crew_rows[] =
	{
	  BUILD_COLOR (MAKE_RGB15 (0x00, 0x08, 0x00), 0x65),
	  BUILD_COLOR (MAKE_RGB15 (0x00, 0x0C, 0x00), 0x65),
	  BUILD_COLOR (MAKE_RGB15 (0x00, 0x10, 0x00), 0x65),
	  BUILD_COLOR (MAKE_RGB15 (0x00, 0x15, 0x00), 0x65),
	  BUILD_COLOR (MAKE_RGB15 (0x00, 0x19, 0x00), 0x65),
	  BUILD_COLOR (MAKE_RGB15 (0x00, 0x1D, 0x00), 0x65),
	  BUILD_COLOR (MAKE_RGB15 (0x0A, 0x1E, 0x09), 0x65),
	};
      
      ppt->x = x + 2 * line_remainder - 1;
      ppt->y = y + 6;
      
      if (optWhichFonts == OPT_PC)
	SetContextForeGroundColor (crew_rows[(y-8)/2]);
      else
	SetContextForeGroundColor (
				   BUILD_COLOR (MAKE_RGB15 (0x05, 0x10, 0x05), 0x65)
				   );
    }
  return (EXPLORER_CREW_CAPACITY);
}

/*** Old function for Vindicator
COUNT
GetCPodCapacity (POINT *ppt)
{
	COORD x;
	COUNT slot, capacity;

	x = 207;
	capacity = 0;
	slot = NUM_MODULE_SLOTS - 1;
	do
	{
		if (GLOBAL_SIS (ModuleSlots[slot]) == CREW_POD)
		{
			if (ppt
					&& capacity <= GLOBAL_SIS (CrewEnlisted)
					&& capacity + CREW_POD_CAPACITY >
					GLOBAL_SIS (CrewEnlisted))
			{
				COUNT pod_remainder, which_row;
				static const COLOR crew_rows[] =
				{
					 BUILD_COLOR (MAKE_RGB15 (0x0A, 0x1E, 0x09), 0x65),
					 BUILD_COLOR (MAKE_RGB15 (0x00, 0x1E, 0x00), 0x65),
					 BUILD_COLOR (MAKE_RGB15 (0x00, 0x1B, 0x00), 0x65),
					 BUILD_COLOR (MAKE_RGB15 (0x00, 0x18, 0x00), 0x65),
					 BUILD_COLOR (MAKE_RGB15 (0x00, 0x15, 0x00), 0x65),
					 BUILD_COLOR (MAKE_RGB15 (0x00, 0x12, 0x00), 0x65),
					 BUILD_COLOR (MAKE_RGB15 (0x00, 0x10, 0x00), 0x65),
					 BUILD_COLOR (MAKE_RGB15 (0x00, 0x0D, 0x00), 0x65),
					 BUILD_COLOR (MAKE_RGB15 (0x00, 0x0A, 0x00), 0x65),
					 BUILD_COLOR (MAKE_RGB15 (0x00, 0x07, 0x00), 0x65),
				};

				pod_remainder = GLOBAL_SIS (CrewEnlisted) - capacity;

				ppt->x = x - ((pod_remainder % CREW_PER_ROW) << 1);
				which_row = pod_remainder / CREW_PER_ROW;
				ppt->y = 34 - (which_row << 1);

				if (optWhichFonts == OPT_PC)
					SetContextForeGroundColor (crew_rows[which_row]);
				else
					SetContextForeGroundColor (
							BUILD_COLOR (MAKE_RGB15 (0x05, 0x10, 0x05), 0x65)
					);
			}

			capacity += CREW_POD_CAPACITY;
		}

		x -= SHIP_PIECE_OFFSET;
	} while (slot--);

	return (capacity);
}
***/

COUNT
GetSBayCapacity (POINT *ppt)
{
	COORD x;
	COUNT slot, capacity;

	if (GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS) == CHMMR_EXPLORER_SHIP)
	    x = 39;
	else
	    x = 207 - 8;
	
	capacity = 0;
	slot = NUM_MODULE_SLOTS - 1;

	if (GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS) != CHMMR_EXPLORER_SHIP)
	{
	    do
		{
			if (GLOBAL_SIS (ModuleSlots[slot]) == STORAGE_BAY)
			{
				if (ppt
						&& capacity < GLOBAL_SIS (TotalElementMass)
						&& capacity + STORAGE_BAY_CAPACITY >=
						GLOBAL_SIS (TotalElementMass))
				{
					COUNT bay_remainder, which_row;
					static const COLOR color_bars[] =
					{
					 BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F),
					 BUILD_COLOR (MAKE_RGB15 (0x1C, 0x1C, 0x1C), 0x11),
					 BUILD_COLOR (MAKE_RGB15 (0x18, 0x18, 0x18), 0x13),
					 BUILD_COLOR (MAKE_RGB15 (0x15, 0x15, 0x15), 0x15),
					 BUILD_COLOR (MAKE_RGB15 (0x12, 0x12, 0x12), 0x17),
					 BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19),
					 BUILD_COLOR (MAKE_RGB15 (0x0D, 0x0D, 0x0D), 0x1B),
					 BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x0A), 0x1D),
					 BUILD_COLOR (MAKE_RGB15 (0x08, 0x08, 0x08), 0x1F),
					 BUILD_COLOR (MAKE_RGB15 (0x05, 0x05, 0x05), 0x21),
					};

					bay_remainder = GLOBAL_SIS (TotalElementMass) - capacity;
					if ((which_row = bay_remainder / SBAY_MASS_PER_ROW) == 0)
						SetContextForeGroundColor (BLACK_COLOR);
					else
						SetContextForeGroundColor (color_bars[--which_row]);

					ppt->x = x;

				if (GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS) == CHMMR_EXPLORER_SHIP)
				{
				    ppt->y = 27 - which_row;
				    if (which_row == 9)
				      {
					ppt->x = x + 1;
				      }
				  }
				else
				  {
				    ppt->y = 34 - (which_row << 1);
				  }
			}

			capacity += STORAGE_BAY_CAPACITY;
		}

		x -= SHIP_PIECE_OFFSET;
	      } while (slot--);
	}
	else
	{		
	}

	return (capacity);
}


// BW: new function for fueling the Explorer
DWORD
GetFTankCapacity (POINT *ppt)
{
	COORD x;
	DWORD capacity;
	DWORD volume = EXPLORER_FUEL_CAPACITY;
	
	x = 200;
	//	capacity = FUEL_RESERVE;
	capacity = 0;
	
	if (ppt)
	  {
	    COUNT which_row;
	    static const COLOR fuel_colors[] =
	      {
		BUILD_COLOR (MAKE_RGB15 (0x0F, 0x00, 0x00), 0x2D),
		BUILD_COLOR (MAKE_RGB15 (0x13, 0x00, 0x00), 0x2C),
		BUILD_COLOR (MAKE_RGB15 (0x17, 0x00, 0x00), 0x2B),
		BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x00), 0x2A),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x03, 0x00), 0x7F),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x07, 0x00), 0x7E),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0A, 0x00), 0x7D),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0E, 0x00), 0x7C),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x11, 0x00), 0x7B),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x15, 0x00), 0x7A),
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x18, 0x00), 0x79),
	      };
	  
	    if (GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS) == CHMMR_EXPLORER_SHIP)
	      {
		which_row = (COUNT)(
				    (GLOBAL_SIS (FuelOnBoard))
				    * 20 / (EXPLORER_FUEL_CAPACITY)
				    );
		
		ppt->x = 31+which_row;
		ppt->y = 22;
		
		which_row = (COUNT)(
				    (GLOBAL_SIS (FuelOnBoard))
				    * MAX_FUEL_BARS / (EXPLORER_FUEL_CAPACITY)
				    );
		
	      }
	    else
	      {
		which_row = (COUNT)(
				    (GLOBAL_SIS (FuelOnBoard) - capacity)
				    * MAX_FUEL_BARS / HEFUEL_TANK_CAPACITY
				    );
		
		ppt->x = x + 1;
		if (volume == FUEL_TANK_CAPACITY)
		  ppt->y = 27 - which_row;
		else
		  ppt->y = 30 - which_row;
	      }
	    
	    SetContextForeGroundColor (fuel_colors[which_row]);
	    SetContextBackGroundColor (fuel_colors[which_row + 1]);
	  }
	
	capacity += volume;
	
	return (capacity);
}

/*** Old function for Vindicator
DWORD
GetFTankCapacity (POINT *ppt)
{
	COORD x;
	COUNT slot;
	DWORD capacity;

	x = 200;
	capacity = FUEL_RESERVE;
	slot = NUM_MODULE_SLOTS - 1;
	do
	{
		if (GLOBAL_SIS (ModuleSlots[slot]) == FUEL_TANK
				|| GLOBAL_SIS (ModuleSlots[slot]) == HIGHEFF_FUELSYS)
		{
			COUNT volume;

			volume = GLOBAL_SIS (ModuleSlots[slot]) == FUEL_TANK
					? FUEL_TANK_CAPACITY : HEFUEL_TANK_CAPACITY;
			if (ppt
					&& capacity <= GLOBAL_SIS (FuelOnBoard)
					&& capacity + volume >
					GLOBAL_SIS (FuelOnBoard))
			{
				COUNT which_row;
				static const COLOR fuel_colors[] =
				{
					BUILD_COLOR (MAKE_RGB15 (0x0F, 0x00, 0x00), 0x2D),
					BUILD_COLOR (MAKE_RGB15 (0x13, 0x00, 0x00), 0x2C),
					BUILD_COLOR (MAKE_RGB15 (0x17, 0x00, 0x00), 0x2B),
					BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x00), 0x2A),
					BUILD_COLOR (MAKE_RGB15 (0x1F, 0x03, 0x00), 0x7F),
					BUILD_COLOR (MAKE_RGB15 (0x1F, 0x07, 0x00), 0x7E),
					BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0A, 0x00), 0x7D),
					BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0E, 0x00), 0x7C),
					BUILD_COLOR (MAKE_RGB15 (0x1F, 0x11, 0x00), 0x7B),
					BUILD_COLOR (MAKE_RGB15 (0x1F, 0x15, 0x00), 0x7A),
					BUILD_COLOR (MAKE_RGB15 (0x1F, 0x18, 0x00), 0x79),
				};

			      if (GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS) == CHMMR_EXPLORER_SHIP)
				{
					which_row = (COUNT)(
							(GLOBAL_SIS (FuelOnBoard))
							* 20 / (HEFUEL_TANK_CAPACITY*2)
							);

					ppt->x = 29+which_row;
					ppt->y = 22;

					which_row = (COUNT)(
							(GLOBAL_SIS (FuelOnBoard))
							* MAX_FUEL_BARS / (HEFUEL_TANK_CAPACITY*2)
							);

					volume = HEFUEL_TANK_CAPACITY*2;
				}
				else
				{
					which_row = (COUNT)(
							(GLOBAL_SIS (FuelOnBoard) - capacity)
							* MAX_FUEL_BARS / HEFUEL_TANK_CAPACITY
							);

					ppt->x = x + 1;
					if (volume == FUEL_TANK_CAPACITY)
						ppt->y = 27 - which_row;
					else
						ppt->y = 30 - which_row;
				}

				SetContextForeGroundColor (fuel_colors[which_row]);
				SetContextBackGroundColor (fuel_colors[which_row + 1]);
			}

			capacity += volume;
		}

		x -= SHIP_PIECE_OFFSET;
	} while (slot--);

	return (capacity);
}
***/

COUNT
CountSISPieces (BYTE piece_type)
{
	COUNT i, num_pieces;

	num_pieces = 0;
	if (piece_type == FUSION_THRUSTER)
	{
		for (i = 0; i < NUM_DRIVE_SLOTS; ++i)
		{
			if (GLOBAL_SIS (DriveSlots[i]) == piece_type)
				++num_pieces;
		}
	}
	else if (piece_type == TURNING_JETS)
	{
		for (i = 0; i < NUM_JET_SLOTS; ++i)
		{
			if (GLOBAL_SIS (JetSlots[i]) == piece_type)
				++num_pieces;
		}
	}
	else
	{
		for (i = 0; i < NUM_MODULE_SLOTS; ++i)
		{
			if (GLOBAL_SIS (ModuleSlots[i]) == piece_type)
				++num_pieces;
		}
	}

	return (num_pieces);
}

Task flash_task = 0;
RECT flash_rect;
static FRAME flash_frame;
static FRAME flash_screen_frame = 0;
static int flash_changed;
Mutex flash_mutex = 0;
// XXX: these are currently defined in libs/graphics/sdl/3do_getbody.c
//  they should be sorted out and cleaned up at some point
extern void arith_frame_blit (FRAME srcFrame, const RECT *rsrc,
		FRAME dstFrame, const RECT *rdst, int num, int denom);

static int
flash_rect_func (void *data)
{
#define NORMAL_STRENGTH 4
#define NORMAL_F_STRENGTH 0
#define CACHE_SIZE 10
	DWORD TimeIn, WaitTime;
	SIZE strength, fstrength, incr;
	RECT cached_rect, framesize_rect;
	FRAME cached_frame = 0;
	FRAME cached_screen_frame = 0;
	Task task = (Task)data;
	int cached[CACHE_SIZE];
	STAMP cached_stamp[CACHE_SIZE];
	int i;

	for (i = 0; i < CACHE_SIZE; i++)
	{
		cached[i] = 0;
		cached_stamp[i].frame = 0;
	}
	fstrength = NORMAL_F_STRENGTH;
	incr = 1;
	strength = NORMAL_STRENGTH;
	TimeIn = GetTimeCounter ();
	WaitTime = ONE_SECOND / 16;
	while (!Task_ReadState(task, TASK_EXIT))
	{
		CONTEXT OldContext;

		LockMutex (flash_mutex);
		if (flash_changed)
		{
			RECT screen_rect;
			cached_rect = flash_rect;
			cached_frame = flash_frame;
			if (cached_screen_frame)
				DestroyDrawable (ReleaseDrawable (cached_screen_frame));
			flash_changed = 0;
			//  Wait for the  flash_screen_frame to get initialized
			FlushGraphics ();
			GetFrameRect (flash_screen_frame, &screen_rect);
			cached_screen_frame = CaptureDrawable (CreateDrawable (WANT_PIXMAP, 
					screen_rect.extent.width, screen_rect.extent.height, 1));
			screen_rect.corner.x = 0;
			screen_rect.corner.y = 0;
			arith_frame_blit (flash_screen_frame, &screen_rect, 
					cached_screen_frame, NULL, 0, 0);
			UnlockMutex (flash_mutex);
			if (cached_frame)
				GetFrameRect (cached_frame, &framesize_rect);
			for (i = 0; i < CACHE_SIZE; i++)
			{
				cached[i] = 0;
				if(cached_stamp[i].frame)
					DestroyDrawable (ReleaseDrawable (cached_stamp[i].frame));
				cached_stamp[i].frame = 0;
			}
		}
		else
			UnlockMutex (flash_mutex);
		
		if (cached_rect.extent.width)
		{
			STAMP *pStamp;
			if (cached_frame)
			{
#define MIN_F_STRENGTH -3
#define MAX_F_STRENGTH 3
				int num = 0, denom = 0;

				if ((fstrength += incr) > MAX_F_STRENGTH)
				{
					fstrength = MAX_F_STRENGTH - 1;
					incr = -1;
				}
				else if (fstrength < MIN_F_STRENGTH)
				{
					fstrength = MIN_F_STRENGTH + 1;
					incr = 1;
				}
				if (cached[fstrength - MIN_F_STRENGTH])
					pStamp = &cached_stamp[fstrength - MIN_F_STRENGTH];
				else
				{
					RECT tmp_rect = framesize_rect;
					pStamp = &cached_stamp[fstrength - MIN_F_STRENGTH];
					cached[fstrength - MIN_F_STRENGTH] = 1;
					pStamp->frame = CaptureDrawable (CreateDrawable (WANT_PIXMAP, 
							framesize_rect.extent.width, framesize_rect.extent.height, 1));
					pStamp->origin.x = framesize_rect.corner.x;
					pStamp->origin.y = framesize_rect.corner.y;
					tmp_rect.corner.x = 0;
					tmp_rect.corner.y = 0;

					if (fstrength != NORMAL_F_STRENGTH)
					{
						arith_frame_blit (cached_frame, &tmp_rect, pStamp->frame, NULL,
								fstrength > 0 ? fstrength : -fstrength, 16);

						if (fstrength < 0)
						{
							// Subtractive blit.
							num = -8;
							denom = 8;
						}
						else
						{
							// Additive blit.
							num = 8;
							denom = -8;
						}
					}

					arith_frame_blit (cached_screen_frame, &framesize_rect, pStamp->frame, 
							&tmp_rect, num, denom);
				}
			}
			else
			{
#define MIN_STRENGTH 4
#define MAX_STRENGTH 6
				if ((strength += 2) > MAX_STRENGTH)
					strength = MIN_STRENGTH;
				if (cached[strength - MIN_STRENGTH])
					pStamp = &cached_stamp[strength - MIN_STRENGTH];
				else
				{
					RECT tmp_rect = cached_rect;
					pStamp = &cached_stamp[strength - MIN_STRENGTH];
					cached[strength - MIN_STRENGTH] = 1;
					pStamp->frame = CaptureDrawable (CreateDrawable (WANT_PIXMAP, 
							cached_rect.extent.width, cached_rect.extent.height, 1));
					pStamp->origin.x = 0;
					pStamp->origin.y = 0;
					tmp_rect.corner.x = 0;
					tmp_rect.corner.y = 0;

					arith_frame_blit (cached_screen_frame, &tmp_rect, pStamp->frame, 
							&tmp_rect, 4, 4);
					if (strength != 4)
						arith_frame_blit (cached_screen_frame, &tmp_rect, pStamp->frame, 
								&tmp_rect, strength, 4);
				}
			}
			LockMutex (GraphicsLock);
			OldContext = SetContext (ScreenContext);
			SetContextClipRect (&cached_rect);
			// flash changed_can't be modified while GraphicSem is held
			if (! flash_changed)
				DrawStamp (pStamp);
			SetContextClipRect (NULL); // this will flush whatever
			SetContext (OldContext);
			UnlockMutex (GraphicsLock);
		}
		FlushGraphics ();
		SleepThreadUntil (TimeIn + WaitTime);
		TimeIn = GetTimeCounter ();
	}
	{
		if (cached_screen_frame)
			DestroyDrawable (ReleaseDrawable (cached_screen_frame));

		for (i = 0; i < CACHE_SIZE; i++)
		{
			if(cached_stamp[i].frame)
				DestroyDrawable (ReleaseDrawable (cached_stamp[i].frame));
		}
	}
	LockMutex (flash_mutex);
	flash_task = 0;
	UnlockMutex (flash_mutex);

	FinishTask (task);
	return(0);
}

void
SetFlashRect (RECT *pRect, FRAME f)
{
	RECT clip_r, temp_r, flash_rect1, old_r;
	CONTEXT OldContext;
	FRAME old_f;
	int create_flash = 0;

	if (! flash_mutex)
		flash_mutex = CreateMutex ("FlashRect Lock", SYNC_CLASS_TOPLEVEL | SYNC_CLASS_VIDEO);

	old_r = flash_rect;
	old_f = flash_frame;
	flash_rect1 = flash_rect;
		
	if (pRect != SFR_MENU_3DO && pRect != SFR_MENU_ANY)
	{
		GetContextClipRect (&clip_r);
		OldContext = SetContext (ScreenContext);
	}
	else
	{
		//Don't flash when using the PC menu
 		if (optWhichMenu == OPT_PC && pRect != SFR_MENU_ANY)
 		{
 			OldContext = SetContext (ScreenContext);
 			pRect = 0;
 		}
 		else
 		{
 			OldContext = SetContext (StatusContext);
 			GetContextClipRect (&clip_r);
 			pRect = &temp_r;
 			temp_r.corner.x = RADAR_X - clip_r.corner.x;
 			temp_r.corner.y = RADAR_Y - clip_r.corner.y;
 			temp_r.extent.width = RADAR_WIDTH;
 			temp_r.extent.height = RADAR_HEIGHT;
 			SetContext (ScreenContext);
		}
	}

	if (pRect == 0 || pRect->extent.width == 0)
	{
		flash_rect1.extent.width = 0;
		if (flash_task)
		{
			UnlockMutex (GraphicsLock);
			ConcludeTask (flash_task);
			LockMutex (GraphicsLock);
		}
	}
	else
	{
		flash_rect1 = *pRect;
		flash_rect1.corner.x += clip_r.corner.x;
		flash_rect1.corner.y += clip_r.corner.y;
		create_flash = 1;
	}
	
	LockMutex (flash_mutex);
	flash_frame = f;
	flash_rect = flash_rect1;

	if (old_r.extent.width
			&& (old_r.extent.width != flash_rect.extent.width
			|| old_r.extent.height != flash_rect.extent.height
			|| old_r.corner.x != flash_rect.corner.x
			|| old_r.corner.y != flash_rect.corner.y
			|| old_f != flash_frame))
	{
		if (flash_screen_frame)
		{
			STAMP old_s;
			old_s.origin.x = old_r.corner.x;
			old_s.origin.y = old_r.corner.y;
			old_s.frame = flash_screen_frame;
			DrawStamp (&old_s);
			DestroyDrawable (ReleaseDrawable (flash_screen_frame));
			flash_screen_frame = 0;
		}
		else
			log_add (log_Debug, "Couldn't locate flash_screen_rect");
	}
	
	if (flash_rect.extent.width)
	{
		// Copy the original contents of the rectangle from the screen.
		if (flash_screen_frame)
			DestroyDrawable (ReleaseDrawable (flash_screen_frame));
		flash_screen_frame =
				CaptureDrawable (LoadDisplayPixmap (&flash_rect, (FRAME)0));
	}
	flash_changed = 1;
	UnlockMutex (flash_mutex);
	// we create the thread after the LoadIntoExtraScreen
	// so there is no race between the FlushGraphics in flash_task
	// and the Enqueue in LoadIntoExtraScreen
	if (create_flash && flash_task == 0)
	{
		flash_task = AssignTask (flash_rect_func, 2048,
				"flash rectangle");
	}

	SetContext (OldContext);
}

void
SaveFlagshipState (void)
{
	if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
	{
		// Player is in HyperSpace or QuasiSpace.
		// Update 'GLOBAL (ShipFacing)' to the direction the flagship
		// is facing.

		HELEMENT hElement, hNextElement;

		// Find the flagship element.
		for (hElement = GetHeadElement ();
				hElement != 0; hElement = hNextElement)
		{
			ELEMENT *ElementPtr;

			LockElement (hElement, &ElementPtr);
			hNextElement = GetSuccElement (ElementPtr);
			if (ElementPtr->state_flags & PLAYER_SHIP)
			{
				STARSHIP *StarShipPtr;

				GetElementStarShip (ElementPtr, &StarShipPtr);
				// XXX: Solar system reentry test depends on ShipFacing != 0
				GLOBAL (ShipFacing) = StarShipPtr->ShipFacing + 1;
				hNextElement = 0;
			}
			UnlockElement (hElement);
		}
	}
	else if (pSolarSysState)
	{
		// Player is in a solar system.
		if (pSolarSysState->MenuState.Initialized < 3)
		{
			// XXX: Solar system reentry test depends on ShipFacing != 0
			GLOBAL (ShipFacing) = GetFrameIndex (GLOBAL (ShipStamp.frame)) + 1;
			GLOBAL (in_orbit) = 0;
			if (pSolarSysState->pBaseDesc == pSolarSysState->PlanetDesc)
			{
				GLOBAL (ip_planet) = 0;
			}
			else
			{
				GLOBAL (ip_planet) = pSolarSysState->pBaseDesc->pPrevDesc
						- pSolarSysState->PlanetDesc + 1;
				GLOBAL (ip_location) =
						pSolarSysState->SunDesc[0].location;
			}
		}
		else
		{
			// In orbit around a planet.
			BYTE moon;

			// Update the starinfo.dat file if necessary.
			if (GET_GAME_STATE (PLANETARY_CHANGE))
			{
				PutPlanetInfo ();
				SET_GAME_STATE (PLANETARY_CHANGE, 0);
			}

			// GLOBAL (ip_planet) is already set
			moon = 1; /* the planet itself */
			if (pSolarSysState->pOrbitalDesc !=
					pSolarSysState->pBaseDesc->pPrevDesc)
				moon += pSolarSysState->pOrbitalDesc
						- pSolarSysState->pBaseDesc + 1;
			GLOBAL (in_orbit) = moon;
		}
	}
}


