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

#include "../build.h"
#include "../colors.h"
#include "../controls.h"
#include "../races.h"
#include "../shipcont.h"
#include "../setup.h"
#include "../sounds.h"
#include "port.h"
#include "libs/gfxlib.h"
#include "libs/tasklib.h"


// Ship icon positions in status display around the flagship
static const POINT ship_pos[MAX_COMBAT_SHIPS] =
{
	SUPPORT_SHIP_PTS
};

// Ship icon positions split into (lower half) left and right (upper)
// and sorted in the Y coord. These are used for navigation around the
// escort positions.
static POINT sorted_ship_pos[MAX_COMBAT_SHIPS];


static int
flash_ship_task (void *data)
{
	DWORD TimeIn;
	COLOR c;
	Task task = (Task) data;

	c = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x00, 0x00), 0x24);
	TimeIn = GetTimeCounter ();
	while (!Task_ReadState (task, TASK_EXIT))
	{
		STAMP s;
		SHIP_FRAGMENT *StarShipPtr;
		COLOR OldColor;
		CONTEXT OldContext;

		LockMutex (GraphicsLock);
		s.origin = pMenuState->first_item;
		StarShipPtr = LockShipFrag (&GLOBAL (built_ship_q),
				(HSHIPFRAG)pMenuState->CurFrame);
		s.frame = StarShipPtr->icons;
		UnlockShipFrag (&GLOBAL (built_ship_q),
				(HSHIPFRAG)pMenuState->CurFrame);
		OldContext = SetContext (StatusContext);
		if (c >= BUILD_COLOR (MAKE_RGB15 (0x1F, 0x19, 0x19), 0x24))
			c = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x00, 0x00), 0x24);
		else
			c += BUILD_COLOR (MAKE_RGB15 (0x00, 0x02, 0x02), 0x00);
		OldColor = SetContextForeGroundColor (c);
		DrawFilledStamp (&s);
		SetContextForeGroundColor (OldColor);
		SetContext (OldContext);
		UnlockMutex (GraphicsLock);
		SleepThreadUntil (TimeIn + ONE_SECOND / 15);
		TimeIn = GetTimeCounter ();
	}
	FinishTask (task);
	return 0;
}

static HSHIPFRAG
MatchSupportShip (MENU_STATE *pMS)
{
	const POINT *pship_pos;
	HSHIPFRAG hStarShip, hNextShip;

	for (hStarShip = GetHeadLink (&GLOBAL (built_ship_q)),
			pship_pos = ship_pos;
			hStarShip; hStarShip = hNextShip, ++pship_pos)
	{
		SHIP_FRAGMENT *StarShipPtr;

		StarShipPtr = LockShipFrag (&GLOBAL (built_ship_q), hStarShip);

		if (pship_pos->x == pMS->first_item.x
				&& pship_pos->y == pMS->first_item.y)
		{
			UnlockShipFrag (&GLOBAL (built_ship_q), hStarShip);
			return hStarShip;
		}

		hNextShip = _GetSuccLink (StarShipPtr);
		UnlockShipFrag (&GLOBAL (built_ship_q), hStarShip);
	}

	return 0;
}

static BOOLEAN
DeltaSupportCrew (SIZE crew_delta)
{
	BOOLEAN ret = FALSE;
	UNICODE buf[40];
	HFLEETINFO hTemplate;
	SHIP_FRAGMENT *StarShipPtr;
	FLEET_INFO *TemplatePtr;

	StarShipPtr = LockShipFrag (&GLOBAL (built_ship_q),
			(HSHIPFRAG)pMenuState->CurFrame);
	hTemplate = GetStarShipFromIndex (&GLOBAL (avail_race_q),
			StarShipPtr->race_id);
	TemplatePtr = LockFleetInfo (&GLOBAL (avail_race_q), hTemplate);

	StarShipPtr->crew_level += crew_delta;

	if (StarShipPtr->crew_level == 0)
		StarShipPtr->crew_level = 1;
	else if (StarShipPtr->crew_level > TemplatePtr->crew_level &&
			crew_delta > 0)
		StarShipPtr->crew_level -= crew_delta;
	else
	{
		if (StarShipPtr->crew_level >= TemplatePtr->crew_level)
			sprintf (buf, "%u", StarShipPtr->crew_level);
		else
			sprintf (buf, "%u/%u",
					StarShipPtr->crew_level,
					TemplatePtr->crew_level);

		DrawStatusMessage (buf);
		DeltaSISGauges (-crew_delta, 0, 0);
		if (crew_delta)
		{
			RECT r;

			r.corner.x = 2;
			r.corner.y = 130;
			r.extent.width = STATUS_MESSAGE_WIDTH;
			r.extent.height = STATUS_MESSAGE_HEIGHT;
			SetContext (StatusContext);
			SetFlashRect (&r);
		}
		ret = TRUE;
	}

	UnlockFleetInfo (&GLOBAL (avail_race_q), hTemplate);
	UnlockShipFrag (&GLOBAL (built_ship_q), (HSHIPFRAG)pMenuState->CurFrame);

	return ret;
}

#define SHIP_TOGGLE ((BYTE)(1 << 7))

static void
RosterCleanup (MENU_STATE *pMS)
{
	if (pMS->flash_task)
	{
		UnlockMutex (GraphicsLock);
		ConcludeTask (pMS->flash_task);
		LockMutex (GraphicsLock);
		pMS->flash_task = 0;
	}

	if (pMS->CurFrame)
	{
		STAMP s;
		SHIP_FRAGMENT *StarShipPtr;

		SetContext (StatusContext);
		s.origin = pMS->first_item;
		StarShipPtr = LockShipFrag (&GLOBAL (built_ship_q),
				(HSHIPFRAG)pMS->CurFrame);
		s.frame = StarShipPtr->icons;
		UnlockShipFrag (&GLOBAL (built_ship_q), (HSHIPFRAG)pMS->CurFrame);
		if (!(pMS->CurState & SHIP_TOGGLE))
			DrawStamp (&s);
		else
		{
			SetContextForeGroundColor (WHITE_COLOR);
			DrawFilledStamp (&s);
		}
	}
}

static BOOLEAN
DoModifyRoster (MENU_STATE *pMS)
{
	BYTE NewState;
	RECT r;
	STAMP s;
	SHIP_FRAGMENT *StarShipPtr;
	BOOLEAN select, cancel, up, down, horiz;

	if (GLOBAL (CurrentActivity) & CHECK_ABORT)
	{
		LockMutex (GraphicsLock);
		RosterCleanup (pMS);
		UnlockMutex (GraphicsLock);
		pMS->CurFrame = 0;

		return FALSE;
	}

	select = PulsedInputState.menu[KEY_MENU_SELECT];
	cancel = PulsedInputState.menu[KEY_MENU_CANCEL];
	up = PulsedInputState.menu[KEY_MENU_UP];
	down = PulsedInputState.menu[KEY_MENU_DOWN];
	// Left or right produces the same effect because there are 2 columns
	horiz = PulsedInputState.menu[KEY_MENU_LEFT] ||
			PulsedInputState.menu[KEY_MENU_RIGHT];

	if (pMS->Initialized && (pMS->CurState & SHIP_TOGGLE))
	{
		SetMenuSounds (MENU_SOUND_UP | MENU_SOUND_DOWN,
				MENU_SOUND_SELECT | MENU_SOUND_CANCEL);
	}
	else
	{
		SetMenuSounds (MENU_SOUND_ARROWS, MENU_SOUND_SELECT);
	}

	if (!pMS->Initialized)
	{
		pMS->InputFunc = DoModifyRoster;
		pMS->Initialized = TRUE;

		pMS->CurState = NewState = 0;
		LockMutex (GraphicsLock);
		SetContext (StatusContext);
		goto SelectSupport;
	}
	else if (cancel && !(pMS->CurState & SHIP_TOGGLE))
	{
		LockMutex (GraphicsLock);
		SetFlashRect (NULL);
		RosterCleanup (pMS);
		pMS->CurFrame = 0;
		DrawStatusMessage (NULL);
		UnlockMutex (GraphicsLock);

		return FALSE;
	}
	else if (select || cancel)
	{
		LockMutex (GraphicsLock);
		pMS->CurState ^= SHIP_TOGGLE;
		if (!(pMS->CurState & SHIP_TOGGLE))
			SetFlashRect (NULL);
		else
		{
			RosterCleanup (pMS);

			r.corner.x = 2;
			r.corner.y = 130;
			r.extent.width = STATUS_MESSAGE_WIDTH;
			r.extent.height = STATUS_MESSAGE_HEIGHT;
			SetContext (StatusContext);
			SetFlashRect (&r);
		}
		UnlockMutex (GraphicsLock);
	}
	else if (pMS->CurState & SHIP_TOGGLE)
	{
		SIZE delta = 0;
		BOOLEAN failed = FALSE;

		if (up)
		{
			if (GLOBAL_SIS (CrewEnlisted))
				delta = 1;
			else
				failed = TRUE;
		}
		else if (down)
		{
			if (GLOBAL_SIS (CrewEnlisted) < GetCPodCapacity (NULL))
				delta = -1;
			else
				failed = TRUE;
		}
		
		if (delta != 0)
		{
			LockMutex (GraphicsLock);
			failed = !DeltaSupportCrew (delta);
			UnlockMutex (GraphicsLock);
		}
		if (failed)
		{	// not enough room or crew
			PlayMenuSound (MENU_SOUND_FAILURE);
		}
	}
	else
	{
		POINT *pship_pos;
		BYTE num_escorts = (BYTE) pMS->delta_item;
		BYTE top_right = (num_escorts + 1) >> 1;

		NewState = pMS->CurState;
		
		if (horiz)
		{
			pship_pos = sorted_ship_pos;
			if (NewState == top_right - 1)
				NewState = num_escorts - 1;
			else if (NewState >= top_right)
			{
				NewState -= top_right;
				if (pship_pos[NewState].y < pship_pos[pMS->CurState].y)
					++NewState;
			}
			else
			{
				NewState += top_right;
				if (NewState != top_right
						&& pship_pos[NewState].y > pship_pos[pMS->CurState].y)
					--NewState;
			}
		}
		else if (down)
		{
			++NewState;
			if (NewState == num_escorts)
				NewState = top_right;
			else if (NewState == top_right)
				NewState = 0;
		}
		else if (up)
		{
			if (NewState == 0)
				NewState = top_right - 1;
			else if (NewState == top_right)
				NewState = num_escorts - 1;
			else
				--NewState;
		}

		if (NewState != pMS->CurState)
		{
			LockMutex (GraphicsLock);
			SetContext (StatusContext);
			s.origin = pMS->first_item;
			StarShipPtr = LockShipFrag (&GLOBAL (built_ship_q),
					(HSHIPFRAG)pMS->CurFrame);
			s.frame = StarShipPtr->icons;
			UnlockShipFrag (&GLOBAL (built_ship_q), (HSHIPFRAG)pMS->CurFrame);
			DrawStamp (&s);
SelectSupport:
			pship_pos = sorted_ship_pos;
			pMS->first_item = pship_pos[NewState];
			pMS->CurFrame = (FRAME)MatchSupportShip (pMS);

			DeltaSupportCrew (0);
			UnlockMutex (GraphicsLock);

			pMS->CurState = NewState;
		}

		if (pMS->flash_task == 0)
			pMS->flash_task = AssignTask (flash_ship_task, 2048,
					"flash roster menu");
	}

	SleepThread (ONE_SECOND / 30);

	return TRUE;
}

static int
compShipPos (const void *ptr1, const void *ptr2)
{
	POINT *pt1 = (POINT *) ptr1;
	POINT *pt2 = (POINT *) ptr2;

	// Ships on the left in the lower half
	if (pt1->x < pt2->x)
		return -1;
	else if (pt1->x > pt2->x)
		return 1;

	// and ordered on Y
	if (pt1->y < pt2->y)
		return -1;
	else if (pt1->y > pt2->y)
		return 1;
	else
		return 0;
}

BOOLEAN
Roster (void)
{
	SIZE num_support_ships;

	num_support_ships = CountLinks (&GLOBAL (built_ship_q));
	if (num_support_ships)
	{
		MENU_STATE MenuState;
		MENU_STATE *pOldMenuState;

		pOldMenuState = pMenuState;
		pMenuState = &MenuState;

		// Get the ship positions we will use and sort on X then Y
		assert (sizeof (sorted_ship_pos) == sizeof (ship_pos));
		memcpy (sorted_ship_pos, ship_pos, sizeof (ship_pos));
		qsort (sorted_ship_pos, num_support_ships,
				sizeof (sorted_ship_pos[0]), compShipPos);

		MenuState.InputFunc = DoModifyRoster;
		MenuState.Initialized = FALSE;
		MenuState.CurState = 0;
		MenuState.flash_task = 0;
		MenuState.delta_item = num_support_ships;
		
		SetMenuSounds (MENU_SOUND_ARROWS, MENU_SOUND_SELECT);
		DoInput (&MenuState, TRUE);

		pMenuState = pOldMenuState;
		
		return TRUE;
	}
	
	return FALSE;
}

