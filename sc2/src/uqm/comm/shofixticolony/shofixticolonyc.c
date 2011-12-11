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

// JMS 2011: Totally new file for shofixti colony comm.

#include "../commall.h"
#include "resinst.h"
#include "strings.h"
#include "libs/sound/sound.h"

#include "uqm/build.h"


static LOCDATA shofixticolony_desc =
{
	SHOFIXTICOLONY_CONVERSATION, /* AlienConv */
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	SHOFIXTI_COLONY_PMAP_ANIM, /* AlienFrame */
	SHOFIXTI_COLONY_FONT, /* AlienFont */
	WHITE_COLOR_INIT, /* AlienTextFColor */
	BLACK_COLOR_INIT, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_BOTTOM, /* AlienTextValign */
	SHOFIXTI_COLONY_COLOR_MAP, /* AlienColorMap */
	SHOFIXTI_COLONY_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	SHOFIXTI_COLONY_CONVERSATION_PHRASES, /* PlayerPhrases */
	14, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{ /* 0 moving sleeves, basic version */
			1, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND * 3, /* RestartRate */
			(1 << 1), /* BlockMask */
		},
		{ /* 1 moving sleeves, alternative version */
			4, /* StartIndex */
			2, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND * 3, /* RestartRate */
			(1 << 0), /* BlockMask */
		},
		{ /* 2 ear wiggle */
			6, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND * 3, ONE_SECOND * 2, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 3 eye blink */
			9, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND, (ONE_SECOND / 10) * 22, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 4 yellow light row */
			12, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND / 15, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 5 top left ship flying */
			23, /* StartIndex */
			8, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND * 4, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 6 ship flying */
			31, /* StartIndex */
			8, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND * 6, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 7 ship flying */
			39, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND * 7, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 8 green and red lights in the mast of the tower */
			46, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND * 2, 0, /* FrameRate */
			ONE_SECOND * 2, 0, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 9 welding #1 */
			48, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 4, ONE_SECOND * 4, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 10 welding #2 */
			54, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 4, ONE_SECOND * 4, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 11 welding #3 */
			60, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 4, ONE_SECOND * 4, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 12 welding #4 */
			66, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 4, ONE_SECOND * 4, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 13 Black screen */
			72, /* StartIndex */
			1, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND, 0, /* FrameRate */
			ONE_SECOND * 10, 0, /* RestartRate */
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
		18, /* StartIndex */
		5, /* NumFrames */
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
MainLoop (RESPONSE_REF R);

static void
ExitConversation (RESPONSE_REF R)
{
	(void) R; // satisfy compiler. Fucking compiler.
	
	if (GET_GAME_STATE (SHOFIXTI_COLONY_MET) > 1)
		NPCPhrase (GOODBYE_CAPTAIN);
	else
		NPCPhrase (BLACKSCREEN_GOODBYE);
	
	SET_GAME_STATE (BATTLE_SEGUE, 0);
}

static void
CultureLoop (RESPONSE_REF R)
{	
	static BYTE historystack = 1;
	
	if (PLAYER_SAID (R, tell_me_about_culture))
	{
		NPCPhrase (GO_TO_CULTURE);
		historystack = 1;
	}
	
	else if (PLAYER_SAID (R, how_it_works_out))
	{
		NPCPhrase (SINCE_YOU_MENTIONED_IT);
		DISABLE_PHRASE (how_it_works_out);
		
		Response (thats_sad, CultureLoop);
		Response (humans_are_different, CultureLoop);
	}
	
	else if (PLAYER_SAID (R, thats_sad) || PLAYER_SAID (R, humans_are_different))
	{
		if (PLAYER_SAID (R, thats_sad))
			NPCPhrase (YOU_UNDERSTAND);
		else if (PLAYER_SAID (R, humans_are_different))
			NPCPhrase (SO_DIFFERENT);
		
		Response (gf_at_home, CultureLoop);
	}
	
	else if (PLAYER_SAID (R, gf_at_home))
	{
		NPCPhrase (YOURE_JUST_AS_BAD);
	}
	
	else if (PLAYER_SAID (R, surprised_seeing_female))
	{
		NPCPhrase (SURPRISED_ANSWER);
		DISABLE_PHRASE (surprised_seeing_female);
	}
	
	else if (PLAYER_SAID (R, history_question_1))
	{
		NPCPhrase (HISTORY_ANSWER_1);
		DISABLE_PHRASE (history_question_1);
		historystack++;
	}
	else if (PLAYER_SAID (R, history_question_2_admire) || PLAYER_SAID (R, history_question_2_sarcasm))
	{
		NPCPhrase (HISTORY_ANSWER_2);
		DISABLE_PHRASE (history_question_2_admire);
		DISABLE_PHRASE (history_question_2_sarcasm);
		historystack++;
	}
	else if (PLAYER_SAID (R, history_question_3))
	{
		NPCPhrase (HISTORY_ANSWER_3);
		DISABLE_PHRASE (history_question_3);
		historystack++;
	}
	else if (PLAYER_SAID (R, history_question_4))
	{
		NPCPhrase (HISTORY_ANSWER_4);
		DISABLE_PHRASE (history_question_4);
		historystack++;
	}

	
	if (PHRASE_ENABLED(surprised_seeing_female))
		Response (surprised_seeing_female, CultureLoop);
	
	if (PHRASE_ENABLED(how_it_works_out))
		Response (how_it_works_out, CultureLoop);
	
	if (PHRASE_ENABLED(history_question_1) && historystack == 1)
		Response (history_question_1, CultureLoop);
	
	else if (historystack == 2)
	{
		if (PHRASE_ENABLED(history_question_2_admire))
			Response (history_question_2_admire, CultureLoop);
		if (PHRASE_ENABLED(history_question_2_sarcasm))
			Response (history_question_2_sarcasm, CultureLoop);
	}
	
	else if (PHRASE_ENABLED(history_question_3) && historystack == 3)
		Response (history_question_3, CultureLoop);
	
	else if (PHRASE_ENABLED(history_question_4) && historystack == 4)
		Response (history_question_4, CultureLoop);
	
	Response (enough_culture, MainLoop);
	Response (goodbye, ExitConversation);
}

static void
MainLoop (RESPONSE_REF R)
{	
	if (GET_GAME_STATE (SHOFIXTI_COLONY_INTRODUCED) == 0)
		SET_GAME_STATE (SHOFIXTI_COLONY_INTRODUCED, 1);
	
	if (PLAYER_SAID (R, courteous_hello) || PLAYER_SAID (R, condescending))
	{
		NPCPhrase (INTRODUCING_HIROKU);
	}
	else if (PLAYER_SAID (R, just_joking))
	{
		NPCPhrase (JUST_JOKING_REPLY);
	}
	else if (PLAYER_SAID (R, sorry_disrespectful))
	{
		NPCPhrase (DISRESPECTFUL_REPLY);
		NPCPhrase (INTRODUCING_HIROKU);
	}
	else if (PLAYER_SAID (R, enough_culture))
	{
		NPCPhrase (LEAVE_CULTURE);
		DISABLE_PHRASE (tell_me_about_culture);
	}
	else if (PLAYER_SAID (R, what_is_going_on))
	{
		if (GET_GAME_STATE (SHOFIXTICOL_CURRENT_NEWS_STACK) == 0)
		{
			NPCPhrase (THIS_IS_GOING_ON);
			SET_GAME_STATE (SHOFIXTICOL_CURRENT_NEWS_STACK, 1);
		}
		else
			NPCPhrase (THIS_IS_GOING_ON2);
		DISABLE_PHRASE (what_is_going_on);
	}
	else if (PLAYER_SAID (R, tell_me_about_distress_call))
	{
		NPCPhrase (DISTRESS_CALL);
		DISABLE_PHRASE (tell_me_about_distress_call);
	}
	
	if (PHRASE_ENABLED(what_is_going_on))
		Response (what_is_going_on, MainLoop);

	if (PHRASE_ENABLED(tell_me_about_culture))
		Response (tell_me_about_culture, CultureLoop);
	
	if (PHRASE_ENABLED(tell_me_about_distress_call))
		Response (tell_me_about_distress_call, MainLoop);
	
	Response (goodbye, ExitConversation);
}

static void
ProperIntroduction (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, a_woman) || PLAYER_SAID (R, i_luv_furries))
	{
		if (PLAYER_SAID (R, a_woman))
			NPCPhrase (YES_WOMAN);
		else if (PLAYER_SAID (R, i_luv_furries))
			NPCPhrase (BACK_OFF_CREEP);
		
		Response (condescending, MainLoop);
		Response (just_joking, MainLoop);
		Response (sorry_disrespectful, MainLoop);
	}
	
	else if (PLAYER_SAID (R, just_joking))
		NPCPhrase (JUST_JOKING_REPLY);
	
	else if (PLAYER_SAID (R, sorry_disrespectful))
		NPCPhrase (DISRESPECTFUL_REPLY);
	
	else if (GET_GAME_STATE (SHOFIXTI_COLONY_INTRODUCED) == 0)
	{
		if (GET_GAME_STATE (SHOFIXTI_COLONY_MET) == 0)
		{
			SET_GAME_STATE (SHOFIXTI_COLONY_MET, 1);
			NPCPhrase (SHO_GREETING1);
		}
		
		Response (a_woman, ProperIntroduction);
		Response (i_luv_furries, ProperIntroduction);
		Response (courteous_hello, MainLoop);
	}

	Response (goodbye, ExitConversation);
}

static void
BlackScreen3 (RESPONSE_REF R)
{
	(void) R; // satisfy compiler.
	
	NPCPhrase (BLACKSCREEN_LAST);
	
	ProperIntroduction ((RESPONSE_REF)0);
}

static void
BlackScreen2 (RESPONSE_REF R)
{
	(void) R; // satisfy compiler.
	
	NPCPhrase (AIRLOCK_CASUALTIES);
	
	Response (sounds_nasty,BlackScreen3);
	Response (whats_up_doc, BlackScreen3);
	Response (goodbye, ExitConversation);
}

static void
BlackScreen1 (RESPONSE_REF R)
{
	if (PLAYER_SAID (R, hello_blackscreen))
		NPCPhrase (BLACKSCREEN_SIGNAL_DEGRADED);
	
	if (PLAYER_SAID (R, why_degraded))
	{
		NPCPhrase (REASON_DEGRADED);
		DISABLE_PHRASE (why_degraded);
		DISABLE_PHRASE (go_ahead);
	}
	
	if (PHRASE_ENABLED(go_ahead))
		Response (go_ahead, BlackScreen2);

	if (PHRASE_ENABLED(why_degraded))	
		Response (why_degraded, BlackScreen1);
		
	if (PLAYER_SAID (R, why_degraded))
		Response (yes_proceed, BlackScreen2);
		
	Response (goodbye, ExitConversation);
}

static void
Intro (void)
{	
	if (GET_GAME_STATE (SHOFIXTI_COLONY_MET) == 0)
	{
		NPCPhrase (BLACKSCREEN);
		Response (hello_blackscreen, BlackScreen1);
		Response (goodbye, ExitConversation);
	}
	else
	{
		NPCPhrase (SHO_GREETING2);
		if (GET_GAME_STATE (SHOFIXTI_COLONY_INTRODUCED) == 0)
			ProperIntroduction ((RESPONSE_REF)0);
		else
			MainLoop ((RESPONSE_REF)0);
	}
}


static COUNT
uninit_shofixticolony (void)
{
	return (0);
}

static void
post_shofixticolony_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_shofixticolony_comm (void)
{
	LOCDATA *retval;

	shofixticolony_desc.init_encounter_func = Intro;
	shofixticolony_desc.post_encounter_func = post_shofixticolony_enc;
	shofixticolony_desc.uninit_encounter_func = uninit_shofixticolony;

	shofixticolony_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	shofixticolony_desc.AlienTextBaseline.y = RES_SIS_SCALE(100);
	shofixticolony_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	retval = &shofixticolony_desc;

	return (retval);
}
