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
	0, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{
			0, /* StartIndex */
			0, /* NumFrames */
			0, /* AnimFlags */
			0, 0, /* FrameRate */
			0, 0, /* RestartRate */
			0, /* BlockMask */
		}
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
		4, /* NumFrames */
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
	NPCPhrase (GOODBYE_CAPTAIN);
	SET_GAME_STATE (BATTLE_SEGUE, 0);
}

static void
WomanMenu (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, you_sound_upset) || PLAYER_SAID (R, glad_youre_not_upset))
	{
		NPCPhrase (SINCE_YOU_MENTIONED_IT);
		
		Response (thats_sad, WomanMenu);
		Response (humans_are_different, WomanMenu);
	}
	
	else if (PLAYER_SAID (R, thats_sad) || PLAYER_SAID (R, humans_are_different))
	{
		if (PLAYER_SAID (R, thats_sad))
			NPCPhrase (YOU_UNDERSTAND);
		else if (PLAYER_SAID (R, humans_are_different))
			NPCPhrase (SO_DIFFERENT);
		
		Response (gf_at_home, MainLoop);
	}
	
	Response (goodbye, ExitConversation);
}

static void
MainLoop (RESPONSE_REF R)
{	
	BYTE prevent_goodbye = 0;
	
	if (PLAYER_SAID (R, a_woman))
	{
		NPCPhrase (YES_WOMAN);
		
		Response (you_sound_upset, WomanMenu);
		Response (glad_youre_not_upset, WomanMenu);
	}

	else if (PLAYER_SAID (R, i_luv_furries))
	{
		prevent_goodbye = 1;
		
		NPCPhrase (BACK_OFF_CREEP);
		
		Response (just_joking, MainLoop);
		Response (sorry_disrespectful, MainLoop);
	}
	else if (PLAYER_SAID (R, tell_me_about_distress_call))
	{
		NPCPhrase (DISTRESS_CALL);
			DISABLE_PHRASE (tell_me_about_distress_call);
	}
	else if (PLAYER_SAID (R, gf_at_home))
		NPCPhrase (YOURE_JUST_AS_BAD);
	
	else if (PLAYER_SAID (R, just_joking))
		NPCPhrase (JUST_JOKING_REPLY);
	
	else if (PLAYER_SAID (R, sorry_disrespectful))
		NPCPhrase (DISRESPECTFUL_REPLY);
	
	if (PHRASE_ENABLED(tell_me_about_distress_call) && !prevent_goodbye)
		Response (tell_me_about_distress_call, MainLoop);
	
	if (!prevent_goodbye)
		Response (goodbye, ExitConversation);
}


static void
Intro (void)
{
	if (GET_GAME_STATE (SHOFIXTI_COLONY_MET) == 0)
	{
		SET_GAME_STATE (SHOFIXTI_COLONY_MET, 1);
		NPCPhrase (SHO_GREETING1);
	}
	else
		NPCPhrase (SHO_GREETING2);

	Response (a_woman, MainLoop);
	Response (i_luv_furries, MainLoop);
	Response (tell_me_about_distress_call, MainLoop);
	Response (goodbye, ExitConversation);
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
