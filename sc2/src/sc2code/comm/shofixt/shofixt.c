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

// JMS 2010: Added viewscreen animu

#include "comm/commall.h"
#include "comm/shofixt/resinst.h"
#include "comm/shofixt/strings.h"

#include "gameev.h"


static LOCDATA shofixti_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	SHOFIXTI_PMAP_ANIM, /* AlienFrame */
	SHOFIXTI_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	SHOFIXTI_COLOR_MAP, /* AlienColorMap */
	SHOFIXTI_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	SHOFIXTI_CONVERSATION_PHRASES, /* PlayerPhrases */
	13, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{
			5, /* StartIndex */
			15, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 30, 0, /* FrameRate */
			ONE_SECOND / 30, 0, /* RestartRate */
			0, /* BlockMask */
		},
		{
			20, /* StartIndex */
			3, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			(ONE_SECOND >> 1), (ONE_SECOND >> 1) * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			23, /* StartIndex */
			3, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			(ONE_SECOND >> 1), (ONE_SECOND >> 1) * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			26, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 30, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			29, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},

		{
			33, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 20, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			39, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 20, ONE_SECOND / 30, /* RestartRate */
			(1 << 7), /* BlockMask */
		},
		{
			46, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 20, ONE_SECOND / 30, /* RestartRate */
			(1 << 6), /* BlockMask */
		},
		{
			52, /* StartIndex */
			4, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 20, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			56, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 20, ONE_SECOND / 30, /* RestartRate */
			(1 << 10), /* BlockMask */
		},
		{
			63, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 20, ONE_SECOND / 30, /* RestartRate */
			(1 << 9), /* BlockMask */
		},
		{
			69, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 30, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{	// JMS: Viewscreen animation is this one.
			72, /* StartIndex */
			14, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND * 4, /* RestartRate */
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
		4, /* NumFrames */
		0, /* AnimFlags */
		ONE_SECOND / 20, 0, /* FrameRate */
		ONE_SECOND / 15, 0, /* RestartRate */
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
	if (PLAYER_SAID (R, pearshaped))
	{
		NPCPhrase (KYAIEE);
		SET_GAME_STATE (SHOFIXTI_ANGRY, 2);
		SET_GAME_STATE (BATTLE_SEGUE, 1);
	}	
	else if (PLAYER_SAID (R, fairwell_shofixti))
	{
		if (GET_GAME_STATE(SHOFIXTI_ANGRY) > 0)
			NPCPhrase (MIFFED_GOODBYE_EARTHLING);
		else
			NPCPhrase (GOODBYE_EARTHLING);
		SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (PLAYER_SAID (R, will_attack))
	{
		NPCPhrase (WILL_ATTACK_TOO);
		SET_GAME_STATE (SHOFIXTI_ANGRY, 3);
		SET_GAME_STATE (BATTLE_SEGUE, 1);
	}	
	else if (PLAYER_SAID (R, sorry))
	{
		NPCPhrase (SORRY_ACCEPTED);
		SET_GAME_STATE (SHOFIXTI_ANGRY, 1);
		SET_GAME_STATE (BATTLE_SEGUE, 0);
	}

}

static void
ThankYou (RESPONSE_REF R)
{	
	NPCPhrase (THANK_YOU);
	DISABLE_PHRASE (sorry_to_hear);	

	Response (fairwell_shofixti, ExitConversation);

}

static void
HowReconstruction (RESPONSE_REF R)
{	
	NPCPhrase (NOT_GOOD_RECONSTRUCTION);
	DISABLE_PHRASE (how_goes_reconstruction);	

	Response (sorry_to_hear, ThankYou);
	Response (fairwell_shofixti, ExitConversation);
}

static void
SmallTalk2 (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, any_news))
	{
		NPCPhrase (NOT_MUCH_NEWS);
		DISABLE_PHRASE (any_news);
	}
	else if (PLAYER_SAID (R, how_goes_reconstruction))
	{
		NPCPhrase (NOT_GOOD_RECONSTRUCTION);
		DISABLE_PHRASE (how_goes_reconstruction);
	}
	
	if (PHRASE_ENABLED (any_news))
	{
		Response (any_news, SmallTalk2);
	}

	if (PHRASE_ENABLED (how_goes_reconstruction))
	{
		Response (how_goes_reconstruction, HowReconstruction);
	}


	Response (fairwell_shofixti, ExitConversation);
}



static void
SmallTalk1 (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, chmmr_hunt_kohrah))
	{
		NPCPhrase (SLAVE_SHIELD_BEST_PLACE);
	}
	else if (PLAYER_SAID (R, no_idea))
	{
		NPCPhrase (SHARE_NEWS);
	}

	Response (any_news, SmallTalk2);
	Response (how_goes_reconstruction, SmallTalk2);
	Response (fairwell_shofixti, ExitConversation);
}



static void
DoShofixtiAngry (RESPONSE_REF R)
{
	NPCPhrase (ANGRY_SHOFIXTI_GREETING_1);

	Response (sorry, ExitConversation);
	Response (will_attack, ExitConversation);
}


static void
Intro (void)
{

	BYTE NumVisits;
	
	if (GET_GAME_STATE (SHOFIXTI_ANGRY) == 1)
	{
		NumVisits = GET_GAME_STATE (SHOFIXTI_VISITS);
		switch (NumVisits++)
		{
			case 1:
				NPCPhrase (MIFFED_SHOFIXTI_GREETING_1);
				break;
			case 2:
				NPCPhrase (MIFFED_SHOFIXTI_GREETING_2);
				break;
			case 3:
				NPCPhrase (MIFFED_SHOFIXTI_GREETING_3);
				--NumVisits;
				break;
		}
		
		SmallTalk1 (0);
		SET_GAME_STATE (SHOFIXTI_VISITS, NumVisits);
	}
	else if (GET_GAME_STATE (SHOFIXTI_ANGRY) == 2)
		DoShofixtiAngry (0);
	else if (GET_GAME_STATE (SHOFIXTI_ANGRY) == 3)
	{
		SET_GAME_STATE (BATTLE_SEGUE, 1);
		NPCPhrase (ANGRY_SHOFIXTI_GREETING_2);
	}
	else
	{
		NumVisits = GET_GAME_STATE (SHOFIXTI_VISITS);
		switch (NumVisits++)
		{
			case 0:
				NPCPhrase (SHOFIXTI_GREETING_1);
				break;
			case 1:
				NPCPhrase (SHOFIXTI_GREETING_2);
				break;
			case 2:
				NPCPhrase (SHOFIXTI_GREETING_3);
				--NumVisits;
				break;
		}
		
		SET_GAME_STATE (SHOFIXTI_VISITS, NumVisits);
		
		if (GET_GAME_STATE(SHOFIXTI_MET) == 0)
		{
			SET_GAME_STATE(SHOFIXTI_MET, 1);
			Response (chmmr_hunt_kohrah, SmallTalk1);
			Response (no_idea, SmallTalk1);
			Response (pearshaped, ExitConversation);
			Response (fairwell_shofixti, ExitConversation);
		}
		else
			SmallTalk1 (0);
	}
}

static COUNT
uninit_shofixti (void)
{
	return(0);
}

static void
post_shofixti_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_shofixti_comm (void)
{
	LOCDATA *retval;

	shofixti_desc.init_encounter_func = Intro;
	shofixti_desc.post_encounter_func = post_shofixti_enc;
	shofixti_desc.uninit_encounter_func = uninit_shofixti;

	shofixti_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	shofixti_desc.AlienTextBaseline.y = 0;
	shofixti_desc.AlienTextWidth = SIS_TEXT_WIDTH;

	if (GET_GAME_STATE (SHOFIXTI_ANGRY) > 1)
		SET_GAME_STATE (BATTLE_SEGUE, 1);
	else
		SET_GAME_STATE (BATTLE_SEGUE, 0);

	retval = &shofixti_desc;

	return (retval);
}
