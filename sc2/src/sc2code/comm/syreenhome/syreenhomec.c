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

// JMS 2010: Totally new file for syreen starbase comm.

#include "comm/commall.h"
#include "comm/syreenhome/resinst.h"
#include "comm/syreenhome/strings.h"
#include "libs/sound/sound.h"

#include "build.h"
#include "gameev.h"


static LOCDATA syreenhome_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	SYREEN_HOME_PMAP_ANIM, /* AlienFrame */
	SYREEN_HOME_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_BOTTOM, /* AlienTextValign */
	SYREEN_HOME_COLOR_MAP, /* AlienColorMap */
	SYREEN_HOME_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	SYREEN_HOME_CONVERSATION_PHRASES, /* PlayerPhrases */
	12, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
	  {             // 0 - left girl turning head
			1, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 2, ONE_SECOND, /* FrameRate */
			ONE_SECOND * 3, ONE_SECOND * 5, /* RestartRate */
			(1 << 1), /* BlockMask */
		},
	  {             // 1 - left girl manipulating starmap
			3, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND * 3, ONE_SECOND * 5, /* RestartRate */
			(1 << 0), /* BlockMask */
		},
	  {             // 2 - water flow
			10, /* StartIndex */
			4, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING | FAST_STOP_AT_TALK_START, /* AnimFlags */
			ONE_SECOND / 2, 0,/* FrameRate */
			0, 0, /* RestartRate */
			0, /* BlockMask */
		},
	  {             // 3 - right girl moving pieces
			14, /* StartIndex */
			8, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING | FAST_STOP_AT_TALK_START, /* AnimFlags */
			ONE_SECOND / 5, ONE_SECOND / 5, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
	  {             // 4 - lights switching on board
			22, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING | FAST_STOP_AT_TALK_START, /* AnimFlags */
			ONE_SECOND / 2, 0, /* FrameRate */
			ONE_SECOND, 0, /* RestartRate */
			0, /* BlockMask */
		},
	  {             // 5 - front girl breathing
			27, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 7, 0, /* FrameRate */
			ONE_SECOND * 4, ONE_SECOND, /* RestartRate */
			(1 << 7), /* BlockMask */
		},
	  {             // 6 - blinking eyes
			31, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND * 7, ONE_SECOND * 6, /* RestartRate */
			(1 << 7) | (1 << 8), /* BlockMask */
		},
	  {             // 7 - moving hip
			34, /* StartIndex */
			19, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 30, 0, /* FrameRate */
			ONE_SECOND * 10, ONE_SECOND * 3, /* RestartRate */
			(1 << 5) | (1 << 6) | (1 << 8), /* BlockMask */
		},
	  {             // 8 - smiley face
			53, /* StartIndex */
			17, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING | FAST_STOP_AT_TALK_START, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 20, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 6) | (1 << 7), /* BlockMask */
		},
	  {             // 9 - zoomed left girl turning head
			79, /* StartIndex */
			3, /* NumFrames */
			CIRCULAR_ANIM | WHEN_TALKING, /* AnimFlags */
			ONE_SECOND / 2, ONE_SECOND, /* FrameRate */
			ONE_SECOND * 3, ONE_SECOND * 5, /* RestartRate */
			(1 << 10), /* BlockMask */
		},
	  {             // 10 - zoomed left girl manipulating starmap
			82, /* StartIndex */
			8, /* NumFrames */
			CIRCULAR_ANIM | WHEN_TALKING, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND * 3, ONE_SECOND * 5, /* RestartRate */
			(1 << 9), /* BlockMask */
		},
	  {             // 11 - zoomed front girl breathing
			90, /* StartIndex */
			8, /* NumFrames */
			CIRCULAR_ANIM | WHEN_TALKING, /* AnimFlags */
			ONE_SECOND / 7, 0, /* FrameRate */
			ONE_SECOND * 4, ONE_SECOND, /* RestartRate */
			0, /* BlockMask */
		},
	},
	{ /* AlienTransitionDesc */
		70, /* StartIndex */
		9, /* NumFrames */
		WHEN_TALKING, /* AnimFlags */
		ONE_SECOND / 30, 0, /* FrameRate */
		0, 0, /* RestartRate */
		0, /* BlockMask */
	},
	{ /* AlienTalkDesc */
		98, /* StartIndex */
		59, /* NumFrames */
		TALK_INTRO, /* AnimFlags */
		ONE_SECOND / 20, 0, /* FrameRate */
		ONE_SECOND / 12, 0, /* RestartRate */
		0, /* BlockMask */
	},
	NULL, /* AlienNumberSpeech - none */
	/* Filler for loaded resources */
	NULL, NULL, NULL,
	NULL,
	NULL,
};

static void
ExitConversation (RESPONSE_REF R)
{
	NPCPhrase (TAKE_CARE);
	
	if(GET_GAME_STATE (SYREEN_WILL_GIVE_VESSEL))
	{
		BYTE mi, di, yi;
		
		mi = GLOBAL (GameClock.month_index);
		SET_GAME_STATE (SYREEN_SHIP_MONTH, mi);
		if ((di = GLOBAL (GameClock.day_index)) > 28)
			di = 28;
		SET_GAME_STATE (SYREEN_SHIP_DAY, di);
		yi = (BYTE)(GLOBAL (GameClock.year_index) - START_YEAR) + 1;
		SET_GAME_STATE (SYREEN_SHIP_YEAR, yi);
		
		ActivateStarShip (SYREEN_SHIP, 1);
		SET_GAME_STATE (SYREEN_WILL_GIVE_VESSEL, 0);
	}
	
	SET_GAME_STATE (BATTLE_SEGUE, 0);
}


static void
AskMenu1 (RESPONSE_REF R)
{	
	SIZE i;
	BOOLEAN ShipsReadySyreen = !((i = (GLOBAL (GameClock.year_index) - START_YEAR) - GET_GAME_STATE (SYREEN_SHIP_YEAR)) < 0
									|| ((i == 0 && (i = GLOBAL (GameClock.month_index) - GET_GAME_STATE (SYREEN_SHIP_MONTH)) < 0)
									|| (i == 0 && GLOBAL (GameClock.day_index) < GET_GAME_STATE (SYREEN_SHIP_DAY))));
	
	if (PLAYER_SAID (R, searching_for_shofixti))
	{
		NPCPhrase (NO_INFORMATION);
		
		if (ActivateStarShip (SYREEN_SHIP, FEASIBILITY_STUDY))
		{
			if (ShipsReadySyreen)
			{
				NPCPhrase (GIVE_VESSEL_1);
				SET_GAME_STATE (SYREEN_WILL_GIVE_VESSEL, 1);
			}
		}
		else
			NPCPhrase (NO_ROOM_FOR_VESSEL);
	}

	else if (PLAYER_SAID (R, where_do_i_start))
	{
		NPCPhrase (REFUEL_AT_STARBASE);
		
		if (ActivateStarShip (SYREEN_SHIP, FEASIBILITY_STUDY))
		{
			if (ShipsReadySyreen)
			{
				NPCPhrase (GIVE_VESSEL_2);
				SET_GAME_STATE (SYREEN_WILL_GIVE_VESSEL, 1);
			}
		}
		else
			NPCPhrase (NO_ROOM_FOR_VESSEL);
	}
		
	Response (thats_all, ExitConversation);
}

static void
Hospitality (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, got_drink))
	{
		NPCPhrase (ETHANOL_FLUIDS);
	}
	else if (PLAYER_SAID (R, no_seats))
	{
		NPCPhrase (LOTUS_POSITION);
	}
	else if (PLAYER_SAID (R, thanks_for_hospitality))
	{
		NPCPhrase (PLEASURE_OURS);
	}
	else if (PLAYER_SAID (R, ill_stand))
	{
		NPCPhrase (AS_YOU_WISH);
	}

	Response (searching_for_shofixti, AskMenu1);
	Response (where_do_i_start, AskMenu1);
	Response (thats_all, ExitConversation);
}


static void
Intro (void)
{
	if (GET_GAME_STATE (SYREEN_MET) == 0)
	{
		NPCPhrase (SYREENHOME_GREETING1);
		SET_GAME_STATE (SYREEN_MET, 1);
	}
	else
		NPCPhrase (SYREENHOME_GREETING2);

	Response (got_drink, Hospitality);
	Response (no_seats, Hospitality);
	Response (thanks_for_hospitality, Hospitality);
	Response (ill_stand, Hospitality);
}


static COUNT
uninit_syreenhome (void)
{
	return (0);
}

static void
post_syreenhome_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_syreenhome_comm (void)
{
	LOCDATA *retval;

	syreenhome_desc.init_encounter_func = Intro;
	syreenhome_desc.post_encounter_func = post_syreenhome_enc;
	syreenhome_desc.uninit_encounter_func = uninit_syreenhome;

	syreenhome_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	syreenhome_desc.AlienTextBaseline.y = 100;
	syreenhome_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	retval = &syreenhome_desc;

	return (retval);
}
