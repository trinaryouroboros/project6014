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

static LOCDATA vux_desc =
{
	VUX_CONVERSATION, /* AlienConv */
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	VUX_PMAP_ANIM, /* AlienFrame */
	VUX_FONT, /* AlienFont */
	WHITE_COLOR_INIT, /* AlienTextFColor */
	BLACK_COLOR_INIT, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* (SIS_TEXT_WIDTH - 16) >> 1, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	VUX_COLOR_MAP, /* AlienColorMap */
	VUX_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	VUX_CONVERSATION_PHRASES, /* PlayerPhrases */
	17, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{
			12, /* StartIndex */
			3, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			15, /* StartIndex */
			5, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			20, /* StartIndex */
			14, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 30, 0, /* FrameRate */
			ONE_SECOND / 30, 0, /* RestartRate */
			0, /* BlockMask */
		},
		{
			34, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			41, /* StartIndex */
			6, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			47, /* StartIndex */
			11, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			58, /* StartIndex */
			3, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			61, /* StartIndex */
			4, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			65, /* StartIndex */
			4, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			69, /* StartIndex */
			2, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			71, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			74, /* StartIndex */
			6, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND / 15, ONE_SECOND / 15, /* RestartRate */
			0, /* BlockMask */
		},
		{
			80, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND / 15, ONE_SECOND / 15, /* RestartRate */
			(1 << 14), /* BlockMask */
		},
		{
			85, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND / 15, ONE_SECOND / 15, /* RestartRate */
			0, /* BlockMask */
		},
		{
			90, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND / 15, ONE_SECOND / 15, /* RestartRate */
			(1 << 12), /* BlockMask */
		},
		{
			95, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND * 5, ONE_SECOND * 5,/* RestartRate */
			0, /* BlockMask */
		},
		{
			99, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND * 5, ONE_SECOND * 5,/* RestartRate */
			0, /* BlockMask */
		},
	},
	{ /* AlienTransitionDesc */
		0, /* StartIndex */
		0, /* NumFrames */
		0, /* AnimFlags */
		0, 0, /* FrameRate */
		0, 0, /* RestartRate */
		0, /* BlockMask */
	},
	{ /* AlienTalkDesc */
		1, /* StartIndex */
		11, /* NumFrames */
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

static void
Intro (RESPONSE_REF R);


static void
ExitConversation (RESPONSE_REF R)
{
	(void) R; // satisfy compiler
	NPCPhrase (GOODBYE_EARTHLING);
	SET_GAME_STATE (BATTLE_SEGUE, 0);
}

static void
ExitConversationSilent (RESPONSE_REF R)
{
	(void) R; // satisfy compiler
	NPCPhrase (SILENT);
	SET_GAME_STATE (BATTLE_SEGUE, 0);	
}



static void
ApologyMenu6 (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, a5apology_3))
		NPCPhrase (A5APOLOGY_3_ANSWER);
	
	Response (a6apology_1, Intro);
	Response (a6apology_2, Intro);
	Response (a6apology_3, Intro);
}

static void
ApologyMenu5 (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, a4apology_1))
		NPCPhrase (A4APOLOGY_1_ANSWER);
	if (PLAYER_SAID (R, a4apology_2))
		NPCPhrase (A4APOLOGY_2_ANSWER);
	
	Response (a5apology_1, Intro);
	Response (a5apology_2, Intro);
	Response (a5apology_3, ApologyMenu6);
	Response (a5apology_4, Intro);
}

static void
ApologyMenu4 (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, a3apology_1))
		NPCPhrase (A3APOLOGY_1_ANSWER);
	if (PLAYER_SAID (R, a3apology_4))
		NPCPhrase (A3APOLOGY_4_ANSWER);
	
	Response (a4apology_1, ApologyMenu5);
	Response (a4apology_2, ApologyMenu5);
	Response (a4apology_3, Intro);
	Response (a4apology_4, Intro);
}

static void
ApologyMenu3 (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, a2apology_2))
		NPCPhrase (A2APOLOGY_2_ANSWER);
	if (PLAYER_SAID (R, a2apology_3))
		NPCPhrase (A2APOLOGY_3_ANSWER);
	
	Response (a3apology_1, ApologyMenu4);
	Response (a3apology_2, Intro);
	Response (a3apology_3, Intro);
	Response (a3apology_4, ApologyMenu4);
}

static void
ApologyMenu2 (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, apology_1))
		NPCPhrase (APOLOGY_1_ANSWER);
	if (PLAYER_SAID (R, apology_2))
		NPCPhrase (APOLOGY_2_ANSWER);
	if (PLAYER_SAID (R, apology_3))
		NPCPhrase (APOLOGY_3_ANSWER);
	
	Response (a2apology_1, Intro);
	Response (a2apology_2, ApologyMenu3);
	Response (a2apology_3, ApologyMenu3);
	Response (a2apology_4, ExitConversationSilent);
}

static void
ApologyMenu1 (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, greeting_1_to_apology))
	{
		if (GET_GAME_STATE (VUX_APOLOGY_TRIED) == 0)
			NPCPhrase (KILL_LIKE_OTHERS);
		else
			NPCPhrase (SILENT);
		
		SET_GAME_STATE (VUX_APOLOGY_TRIED, 1);
		DISABLE_PHRASE (greeting_1_to_apology);
	}
	
	Response (apology_1, ApologyMenu2);
	Response (apology_2, ApologyMenu2);
	Response (apology_3, ApologyMenu2);
	Response (apology_4, Intro);
}


static void
StarBaseMenu (RESPONSE_REF R)
{	
	static BYTE spiteful_answers = 0;
	
	/* Alien speech */
	if (PLAYER_SAID (R, greeting_1_whats_like))
	{
		NPCPhrase (TAKE_GUESS);
		DISABLE_PHRASE (greeting_1_whats_like);
	}
	
	else if (PLAYER_SAID (R, sb1_science_teams))
	{
		NPCPhrase (SILENT);
		DISABLE_PHRASE (sb1_science_teams);
	}
	
	else if (PLAYER_SAID (R, sb2_neglect_mismanagement))
	{
		NPCPhrase (NOT_IDIOTS);
		DISABLE_PHRASE (sb2_neglect_mismanagement);
	}
	
	else if (PLAYER_SAID (R, sb3_rethinking_position))
	{
		NPCPhrase (ARGUMENT_SUBJECT);
		DISABLE_PHRASE (sb3_rethinking_position);
	}
	
	else if (PLAYER_SAID (R, sb4_seriously_how_is))
	{
		if (GET_GAME_STATE (VUX_RANT_HEARD) == 0)
			NPCPhrase (RANT);
		else
			NPCPhrase (AFTER_RANT);
		
		SET_GAME_STATE (VUX_RANT_HEARD, 1);
		DISABLE_PHRASE (sb4_seriously_how_is);
	}
	
	else if (PLAYER_SAID (R, sb5_safe_under_shield))
	{
		if (spiteful_answers)
			NPCPhrase (SPITEFUL_VUX_2);
		else
			NPCPhrase (SPITEFUL_VUX_1);
		
		DISABLE_PHRASE (sb5_safe_under_shield);
			
		spiteful_answers++;
		spiteful_answers %= 2;
	}

	/* Human speech options */
    if (PHRASE_ENABLED (sb1_science_teams))
		Response (sb1_science_teams, StarBaseMenu);
	
	if (PHRASE_ENABLED (sb2_neglect_mismanagement))
		Response (sb2_neglect_mismanagement, StarBaseMenu);
	
	if (PHRASE_ENABLED (sb3_rethinking_position))
		Response (sb3_rethinking_position, StarBaseMenu);
	
	if (!(PHRASE_ENABLED (sb1_science_teams)) && !(PHRASE_ENABLED (sb2_neglect_mismanagement)) 
		&& !(PHRASE_ENABLED (sb3_rethinking_position)) && PHRASE_ENABLED (sb4_seriously_how_is))
		Response (sb4_seriously_how_is, StarBaseMenu);
	
	if (PHRASE_ENABLED (sb5_safe_under_shield) && (GET_GAME_STATE (VUX_RANT_HEARD) == 1))
		Response (sb5_safe_under_shield, StarBaseMenu);

	Response (sb6_okay_then, Intro);
}



static void
Intro (RESPONSE_REF R)
{
	/* Alien speech */
	if (PLAYER_SAID (R, greeting_1_who_you))
	{
		NPCPhrase (STRONG_STOMACH);
		SET_GAME_STATE (VUX_MET, 1);
	}
	
	else if (PLAYER_SAID (R, a2apology_1))
		NPCPhrase (A2APOLOGY_1_ANSWER);
	
	else if (PLAYER_SAID (R, apology_4))
		NPCPhrase (APOLOGY_4_ANSWER);
	
	else if (PLAYER_SAID (R, a3apology_2))
		NPCPhrase (A3APOLOGY_2_ANSWER);
	
	else if (PLAYER_SAID (R, a3apology_3))
		NPCPhrase (A3APOLOGY_3_ANSWER);
	
	else if (PLAYER_SAID (R, a4apology_3))
		NPCPhrase (A4APOLOGY_3_ANSWER);
	
	else if (PLAYER_SAID (R, a4apology_4))
		NPCPhrase (A4APOLOGY_4_ANSWER);
	
	else if (PLAYER_SAID (R, a5apology_1))
		NPCPhrase (A5APOLOGY_1_ANSWER);
	
	else if (PLAYER_SAID (R, a5apology_2))
		NPCPhrase (A5APOLOGY_2_ANSWER);
	
	else if (PLAYER_SAID (R, a5apology_4))
		NPCPhrase (A5APOLOGY_4_ANSWER);
	
	else if (PLAYER_SAID (R, a6apology_1))
		NPCPhrase (A6APOLOGY_1_ANSWER);
	
	else if (PLAYER_SAID (R, a6apology_2))
		NPCPhrase (A6APOLOGY_2_ANSWER);
	
	else if (PLAYER_SAID (R, a6apology_3))
		NPCPhrase (A6APOLOGY_3_ANSWER);
	
	else if (PLAYER_SAID (R, whats_news))
	{
		NPCPhrase (DAX_INFORMATION);
		DISABLE_PHRASE (whats_news);
	}
	
	else if (PLAYER_SAID (R, sb6_okay_then))
		NPCPhrase (SILENT);
	
	/* Human speech options */
	if (PHRASE_ENABLED (greeting_1_who_you) && GET_GAME_STATE (VUX_MET) == 0)
		Response (greeting_1_who_you, Intro);
	
	if (PHRASE_ENABLED (greeting_1_whats_like))
		Response (greeting_1_whats_like, StarBaseMenu);
	
	if (PHRASE_ENABLED (greeting_1_to_apology))
		Response (greeting_1_to_apology, ApologyMenu1);
	
	if (PHRASE_ENABLED (whats_news))
		Response (whats_news, Intro);
	
	Response (goodbye_vux, ExitConversation);
}

static void
StartComm (void)
{
	if (GET_GAME_STATE (VUX_MET) == 0)
		NPCPhrase (VUX_GREETING1);
	else
		NPCPhrase (VUX_GREETING2);

	Intro (0);
}


static COUNT
uninit_vux (void)
{
	return (0);
}

static void
post_vux_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_vux_comm (void)
{
	LOCDATA *retval;

	vux_desc.init_encounter_func = StartComm;
	vux_desc.post_encounter_func = post_vux_enc;
	vux_desc.uninit_encounter_func = uninit_vux;

	vux_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1) + (SIS_TEXT_WIDTH >> 2);
	vux_desc.AlienTextBaseline.y = 0;
	vux_desc.AlienTextWidth = (SIS_TEXT_WIDTH - 16) >> 1;

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	retval = &vux_desc;

	return (retval);
}
