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

// JMS 2009 -In Orz space no one can hear your 'caster...
// JMS 2010 -Black orb replaces rosy sphere
//			-Temporal wrapper replaces shofixti maidens
//			-Slaveshield buster replaces Clear spindle
//			-Disabled sun device's and casters' effect on Chmmr at Procyon orbit.

// JMS_GFX 2011: Merged the resolution Factor stuff from UQM-HD.

#include "../build.h"
#include "../encount.h"
#include "../gamestr.h"
#include "../controls.h"
#include "../settings.h"
#include "../shipcont.h"
#include "../load.h"
#include "../setup.h"
#include "../state.h"
#include "../sis.h"
		// for ClearSISRect()
#include "../grpinfo.h"
#include "../sounds.h"
#include "../util.h"
#include "../hyper.h"
		// for SaveSisHyperState()
#include "planets.h"
		// for SaveSolarSysLocation() and tests
#include "libs/strlib.h"


// If DEBUG_DEVICES is defined, the device list shown in the game will
// include the pictures of all devices defined, regardless of which
// devices the player actually possesses.
//#define DEBUG_DEVICES

#define DEVICE_ICON_WIDTH  RES_STAT_SCALE(16) // JMS_GFX
#define DEVICE_ICON_HEIGHT RES_STAT_SCALE(16) // JMS_GFX

#define DEVICE_ORG_Y       RES_STAT_SCALE(33) // JMS_GFX
#define DEVICE_SPACING_Y   (DEVICE_ICON_HEIGHT + RES_STAT_SCALE(2)) // JMS_GFX

#define DEVICE_COL_0       RES_STAT_SCALE(4) // JMS_GFX
#define DEVICE_COL_1       RES_STAT_SCALE(40) // JMS_GFX

#define DEVICE_SEL_ORG_X  (DEVICE_COL_0 + DEVICE_ICON_WIDTH)
#define DEVICE_SEL_WIDTH  (FIELD_WIDTH + RES_STAT_SCALE(1) - DEVICE_SEL_ORG_X + RES_STAT_SCALE(1)) // JMS_GFX

#define ICON_OFS_Y         RES_STAT_SCALE(1) // JMS_GFX
#define NAME_OFS_Y         RES_STAT_SCALE(2) // JMS_GFX
#define TEXT_BASELINE      RES_STAT_SCALE(6) // JMS_GFX
#define TEXT_SPACING_Y     RES_STAT_SCALE(7) // JMS_GFX

#define MAX_VIS_DEVICES    ((RES_STAT_SCALE(129) - DEVICE_ORG_Y) / DEVICE_SPACING_Y) // JMS_GFX

static void
DrawDevices (MENU_STATE *pMS, BYTE OldDevice, BYTE NewDevice)
{
	COORD y, cy;
	TEXT t;
	RECT r;
	BYTE *pDeviceMap;

	LockMutex (GraphicsLock);

	SetContext (StatusContext);
	SetContextFont (TinyFont);

	y = DEVICE_COL_1 + ICON_OFS_Y; // JMS_GFX
	
	t.baseline.x = DEVICE_COL_1; // JMS_GFX
	t.align = ALIGN_CENTER;
	t.CharCount = 3;

	pDeviceMap = (BYTE*)pMS->CurFrame;
	if (OldDevice > NUM_DEVICES
			|| (NewDevice < NUM_DEVICES
			&& (NewDevice < (BYTE)pMS->first_item.y
			|| NewDevice >= (BYTE)(pMS->first_item.y + MAX_VIS_DEVICES))))
	{
		STAMP s;

		r.corner.x = RES_STAT_SCALE(2); // JMS_GFX
		r.extent.width = FIELD_WIDTH + ICON_OFS_Y; // JMS_GFX

		if (!(pMS->Initialized & 1))
		{
			r.corner.x += ICON_OFS_Y; // JMS_GFX
			r.extent.width -= RES_STAT_SCALE(2); // JMS_GFX
			r.corner.y = DEVICE_ORG_Y; // JMS_GFX
			r.extent.height = RES_STAT_SCALE(89); // JMS_GFX
			SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
			DrawFilledRectangle (&r);
		}
		
		// print the "DEVICES" title
		else
		{
			TEXT ct;

			r.corner.y = RES_STAT_SCALE(20); // JMS_GFX
			r.extent.height = RES_STAT_SCALE(129) - r.corner.y; // JMS_GFX
			DrawStarConBox (&r, 1,
					BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19),
					BUILD_COLOR (MAKE_RGB15 (0x08, 0x08, 0x08), 0x1F),
					TRUE,
					BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));

			SetContextFont (StarConFont);
			ct.baseline.x = (STATUS_WIDTH >> 1) - ICON_OFS_Y; // JMS_GFX
			ct.baseline.y = r.corner.y + TEXT_SPACING_Y; // JMS_GFX
			ct.align = ALIGN_CENTER;
			ct.pStr = GAME_STRING (DEVICE_STRING_BASE);
			ct.CharCount = (COUNT)~0;
			SetContextForeGroundColor (
					BUILD_COLOR (MAKE_RGB15 (0x0A, 0x1F, 0x1F), 0x0B));
			font_DrawText (&ct);

			SetContextFont (TinyFont);
		}

		if (NewDevice < (BYTE)pMS->first_item.y)
			pMS->first_item.y = NewDevice;
		else if (NewDevice >= (BYTE)(pMS->first_item.y + MAX_VIS_DEVICES))
			pMS->first_item.y = NewDevice - (MAX_VIS_DEVICES - 1);

		s.origin.x = DEVICE_COL_0; // JMS_GFX
		s.origin.y = DEVICE_ORG_Y + ICON_OFS_Y; // JMS_GFX
		cy = y;

		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
		
		for (OldDevice = (BYTE)pMS->first_item.y;
				OldDevice < (BYTE)(pMS->first_item.y + MAX_VIS_DEVICES)
				&& OldDevice < (BYTE)pMS->first_item.x;
				++OldDevice)
		{
			s.frame = SetAbsFrameIndex (MiscDataFrame, 77 + pDeviceMap[OldDevice]);
			DrawStamp (&s);

			// print device name
			if (OldDevice != NewDevice)
			{
				t.baseline.y = cy;
				t.pStr = GAME_STRING (pDeviceMap[OldDevice] + DEVICE_STRING_BASE + 1);
				t.CharCount = utf8StringPos (t.pStr, ' ');
				font_DrawText (&t);
				t.baseline.y += TEXT_SPACING_Y; // JMS_GFX
				t.pStr = skipUTF8Chars (t.pStr, t.CharCount + 1);
				t.CharCount = (COUNT)~0;
				font_DrawText (&t);
			}

			cy += DEVICE_SPACING_Y; // JMS_GFX
			s.origin.y += DEVICE_SPACING_Y; // JMS_GFX
		}

		OldDevice = NewDevice;
	}

	r.extent.width = DEVICE_SEL_WIDTH; // JMS_GFX
	r.extent.height = RES_STAT_SCALE(14); // JMS_GFX
	r.corner.x = t.baseline.x - (r.extent.width >> 1);

	if (OldDevice != NewDevice)
	{
		cy = y + ((OldDevice - pMS->first_item.y) * DEVICE_SPACING_Y); // JMS_GFX
		r.corner.y = cy - TEXT_BASELINE; // JMS_GFX
		SetContextForeGroundColor (
				BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
		DrawFilledRectangle (&r);

		SetContextForeGroundColor (
				BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
		t.baseline.y = cy;
		t.pStr = GAME_STRING (pDeviceMap[OldDevice] + DEVICE_STRING_BASE + 1);
		t.CharCount = utf8StringPos (t.pStr, ' ');
		font_DrawText (&t);
		t.baseline.y += TEXT_SPACING_Y; // JMS_GFX
		t.pStr = skipUTF8Chars (t.pStr, t.CharCount + 1);
		t.CharCount = (COUNT)~0;
		font_DrawText (&t);
	}

	if (NewDevice < NUM_DEVICES)
	{
		cy = y + ((NewDevice - pMS->first_item.y) * DEVICE_SPACING_Y); // JMS_GFX
		r.corner.y = cy - TEXT_BASELINE; // JMS_GFX
		SetContextForeGroundColor (
				BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x1F), 0x09));
		DrawFilledRectangle (&r);

		SetContextForeGroundColor (
				BUILD_COLOR (MAKE_RGB15 (0x0A, 0x1F, 0x1F), 0x0B));
		t.baseline.y = cy;
		t.pStr = GAME_STRING (pDeviceMap[NewDevice] + DEVICE_STRING_BASE + 1);
		t.CharCount = utf8StringPos (t.pStr, ' ');
		font_DrawText (&t);
		t.baseline.y += TEXT_SPACING_Y; // JMS_GFX
		t.pStr = skipUTF8Chars (t.pStr, t.CharCount + 1);
		t.CharCount = (COUNT)~0;
		font_DrawText (&t);
	}

	UnlockMutex (GraphicsLock);
}

// Returns TRUE if the broadcaster has been successfully activated,
// and FALSE otherwise.
static BOOLEAN
UseCaster (void)
{
	if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
	{
		// BY JMS: ORZ space condition added - can't use caster in ORZ space.
		if (GET_GAME_STATE (ARILOU_SPACE_SIDE) <= 1 && GET_GAME_STATE (ORZ_SPACE_SIDE) <= 1)
		{
			SET_GAME_STATE (USED_BROADCASTER, 1);
			return TRUE;
		}
		return FALSE;
	}
	
	if (LOBYTE (GLOBAL (CurrentActivity)) != IN_INTERPLANETARY
			|| !playerInSolarSystem ())
		return FALSE;

	/*if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[1]
			&& playerInPlanetOrbit ()
			&& CurStarDescPtr->Index == CHMMR_DEFINED
			&& !GET_GAME_STATE (CHMMR_UNLEASHED)) */
	// JMS: Disabled the caster's effect on CHMMR at Procyon.
	if(0)
	{
		// In orbit around the Chenjesu/Mmrnmhrm home planet.
		NextActivity |= CHECK_LOAD;  /* fake a load game */
		GLOBAL (CurrentActivity) |= START_ENCOUNTER;

		EncounterGroup = 0;
		PutGroupInfo (GROUPS_RANDOM, GROUP_SAVE_IP);
		ReinitQueue (&GLOBAL (ip_group_q));
		assert (CountLinks (&GLOBAL (npc_built_ship_q)) == 0);

		SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 7);
		SaveSolarSysLocation ();
		return TRUE;
	}

	{
		BOOLEAN FoundIlwrath;
		HIPGROUP hGroup;

		FoundIlwrath = (BOOLEAN)(CurStarDescPtr->Index == ILWRATH_DEFINED);
				// In the Ilwrath home system?

		if (!FoundIlwrath &&
				(hGroup = GetHeadLink (&GLOBAL (ip_group_q))))
		{
			// Is an Ilwrath ship in the system?
			IP_GROUP *GroupPtr;

			GroupPtr = LockIpGroup (&GLOBAL (ip_group_q), hGroup);
			FoundIlwrath = (GroupPtr->race_id == ILWRATH_SHIP);
			UnlockIpGroup (&GLOBAL (ip_group_q), hGroup);
		}

		if (FoundIlwrath)
		{
			NextActivity |= CHECK_LOAD; /* fake a load game */
			GLOBAL (CurrentActivity) |= START_ENCOUNTER;

			EncounterGroup = 0;
			PutGroupInfo (GROUPS_RANDOM, GROUP_SAVE_IP);
			ReinitQueue (&GLOBAL (ip_group_q));
			assert (CountLinks (&GLOBAL (npc_built_ship_q)) == 0);

			if (CurStarDescPtr->Index == ILWRATH_DEFINED)
			{
				// Ilwrath home system.
				SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 4);
			}
			else
			{
				// Ilwrath ship.
				SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 5);
			}
			
			if (playerInPlanetOrbit ())
				SaveSolarSysLocation ();
			return TRUE;
		}
	}

	return FALSE;
}

static UWORD
DeviceFailed (BYTE which_device)
{
	BYTE val;

	switch (which_device)
	{
		// JMS: Added Black orb device
		case BLACK_ORB_DEVICE:
			break;
		case ARTIFACT_2_DEVICE:
			break;
		case ARTIFACT_3_DEVICE:
			break;
		case SUN_EFFICIENCY_DEVICE:
			if (LOBYTE (GLOBAL (CurrentActivity)) == IN_INTERPLANETARY
					&& playerInPlanetOrbit ())
			{
				BYTE fade_buf[1];

				PlayMenuSound (MENU_SOUND_INVOKED);
				fade_buf[0] = FadeAllToWhite;
				SleepThreadUntil (
						XFormColorMap ((COLORMAPPTR)fade_buf, ONE_SECOND * 1)
						+ (ONE_SECOND * 2));
				
				
				//if (CurStarDescPtr->Index != CHMMR_DEFINED
				//		|| pSolarSysState->pOrbitalDesc !=
				//		&pSolarSysState->PlanetDesc[1])
				// JMS: Although the Sun device won't be in the final game, disabled its
				// effect on Chmmr at procyon anyways.
				if(1)
				{
					fade_buf[0] = FadeAllToColor;
					XFormColorMap ((COLORMAPPTR)fade_buf, ONE_SECOND * 2);
				}
				else
				{
					SET_GAME_STATE (CHMMR_EMERGING, 1);

					EncounterGroup = 0;
					GLOBAL (CurrentActivity) |= START_ENCOUNTER;

					PutGroupInfo (GROUPS_RANDOM, GROUP_SAVE_IP);
					ReinitQueue (&GLOBAL (ip_group_q));
					assert (CountLinks (&GLOBAL (npc_built_ship_q)) == 0);

					CloneShipFragment (CHMMR_SHIP,
							&GLOBAL (npc_built_ship_q), 0);
				}
				return (MAKE_WORD (0, 1));
			}
			break;
		case UTWIG_BOMB_DEVICE:
			SET_GAME_STATE (UTWIG_BOMB, 0);
			GLOBAL (CurrentActivity) &= ~IN_BATTLE;
			GLOBAL_SIS (CrewEnlisted) = (COUNT)~0;
			return (FALSE);
		case ULTRON_0_DEVICE:
			break;
		case ULTRON_1_DEVICE:
			break;
		case ULTRON_2_DEVICE:
			break;
		case ULTRON_3_DEVICE:
			break;
		// JMS: Temporal Wrapper replaces Maidens device
		case TEMPORAL_WRAPPER_DEVICE:
			break;
		case TALKING_PET_DEVICE:
			NextActivity |= CHECK_LOAD; /* fake a load game */
			GLOBAL (CurrentActivity) |= START_ENCOUNTER;
			SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 0);
			if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
			{
				if (GetHeadEncounter ())
				{
					SET_GAME_STATE (SHIP_TO_COMPEL, 1);
				}
				GLOBAL (CurrentActivity) &= ~IN_BATTLE;

				SaveSisHyperState ();
			}
			else
			{
				EncounterGroup = 0;
				if (GetHeadLink (&GLOBAL (ip_group_q)))
				{
					SET_GAME_STATE (SHIP_TO_COMPEL, 1);

					PutGroupInfo (GROUPS_RANDOM, GROUP_SAVE_IP);
					ReinitQueue (&GLOBAL (ip_group_q));
					assert (CountLinks (&GLOBAL (npc_built_ship_q)) == 0);
				}

				if (CurStarDescPtr->Index == SAMATRA_DEFINED)
				{
					SET_GAME_STATE (READY_TO_CONFUSE_URQUAN, 1);
				}
				if (playerInPlanetOrbit ())
					SaveSolarSysLocation ();
			}
			return (FALSE);
		case AQUA_HELIX_DEVICE:
			val = GET_GAME_STATE (ULTRON_CONDITION);
			if (val)
			{
				SET_GAME_STATE (ULTRON_CONDITION, val + 1);
				SET_GAME_STATE (AQUA_HELIX_ON_SHIP, 0);
				SET_GAME_STATE (DISCUSSED_ULTRON, 0);
				SET_GAME_STATE (SUPOX_ULTRON_HELP, 0);
				return (FALSE);
			}
			break;
			// JMS: Slaveshield buster replaces Clear Spindle
		case SHIELD_BUSTER_DEVICE:
			if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
			{
				if(!GET_GAME_STATE(USED_BUSTER))
					SET_GAME_STATE (USED_BUSTER, 1);
				else
					SET_GAME_STATE (USED_BUSTER, 0);
				return(FALSE);
			}
			else
				return(TRUE);

			break;
		case UMGAH_HYPERWAVE_DEVICE:
		case BURVIX_HYPERWAVE_DEVICE:
			if (UseCaster ())
				return FALSE;
			break;
		case TAALO_PROTECTOR_DEVICE:
			break;
		case EGG_CASING0_DEVICE:
		case EGG_CASING1_DEVICE:
		case EGG_CASING2_DEVICE:
			break;
		case SYREEN_SHUTTLE_DEVICE:
			break;
		case VUX_BEAST_DEVICE:
			break;
		case DESTRUCT_CODE_DEVICE:
			break;
		case PORTAL_SPAWNER_DEVICE:
#define PORTAL_FUEL_COST (10 * FUEL_TANK_SCALE)
			if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE
					&& GET_GAME_STATE (ARILOU_SPACE_SIDE) <= 1
					&& GLOBAL_SIS (FuelOnBoard) >= PORTAL_FUEL_COST)
			{
				/* No DeltaSISGauges because the flagship picture
				 * is currently obscured.
				 */
				GLOBAL_SIS (FuelOnBoard) -= PORTAL_FUEL_COST;
				SET_GAME_STATE (PORTAL_COUNTER, 1);
				return (FALSE);
			}
			break;
		case URQUAN_WARP_DEVICE:
			break;
		case LUNAR_BASE_DEVICE:
			break;
	}

	return (TRUE);
}

static BOOLEAN
DoManipulateDevices (MENU_STATE *pMS)
{
	BYTE NewState;
	BOOLEAN select, cancel, back, forward;
	select = PulsedInputState.menu[KEY_MENU_SELECT];
	cancel = PulsedInputState.menu[KEY_MENU_CANCEL];
	back = PulsedInputState.menu[KEY_MENU_UP] ||
			PulsedInputState.menu[KEY_MENU_LEFT];
	forward = PulsedInputState.menu[KEY_MENU_DOWN]
			|| PulsedInputState.menu[KEY_MENU_RIGHT];

	if (GLOBAL (CurrentActivity) & CHECK_ABORT)
		return (FALSE);

	if (!pMS->Initialized)
	{
		DrawDevices (pMS, (BYTE)~0, (BYTE)~0);

		pMS->InputFunc = DoManipulateDevices;
		pMS->Initialized = TRUE;
		NewState = pMS->CurState;
		goto SelectDevice;
	}
	else if (cancel)
	{
		return (FALSE);
	}
	else if (select)
	{
		UWORD status;

		LockMutex (GraphicsLock);
		status = DeviceFailed (
				((BYTE*)pMS->CurFrame)[pMS->CurState - 1]
				);
		NewState = LOBYTE (status);
		if (NewState)
			PlayMenuSound (MENU_SOUND_FAILURE);
		else if (HIBYTE (status) == 0)
			PlayMenuSound (MENU_SOUND_INVOKED);
		UnlockMutex (GraphicsLock);

		return ((BOOLEAN)NewState);
	}
	else
	{
		SIZE NewTop;

		NewTop = pMS->first_item.y;
		NewState = pMS->CurState - 1;
		if (back)
		{
			if (NewState > 0)
				--NewState;

			if ((SIZE)NewState < NewTop && (NewTop -= MAX_VIS_DEVICES) < 0)
				NewTop = 0;
		}
		else if (forward)
		{
			++NewState;
			if (NewState == (BYTE)pMS->first_item.x)
				NewState = (BYTE)(pMS->first_item.x - 1);

			if (NewState >= NewTop + MAX_VIS_DEVICES)
				NewTop = NewState;
		}

		++NewState;
		if (NewState != pMS->CurState)
		{
			if (NewTop != pMS->first_item.y)
			{
				pMS->first_item.y = NewTop;
				pMS->CurState = (BYTE)~0;
			}
SelectDevice:
			DrawDevices (pMS, (BYTE)(pMS->CurState - 1), (BYTE)(NewState - 1));
			pMS->CurState = NewState;
		}

		SleepThread (ONE_SECOND / 30);
	}

	return (TRUE);
}

SIZE
InventoryDevices (BYTE *pDeviceMap, COUNT Size)
{
	BYTE i;
	SIZE DevicesOnBoard;
	
	DevicesOnBoard = 0;
	for (i = 0; i < NUM_DEVICES && Size > 0; ++i)
	{
		BYTE DeviceState;

		DeviceState = 0;
		switch (i)
		{
			// JMS: Black orb device
			case BLACK_ORB_DEVICE:
				DeviceState = GET_GAME_STATE (BLACK_ORB_ON_SHIP);
				break;
			case ARTIFACT_2_DEVICE:
				DeviceState = GET_GAME_STATE (ARTIFACT_2_ON_SHIP);
				break;
			case ARTIFACT_3_DEVICE:
				DeviceState = GET_GAME_STATE (ARTIFACT_3_ON_SHIP);
				break;
			case SUN_EFFICIENCY_DEVICE:
				DeviceState = GET_GAME_STATE (SUN_DEVICE_ON_SHIP);
				break;
			case UTWIG_BOMB_DEVICE:
				DeviceState = GET_GAME_STATE (UTWIG_BOMB_ON_SHIP);
				break;
			case ULTRON_0_DEVICE:
				DeviceState = (GET_GAME_STATE (ULTRON_CONDITION) == 1);
				break;
			case ULTRON_1_DEVICE:
				DeviceState = (GET_GAME_STATE (ULTRON_CONDITION) == 2);
				break;
			case ULTRON_2_DEVICE:
				DeviceState = (GET_GAME_STATE (ULTRON_CONDITION) == 3);
				break;
			case ULTRON_3_DEVICE:
				DeviceState = (GET_GAME_STATE (ULTRON_CONDITION) == 4);
				break;
			// JMS: Temporal Wrapper device
			case TEMPORAL_WRAPPER_DEVICE:
				DeviceState = GET_GAME_STATE (TEMPORAL_WRAPPER_ON_SHIP);
				break;
			case TALKING_PET_DEVICE:
				DeviceState = GET_GAME_STATE (TALKING_PET_ON_SHIP);
				break;
			case AQUA_HELIX_DEVICE:
				DeviceState = GET_GAME_STATE (AQUA_HELIX_ON_SHIP);
				break;
			// JMS: Slaveshield buster device
			case SHIELD_BUSTER_DEVICE:
				DeviceState = GET_GAME_STATE (SHIELD_BUSTER_ON_SHIP);
				break;
			case UMGAH_HYPERWAVE_DEVICE:
				DeviceState = GET_GAME_STATE (UMGAH_BROADCASTERS_ON_SHIP);
				break;
			case TAALO_PROTECTOR_DEVICE:
				DeviceState = GET_GAME_STATE (TAALO_PROTECTOR_ON_SHIP);
				break;
			case EGG_CASING0_DEVICE:
				DeviceState = GET_GAME_STATE (EGG_CASE0_ON_SHIP);
				break;
			case EGG_CASING1_DEVICE:
				DeviceState = GET_GAME_STATE (EGG_CASE1_ON_SHIP);
				break;
			case EGG_CASING2_DEVICE:
				DeviceState = GET_GAME_STATE (EGG_CASE2_ON_SHIP);
				break;
			case SYREEN_SHUTTLE_DEVICE:
				DeviceState = GET_GAME_STATE (SYREEN_SHUTTLE_ON_SHIP);
				break;
			case VUX_BEAST_DEVICE:
				DeviceState = GET_GAME_STATE (VUX_BEAST_ON_SHIP);
				break;
			case DESTRUCT_CODE_DEVICE:
#ifdef NEVER
				DeviceState = GET_GAME_STATE (DESTRUCT_CODE_ON_SHIP);
#endif /* NEVER */
				break;
			case PORTAL_SPAWNER_DEVICE:
				DeviceState = GET_GAME_STATE (PORTAL_SPAWNER_ON_SHIP);
				break;
			case URQUAN_WARP_DEVICE:
				DeviceState = GET_GAME_STATE (PORTAL_KEY_ON_SHIP);
				break;
			case BURVIX_HYPERWAVE_DEVICE:
				DeviceState = GET_GAME_STATE (BURV_BROADCASTERS_ON_SHIP);
				break;
			case LUNAR_BASE_DEVICE:
				DeviceState = GET_GAME_STATE (MOONBASE_ON_SHIP);
				break;
		}

#ifndef DEBUG_DEVICES
		if (DeviceState)
#endif /* DEBUG_DEVICES */
		{
			*pDeviceMap++ = i;
			++DevicesOnBoard;
			--Size;
		}
	}
	
	return (DevicesOnBoard);
}

BOOLEAN
DevicesMenu (void)
{
	BYTE DeviceMap[NUM_DEVICES];
	MENU_STATE MenuState;

	memset (&MenuState, 0, sizeof MenuState);

	MenuState.first_item.x = InventoryDevices (DeviceMap, NUM_DEVICES);
	if (MenuState.first_item.x)
	{
		MenuState.InputFunc = DoManipulateDevices;
		MenuState.Initialized = FALSE;
		// XXX: 1-based index because this had to work around the
		//   pSolarSysState->MenuState.CurState abuse. Should be changed.
		MenuState.CurState = 1;
		MenuState.first_item.y = 0;

		// XXX: CurFrame hack
		MenuState.CurFrame = (FRAME)DeviceMap;
		//DoManipulateDevices (pMS); /* to make sure it's initialized */
		SetMenuSounds (MENU_SOUND_ARROWS, MENU_SOUND_SELECT);
		DoInput (&MenuState, TRUE);

		if (GLOBAL_SIS (CrewEnlisted) != (COUNT)~0
				&& !(GLOBAL (CurrentActivity) & CHECK_ABORT))
		{
			LockMutex (GraphicsLock);
			ClearSISRect (DRAW_SIS_DISPLAY);
			UnlockMutex (GraphicsLock);

			if (!GET_GAME_STATE (PORTAL_COUNTER)
					&& !(GLOBAL (CurrentActivity) & START_ENCOUNTER)
					&& GLOBAL_SIS (CrewEnlisted) != (COUNT)~0)
// DrawMenuStateStrings (PM_SCAN, pMS->CurState - 1);
				return (TRUE);
		}
	}
	
	return (FALSE);
}

