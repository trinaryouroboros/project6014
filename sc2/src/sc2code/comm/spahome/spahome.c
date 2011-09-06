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

#include "comm/commall.h"
#include "comm/spathi/resinst.h"
#include "comm/spahome/strings.h"

#include "build.h"
#include "gameev.h"


static LOCDATA spahome_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	SPATHI_HOME_PMAP_ANIM, /* AlienFrame */
	SPATHI_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	SPATHI_HOME_COLOR_MAP, /* AlienColorMap */
	SPATHI_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	SPATHI_HOME_CONVERSATION_PHRASES, /* PlayerPhrases */
	14, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{
			1, /* StartIndex */
			3, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			4, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			9, /* StartIndex */
			4, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 10) | (1 << 11), /* BlockMask */
		},
		{
			13, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND / 20, 0, /* RestartRate */
			(1 << 4) | (1 << 5) /* BlockMask */
		},
		{
			19, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 3) | (1 << 5), /* BlockMask */
		},
		{
			22, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 10, ONE_SECOND / 30, /* RestartRate */
			(1 << 3) | (1 << 4)
			| (1 << 10), /* BlockMask */
		},
		{
			26, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND * 10, ONE_SECOND * 3, /* RestartRate */
			(1 << 10), /* BlockMask */
		},
		{
			29, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND * 10, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			32, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND / 20, 0, /* RestartRate */
			(1 << 9) | (1 << 10), /* BlockMask */
		},
		{
			39, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 8) | (1 << 10), /* BlockMask */
		},
		{
			42, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, 0, /* RestartRate */
			(1 << 8) | (1 << 9)
			| (1 << 6) | (1 << 2)
			| (1 << 11) | (1 << 5), /* BlockMask */
		},
		{
			46, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 10, ONE_SECOND / 30, /* RestartRate */
			(1 << 2) | (1 << 10), /* BlockMask */
		},
		{
			50, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND / 20, 0, /* RestartRate */
			(1 << 13), /* BlockMask */
		},
		{
			56, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 12), /* BlockMask */
		},
	},
	{ /* AlienTransitionDesc - empty */
		0, /* StartIndex */
		0, /* NumFrames */
		0, /* AnimFlags */
		0, 0, /* FrameRate */
		0, 0, /* RestartRate */
		0, /* BlockMask */
	},
	{ /* AlienTalkDesc - empty */
		0, /* StartIndex */
		0, /* NumFrames */
		0, /* AnimFlags */
		0, 0, /* FrameRate */
		0, 0, /* RestartRate */
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
	if (PLAYER_SAID (R, huffi_muffi_guffi))
	{
		NPCPhrase (CYPHER_OBSOLETE);
		SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (PLAYER_SAID (R, gods_work))
	{
		NPCPhrase (GOD_KNOWS);
		SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (PLAYER_SAID (R, left_at_home))
	{
		NPCPhrase (GO_GET_CYPHER);
		SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
}



static void
Intro (void)
{
	if (GET_GAME_STATE (SPATHIHOME_MET) == 0)
	{
		//SET_GAME_STATE (SPATHIHOME_MET, 1);
		NPCPhrase (WHAT_IS_CYPHER);
	}
	Response (huffi_muffi_guffi, ExitConversation);
	Response (gods_work, ExitConversation);
	Response (left_at_home, ExitConversation);
}

static COUNT
uninit_spahome (void)
{
	return (0);
}

static void
post_spahome_enc (void)
{
	BYTE Manner;

	if (GET_GAME_STATE (BATTLE_SEGUE) == 1
			&& (Manner = GET_GAME_STATE (SPATHI_MANNER)) != 2)
	{
		SET_GAME_STATE (SPATHI_MANNER, 1);
		if (Manner != 1)
		{
			SET_GAME_STATE (SPATHI_VISITS, 0);
			SET_GAME_STATE (SPATHI_HOME_VISITS, 0);
		}
	}
}

LOCDATA*
init_spahome_comm ()
{
	LOCDATA *retval;

	spahome_desc.init_encounter_func = Intro;
	spahome_desc.post_encounter_func = post_spahome_enc;
	spahome_desc.uninit_encounter_func = uninit_spahome;

	spahome_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	spahome_desc.AlienTextBaseline.y = 0;
	spahome_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	// use alternate "Safe Ones" track if available
	spahome_desc.AlienAltSongRes = SPAHOME_MUSIC;
	spahome_desc.AlienSongFlags |= LDASF_USE_ALTERNATE;

	SET_GAME_STATE (BATTLE_SEGUE, 0);

	retval = &spahome_desc;

	return (retval);
}
