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
#include "libs/mathlib.h"

#include "uqm/build.h"
#include "uqm/gameev.h"

#include "uqm/conversation.h"



static LOCDATA pkunk_desc =
{
	PKUNK_CONVERSATION, /* AlienConv */
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	PKUNK_PMAP_ANIM, /* AlienFrame */
	PKUNK_FONT, /* AlienFont */
	WHITE_COLOR_INIT, /* AlienTextFColor */
	BLACK_COLOR_INIT, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	PKUNK_COLOR_MAP, /* AlienColorMap */
	PKUNK_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	PKUNK_CONVERSATION_PHRASES, /* PlayerPhrases */
	3, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{
			3, /* StartIndex */
			4, /* NumFrames */
			CIRCULAR_ANIM
					| WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 30, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			7, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM
					| WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 2), /* BlockMask */
		},
		{
			11, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM
					| WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 1), /* BlockMask */
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
		2, /* NumFrames */
		0, /* AnimFlags */
		ONE_SECOND / 15, ONE_SECOND / 6, /* FrameRate */
		ONE_SECOND / 12, ONE_SECOND / 2, /* RestartRate */
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


BOOLEAN heardAdapt = FALSE;

static void Court (RESPONSE_REF R);
static void PkunkMain (RESPONSE_REF R);

static void
ExitConversation (RESPONSE_REF R)
{
	static BOOLEAN firstSpanish = TRUE;
		
	SET_GAME_STATE (BATTLE_SEGUE, 0);
	if (PLAYER_SAID (R, spanish))
	{
		if (firstSpanish)
		{
			firstSpanish = FALSE;
			NPCPhrase(ADIOS1);
		}
		else
			NPCPhrase(ADIOS2);
	}
	else if (PLAYER_SAID (R, later))
		NPCPhrase (MMM_EXIT);
}

static void Guidance (RESPONSE_REF R)
{
	if (PLAYER_SAID (R, thanks))
	{
		DISABLE_PHRASE (thanks);
		NPCPhrase(GUIDANCE);
	}
	
	if (PHRASE_ENABLED(coming_out))
		Response(coming_out, PkunkMain);
	if (PHRASE_ENABLED(ouija))
		Response(ouija, PkunkMain);
	if (PHRASE_ENABLED(glad))
		Response(glad, PkunkMain);
}

static void Humor (RESPONSE_REF R)
{
	(void) R;
	if (PHRASE_ENABLED(wtf_joke))
		Response (wtf_joke, PkunkMain);
	if (PHRASE_ENABLED(brick))
		Response (brick, PkunkMain);
	if (PHRASE_ENABLED(delivery))
		Response (delivery, Court);
	if (PHRASE_ENABLED(important))
		Response (important, PkunkMain);
}

static void Joke (RESPONSE_REF R)
{
	if (PLAYER_SAID (R, astral))
	{
		DISABLE_PHRASE (astral);
		heardAdapt = TRUE;
		NPCPhrase(PHOTOGRAPHS);
	}

	NPCPhrase(JOKE);
	SET_GAME_STATE (PKUNK_JOKE, 1);
	Humor (R);
}

static void PkunkMain (RESPONSE_REF R)
{
	if (PLAYER_SAID (R, did_we_speak))
	{
		DISABLE_PHRASE (did_we_speak);
		NPCPhrase (ONE_AS_ALL);
	}
	else if (PLAYER_SAID (R, how_queen))
	{
		DISABLE_PHRASE (how_queen);
		NPCPhrase (REBEL_RANT);
	}
	else if (PLAYER_SAID (R, nafs))
	{
		DISABLE_PHRASE (nafs);
		NPCPhrase (WAY_BETTER);
	}
	else if (PLAYER_SAID (R, need_help))
	{
		DISABLE_PHRASE (need_help);
		NPCPhrase (COSMOLOGY);
	}
	else if (PLAYER_SAID (R, interesting))
	{
		DISABLE_PHRASE (interesting);
		NPCPhrase (MELTINGPOT);
	}
	else if (PLAYER_SAID (R, sorry))
	{
		NPCPhrase (WORDS);
	}
	else if (PLAYER_SAID (R, wtf_joke))
	{
		DISABLE_PHRASE (wtf_joke);
		NPCPhrase (PHILO_JOKE);
	}
	else if (PLAYER_SAID (R, brick))
	{
		DISABLE_PHRASE (brick);
		NPCPhrase (YOU_KNOW_ALL);
	}
	else if (PLAYER_SAID (R, important))
	{
		DISABLE_PHRASE (important);
		NPCPhrase (BUSINESS);
	}
	else if (PLAYER_SAID (R, coming_out))
	{
		DISABLE_PHRASE (coming_out);
		NPCPhrase (YOU_WERE_RIGHT);
	}
	else if (PLAYER_SAID (R, ouija))
	{
		DISABLE_PHRASE (ouija);
		NPCPhrase (EXCUSE);
		if (GET_GAME_STATE(PKUNK_JOKE) == 0)
			Joke (R);
	}
	else if (PLAYER_SAID (R, glad))
	{
		DISABLE_PHRASE (glad);
		NPCPhrase (WELL_MEANT);
	}

	if (PHRASE_ENABLED (thanks))
		Response (thanks, Guidance);
	if (PHRASE_ENABLED (how_queen))
		Response (how_queen, PkunkMain);
	if (PHRASE_ENABLED (how_is_it))
	    Response (how_is_it, Court);
	else if (PHRASE_ENABLED (court))
		Response (court, Court);
	if (PHRASE_ENABLED (nafs))
		Response (nafs, PkunkMain);
	if (PHRASE_ENABLED (need_help))
		Response (need_help, PkunkMain);
	if (PHRASE_ENABLED (spanish))
		Response (spanish, ExitConversation);
}

static void Court (RESPONSE_REF R)
{
	static BOOLEAN busy = FALSE;
	if (PLAYER_SAID (R, how_is_it))
	{
		NPCPhrase (SYMBOL);
		DISABLE_PHRASE(how_is_it);
	}
	else if (PLAYER_SAID (R, court))
	{
		NPCPhrase (CANT);
		DISABLE_PHRASE(court);
	}
	else if (PLAYER_SAID (R, nicee))
	{
		DISABLE_PHRASE (nicee);
		NPCPhrase (ADAPT);
	}
	else if (PLAYER_SAID (R, queen_different))
	{
		DISABLE_PHRASE (queen_different);
		heardAdapt = TRUE;
		busy = TRUE;
		NPCPhrase (ADAPT2);
	}
	else if (PLAYER_SAID (R, court_troubles))
	{
		DISABLE_PHRASE (court_troubles);
		busy = TRUE;
		NPCPhrase (COURT_SHIT);
	}
	else if (PLAYER_SAID (R, me_help))
	{
		DISABLE_PHRASE (me_help);
		NPCPhrase (SAVE_WORLD);
	}
	else if (PLAYER_SAID (R, you_busy))
	{
		DISABLE_PHRASE (you_busy);
		NPCPhrase (ME_BUSY);
	}
	else if (PLAYER_SAID (R, delivery))
	{
		DISABLE_PHRASE (delivery);
		NPCPhrase (THINGS_NOT_RIGHT);
	}
	
	if (!heardAdapt)
		Response(queen_different, Court);
	if (PHRASE_ENABLED (court_troubles))
		Response (court_troubles, Court);
	if (PHRASE_ENABLED (me_help))
		Response (me_help, Court);
	if ((busy) && (PHRASE_ENABLED (you_busy)))
		Response (you_busy, Court);
	if (PHRASE_ENABLED (interesting))
		Response (interesting, PkunkMain);
	else
		Response (sorry, PkunkMain);
}

static void Mmmm (RESPONSE_REF R)
{
	static BOOLEAN asked = FALSE;

	if (PLAYER_SAID (R, uh_hi))
	{
		DISABLE_PHRASE (uh_hi);
		asked = TRUE;
		NPCPhrase (MMM);
	}
	else if (PLAYER_SAID (R, yes_mmm))
	{
		DISABLE_PHRASE (yes_mmm);
		NPCPhrase (MMMUUU);
	}
	else if (PLAYER_SAID (R, see_me))
	{
		DISABLE_PHRASE (see_me);
		NPCPhrase (MMMMMM);
	}
	else if (PLAYER_SAID (R, mm))
	{
		DISABLE_PHRASE (mm);
		NPCPhrase (MREORM);
	}
	
	
	if (PHRASE_ENABLED (uh_hi))
		Response (uh_hi, Mmmm);
	if ((asked) && (PHRASE_ENABLED (yes_mmm)))
		Response (yes_mmm, Mmmm);
	if (PHRASE_ENABLED (see_me))
		Response (see_me, Mmmm);
	if (PHRASE_ENABLED (mm))
		Response (mm, Mmmm);
	Response (later, ExitConversation);
}

static void Orly (RESPONSE_REF R)
{
	(void) R;
	if (PHRASE_ENABLED (did_we_speak))
		Response (did_we_speak, PkunkMain);
	if (PHRASE_ENABLED (astral))
		Response (astral, Joke);
	if (PHRASE_ENABLED (nicee))
		Response (nicee, Court);
}


static void
Intro (void)
{
	if (GET_GAME_STATE (PKUNK_MANNER) == 0)
	{
		NPCPhrase (WONDERFUL);
		Orly ((RESPONSE_REF)0);
	}
	else
	{
		switch (TFB_Random() % 3)
		{
		case 0:
			NPCPhrase(MMMM);
			Mmmm ((RESPONSE_REF)0);
			break;
		case 1:
			NPCPhrase(WELCOME_BACK);
			Court ((RESPONSE_REF)0);
			break;
		case 2:
		default:
			NPCPhrase(HOWDY);
			PkunkMain ((RESPONSE_REF)0);
			break;
		}
	}
}

// Called after combat or communications
static COUNT
uninit_pkunk (void)
{
	return (0);
}

static void
post_pkunk_enc (void)
{

}

LOCDATA*
init_pkunk_comm (void)
{
	LOCDATA *retval;

	pkunk_desc.init_encounter_func = cm_intro; // entry point for conversation manager
	pkunk_desc.post_encounter_func = post_pkunk_enc;
	pkunk_desc.uninit_encounter_func = uninit_pkunk;

	pkunk_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	pkunk_desc.AlienTextBaseline.y = 0;
	pkunk_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	SET_GAME_STATE (BATTLE_SEGUE, 0);

	retval = &pkunk_desc;
  prep_conversation_module("pkunk", retval); // set up conversation manager

	return (retval);
}


