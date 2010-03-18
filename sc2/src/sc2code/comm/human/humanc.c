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

static void
ExitConversation (RESPONSE_REF R)
{
	SET_GAME_STATE (BATTLE_SEGUE, 0);

	if (PLAYER_SAID (R, yessiree))
	{
		SET_GAME_STATE (HUMAN_HOSTILE, 1);
		NPCPhrase (MADE_MY_DAY);
		SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (PLAYER_SAID (R, nonono))
	{
		NPCPhrase (MADE_WOOD);
		SET_GAME_STATE (HUMAN_HOSTILE, 0);
		SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
}

static void
Intro (void)
{
	BYTE NumVisits;
	
	if (!GET_GAME_STATE(HUMAN_MET))
	{
		SET_GAME_STATE(HUMAN_MET,1);
	}
	
	if (GET_GAME_STATE (HUMAN_HOSTILE))
	{
		NumVisits = GET_GAME_STATE (HUMAN_VISITS);
		switch (NumVisits++)
		{
			case 0:
				NPCPhrase (HOSTILE_SPACE_HELLO_1);
				break;
			case 1:
				NPCPhrase (HOSTILE_SPACE_HELLO_2);
				NumVisits=0;
				break;
		}
		SET_GAME_STATE (HUMAN_VISITS, NumVisits);
		Response (yessiree, ExitConversation);
		Response (nonono, ExitConversation);
	}
	
	else
	{
		NumVisits = GET_GAME_STATE (HUMAN_VISITS);
		switch (NumVisits++)
		{
			case 0:
				NPCPhrase (NEUTRAL_SPACE_HELLO_1);
				break;
			case 1:
				NPCPhrase (NEUTRAL_SPACE_HELLO_2);
				NumVisits=0;
				break;
		}
		SET_GAME_STATE (HUMAN_VISITS, NumVisits);
		Response (yessiree, ExitConversation);
		Response (nonono, ExitConversation);
	}
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

	human_desc.init_encounter_func = Intro;
	human_desc.post_encounter_func = post_human_enc;
	human_desc.uninit_encounter_func = uninit_human;

	human_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	human_desc.AlienTextBaseline.y = 0;
	human_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	
	retval = &human_desc;

	return (retval);
}
