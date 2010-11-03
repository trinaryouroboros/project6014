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

// JMS 2010: -Completely new file for transport ship dialogue. Lot of randomly picked lines here.
//			  When asking about cargo, the same ship can have many kinds of crap aboard then. Kind of buggy but funny :D

#include "../commall.h"
#include "igfxres.h"
#include "ifontres.h"
#include "imusicre.h"
#include "istrtab.h"
#include "resinst.h"
#include "strings.h"

#include "libs/mathlib.h"

#include "build.h"


static LOCDATA transport_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	TRANSPORT_PMAP_ANIM, /* AlienFrame */
	TRANSPORT_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	TRANSPORT_COLOR_MAP, /* AlienColorMap */
	TRANSPORT_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	TRANSPORT_CONVERSATION_PHRASES, /* PlayerPhrases */
	10, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{ /* Blink */
			1, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND, (ONE_SECOND / 10) * 22, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* Captain's "Picard Tug" */
			11, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 6, 0, /* FrameRate */
			ONE_SECOND * 4, ONE_SECOND * 5, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* Big Ceiling Monitor */
			13, /* StartIndex */
			125, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 1, ONE_SECOND * 4, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* Small Monitor - left */
			138, /* StartIndex */
			94, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 1, ONE_SECOND * 2, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* Small Monitor - Right */
			232, /* StartIndex */
			26, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 12, 0, /* FrameRate */
			ONE_SECOND * 1, ONE_SECOND * 1, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* Green Pulsing Stuff */
			258, /* StartIndex */
			18, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			0, 0, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* Back Wall Computer */
			276, /* StartIndex */
			12, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			0, 0, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* Guy in the Back */
			288, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 12, 0, /* FrameRate */
			(ONE_SECOND / 10) * 2, (ONE_SECOND / 10) * 13, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* Woman's Left Arm */
			291, /* StartIndex */
			3, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 12, 0, /* FrameRate */
			ONE_SECOND / 12, (ONE_SECOND / 12) * 15, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* Woman's Right Arm */
			294, /* StartIndex */
			3, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 12, 0, /* FrameRate */
			ONE_SECOND / 12, (ONE_SECOND / 12) * 15, /* RestartRate */
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
		7, /* NumFrames */
		RANDOM_ANIM, /* AnimFlags */
		ONE_SECOND / 12, ONE_SECOND / 15, /* FrameRate */
		ONE_SECOND * 7 / 60, ONE_SECOND / 12, /* RestartRate */
		0, /* BlockMask */
	},
	NULL, /* AlienNumberSpeech - none */
	/* Filler for loaded resources */
	NULL, NULL, NULL,
	NULL,
	NULL,
	0, /* NumFeatures */
	{ /*AlienFeaturesArray (alternative features) */
	},
	{ /* AlienFeatureChoice (will be computed later) */
	},
};

static void
ExitConversation (RESPONSE_REF R)
{
	BYTE NumVisits;
	
	SET_GAME_STATE (BATTLE_SEGUE, 0);
	
	NPCPhrase(GOOD_BYE);
}

static void
TransportConversationMain (RESPONSE_REF R)
{
	BYTE NumVisits;
	
	if (PLAYER_SAID (R, whats_up))
	{
		COUNT phrase;
		NumVisits = GET_GAME_STATE (TRANSPORT_SHIP_0_STATUS);
		
		switch (NumVisits)
		{
			case 0:
				if(CurStarDescPtr->Index==SOL_DEFINED)
				{
					phrase=WAITING_FOR_LEAVE_1;
					phrase+=(COUNT)TFB_Random () % 3;
				}
				else
				{
					phrase=ARRIVED_1;
					phrase+=(COUNT)TFB_Random () % 2;
				}
				
				NPCPhrase (phrase);
				if(CurStarDescPtr->Index==CHMMR_DEFINED)
				{
					phrase=AT_PROCYON_1;
					phrase+=(COUNT)TFB_Random () % 2;
					NPCPhrase (phrase);
				}
				break;
			case 1:
				phrase=LEAVING_1;
				phrase+=(COUNT)TFB_Random () % 2;
				NPCPhrase (phrase);
				break;
			case 2:
				phrase=HYPERSPACE_1;
				phrase+=(COUNT)TFB_Random () % 2;
				NPCPhrase (phrase);
				break;
			case 3:
				phrase=HYPERSPACE_1;
				phrase+=(COUNT)TFB_Random () % 2;
				NPCPhrase (phrase);
				break;
			case 4:
				phrase=ARRIVING_1;
				phrase+=(COUNT)TFB_Random () % 2;
				NPCPhrase (phrase);
				break;
		}
		
		DISABLE_PHRASE (whats_up);
	}
	
	if (PLAYER_SAID (R, what_cargo))
	{
		COUNT phrase;
		
		phrase= CARGO_1 + (COUNT)TFB_Random () % 12;
		NPCPhrase(CARGO_INIT);
		NPCPhrase(phrase);
		
		DISABLE_PHRASE (what_cargo);
	}
	
	if (PLAYER_SAID (R, whats_your_schedule))
	{
		COUNT phrase;
		
		phrase= SCHEDULE_1 + (COUNT)TFB_Random () % 2;
		NPCPhrase(phrase);
		
		DISABLE_PHRASE (whats_your_schedule);
	}
	
	if (PHRASE_ENABLED (whats_up))
	{
		Response (whats_up, TransportConversationMain);
	}
	
	if (PHRASE_ENABLED (whats_your_schedule))
	{
		Response (whats_your_schedule, TransportConversationMain);
	}
	
	if (PHRASE_ENABLED (what_cargo))
	{
		Response (what_cargo, TransportConversationMain);
	}
	
	Response (good_bye_safe_trip, ExitConversation);
}

static void
Intro (void)
{
	BYTE NumVisits;
	
	NumVisits = (COUNT)TFB_Random () % 4;
	switch (NumVisits)
	{
		case 0:
			NPCPhrase (GREETINGS_1);
			break;
		case 1:
			NPCPhrase (GREETINGS_2);
			break;
		case 2:
			NPCPhrase (GREETINGS_3);
			break;
		case 3:
			NPCPhrase (GREETINGS_4);
			break;
	}
	
	TransportConversationMain ((RESPONSE_REF)0);

}

static COUNT
uninit_transport (void)
{
	return (0);
}

static void
post_transport_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_transport_comm (void)
{
	LOCDATA *retval;
	
	SET_GAME_STATE (BATTLE_SEGUE, 0);
	
	transport_desc.init_encounter_func = Intro;
	transport_desc.post_encounter_func = post_transport_enc;
	transport_desc.uninit_encounter_func = uninit_transport;

	transport_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	transport_desc.AlienTextBaseline.y = 0;
	transport_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	retval = &transport_desc;
	
	return (retval);
}
