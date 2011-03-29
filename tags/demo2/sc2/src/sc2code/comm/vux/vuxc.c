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
#include "comm/vux/resinst.h"
#include "comm/vux/strings.h"

static LOCDATA vux_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	VUX_PMAP_ANIM, /* AlienFrame */
	VUX_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* (SIS_TEXT_WIDTH - 16) >> 1, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	VUX_COLOR_MAP, /* AlienColorMap */
	VUX_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	VUX_CONVERSATION_PHRASES, /* PlayerPhrases */
	17, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{
			12, /* StartIndex */
			3, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			15, /* StartIndex */
			5, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			20, /* StartIndex */
			14, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 30, 0, /* FrameRate */
			ONE_SECOND / 30, 0, /* RestartRate */
			0, /* BlockMask */
		},
		{
			34, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			41, /* StartIndex */
			6, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			47, /* StartIndex */
			11, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			58, /* StartIndex */
			3, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			61, /* StartIndex */
			4, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			65, /* StartIndex */
			4, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			69, /* StartIndex */
			2, /* NumFrames */
			RANDOM_ANIM, /* AnimFlags */
			ONE_SECOND / 30, ONE_SECOND / 30, /* FrameRate */
			ONE_SECOND / 30, ONE_SECOND / 30, /* RestartRate */
			0, /* BlockMask */
		},
		{
			71, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			74, /* StartIndex */
			6, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND / 15, ONE_SECOND / 15, /* RestartRate */
			0, /* BlockMask */
		},
		{
			80, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND / 15, ONE_SECOND / 15, /* RestartRate */
			(1 << 14), /* BlockMask */
		},
		{
			85, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND / 15, ONE_SECOND / 15, /* RestartRate */
			0, /* BlockMask */
		},
		{
			90, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND / 15, ONE_SECOND / 15, /* RestartRate */
			(1 << 12), /* BlockMask */
		},
		{
			95, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND * 5, ONE_SECOND * 5,/* RestartRate */
			0, /* BlockMask */
		},
		{
			99, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND * 5, ONE_SECOND * 5,/* RestartRate */
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
		11, /* NumFrames */
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
ExitConversation (RESPONSE_REF R)
{
	(void) R; // satisfy compiler
	NPCPhrase (GOODBYE_EARTHLING);
	SET_GAME_STATE (BATTLE_SEGUE, 0);

}


static void
HumanSubversion (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, safe_under_shield))
	{
		NPCPhrase (VALUE_FREEDOM);
		DISABLE_PHRASE (safe_under_shield);
	}
	

}

static void
AskMenu1 (RESPONSE_REF R)
{	
	/* Alien speech */



	if (PLAYER_SAID (R, whats_news))
	{
		NPCPhrase (DAX_INFORMATION);
		DISABLE_PHRASE (whats_news);
	}
	else if (PLAYER_SAID (R, how_is_slave_shield))
	{
		NPCPhrase (SPITEFUL_VUX);
		DISABLE_PHRASE (how_is_slave_shield);

	Response (safe_under_shield, HumanSubversion);
	}
	
	
	

	/* Human speech options */
	
	if (PHRASE_ENABLED (whats_news))
	{
		Response (whats_news, AskMenu1);
	}

    if (PHRASE_ENABLED (how_is_slave_shield))
	{
		Response (how_is_slave_shield, AskMenu1);
	}
	
	if (PHRASE_ENABLED (goodbye_vux))
	{
		Response (goodbye_vux, ExitConversation);
	}
}



static void
Intro (void)
{
	if (GET_GAME_STATE (VUX_MET) == 0)
	{
		SET_GAME_STATE (VUX_MET, 1);
	}

	NPCPhrase (VUX_GREETING1);


	Response (whats_news, AskMenu1);
	Response (how_is_slave_shield, AskMenu1);
	Response (goodbye_vux, ExitConversation);
}


static COUNT
uninit_vux (void)
{
	return (0);
}

static void
post_vux_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_vux_comm (void)
{
	LOCDATA *retval;

	vux_desc.init_encounter_func = Intro;
	vux_desc.post_encounter_func = post_vux_enc;
	vux_desc.uninit_encounter_func = uninit_vux;

	vux_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1)
			+ (SIS_TEXT_WIDTH >> 2);
	vux_desc.AlienTextBaseline.y = 0;
	vux_desc.AlienTextWidth = (SIS_TEXT_WIDTH - 16) >> 1;

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	retval = &vux_desc;

	return (retval);
}
