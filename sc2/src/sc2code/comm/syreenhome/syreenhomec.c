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


static LOCDATA syreen_desc =
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
	VALIGN_TOP, /* AlienTextValign */
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
			(1 << 8), /* BlockMask */
		},
	  {             // 1 - water flow
			10, /* StartIndex */
			4, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 2, 0,/* FrameRate */
			0, 0, /* RestartRate */
			0, /* BlockMask */
		},
	  {             // 2 - right girl moving pieces
			14, /* StartIndex */
			8, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 5, ONE_SECOND / 5, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
	  {             // 3 - lights switching on board
			22, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 2, 0, /* FrameRate */
			ONE_SECOND, 0, /* RestartRate */
			0, /* BlockMask */
		},
	  {             // 4 - front girl breathing
			27, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 7, 0, /* FrameRate */
			ONE_SECOND*4, ONE_SECOND, /* RestartRate */
			(1 << 6), /* BlockMask */
		},
	  {             // 5 - blinking eyes
			31, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND * 7, ONE_SECOND * 6, /* RestartRate */
			(1 << 6) | (1 << 7), /* BlockMask */
		},
	  {             // 6 - moving hip
			34, /* StartIndex */
			19, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 30, 0, /* FrameRate */
			ONE_SECOND * 10, ONE_SECOND * 3, /* RestartRate */
			(1 << 4) | (1 << 5) | (1 << 7), /* BlockMask */
		},
	  {             // 7 - smiley face
			53, /* StartIndex */
			17, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 10, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 5) | (1 << 6), /* BlockMask */
		},
	  {             // 8 - left girl manipulating starmap
			3, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND * 3, ONE_SECOND * 5, /* RestartRate */
			(1 << 0), /* BlockMask */
		},
	  {             // 9 - zoomed left girl turning head
			79, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM | WHEN_TALKING, /* AnimFlags */
			ONE_SECOND / 2, ONE_SECOND, /* FrameRate */
			ONE_SECOND * 3, ONE_SECOND * 5, /* RestartRate */
			(1 << 10), /* BlockMask */
		},
	  {             // 10 - zoomed left girl manipulating starmap
			81, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM | WHEN_TALKING, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND * 3, ONE_SECOND * 5, /* RestartRate */
			(1 << 9), /* BlockMask */
		},
	  {             // 11 - zoomed front girl breathing
			88, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM | WHEN_TALKING, /* AnimFlags */
			ONE_SECOND / 7, 0, /* FrameRate */
			ONE_SECOND * 4, ONE_SECOND, /* RestartRate */
			0, /* BlockMask */
		},
	},
	{ /* AlienTransitionDesc */
		70, /* StartIndex */
		9, /* NumFrames */
		0, /* AnimFlags */
		ONE_SECOND / 30, 0, /* FrameRate */
		0, 0, /* RestartRate */
		0, /* BlockMask */
	},
	{ /* AlienTalkDesc */
		95, /* StartIndex */
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
	NPCPhrase (COME_BACK_ANYTIME);
	SET_GAME_STATE (BATTLE_SEGUE, 0);
}


static void
AskMenu1 (RESPONSE_REF R)
{	
	/* Alien speech */


	if (PLAYER_SAID (R, any_news))
	{
		NPCPhrase (NO_NEWS);
		DISABLE_PHRASE (any_news);
	}
	else if (PLAYER_SAID (R, roam_stars))
	{
		NPCPhrase (WE_DONT_STRAY);
		DISABLE_PHRASE (roam_stars);
	}
			
	/* Human speech options */
	
	if (PHRASE_ENABLED (any_news))
	{
		Response (any_news, AskMenu1);
	}

    if (PHRASE_ENABLED (roam_stars))
	{
		Response (roam_stars, AskMenu1);
	}
	
	if (PHRASE_ENABLED (spot_you_later))
	{
		Response (spot_you_later, ExitConversation);
	}
}



static void
SyreenResponse1 (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, thanks_tug_complement))
	{
		NPCPhrase (SURE_I_WAS);
	}
	else if (PLAYER_SAID (R, not_fooled))
	{
		NPCPhrase (DOUBLE_WHATNOW);
	}

	Response (roam_stars, AskMenu1);
	Response (any_news, AskMenu1);
	Response (spot_you_later, ExitConversation);
}


static void
NiceComplement (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, spacebabes))
	{
		NPCPhrase (NICE_TUG);
	}
	
	Response (thanks_tug_complement, SyreenResponse1);
	Response (not_fooled, SyreenResponse1);
	Response (spot_you_later, ExitConversation);
}

static void
AnyAssistance (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, looking_for_artefacts))
	{
		NPCPhrase (ANY_ASSISTANCE);
	}

	else if (PLAYER_SAID (R, because_we_can))
	{
		NPCPhrase (ANY_ASSISTANCE);
	}

	Response (roam_stars, AskMenu1);
	Response (any_news, AskMenu1);
	Response (spot_you_later, ExitConversation);
}


static void
Intro (void)
{
	if (GET_GAME_STATE (SYREEN_MET) == 0)
	{
		SET_GAME_STATE (SYREEN_MET, 1);
	}

	NPCPhrase (SYREEN_GREETING1);

	Response (because_we_can, AnyAssistance);
	Response (looking_for_artefacts, AnyAssistance);
	Response (spacebabes, NiceComplement);
}


static COUNT
uninit_syreen (void)
{
	return (0);
}

static void
post_syreen_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_syreenhome_comm (void)
{
	LOCDATA *retval;

	syreen_desc.init_encounter_func = Intro;
	syreen_desc.post_encounter_func = post_syreen_enc;
	syreen_desc.uninit_encounter_func = uninit_syreen;

	syreen_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	syreen_desc.AlienTextBaseline.y = 0;
	syreen_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	retval = &syreen_desc;

	return (retval);
}
