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

// JMS_GFX 2011: Merged the resolution Factor stuff from UQM-HD.

#include "../colors.h"
#include "../controls.h"
#include "../gamestr.h"
#include "../shipcont.h"
#include "../setup.h"
#include "../sounds.h"
#include "../util.h"

#define ELEMENT_ORG_Y      RES_STAT_SCALE(35) // JMS_GFX
#define FREE_ORG_Y         (ELEMENT_ORG_Y + (NUM_ELEMENT_CATEGORIES * ELEMENT_SPACING_Y))
#define BIO_ORG_Y          RES_STAT_SCALE(119) // JMS_GFX
#define ELEMENT_SPACING_Y  RES_STAT_SCALE(9) // JMS_GFX

#define ELEMENT_COL_0      RES_STAT_SCALE(7) // JMS_GFX
#define ELEMENT_COL_1      RES_STAT_SCALE(32) // JMS_GFX
#define ELEMENT_COL_2      RES_STAT_SCALE(58) // JMS_GFX

#define ELEMENT_SEL_ORG_X  (ELEMENT_COL_0 + RES_STAT_SCALE(7 + 5)) // JMS_GFX
#define ELEMENT_SEL_WIDTH  (ELEMENT_COL_2 - ELEMENT_SEL_ORG_X + RES_STAT_SCALE(1)) // JMS_GFX

#define TEXT_BASELINE      RES_STAT_SCALE(6) // JMS_GFX

void
ShowRemainingCapacity (void)
{
	RECT r;
	TEXT rt;
	CONTEXT OldContext;
	UNICODE rt_amount_buf[40];

	OldContext = SetContext (StatusContext);
	SetContextFont (TinyFont);

	sprintf (rt_amount_buf, "%u", GetSBayCapacity (NULL) - GLOBAL_SIS (TotalElementMass));
	
	r.corner.x = RES_STAT_SCALE(40); // JMS_GFX
	r.corner.y = FREE_ORG_Y; // JMS_GFX
	
	rt.baseline.x = ELEMENT_COL_2 + RES_STAT_SCALE(1); // JMS_GFX
	rt.baseline.y = r.corner.y + TEXT_BASELINE;
	rt.align = ALIGN_RIGHT;
	rt.pStr = rt_amount_buf;
	rt.CharCount = (COUNT)~0;

	r.extent.width = rt.baseline.x - r.corner.x + RES_STAT_SCALE(1); // JMS_GFX
	r.extent.height = ELEMENT_SPACING_Y - RES_STAT_SCALE(2); // JMS_GFX

	BatchGraphics ();
	// erase previous free amount
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
	DrawFilledRectangle (&r);
	// print the new free amount
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x1F), 0x09));
	font_DrawText (&rt);
	UnbatchGraphics ();
	
	SetContext (OldContext);
}

void
DrawCargoStrings (BYTE OldElement, BYTE NewElement)
{
	COORD y, cy;
	TEXT rt;
	RECT r;
	CONTEXT OldContext;
	UNICODE rt_amount_buf[40];

	LockMutex (GraphicsLock);

	OldContext = SetContext (StatusContext);
	SetContextFont (TinyFont);

	BatchGraphics ();

	y = RES_STAT_SCALE(41); // JMS_GFX
	rt.align = ALIGN_RIGHT;
	rt.pStr = rt_amount_buf;

	if (OldElement > NUM_ELEMENT_CATEGORIES)
	{
		STAMP s;

		r.corner.x = RES_STAT_SCALE(2); // JMS_GFX
		r.extent.width = FIELD_WIDTH + RES_STAT_SCALE(1); // JMS_GFX

		{
			TEXT ct;

			r.corner.y = RES_STAT_SCALE(20); // JMS_GFX
			r.extent.height = RES_STAT_SCALE(129) - r.corner.y; // JMS_GFX
			DrawStarConBox (&r, 1,
					BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19),
					BUILD_COLOR (MAKE_RGB15 (0x08, 0x08, 0x08), 0x1F),
					TRUE,
					BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));

			// draw the "CARGO" title
			SetContextFont (StarConFont);
			ct.baseline.x = (STATUS_WIDTH >> 1) - RES_STAT_SCALE(1); // JMS_GFX
			ct.baseline.y = RES_STAT_SCALE(27); // JMS_GFX
			ct.align = ALIGN_CENTER;
			ct.pStr = GAME_STRING (CARGO_STRING_BASE);
			ct.CharCount = (COUNT)~0;
			SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x0A, 0x1F, 0x1F), 0x0B));
			font_DrawText (&ct);

			SetContextFont (TinyFont);
		}

		r.corner.x = ELEMENT_COL_0;			// JMS_GFX
		r.extent.width = ELEMENT_COL_0;		// JMS_GFX
		r.extent.height = ELEMENT_COL_0;	// JMS_GFX

		s.origin.x = r.corner.x + (r.extent.width >> 1);
		s.frame = SetAbsFrameIndex (MiscDataFrame, (NUM_SCANDOT_TRANSITIONS << 1) + 3);
		
		if (RESOLUTION_FACTOR == 2)
			s.frame = SetRelFrameIndex (s.frame, -1); // JMS_GFX
		
		cy = ELEMENT_ORG_Y;

		rt.baseline.y = cy - RES_STAT_SCALE(1); // JMS_GFX
		rt.CharCount = 1;

		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x1F), 0x09));
		
		rt.baseline.x = ELEMENT_COL_1; // JMS_GFX
		rt_amount_buf[0] = '$';
		font_DrawText (&rt);

		rt.baseline.x = ELEMENT_COL_2; // JMS_GFX
		rt_amount_buf[0] = '#';
		font_DrawText (&rt);

		for (OldElement = 0; OldElement < NUM_ELEMENT_CATEGORIES; ++OldElement)
		{
			// erase background under an element icon
			SetContextForeGroundColor (BLACK_COLOR);
			r.corner.y = cy; // JMS_GFX
			DrawFilledRectangle (&r);

			// draw an element icon
			s.origin.y = r.corner.y + (r.extent.height >> 1);
			DrawStamp (&s);
			s.frame = SetRelFrameIndex (s.frame, 5);

			if (OldElement != NewElement)
			{
				rt.baseline.y = cy + TEXT_BASELINE;

				SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x1F), 0x09));
				rt.baseline.x = ELEMENT_COL_1; // JMS_GFX
				sprintf (rt_amount_buf, "%u", GLOBAL (ElementWorth[OldElement]));
				rt.CharCount = (COUNT)~0;
				font_DrawText (&rt);

				SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
				rt.baseline.x = ELEMENT_COL_2; // JMS_GFX
				sprintf (rt_amount_buf, "%u", GLOBAL_SIS (ElementAmounts[OldElement]));
				rt.CharCount = (COUNT)~0;
				font_DrawText (&rt);
			}

			cy += ELEMENT_SPACING_Y; // JMS_GFX
		}

		OldElement = NewElement;

		// erase background under the Bio icon
		SetContextForeGroundColor (BLACK_COLOR);
		r.corner.y = BIO_ORG_Y; // JMS_GFX
		DrawFilledRectangle (&r);

		// draw the Bio icon
		s.origin.y = r.corner.y + (r.extent.height >> 1);
		s.frame = SetAbsFrameIndex (s.frame, 68);
		DrawStamp (&s);

		// print the Bio amount
		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
		rt.baseline.x = ELEMENT_COL_2; // JMS_GFX
		sprintf (rt_amount_buf, "%u", GLOBAL_SIS (TotalBioMass));
		rt.CharCount = (COUNT)~0;
		font_DrawText (&rt);

		// draw the line over the Bio amount
		r.corner.x = RES_STAT_SCALE(4); // JMS_GFX
		r.corner.y = BIO_ORG_Y - RES_STAT_SCALE(2); // JMS_GFX
		r.extent.width = FIELD_WIDTH - RES_STAT_SCALE(3); // JMS_GFX
		r.extent.height = 1;
		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x1F), 0x09));
		DrawFilledRectangle (&r);

		// print "Free"
		{
			TEXT lt;
			
			lt.baseline.x = RES_STAT_SCALE(5); // JMS_GFX
			lt.baseline.y = FREE_ORG_Y + TEXT_BASELINE; // JMS_GFX
			lt.align = ALIGN_LEFT;
			lt.pStr = GAME_STRING (CARGO_STRING_BASE + 1);
			lt.CharCount = (COUNT)~0;
			font_DrawText (&lt);
		}

		ShowRemainingCapacity ();
	}

	r.corner.x = ELEMENT_SEL_ORG_X; // JMS_GFX
	r.extent.width = ELEMENT_SEL_WIDTH; // JMS_GFX
	r.extent.height = ELEMENT_SPACING_Y - RES_STAT_SCALE(2); // JMS_GFX

	if (OldElement != NewElement)
	{
		if (OldElement == NUM_ELEMENT_CATEGORIES)
			r.corner.y = BIO_ORG_Y; // JMS_GFX
		else
			r.corner.y = ELEMENT_ORG_Y + (OldElement * ELEMENT_SPACING_Y); // JMS_GFX

		// draw line background
		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
		DrawFilledRectangle (&r);

		rt.baseline.y = r.corner.y + TEXT_BASELINE;

		if (OldElement == NUM_ELEMENT_CATEGORIES)
		{
			// Bio
			sprintf (rt_amount_buf, "%u", GLOBAL_SIS (TotalBioMass));
		}
		else
		{	// Element
			// print element's worth
			SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x1F), 0x09));
			rt.baseline.x = ELEMENT_COL_1; // JMS_GFX
			sprintf (rt_amount_buf, "%u", GLOBAL (ElementWorth[OldElement]));
			rt.CharCount = (COUNT)~0;
			font_DrawText (&rt);
			sprintf (rt_amount_buf, "%u", GLOBAL_SIS (ElementAmounts[OldElement]));
		}

		// print the element/bio amount
		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
		rt.baseline.x = ELEMENT_COL_2; // JMS_GFX
		rt.CharCount = (COUNT)~0;
		font_DrawText (&rt);
	}

	if (NewElement != (BYTE)~0)
	{
		if (NewElement == NUM_ELEMENT_CATEGORIES)
			r.corner.y = BIO_ORG_Y; // JMS_GFX
		else
			r.corner.y = ELEMENT_ORG_Y + (NewElement * ELEMENT_SPACING_Y); // JMS_GFX
		
		// draw line background
		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x1F), 0x09));
		DrawFilledRectangle (&r);

		rt.baseline.y = r.corner.y + TEXT_BASELINE;

		if (NewElement == NUM_ELEMENT_CATEGORIES)
		{
			// Bio
			sprintf (rt_amount_buf, "%u", GLOBAL_SIS (TotalBioMass));
		}
		else
		{	// Element
			// print element's worth
			SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
			rt.baseline.x = ELEMENT_COL_1; // JMS_GFX
			sprintf (rt_amount_buf, "%u", GLOBAL (ElementWorth[NewElement]));
			rt.CharCount = (COUNT)~0;
			font_DrawText (&rt);
			sprintf (rt_amount_buf, "%u", GLOBAL_SIS (ElementAmounts[NewElement]));
		}

		// print the element/bio amount
		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x0A, 0x1F, 0x1F), 0x0B));
		rt.baseline.x = ELEMENT_COL_2; // JMS_GFX
		rt.CharCount = (COUNT)~0;
		font_DrawText (&rt);
	}

	UnbatchGraphics ();
	SetContext (OldContext);
	UnlockMutex (GraphicsLock);
}

static BOOLEAN
DoDiscardCargo (MENU_STATE *pMS)
{
	BYTE NewState;
	BOOLEAN select, cancel, back, forward;
	select = PulsedInputState.menu[KEY_MENU_SELECT];
	cancel = PulsedInputState.menu[KEY_MENU_CANCEL];
	back = PulsedInputState.menu[KEY_MENU_UP] || PulsedInputState.menu[KEY_MENU_LEFT];
	forward = PulsedInputState.menu[KEY_MENU_DOWN] || PulsedInputState.menu[KEY_MENU_RIGHT];

	if (GLOBAL (CurrentActivity) & CHECK_ABORT)
		return (FALSE);

	if (!(pMS->Initialized & 1))
	{
		pMS->InputFunc = DoDiscardCargo;
		++pMS->Initialized;

		NewState = pMS->CurState;
		pMS->CurState = (BYTE)~0;
		goto SelectCargo;
	}
	else if (cancel)
	{
		LockMutex (GraphicsLock);
		ClearSISRect (DRAW_SIS_DISPLAY);
		UnlockMutex (GraphicsLock);

		return (FALSE);
	}
	else if (select)
	{
		if (GLOBAL_SIS (ElementAmounts[pMS->CurState - 1]))
		{
			--GLOBAL_SIS (ElementAmounts[pMS->CurState - 1]);
			DrawCargoStrings ((BYTE)(pMS->CurState - 1), (BYTE)(pMS->CurState - 1));

			LockMutex (GraphicsLock);
			--GLOBAL_SIS (TotalElementMass);
			ShowRemainingCapacity ();
			UnlockMutex (GraphicsLock);
		}
		else
		{	// no element left in cargo hold
			PlayMenuSound (MENU_SOUND_FAILURE);
		}
	}
	else
	{
		NewState = pMS->CurState - 1;
		if (back)
		{
			if (NewState == 0)
				NewState += NUM_ELEMENT_CATEGORIES;
			--NewState;
		}
		else if (forward)
		{
			++NewState;
			if (NewState == NUM_ELEMENT_CATEGORIES)
				NewState = 0;
		}

		++NewState;
		if (NewState != pMS->CurState)
		{
SelectCargo:
			DrawCargoStrings ((BYTE)(pMS->CurState - 1), (BYTE)(NewState - 1));
			LockMutex (GraphicsLock);
			DrawStatusMessage (GAME_STRING (NewState - 1 + (CARGO_STRING_BASE + 2)));
			UnlockMutex (GraphicsLock);

			pMS->CurState = NewState;
		}
	}

	SleepThread (ONE_SECOND / 30);

	return (TRUE);
}

void
Cargo (MENU_STATE *pMS)
{
	pMS->InputFunc = DoDiscardCargo;
	--pMS->Initialized;
	pMS->CurState = 1;

	LockMutex (GraphicsLock);
	DrawStatusMessage ((UNICODE *)~0);
	UnlockMutex (GraphicsLock);

	SetMenuSounds (MENU_SOUND_ARROWS, MENU_SOUND_SELECT);
	DoInput (pMS, TRUE);

	pMS->InputFunc = DoFlagshipCommands;
	pMS->CurState = CARGO + 1;
}

