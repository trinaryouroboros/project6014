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

#include "../commall.h"
#include "resinst.h"
#include "strings.h"
#include "uqm/nameref.h"

#include "uqm/build.h"
#include "uqm/gameev.h"


#define YEHAT_FG_COLOR WHITE_COLOR
#define YEHAT_BG_COLOR BLACK_COLOR
#define YEHAT_BASE_X (SIS_SCREEN_WIDTH * 2 / 3)
#define YEHAT_BASE_Y RES_SIS_SCALE(60)
#define YEHAT_TALK_INDEX 35
#define YEHAT_TALK_FRAMES 3

#define PKUNK_FG_COLOR WHITE_COLOR
#define PKUNK_BG_COLOR BLACK_COLOR
#define PKUNK_BASE_X (SIS_SCREEN_WIDTH / 3)
#define PKUNK_BASE_Y RES_SIS_SCALE(75)
#define PKUNK_TALK_INDEX 14
#define PKUNK_TALK_FRAMES 2

static LOCDATA yehatpkunk_desc =
{
	YEHATPKUNK_CONVERSATION, /* AlienConv */
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	YEHATPKUNK_PMAP_ANIM, /* AlienFrame */
	YEHAT_FONT, /* AlienFont */
	WHITE_COLOR_INIT, /* AlienTextFColor */
	BLACK_COLOR_INIT, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_MIDDLE, /* AlienTextValign */
	YEHAT_COLOR_MAP, /* AlienColorMap */
	YEHAT_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	YEHATPKUNK_CONVERSATION_PHRASES, /* PlayerPhrases */
	15, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{ /* 0 Pkunk eye blink */
			1, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 30, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 1), /* BlockMask */
		},
		{ /* 1 Pkunk eye roll */
			5, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 0), /* BlockMask */
		},
		{ /* 2 Gulp */
			9, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 30, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 3 Pkunk light */
			16, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			(1 << 4) | (1 << 5), /* BlockMask */
		},
		{ /* 4 Pkunk light */
			19, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			(1 << 3) | (1 << 5), /* BlockMask */
		},
		{ /* 5 Pkunk light */
			22, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			(1 << 3) | (1 << 4), /* BlockMask */
		},
		{ /* 6 Pkunk left hand */
			25, /* StartIndex */
			7, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND * 6, ONE_SECOND * 3,/* RestartRate */
			0, /* BlockMask */
		},
		{ /* 7 Pkunk right hand */
			32, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND * 6, ONE_SECOND * 3,/* RestartRate */
			0, /* BlockMask */
		},
		{ /* 8 Yehat right hand */
			38, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 10, /* FrameRate */
			ONE_SECOND / 4, ONE_SECOND / 2,/* RestartRate */
			0, /* BlockMask */
		},
		{ /* 9 Yehat left hand */
			41, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 10, /* FrameRate */
			ONE_SECOND / 4, ONE_SECOND / 2,/* RestartRate */
			0, /* BlockMask */
		},
		{ /* 10 Yehat eye glow */
			44, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 14), /* BlockMask */
		},
		{ /* 11 Yehat light */
			47, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			(1 << 12) | (1 << 13), /* BlockMask */
		},
		{ /* 12 Yehat light */
			50, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			(1 << 11) | (1 << 13), /* BlockMask */
		},
		{ /* 13 Yehat light */
			53, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			(1 << 11) | (1 << 12), /* BlockMask */
		},
		{ /* 14 Yehat eye blink */
			56, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 30, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 10), /* BlockMask */
		},
	},
	{ /* AlienTransitionDesc */
		47, /* StartIndex */
		3, /* NumFrames */
		YOYO_ANIM, /* AnimFlags */
		ONE_SECOND / 30, 0, /* FrameRate */
		ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
		0, /* BlockMask */
	},
	{ /* AlienTalkDesc */
		YEHAT_TALK_INDEX, /* StartIndex */
		YEHAT_TALK_FRAMES, /* NumFrames */
		0, /* AnimFlags */
		ONE_SECOND / 15, 0, /* FrameRate */
		ONE_SECOND / 12, 0, /* RestartRate */
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

enum
{
	YEHAT_ALIEN,
	PKUNK_ALIEN
};

static int LastAlien;
static FONT PkunkFont, YehatFont;

static void
SelectAlienYEHAT (void)
{
	if (LastAlien != YEHAT_ALIEN)
	{
		// XXX: This should hold the GraphicsLock to block comm anims and
		//   prevent CommData half-updates, but if we do so, the stream
		//   decoder will deadlock with the drawing thread.

		// // Transition to neutral state first if Pik was talking
		CommData.AlienTransitionDesc.AnimFlags |= TALK_DONE;
		LastAlien = YEHAT_ALIEN;
		CommData.AlienTransitionDesc.AnimFlags |= TALK_INTRO;
		// CommData.AlienTransitionDesc.StartIndex = FOT_TO_YEHAT;
		CommData.AlienTalkDesc.StartIndex = YEHAT_TALK_INDEX;
		CommData.AlienTalkDesc.NumFrames = YEHAT_TALK_FRAMES;
		CommData.AlienAmbientArray[0].AnimFlags &= ~WAIT_TALKING;
		CommData.AlienAmbientArray[1].AnimFlags &= ~WAIT_TALKING;
		CommData.AlienAmbientArray[2].AnimFlags &= ~WAIT_TALKING;
		CommData.AlienAmbientArray[8].AnimFlags |= WAIT_TALKING;
		CommData.AlienAmbientArray[9].AnimFlags |= WAIT_TALKING;

		CommData.AlienTextBaseline.x = (SWORD)YEHAT_BASE_X;
		CommData.AlienTextBaseline.y = YEHAT_BASE_Y;
		CommData.AlienTextFColor = YEHAT_FG_COLOR;
		CommData.AlienTextBColor = YEHAT_BG_COLOR;
		CommData.AlienFont = YehatFont;
	}
}

static void
SelectAlienPKUNK (void)
{
	if (LastAlien != PKUNK_ALIEN)
	{
		// XXX: This should hold the GraphicsLock to block comm anims and
		//   prevent CommData half-updates, but if we do so, the stream
		//   decoder will deadlock with the drawing thread.

		// // Transition to neutral state first if Zoq was talking
		// if (LastAlien != FOT_ALIEN)
		CommData.AlienTransitionDesc.AnimFlags |= TALK_DONE;
		LastAlien = PKUNK_ALIEN;
		CommData.AlienTransitionDesc.AnimFlags |= TALK_INTRO;
		// CommData.AlienTransitionDesc.StartIndex = FOT_TO_PIK;
		CommData.AlienTalkDesc.StartIndex = PKUNK_TALK_INDEX;
		CommData.AlienTalkDesc.NumFrames = PKUNK_TALK_FRAMES;
		CommData.AlienAmbientArray[0].AnimFlags |= WAIT_TALKING;
		CommData.AlienAmbientArray[1].AnimFlags |= WAIT_TALKING;
		CommData.AlienAmbientArray[2].AnimFlags |= WAIT_TALKING;
		CommData.AlienAmbientArray[8].AnimFlags &= ~WAIT_TALKING;
		CommData.AlienAmbientArray[9].AnimFlags &= ~WAIT_TALKING;

		CommData.AlienTextBaseline.x = (SWORD)PKUNK_BASE_X;
		CommData.AlienTextBaseline.y = PKUNK_BASE_Y;
		CommData.AlienTextFColor = PKUNK_FG_COLOR;
		CommData.AlienTextBColor = PKUNK_BG_COLOR;
		CommData.AlienFont = PkunkFont;
	}
}

static void
YPTalkSegue (COUNT wait_track)
{
	LastAlien = PKUNK_ALIEN;
	SelectAlienYEHAT ();
	AlienTalkSegue (wait_track);
}

static void
ExitConversation (RESPONSE_REF R)
{
	(void) R;
	
	SET_GAME_STATE (BATTLE_SEGUE, 0);
	/***
	if (PLAYER_SAID (R, bye_homeworld))
	{
		NPCPhrase_cb (GOODBYE_HOME0, &SelectAlienZOQ);
		NPCPhrase_cb (GOODBYE_HOME1, &SelectAlienPIK);
		ZFPTalkSegue ((COUNT)~0);
	}
	else if (PLAYER_SAID (R, decide_later))
	{
		NPCPhrase_cb (PLEASE_HURRY0, &SelectAlienZOQ);
		NPCPhrase_cb (PLEASE_HURRY1, &SelectAlienPIK);
		ZFPTalkSegue ((COUNT)~0);
	}
	else if (PLAYER_SAID (R, valuable_info))
	{
		NPCPhrase_cb (GOODBYE0, &SelectAlienZOQ);
		NPCPhrase_cb (GOODBYE1, &SelectAlienPIK);
		NPCPhrase_cb (GOODBYE2, &SelectAlienZOQ);
		NPCPhrase_cb (GOODBYE3, &SelectAlienPIK);
		ZFPTalkSegue ((COUNT)~0);
	}
	else if (PLAYER_SAID (R, how_can_i_help))
	{
		NPCPhrase_cb (EMMISSARIES0, &SelectAlienZOQ);
		NPCPhrase_cb (EMMISSARIES1, &SelectAlienPIK);
		NPCPhrase_cb (EMMISSARIES2, &SelectAlienZOQ);
		NPCPhrase_cb (EMMISSARIES3, &SelectAlienPIK);
		NPCPhrase_cb (EMMISSARIES4, &SelectAlienZOQ);
		NPCPhrase_cb (EMMISSARIES5, &SelectAlienPIK);
		NPCPhrase_cb (EMMISSARIES6, &SelectAlienZOQ);
		NPCPhrase_cb (EMMISSARIES7, &SelectAlienPIK);
		ZFPTalkSegue ((COUNT)~0);
	}
	else if (PLAYER_SAID (R, sure))
	{
		NPCPhrase_cb (WE_ALLY0, &SelectAlienZOQ);
		NPCPhrase_cb (WE_ALLY1, &SelectAlienPIK);
		NPCPhrase_cb (WE_ALLY2, &SelectAlienZOQ);
		NPCPhrase_cb (WE_ALLY3, &SelectAlienPIK);
		NPCPhrase_cb (WE_ALLY4, &SelectAlienZOQ);
		NPCPhrase_cb (WE_ALLY5, &SelectAlienPIK);
		ZFPTalkSegue ((COUNT)~0);
		ActivateStarShip (ZOQFOTPIK_SHIP, SET_ALLIED);
		AddEvent (RELATIVE_EVENT, 3, 0, 0, ZOQFOT_DISTRESS_EVENT);
		SET_GAME_STATE (ZOQFOT_HOME_VISITS, 0);
	}
	else if (PLAYER_SAID (R, all_very_interesting))
	{
		NPCPhrase_cb (SEE_TOLD_YOU0, &SelectAlienZOQ);
		NPCPhrase_cb (SEE_TOLD_YOU1, &SelectAlienPIK);
		NPCPhrase_cb (SEE_TOLD_YOU2, &SelectAlienZOQ);
		NPCPhrase_cb (SEE_TOLD_YOU3, &SelectAlienPIK);
		ZFPTalkSegue ((COUNT)~0);

		SET_GAME_STATE (ZOQFOT_HOSTILE, 1);
		SET_GAME_STATE (ZOQFOT_HOME_VISITS, 0);
		SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (PLAYER_SAID (R, never))
	{
		NPCPhrase_cb (WE_ENEMIES0, &SelectAlienZOQ);
		NPCPhrase_cb (WE_ENEMIES1, &SelectAlienPIK);
		ZFPTalkSegue ((COUNT)~0);

		SET_GAME_STATE (ZOQFOT_HOME_VISITS, 0);
		SET_GAME_STATE (ZOQFOT_HOSTILE, 1);
		SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	***/
}

static void
Intro (void)
{
	YehatFont = CommData.AlienFont;
	
	NPCPhrase_cb (Y_HELLO, &SelectAlienYEHAT);
	NPCPhrase_cb (P_HELLO, &SelectAlienPKUNK);
	NPCPhrase_cb (Y_BYEBYE, &SelectAlienYEHAT);
	//	YPTalkSegue ((COUNT)~0);
	ExitConversation ((RESPONSE_REF)0);
}

static COUNT
uninit_yehatpkunk (void)
{
	return (0);
}

static void
post_yehatpkunk_enc (void)
{
	DestroyFont(PkunkFont);
	// nothing defined so far
}

LOCDATA*
init_yehatpkunk_comm (void)
{
	LOCDATA *retval;

	yehatpkunk_desc.init_encounter_func = Intro;
	yehatpkunk_desc.post_encounter_func = post_yehatpkunk_enc;
	yehatpkunk_desc.uninit_encounter_func = uninit_yehatpkunk;

	yehatpkunk_desc.AlienTextBaseline.x = (SWORD)YEHAT_BASE_X;
	yehatpkunk_desc.AlienTextBaseline.y = YEHAT_BASE_Y;
	yehatpkunk_desc.AlienTextWidth = (SIS_TEXT_WIDTH - 16) * 2 / 3;	
	PkunkFont = LoadFont (PKUNK_FONT);
	SET_GAME_STATE (BATTLE_SEGUE, 0);
	
	retval = &yehatpkunk_desc;

	return (retval);
}

