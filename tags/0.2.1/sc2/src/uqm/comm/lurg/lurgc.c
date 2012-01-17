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

// JMS 2010: Totally new file: Lurg communications

#include "../commall.h"
#include "resinst.h"
#include "strings.h"

#include "uqm/build.h"


static LOCDATA lurg_desc_1x =
{
	LURG_CONVERSATION, /* AlienConv */
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	LURG_PMAP_ANIM, /* AlienFrame */
	LURG_FONT, /* AlienFont */
	WHITE_COLOR_INIT, /* AlienTextFColor */
	BLACK_COLOR_INIT, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_BOTTOM, /* AlienTextValign */
	LURG_COLOR_MAP, /* AlienColorMap */
	LURG_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	LURG_CONVERSATION_PHRASES, /* PlayerPhrases */
	21, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{	// 0 - Background throb animation
			5, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 10, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1L << 8) | (1L << 9) | (1L << 10) | (1L << 13) | (1L << 14) | (1L << 17) | (1L << 18), /* BlockMask */
		},
		{	// 1 - Eye blink
			8, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND, /* RestartRate */
			0, /* BlockMask */
		},
		{	// 2 - Spinchter A - Leftmost
			11, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{	// 3 - Spinchter B - Second left
			13, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1L << 17), /* BlockMask */
		},
		{	// 4 - Spinchter C - Middle left
			15, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1L << 19), /* BlockMask */
		},
		{	// 5 - Spinchter D - Middle right
			17, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1L << 13), /* BlockMask */
		},
		{	// 6 - Spinchter E - Second right
			19, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1L << 13), /* BlockMask */
		},
		{	// 7 - Spinchter F - Rightmost
			21, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{	// 8 - Intestine Left
			23, /* StartIndex */
			3, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND * 3, /* RestartRate */
			(1L << 0) | (1L << 12) | (1L << 17), /* BlockMask */
		},
		{	// 9 - Intestine Middle
			26, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 12, 0, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND * 5, /* RestartRate */
			(1L << 0), /* BlockMask */
		},
		{	// 10 - Intestine Right
			29, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND * 3, ONE_SECOND * 3, /* RestartRate */
			(1L << 0) | (1L << 13), /* BlockMask */
		},
		{	// 11 - Lavalamp anim Right
			34, /* StartIndex */
			19, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 18, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND / 2, /* RestartRate */
			(1L << 13), /* BlockMask */
		},
		{	// 12 - Lavalamp anim Left
			53, /* StartIndex */
			19, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 19, 0, /* FrameRate */
			3 * ONE_SECOND / 2, ONE_SECOND / 2, /* RestartRate */
			(1L << 17) | (1L << 8), /* BlockMask */
		},
		{	// 13 - Move lavalamp Right
			72, /* StartIndex */
			25, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND * 5, /* RestartRate */
			(1L << 0) | (1L << 5) | (1L << 6) | (1L << 10) | (1L << 11) | (1L << 15), /* BlockMask */
		},
		{	// 14 - Tentacle R2
			97, /* StartIndex */
			6, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			3 * ONE_SECOND / 4, ONE_SECOND, /* RestartRate */
			(1L << 0) | (1L << 15) | (1L << 16), /* BlockMask */
		},
		{	// 15 - Tentacle R3
			103, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND / 2, 3 * ONE_SECOND / 2, /* RestartRate */
			(1L << 13) | (1L << 14) | (1L << 16), /* BlockMask */
		},
		{	// 16 - Tentacle R4
			108, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 12, 0, /* FrameRate */
			ONE_SECOND / 2, 3 * ONE_SECOND / 2, /* RestartRate */
			(1L << 14) | (1L << 15), /* BlockMask */
		},
		{	// 17 - Move lavalamp Left
			112, /* StartIndex */
			30, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND * 3, /* RestartRate */
			(1L << 0) | (1L << 3) | (1L << 8) | (1L << 12) | (1L << 18) | (1L << 19), /* BlockMask */
		},
		{	// 18 - Tentacle L2
			142, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND / 2, ONE_SECOND * 2, /* RestartRate */
			(1L << 0)  | (1L << 17) | (1L << 19) | (1L << 20), /* BlockMask */
		},
		{	// 19 - Tentacle L3
			147, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 12, 0, /* FrameRate */
			3 * ONE_SECOND / 2, ONE_SECOND, /* RestartRate */
			(1L << 4) | (1L << 17) | (1L << 18) | (1L << 20), /* BlockMask */
		},
		{	// 20 - Tentacle L4
			150, /* StartIndex */
			4, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND / 2, 3 * ONE_SECOND / 2, /* RestartRate */
			(1L << 18) | (1L << 19), /* BlockMask */
		},
	},
	{ /* AlienTransitionDesc */
		1, /* StartIndex */
		2, /* NumFrames */
		0, /* AnimFlags */
		ONE_SECOND / 15, 0, /* FrameRate */
		0, 0, /* RestartRate */
		0, /* BlockMask */
	},
	{ /* AlienTalkDesc */
		1, /* StartIndex */
		4, /* NumFrames */
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


static LOCDATA lurg_desc_4x =
{
	LURG_CONVERSATION, /* AlienConv */
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	LURG_PMAP_ANIM, /* AlienFrame */
	LURG_FONT, /* AlienFont */
	WHITE_COLOR_INIT, /* AlienTextFColor */
	BLACK_COLOR_INIT, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_BOTTOM, /* AlienTextValign */
	LURG_COLOR_MAP, /* AlienColorMap */
	LURG_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	LURG_CONVERSATION_PHRASES, /* PlayerPhrases */
	14, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{	// 0 - Background throb animation
			5, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM | WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 10, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1L << 8) | (1L << 9) | (1L << 10) | (1L << 13), /* BlockMask */
		},
		{	// 1 - Eye blink
			8, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND, /* RestartRate */
			0, /* BlockMask */
		},
		{	// 2 - Spinchter A - Leftmost
			11, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{	// 3 - Spinchter B - Second left
			13, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{	// 4 - Spinchter C - Middle left
			15, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{	// 5 - Spinchter D - Middle right
			17, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1L << 13), /* BlockMask */
		},
		{	// 6 - Spinchter E - Second right
			19, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1L << 13), /* BlockMask */
		},
		{	// 7 - Spinchter F - Rightmost
			21, /* StartIndex */
			2, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{	// 8 - Intestine Left
			23, /* StartIndex */
			3, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND * 3, /* RestartRate */
			(1L << 0) | (1L << 12), /* BlockMask */
		},
		{	// 9 - Intestine Middle
			26, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 12, 0, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND * 5, /* RestartRate */
			(1L << 0), /* BlockMask */
		},
		{	// 10 - Intestine Right
			29, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND * 3, ONE_SECOND * 3, /* RestartRate */
			(1L << 0) | (1L << 13), /* BlockMask */
		},
		{	// 11 - Lavalamp anim Right
			34, /* StartIndex */
			19, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 18, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND / 2, /* RestartRate */
			(1L << 13), /* BlockMask */
		},
		{	// 12 - Lavalamp anim Left
			53, /* StartIndex */
			19, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 19, 0, /* FrameRate */
			3 * ONE_SECOND / 2, ONE_SECOND / 2, /* RestartRate */
			(1L << 8), /* BlockMask */
		},
		{	// 13 - Move lavalamp Right
			72, /* StartIndex */
			25, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 24, 0, /* FrameRate */
			ONE_SECOND * 2, ONE_SECOND * 5, /* RestartRate */
			(1L << 0) | (1L << 5) | (1L << 6) | (1L << 10) | (1L << 11), /* BlockMask */
		},
	},
	{ /* AlienTransitionDesc */
		1, /* StartIndex */
		2, /* NumFrames */
		0, /* AnimFlags */
		ONE_SECOND / 15, 0, /* FrameRate */
		0, 0, /* RestartRate */
		0, /* BlockMask */
	},
	{ /* AlienTalkDesc */
		1, /* StartIndex */
		4, /* NumFrames */
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
	//	BYTE NumVisits;
    int at_home = GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7);
    
    if (at_home && PLAYER_SAID (R, we_mean_no_harm))
	{
        // Lurg says nothing, player leaves peacefully.
		SET_GAME_STATE (BATTLE_SEGUE, 0);
        return;
	}
    
    NPCPhrase (GOODBYE);
	
	SET_GAME_STATE (BATTLE_SEGUE, 1);
}

static void YouCantPass (RESPONSE_REF R)
{
    (void) R; // satisfy compiler
    NPCPhrase (CANT_PASS);
	
    Response (last_chance, ExitConversation);
    Response (lets_fight, ExitConversation);
}

static void
AskQuestions (RESPONSE_REF R);

static void
WeKnowAll (RESPONSE_REF R)
{
    (void) R; // satisfy compiler
    NPCPhrase (WE_KNOW_ALL);
    
    Response (we_kick_ass, AskQuestions);
    Response (we_are_powerful, AskQuestions);
}

static void
YouFeelUs (RESPONSE_REF R)
{
    (void) R; // satisfy compiler
    DISABLE_PHRASE (you_look_familiar);
    
    NPCPhrase (YOU_FEEL_US);
    
    Response (alien_stalkers, WeKnowAll);
    Response (creepy, WeKnowAll);
    Response (big_imagination, WeKnowAll);
}

void
AskQuestions (RESPONSE_REF R)
{
    int num_left = 0;
    
    if (PLAYER_SAID (R, we_kick_ass) || PLAYER_SAID (R, we_are_powerful))
    {
        NPCPhrase (EASILY_DESTROYED);
    }
    else if (PLAYER_SAID (R, tellus_species))
    {
        DISABLE_PHRASE (tellus_species);
        NPCPhrase (OUR_SPECIES);
    }
	else if (PLAYER_SAID (R, tellus_ship))
    {
        DISABLE_PHRASE (tellus_ship);
        NPCPhrase (OUR_SHIP);
    }
	else if (PLAYER_SAID (R, tellus_shielded))
    {
        DISABLE_PHRASE (tellus_shielded);
        NPCPhrase (URQUAN_ALLIANCE);
    }
	else if (PLAYER_SAID (R, we_are_peaceful))
    {
        DISABLE_PHRASE (we_are_peaceful);
        NPCPhrase (NOT_PEACEFUL);
    }
    else
        NPCPhrase (ASK_YOUR_QUESTIONS);
    
    if (PHRASE_ENABLED (tellus_species))
    {
        Response (tellus_species, AskQuestions);
        num_left++;
    }
    if (PHRASE_ENABLED (tellus_ship))
    {
        Response (tellus_ship, AskQuestions);
        num_left++;
    }
    if (PHRASE_ENABLED (tellus_shielded))
    {
        Response (tellus_shielded, AskQuestions);
        num_left++;
    }
    if (PHRASE_ENABLED (we_are_peaceful))
    {
        Response (we_are_peaceful, AskQuestions);
        num_left++;
    }
    if (PHRASE_ENABLED (you_look_familiar))
    {
        Response (you_look_familiar, YouFeelUs);
        num_left++;
    }
    
    if (num_left > 0)
        Response (dont_tellus, YouCantPass);
    else
        YouCantPass(R);
}

static void
YouCantLeave (RESPONSE_REF R)
{
	if (PLAYER_SAID (R, we_mean_no_harm))
        NPCPhrase (CANT_LEAVE);
    else
        NPCPhrase (DONT_INSULT_US);
    
	Response (can_we_pass, YouCantPass);
	Response (let_us_pass, YouCantPass);
	Response (tell_us_stuff, AskQuestions);
}

static void
Intro (void)
{
	//	BYTE NumVisits;
    int at_home = GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7);

	if (at_home)
	{
		NPCPhrase (LURG_HOME_HELLO_1);
	}
	else
	{
		NPCPhrase (LURG_SPACE_HELLO_1);
	}
	
    if (at_home)
    {
    	Response (we_mean_no_harm, ExitConversation);
        Response (lets_fight, ExitConversation);
    }
    else
    {
    	Response (we_mean_no_harm, YouCantLeave);
        Response (distress_call, YouCantLeave);
        Response (suck_vacuum, ExitConversation);
    }
}

static COUNT
uninit_lurg (void)
{
	return (0);
}

static void
post_lurg_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_lurg_comm (void)
{
	LOCDATA *retval;
	static LOCDATA lurg_desc;

	switch (RESOLUTION_FACTOR)
	{
	case 2:
		lurg_desc = lurg_desc_4x;
		break;
	case 0:
	default:
		lurg_desc = lurg_desc_1x;
		break;
	}

	lurg_desc.init_encounter_func = Intro;
	lurg_desc.post_encounter_func = post_lurg_enc;
	lurg_desc.uninit_encounter_func = uninit_lurg;

	lurg_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	lurg_desc.AlienTextBaseline.y = RES_SIS_SCALE(100);
	lurg_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	SET_GAME_STATE (BATTLE_SEGUE, 1);
	retval = &lurg_desc;

	return (retval);
}
