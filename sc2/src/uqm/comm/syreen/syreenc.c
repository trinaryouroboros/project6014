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

// JMS 2010: -Cleaned up the comm logic a bit...
//			 -Added a whole slew of new animations

#include "../commall.h"
#include "resinst.h"
#include "strings.h"
#include "libs/sound/sound.h"
#include "uqm/build.h"


static LOCDATA syreen_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	SYREEN_PMAP_ANIM, /* AlienFrame */
	SYREEN_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_BOTTOM, /* AlienTextValign */
	SYREEN_COLOR_MAP, /* AlienColorMap */
	SYREEN_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	SYREEN_CONVERSATION_PHRASES, /* PlayerPhrases */
	9, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{	// 0 - Flash eyes
			1, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 24, ONE_SECOND / 24, /* FrameRate */
			ONE_SECOND * 7, ONE_SECOND * 6, /* RestartRate */
			0, /* BlockMask */
		},
		{	// 1 - Bust pump
			6, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 7, 0, /* FrameRate */
			ONE_SECOND * 4, ONE_SECOND, /* RestartRate */
			(1 << 2) | (1 << 3), /* BlockMask */
		},
		{	// 2 - The seductive leg movement
			10, /* StartIndex */
			12, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 13, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND * 10, ONE_SECOND * 3, /* RestartRate */
			(1 << 1) | (1 << 5), /* BlockMask */
		},
		{	// 3 - Hand moving joystick, resulting in electricity on Tesla coil
			22, /* StartIndex */
			28, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND * 8, ONE_SECOND * 4, /* RestartRate */
			(1 << 1) | (1 << 5), /* BlockMask */
		},
		{	// 4 - Green syreen ship in oscilloscope view
			50, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 12, ONE_SECOND / 15, /* FrameRate */
			0, 0, /* RestartRate */
			0, /* BlockMask */
		},
		{	// 5 - Hand stroking joystick
			56, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 8, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND * 6, ONE_SECOND * 2, /* RestartRate */
			(1 << 2) | (1 << 3), /* BlockMask */
		},
		{	// 6 - The looong ship spin anim on big screen
			60, /* StartIndex */
			106, /* NumFrames */
			CIRCULAR_ANIM | WAIT_TALKING | FAST_STOP_AT_TALK_START, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 3, ONE_SECOND * 2, /* RestartRate */
			0, /* BlockMask */
		},
		{	// 7 - Close-up eyes blink
			182, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM | WHEN_TALKING, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND * 2, /* RestartRate */
			(1 << 8), /* BlockMask */
		},
		{	// 8 - Close-up eyebrow lift
			189, /* StartIndex */
			17, /* NumFrames */
			CIRCULAR_ANIM | WHEN_TALKING, /* AnimFlags */
			ONE_SECOND / 16, 0, /* FrameRate */
			ONE_SECOND * 5, ONE_SECOND * 4, /* RestartRate */
			(1 << 7), /* BlockMask */
		},
	},
	{ /* AlienTransitionDesc */
		166, /* StartIndex */
		8, /* NumFrames */
		0, /* AnimFlags */
		ONE_SECOND / 30, 0, /* FrameRate */
		0, 0, /* RestartRate */
		0, /* BlockMask */
	},
	{ /* AlienTalkDesc */
		174, /* StartIndex */
		8, /* NumFrames */
		TALK_INTRO, /* AnimFlags */
		ONE_SECOND / 16, ONE_SECOND / 30, /* FrameRate */
		ONE_SECOND / 10, 0, /* RestartRate */
		0, /* BlockMask */
	},
	NULL, /* AlienNumberSpeech - none */
	/* Filler for loaded resources */
	NULL, NULL, NULL,
	NULL,
	NULL,
	0, /* NumFeatures */
	{{0, 0, {0}} /*AlienFeatureArray (alternative features) */
	},
	{0 /* AlienFeatureChoice (will be computed later) */
	},
};


static void
ExitConversation (RESPONSE_REF R)
{
	(void) R; // satisfy compiler
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
init_syreen_comm (void)
{
	LOCDATA *retval;

	syreen_desc.init_encounter_func = Intro;
	syreen_desc.post_encounter_func = post_syreen_enc;
	syreen_desc.uninit_encounter_func = uninit_syreen;

	syreen_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	syreen_desc.AlienTextBaseline.y = 100;
	syreen_desc.AlienTextWidth = SIS_TEXT_WIDTH - 4;

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	retval = &syreen_desc;

	return (retval);
}
