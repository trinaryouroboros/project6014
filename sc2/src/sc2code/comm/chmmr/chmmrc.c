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
#include "comm/chmmr/resinst.h"
#include "comm/chmmr/strings.h"

#include "build.h"


static LOCDATA chmmr_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	CHMMR_PMAP_ANIM, /* AlienFrame */
	CHMMR_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	CHMMR_COLOR_MAP, /* AlienColorMap */
	CHMMR_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	CHMMR_CONVERSATION_PHRASES, /* PlayerPhrases */
	6, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{
			12, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			17, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			22, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			27, /* StartIndex */
			20, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			47, /* StartIndex */
			14, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			61, /* StartIndex */
			24, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
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
		ONE_SECOND / 60, 0, /* FrameRate */
		ONE_SECOND / 60, 0, /* RestartRate */
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
	NPCPhrase (GOODBYE_HUMAN1);
}

static void
AskMenu (RESPONSE_REF R);

static void
WhereKohrah (RESPONSE_REF R)
{	
	NPCPhrase (KOHRAH_ARE);
	DISABLE_PHRASE (where_kohrah);	

	Response (why_kohrah_still_here, AskMenu);
}


static void
ProvokeAboutYehat (RESPONSE_REF R)
{	
	NPCPhrase (CHMMR_ARE_AWESOME);
	DISABLE_PHRASE (yehat_not_impressed);	

	Response (chenjesu_guiding_light, AskMenu);
	Response (pterodactyls_know_nothing, AskMenu);
}

static void
AskMenu (RESPONSE_REF R)
{	
	/* Alien speech */
	if (PLAYER_SAID (R, where_urquan))
	{
		NPCPhrase (URQUAN_ARE);
		DISABLE_PHRASE (where_urquan);
	}
	else if (PLAYER_SAID (R, better_starship))
	{
		NPCPhrase (NO_BETTER_STARSHIP);
		DISABLE_PHRASE (better_starship);
	}
	else if (PLAYER_SAID (R, why_kohrah_still_here))
	{
		NPCPhrase (KOHRAH_HERE_BECAUSE);
	}
	else if (PLAYER_SAID (R, chenjesu_guiding_light))
	{
		NPCPhrase (CHMMR_PROTECT);
	}
	else if (PLAYER_SAID (R, pterodactyls_know_nothing))
	{
		NPCPhrase (SPEAK_TO_YEHAT);
	}
	else
	{
		NPCPhrase (YOU_CAN_ASK);
	}

	/* Human speech options */
	
	if (PHRASE_ENABLED (better_starship))
	{
		Response (better_starship, AskMenu);
	}

    if (PHRASE_ENABLED (where_urquan))
	{
		Response (where_urquan, AskMenu);
	}
	
    if (PHRASE_ENABLED (where_kohrah))
	{
		Response (where_kohrah, WhereKohrah);
	}

	if (GET_GAME_STATE (YEHAT_MET) == 1
		&& PHRASE_ENABLED (yehat_not_impressed))
	{
		Response (yehat_not_impressed, ProvokeAboutYehat);
	}

	Response (on_our_way, ExitConversation);
}

static void
HaveYouFoundArtefact (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, yes_but_dont_have))
	{
		NPCPhrase (BRING_HERE);
	}
	else if (PLAYER_SAID (R, not_yet))
	{
		SET_GAME_STATE(TRIANGULATION_SPHERES_CHMMR, 1);
		NPCPhrase (SEARCH_FOR_IT);
	}

	Response (can_we_ask, AskMenu);
	Response (on_our_way, ExitConversation);
}

static void
Intro (void)
{
	if (GET_GAME_STATE (CHMMR_MET) == 0)
	{
		SET_GAME_STATE (CHMMR_MET, 1);
	}

	NPCPhrase (CHMMR_GREETING1);


	Response (yes_but_dont_have, HaveYouFoundArtefact);
	Response (not_yet, HaveYouFoundArtefact);
	Response (on_our_way, ExitConversation);
}

static COUNT
uninit_chmmr (void)
{
	return (0);
}

static void
post_chmmr_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_chmmr_comm (void)
{
	LOCDATA *retval;

	chmmr_desc.init_encounter_func = Intro;
	chmmr_desc.post_encounter_func = post_chmmr_enc;
	chmmr_desc.uninit_encounter_func = uninit_chmmr;

	chmmr_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	chmmr_desc.AlienTextBaseline.y = 0;
	chmmr_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	retval = &chmmr_desc;

	return (retval);
}