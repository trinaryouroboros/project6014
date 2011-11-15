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

// JMS 2010: -Added viewscreen animu
//			 -Added some dialogue options
//			 -Lost shofixti patrols sidequest, one of 3 patrols returns home after 2 months

#include "../commall.h"
#include "resinst.h"
#include "strings.h"

#include "uqm/races.h"
#include "uqm/grpinfo.h"
#include "uqm/gameev.h"
#include "uqm/encount.h"
// BW: for EncounterGroup, no longer included in commall.h

#include "libs/mathlib.h" // for TFB_Random
#include "libs/log.h"


static LOCDATA shofixti_desc =
{
	SHOFIXTI_CONVERSATION, /* AlienConv */
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	SHOFIXTI_PMAP_ANIM, /* AlienFrame */
	SHOFIXTI_FONT, /* AlienFont */
	WHITE_COLOR_INIT, /* AlienTextFColor */
	BLACK_COLOR_INIT, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	SHOFIXTI_COLOR_MAP, /* AlienColorMap */
	SHOFIXTI_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	SHOFIXTI_CONVERSATION_PHRASES, /* PlayerPhrases */
	8, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{ /* 0 bottom left star */
			1, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 20, ONE_SECOND / 30, /* RestartRate */
			(1 << 2), /* BlockMask */
		},
		{ /* 1 bottom right star */
			7, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 20, ONE_SECOND / 30, /* RestartRate */
			(1 << 3), /* BlockMask */
		},
		{ /* 2 top left star */
			13, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 20, ONE_SECOND / 30, /* RestartRate */
			(1 << 0), /* BlockMask */
		},
		{ /* 3 top right star */
			18, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 20, ONE_SECOND / 30, /* RestartRate */
			(1 << 1), /* BlockMask */
		},
		{ /* 4 eye blink */
			23, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 30, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 5 right hand */
			26, /* StartIndex */
			3, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			(ONE_SECOND >> 1), (ONE_SECOND >> 1) * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 6 radar */
			34, /* StartIndex */
			8, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			ONE_SECOND / 10, 0, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* 7 left hand */
			42, /* StartIndex */
			3, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			(ONE_SECOND >> 1), (ONE_SECOND >> 1) * 3, /* RestartRate */
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
		29, /* StartIndex */
		5, /* NumFrames */
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
	0, /* NumFeatures */
	{{0, 0, {0}} /*AlienFeatureArray (alternative features) */
	},
	{0 /* AlienFeatureChoice (will be computed later) */
	},
};

static void
SmallTalk2 (RESPONSE_REF R);

static void
ExitConversation (RESPONSE_REF R)
{
	if (PLAYER_SAID (R, no_joke_attack))
	{
		NPCPhrase (KYAIEE);
		SET_GAME_STATE (SHOFIXTI_ANGRY, 2);
		SET_GAME_STATE (BATTLE_SEGUE, 1);
	}	
	else if (PLAYER_SAID (R, farewell_shofixti))
	{
		BYTE NumVisits = GET_GAME_STATE (SHOFIXTI_VISITS);
		
		if (GET_GAME_STATE(SHOFIXTI_ANGRY) > 0)
		{
			NPCPhrase (MIFFED_GOODBYE_EARTHLING);
		}
		else
		{
			switch (NumVisits++)
			{
				case 1:
					NPCPhrase (GOODBYE_EARTHLING_1);
					break;
				case 2:
					NPCPhrase (GOODBYE_EARTHLING_2);
					break;
				case 3:
					NPCPhrase (GOODBYE_EARTHLING_3);
					break;
				case 4:
					NPCPhrase (GOODBYE_EARTHLING_4);
					break;
			}
		}
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
HowReconstruction (RESPONSE_REF R)
{	
	BYTE ThisGroupIsReligious;
	
	(void) R; // satisfy compiler
	
	if (LOBYTE (GLOBAL (CurrentActivity)) == IN_INTERPLANETARY)
		ThisGroupIsReligious = EncounterGroup % 2;
	else
		ThisGroupIsReligious = TFB_Random () % 2;
	
	if (GET_GAME_STATE(SHOFIXTI_ANGRY) > 0)
		NPCPhrase (NOT_GOOD_RECONSTRUCTION_PRE_MIFFED);
	else
		NPCPhrase (NOT_GOOD_RECONSTRUCTION_PRE);
	
	if (ThisGroupIsReligious)
	{
		NPCPhrase (RECONSTRUCTION_KISHIBOJIN); // Religious faction
		SET_GAME_STATE(SHOFIXTI_RELIGIOUS_MET, 1);
	}
	else
	{
		NPCPhrase (RECONSTRUCTION_TECHNOLOGY); // Tech faction
		SET_GAME_STATE(SHOFIXTI_TECH_MET, 1);
	}

	DISABLE_PHRASE (how_goes_reconstruction);	

	if (ThisGroupIsReligious)
		Response (who_is_kishibojin, SmallTalk2);
	else
		Response (sorry_to_hear, SmallTalk2);
	
	if (PHRASE_ENABLED (ask_scar))
		Response (ask_scar, SmallTalk2);
	
	Response (farewell_shofixti, ExitConversation);
}

static void
SmallTalk2 (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, sorry_to_hear))
	{
		if (GET_GAME_STATE(SHOFIXTI_ANGRY) > 0)
			NPCPhrase (THANK_YOU_MIFFED);
		else
			NPCPhrase (THANK_YOU);
		
		DISABLE_PHRASE (sorry_to_hear);
	}	
	else if (PLAYER_SAID (R, who_is_kishibojin))
	{
		NPCPhrase (SHE_IS_KISHIBOJIN);

		DISABLE_PHRASE (who_is_kishibojin);
	}
	else if (PLAYER_SAID (R, you_talk_differently_tech))
	{
		NPCPhrase (YOU_MET_RELIGIOUS);

		DISABLE_PHRASE (you_talk_differently_tech);
	}
	else if (PLAYER_SAID (R, you_talk_differently_religious))
	{
		NPCPhrase (YOU_MET_NONBELIEVERS);
		
		DISABLE_PHRASE (you_talk_differently_religious);
	}
	
	else if (PLAYER_SAID (R, where_patrol)
		|| PLAYER_SAID (R, where_patrol_2))
	{
		if (PLAYER_SAID (R, where_patrol_2))
			NPCPhrase (HERE_GOES);
			
		NPCPhrase (LOST_PATROLS);
		
		if (GET_GAME_STATE(SHOFIXTI_PATROL_RETURNED))
			NPCPhrase (TWO_PATROLS_MISSING);
		else
			NPCPhrase (THREE_PATROLS_MISSING);
		
		if (PLAYER_SAID (R, where_patrol))
		{
			DISABLE_PHRASE (where_patrol);
			DISABLE_PHRASE (where_patrol_2);
		}
		else if (PLAYER_SAID (R, where_patrol_2))
			DISABLE_PHRASE (where_patrol_2);
	}
	
	else if (PLAYER_SAID (R, why_not_call))
	{
		NPCPhrase (NO_RESOURCES_TO_CALL);
		
		if (GET_GAME_STATE(TRIANGULATION_SPHERES_SHOFIXTI))
			NPCPhrase (YOU_HAVE_COORDINATES);
		else
			NPCPhrase (TRANSFER_COORDINATES);
	
		if(!(GET_GAME_STATE(TRIANGULATION_SPHERES_SHOFIXTI)))
		{
			// JMS: Trigger one of the lost patrols to come home after 2 months
			AddEvent (RELATIVE_EVENT, 2, 0, 0, SHOFIXTI_PATROL_RETURNS_HOME_EVENT);
			SET_GAME_STATE (TRIANGULATION_SPHERES_SHOFIXTI, 1);
		}
		
		if (GET_GAME_STATE(SHOFIXTI_PATROL_RETURNED))
			NPCPhrase (ONLY_TWO_PATROLS);
		
		DISABLE_PHRASE (why_not_call);
	}
	else if (PLAYER_SAID (R, ask_scar))
	{
		NPCPhrase (ANSWER_SCAR);
		DISABLE_PHRASE (ask_scar);
	}
	
	/* Player's answers */
	
	if (PHRASE_ENABLED (where_patrol) && !(GET_GAME_STATE(TRIANGULATION_SPHERES_SHOFIXTI)))
		Response (where_patrol, SmallTalk2);
				 
	if (PHRASE_ENABLED (where_patrol_2) && GET_GAME_STATE(TRIANGULATION_SPHERES_SHOFIXTI))
		Response (where_patrol_2, SmallTalk2);
	
	if ((PLAYER_SAID (R, where_patrol) || PLAYER_SAID (R, where_patrol_2)) && PHRASE_ENABLED (why_not_call))
		Response (why_not_call, SmallTalk2);

	if (PHRASE_ENABLED (how_goes_reconstruction))
		Response (how_goes_reconstruction, HowReconstruction);
	
	if (PLAYER_SAID (R, who_is_kishibojin) && PHRASE_ENABLED (you_talk_differently_tech) && GET_GAME_STATE(SHOFIXTI_TECH_MET))
		Response (you_talk_differently_religious, SmallTalk2);
	else if (PLAYER_SAID (R, sorry_to_hear) && PHRASE_ENABLED (you_talk_differently_religious) && GET_GAME_STATE(SHOFIXTI_RELIGIOUS_MET))
		Response (you_talk_differently_tech, SmallTalk2);

	if (PHRASE_ENABLED (ask_scar))
		Response (ask_scar, SmallTalk2);
	
	Response (farewell_shofixti, ExitConversation);
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
	else if (PLAYER_SAID (R, take_a_joke))
	{
		NPCPhrase (DONT_JOKE);
	}

	if (!(GET_GAME_STATE(TRIANGULATION_SPHERES_SHOFIXTI)))
		Response (where_patrol, SmallTalk2);
	else
		Response (where_patrol_2, SmallTalk2);
	Response (how_goes_reconstruction, HowReconstruction);
	Response (ask_scar, SmallTalk2);
	Response (farewell_shofixti, ExitConversation);
}



static void
DoShofixtiAngry (RESPONSE_REF R)
{
	(void) R; // satisfy compiler
	NPCPhrase (ANGRY_SHOFIXTI_GREETING_1);

	Response (sorry, ExitConversation);
	Response (will_attack, ExitConversation);
}

static void
LastChanceToBackUp (RESPONSE_REF R)
{
	(void) R; // satisfy compiler
	NPCPhrase (WHAT);
	
	Response (take_a_joke, SmallTalk1);
	Response (no_joke_attack, ExitConversation);
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
				break;
			case 4:
				NPCPhrase (MIFFED_SHOFIXTI_GREETING_4);
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
		if (GET_GAME_STATE(SHOFIXTI_PATROL_RETURNED) && !(GET_GAME_STATE(SHOFIXTI_GREAT_NEWS_HEARD)))
		{
			NPCPhrase (GREAT_NEWS);
			SET_GAME_STATE(SHOFIXTI_GREAT_NEWS_HEARD, 1);
		}
		else 
		{
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
					break;
				case 3:
					NPCPhrase (SHOFIXTI_GREETING_4);
					break;
				case 4:
					NPCPhrase (SHOFIXTI_GREETING_4);
					--NumVisits;
					break;
			}
		}
		
		SET_GAME_STATE (SHOFIXTI_VISITS, NumVisits);
		
		if (GET_GAME_STATE(SHOFIXTI_MET) == 0)
		{
			SET_GAME_STATE(SHOFIXTI_MET, 1);
			Response (chmmr_hunt_kohrah, SmallTalk1);
			Response (no_idea, SmallTalk1);
			Response (pearshaped, LastChanceToBackUp);
			Response (farewell_shofixti, ExitConversation);
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
	shofixti_desc.AlienTextWidth = SIS_TEXT_WIDTH - 4;

	if (GET_GAME_STATE (SHOFIXTI_ANGRY) > 1)
		SET_GAME_STATE (BATTLE_SEGUE, 1);
	else
		SET_GAME_STATE (BATTLE_SEGUE, 0);

	retval = &shofixti_desc;

	return (retval);
}
