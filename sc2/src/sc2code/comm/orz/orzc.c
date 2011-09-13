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

// JMS 2010: Cleaned up the comm logic

#include "comm/commall.h"
#include "comm/orz/resinst.h"
#include "comm/orz/strings.h"
#include "build.h"


static LOCDATA orz_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	ORZ_PMAP_ANIM, /* AlienFrame */
	ORZ_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	ORZ_COLOR_MAP, /* AlienColorMap */
	ORZ_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	ORZ_CONVERSATION_PHRASES, /* PlayerPhrases */
	12 /* 13 */, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{
			4, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			10, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			15, /* StartIndex */
			2, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 10, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			17, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 10, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			20, /* StartIndex */
			2, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			ONE_SECOND / 10, ONE_SECOND * 3, /* RestartRate */
			(1 << 7), /* BlockMask */
		},
		{
			22, /* StartIndex */
			8, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			ONE_SECOND / 10, ONE_SECOND * 3, /* RestartRate */
			(1 << 6), /* BlockMask */
		},
		{
			30, /* StartIndex */
			3, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			ONE_SECOND / 10, ONE_SECOND * 3, /* RestartRate */
			(1 << 5), /* BlockMask */
		},
		{
			33, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			ONE_SECOND / 10, ONE_SECOND * 3, /* RestartRate */
			(1 << 4), /* BlockMask */
		},
		{
			36, /* StartIndex */
			25, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 60, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			61, /* StartIndex */
			15, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 60, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			76, /* StartIndex */
			17, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 60, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 12), /* BlockMask */
		},
		{
			93, /* StartIndex */
			25, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 60, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			118, /* StartIndex */
			11, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 60, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 10), /* BlockMask */
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
		3, /* NumFrames */
		0, /* AnimFlags */
		ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
		ONE_SECOND / 12, ONE_SECOND * 3 / 8, /* RestartRate */
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
	if (PLAYER_SAID (R, goodbye_orz))
	{
		NPCPhrase (GOODBYE_EARTHLING);
		SET_GAME_STATE (BATTLE_SEGUE, 0);
	}	

}

// JMS: Commented out for demo
/*static void
OhFjorn (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, you_have_met_fjorn))
	{
		NPCPhrase (TALK_ABOUT_FJORN);
	}
	
		Response (goodbye_orz, ExitConversation);
}*/

// JMS: Commented out for demo
/* static void
AllianceMatters2 (RESPONSE_REF R)
{	
	
	else if (PLAYER_SAID (R, ask_know_fjorn))
	{
		NPCPhrase (ORZ_DENY_FJORN);
		DISABLE_PHRASE (ask_know_fjorn);
	}

	Response (you_have_met_fjorn, OhFjorn);
	Response (goodbye_orz, ExitConversation);
}*/

static void SmallTalk1 (RESPONSE_REF R);

static void
AllianceMatters (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, discuss_alliance_matters))
		NPCPhrase (HAPPY_TO_DISCUSS);
	
	if (PLAYER_SAID (R, alliance_question_1))
	{
		NPCPhrase (ALLIANCE_ANSWER_1);
		DISABLE_PHRASE (alliance_question_1);
	}
	else if (PLAYER_SAID (R, alliance_question_2))
	{
		NPCPhrase (ALLIANCE_ANSWER_2);  //TODO 2
		DISABLE_PHRASE (alliance_question_2);
	}
	
	if (PHRASE_ENABLED (alliance_question_1))
		Response (alliance_question_1, AllianceMatters);
	if (PHRASE_ENABLED (alliance_question_2))
		Response (alliance_question_2, AllianceMatters);
	//if (PHRASE_ENABLED (ask_know_fjorn))
	//	Response (ask_know_fjorn, AllianceMatters2); // JMS: Commented out for demo
	
	Response (enough_alliance, SmallTalk1); // JMS
	
	Response (goodbye_orz, ExitConversation);
}

static void
AnnoyOrz (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, tell_buddies))
	{
		NPCPhrase (ORZ_TELL_2);
		DISABLE_PHRASE (tell_buddies);
		SET_GAME_STATE(ORZ_STACK0, 1);
	}
	
	SmallTalk1 ((RESPONSE_REF)0);
}

void
SmallTalk1 (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, enough_alliance))
	{
		NPCPhrase (ENOUGH_ALLIANCE_OK);
	}
	if (PLAYER_SAID (R, how_are_things))
	{
		NPCPhrase (HAPPY_ORZ);
		DISABLE_PHRASE (how_are_things);
	}
	else if (PLAYER_SAID (R, any_news))
	{
		NPCPhrase (ORZ_NEWS);
		DISABLE_PHRASE (any_news);
	}
	else if (PLAYER_SAID (R, discuss_alliance_matters))
	{
		NPCPhrase (HAPPY_TO_DISCUSS);
		DISABLE_PHRASE (discuss_alliance_matters);
	}
	else if (PLAYER_SAID (R, tell_of_nature))
	{
		NPCPhrase (ORZ_TELL_1);
		DISABLE_PHRASE (tell_of_nature);
		SET_GAME_STATE(ORZ_STACK0, 1);
	}

	if (PHRASE_ENABLED (how_are_things))
		Response (how_are_things, SmallTalk1);
	if (PHRASE_ENABLED (any_news))
		Response (any_news, SmallTalk1);
	if (PHRASE_ENABLED (discuss_alliance_matters))
		Response (discuss_alliance_matters, AllianceMatters);
	
	switch (GET_GAME_STATE(ORZ_STACK0))
	{
		case 0:
			if (PHRASE_ENABLED (tell_of_nature))
				Response (tell_of_nature, SmallTalk1);
			break;
		case 1:
			if (PHRASE_ENABLED (tell_buddies))
				Response (tell_buddies, AnnoyOrz);
			break;
		default:
			break;
	}
	
	Response (goodbye_orz, ExitConversation);
}

static void
Intro (void)
{
	if (GET_GAME_STATE (ORZ_MET) == 0)
	{
		NPCPhrase (ORZ_GREETING);
		SET_GAME_STATE (ORZ_MET, 1);
	}
	else
		NPCPhrase (ORZ_GREETING_2);

	SmallTalk1 ((RESPONSE_REF)0);
}

static void
post_orz_enc (void)
{
}

static COUNT
uninit_orz (void)
{
	return (0);
}


LOCDATA*
init_orz_comm (void)
{
	LOCDATA *retval;

	orz_desc.init_encounter_func = Intro;
	orz_desc.post_encounter_func = post_orz_enc;
	orz_desc.uninit_encounter_func = uninit_orz;

	orz_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	orz_desc.AlienTextBaseline.y = 0;
	orz_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	
	retval = &orz_desc;

	return (retval);
}
