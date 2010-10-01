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

// JMS 2010: Completely new file for androsynth communications

#include "../commall.h"
#include "resinst.h"
#include "strings.h"

#include "build.h"


static LOCDATA androsynth_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	ANDROSYNTH_PMAP_ANIM, /* AlienFrame */
	ANDROSYNTH_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	ANDROSYNTH_COLOR_MAP, /* AlienColorMap */
	ANDROSYNTH_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	ANDROSYNTH_CONVERSATION_PHRASES, /* PlayerPhrases */
	2, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{ /* Blink */
			1, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			0, ONE_SECOND * 8, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* Running light */
			10, /* StartIndex */
			30, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 40, 0, /* FrameRate */
			ONE_SECOND * 2, 0, /* RestartRate */
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
		4, /* StartIndex */
		6, /* NumFrames */
		0, /* AnimFlags */
		ONE_SECOND / 10, ONE_SECOND / 15, /* FrameRate */
		ONE_SECOND * 7 / 60, ONE_SECOND / 12, /* RestartRate */
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
	SET_GAME_STATE (BATTLE_SEGUE, 0);
	SET_GAME_STATE (ANDROSYNTH_HOSTILE, 0);

	if (PLAYER_SAID (R, goodbye_at_hello))
	{
		NPCPhrase (HELLO_GOODBYE);
	}
	
	if (PLAYER_SAID (R, i_wish_goodbye))
	{
		NPCPhrase (GIVE_BLAZER);
	}
	
	if (PLAYER_SAID (R, will_attack))
	{
		NPCPhrase (WILL_COME_TO_YOU);
	}
}

static void
FriendlySpaceAndrosynth (RESPONSE_REF R)
{
	BYTE NumVisits;
	
	if (PLAYER_SAID (R, huh_at_hello))
	{
		NPCPhrase (HUH_RESPONSE);
		Response (interested_at_hello, FriendlySpaceAndrosynth);
		Response (angry_at_hello, FriendlySpaceAndrosynth);
	}

	if (PLAYER_SAID (R, angry_at_hello))
	{
		NPCPhrase (ANGRY_RESPONSE);
		DISABLE_PHRASE(goodbye_at_hello);
		Response (calming_down, FriendlySpaceAndrosynth);
		Response (nice_try, FriendlySpaceAndrosynth);
		
	}
	
	if (PLAYER_SAID (R, nice_try))
	{
		NPCPhrase (WARNING);
		DISABLE_PHRASE(goodbye_at_hello);
		Response (will_attack, ExitConversation);
		Response (not_attack, FriendlySpaceAndrosynth);
	}
	
	if (PLAYER_SAID (R, interested_at_hello) || PLAYER_SAID (R, calming_down) || PLAYER_SAID (R, not_attack))
	{
		NPCPhrase (IN_ORZ);
		SET_GAME_STATE (KNOW_ANDROSYNTH_STATE, 1);
		Response (what_about_orz_ships, FriendlySpaceAndrosynth);
		Response (not_trapped, FriendlySpaceAndrosynth);
	}
	
	if (PLAYER_SAID (R, what_about_orz_ships))
	{
		NPCPhrase (RECYCLED);
		SET_GAME_STATE (ANDROSYNTH_STACK_1, 1);
	}
	
	if (PLAYER_SAID (R, thats_horrible))
	{
		NPCPhrase (EXISTING_STRUCTURES);
		SET_GAME_STATE (ANDROSYNTH_STACK_1, 2);
		Response (not_trapped, FriendlySpaceAndrosynth);
	}
	
	if (PLAYER_SAID (R, not_trapped))
	{
		NPCPhrase (INDEPENDENT);
		SET_GAME_STATE (ANDROSYNTH_STACK_2, 1);
		Response (kick_ass, FriendlySpaceAndrosynth);
		Response (rather_not_tell, FriendlySpaceAndrosynth);
		Response (protective_device, FriendlySpaceAndrosynth);
	}
	
	if (PLAYER_SAID (R, kick_ass) || PLAYER_SAID (R, rather_not_tell) || PLAYER_SAID (R, protective_device))
	{
		if (PLAYER_SAID (R, kick_ass))
			NPCPhrase (KICKASS_RESPONSE);
		else if (PLAYER_SAID (R, rather_not_tell))
			NPCPhrase (RATHER_NOT_TELL_RESPONSE);
		else if (PLAYER_SAID (R, protective_device))
			NPCPhrase (PROTECTIVE_RESPONSE);
		
		SET_GAME_STATE (ANDROSYNTH_STACK_2, 0);		
		NPCPhrase (JOINT_RESPONSE);
	}
	
	switch (GET_GAME_STATE (ANDROSYNTH_STACK_1))
	{
		case 1:
			Response (thats_horrible, FriendlySpaceAndrosynth);
			Response (not_trapped, FriendlySpaceAndrosynth);
			break;
	}
	
	if (GET_GAME_STATE (KNOW_ANDROSYNTH_STATE))
	{
		if(PHRASE_ENABLED(i_wish_goodbye) && !GET_GAME_STATE (ANDROSYNTH_STACK_2))
			Response (i_wish_goodbye, ExitConversation);
	}
	else 
	{
		if(PHRASE_ENABLED(goodbye_at_hello))
			Response (goodbye_at_hello, ExitConversation);
	}
}
	
static void
Intro (void)
{
	BYTE NumVisits;
	
	RESPONSE_FUNC  RespFunc;
	if (GET_GAME_STATE(ORZ_SPACE_SIDE) <= 1) {
		NPCPhrase (NOT_DONE_YET);
	}
	
	else if (!GET_GAME_STATE(ANDROSYNTH_MET))
	{
		NPCPhrase (INIT_SPACE_HELLO);
		RespFunc = (RESPONSE_FUNC)FriendlySpaceAndrosynth;
		
		Response (huh_at_hello, RespFunc);
		Response (interested_at_hello, RespFunc);
		Response (angry_at_hello, RespFunc);
		Response (goodbye_at_hello, ExitConversation);
		
		SET_GAME_STATE(ANDROSYNTH_MET,1);
	}
	else
	{
		NPCPhrase (AGAIN_HELLO);
		RespFunc = (RESPONSE_FUNC)FriendlySpaceAndrosynth;
		
		Response (interested_at_hello, RespFunc);
		Response (angry_at_hello, RespFunc);
		Response (goodbye_at_hello, ExitConversation);
	}
}

static COUNT
uninit_androsynth (void)
{
	return (0);
}

static void
post_androsynth_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_androsynth_comm (void)
{
	LOCDATA *retval;

	androsynth_desc.init_encounter_func = Intro;
	androsynth_desc.post_encounter_func = post_androsynth_enc;
	androsynth_desc.uninit_encounter_func = uninit_androsynth;

	androsynth_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	androsynth_desc.AlienTextBaseline.y = 0;
	androsynth_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;
	
	SET_GAME_STATE (BATTLE_SEGUE, 1);
	
	if (GET_GAME_STATE(ORZ_SPACE_SIDE) > 1)
		SET_GAME_STATE (BATTLE_SEGUE, 0);
		
	retval = &androsynth_desc;

	return (retval);
}