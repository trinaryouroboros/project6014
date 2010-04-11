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

// JMS 2010:	-New dialogue
//				-Mechanism that allows for male/female captains in comm screens

#include "../commall.h"
#include "resinst.h"
#include "strings.h"
#include "libs/mathlib.h"

#include "build.h"


static LOCDATA human_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	HUMAN_PMAP_ANIM, /* AlienFrame */
	HUMAN_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	HUMAN_COLOR_MAP, /* AlienColorMap */
	HUMAN_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	HUMAN_CONVERSATION_PHRASES, /* PlayerPhrases */
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

static LOCDATA human_desc2 =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	HUMAN_PMAP_ANIM2, /* AlienFrame */
	HUMAN_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	HUMAN_COLOR_MAP2, /* AlienColorMap */
	HUMAN_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	HUMAN_CONVERSATION_PHRASES, /* PlayerPhrases */
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
NukesConversation (RESPONSE_REF R);

static void
ExitConversation (RESPONSE_REF R)
{
	BYTE NumVisits;
	
	SET_GAME_STATE (BATTLE_SEGUE, 0);
	
	NumVisits= GET_GAME_STATE (HUMAN_VISITS);
	
	switch (NumVisits++)
	{
		case 0:
			NPCPhrase (GOODBYE_CAPTAIN_1);
			break;
		case 1:
			NPCPhrase (GOODBYE_CAPTAIN_2);
			break;
		case 2:
			NPCPhrase (GOODBYE_CAPTAIN_3);
			--NumVisits;
			--NumVisits;
			break;
	}
	
	SET_GAME_STATE (HUMAN_VISITS, NumVisits);
}

static void
HumanConversationMain (RESPONSE_REF R)
{
	BYTE NumVisits;
	
	if (PLAYER_SAID (R, ask_news))
	{
		NumVisits = GET_GAME_STATE (HUMAN_NEWS);
		switch (NumVisits++)
		{
			case 0:
				NPCPhrase (NEWS_1);
				break;
			case 1:
				NPCPhrase (NEWS_2);
				break;
			case 2:
				NPCPhrase (NEWS_3);
				break;
			case 3:
				NPCPhrase (NEWS_4);
				break;
			case 4:
				NPCPhrase (NEWS_5);
				NumVisits--;
				break;
		}
		
		SET_GAME_STATE (HUMAN_NEWS, NumVisits);
		DISABLE_PHRASE (ask_news);
	}
	
	if (PLAYER_SAID (R, short_on_fuel))
	{
		NumVisits = GET_GAME_STATE (HUMAN_FUEL_INFO);
		switch (NumVisits++)
		{
			case 0:
				NPCPhrase (FUEL_1);
				break;
			case 1:
				NPCPhrase (FUEL_2);
				break;
			case 2:
				NPCPhrase (FUEL_3);
				NumVisits=0;
				break;
		}
		
		SET_GAME_STATE (HUMAN_FUEL_INFO, NumVisits);
		DISABLE_PHRASE (short_on_fuel);
	}
	
	if (PLAYER_SAID (R, serious) || PLAYER_SAID (R, joking))
	{
		if (PLAYER_SAID (R, serious))
			NPCPhrase (NUKES_SERIOUS);
		if (PLAYER_SAID (R, joking))
			NPCPhrase (NUKES_JOKING);
		
		SET_GAME_STATE (HUMAN_NUKES_DONE, 1);
	}
	
	if (PHRASE_ENABLED (ask_news))
	{
		Response (ask_news, HumanConversationMain);
	}
	
	if (PHRASE_ENABLED (short_on_fuel))
	{
		Response (short_on_fuel, HumanConversationMain);
	}
	
	if (PHRASE_ENABLED (spare_nukes) && !GET_GAME_STATE (HUMAN_NUKES_DONE))
	{
		Response (spare_nukes, NukesConversation);
	}
	
	Response (keep_up_goodbye, ExitConversation);
}

static void
NukesConversation (RESPONSE_REF R)
{
	if (PLAYER_SAID (R, spare_nukes))
	{
		NPCPhrase (NUKES_QUESTION);
		Response (serious, HumanConversationMain);
		Response (joking, HumanConversationMain);
		
		DISABLE_PHRASE (spare_nukes);
	}
}

static void
Intro (void)
{
	BYTE NumVisits;
	
	NumVisits = GET_GAME_STATE (HUMAN_VISITS);
	switch (NumVisits)
	{
		case 0:
			NPCPhrase (SPACE_HELLO_1);
			break;
		case 1:
			NPCPhrase (SPACE_HELLO_2);
			break;
		case 2:
			NPCPhrase (SPACE_HELLO_3);
			break;
	}
	
	HumanConversationMain ((RESPONSE_REF)0);

}

static COUNT
uninit_human (void)
{
	return (0);
}

static void
post_human_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_human_comm (void)
{
	LOCDATA *retval;
	BOOLEAN male; // JMS: Draw either male or female earthling captain
	
	SET_GAME_STATE (BATTLE_SEGUE, 0);
	
	// JMS: In hyperspace just pick random male/female graphics
	// This does pose a problem if the hyperspace blip is fought, escaped from and encountered again:
	// It might then get different random number and have captain of different gender...
	if(LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
		male=(TFB_Random()%2);
	// JMS: In Interplanetary pick graphics based on battle group index number.
	// This way the same ship has always the same captain as long as it exists.
	else
		male=(EncounterGroup%2);

	
	 // JMS: Male earthling gfx
	if(male)
	{
		human_desc.init_encounter_func = Intro;
		human_desc.post_encounter_func = post_human_enc;
		human_desc.uninit_encounter_func = uninit_human;

		human_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
		human_desc.AlienTextBaseline.y = 0;
		human_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

		retval = &human_desc;
	}
	// JMS: Female earthling gfx
	else
	{
		human_desc2.init_encounter_func = Intro;
		human_desc2.post_encounter_func = post_human_enc;
		human_desc2.uninit_encounter_func = uninit_human;
	
		human_desc2.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
		human_desc2.AlienTextBaseline.y = 0;
		human_desc2.AlienTextWidth = SIS_TEXT_WIDTH - 16;
	
		retval = &human_desc2;
	}

	return (retval);
}
