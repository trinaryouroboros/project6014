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

// JMS 2010: New dialogue for the new, vile, disgusting and enhanced Kohr-Ah-Rath!

#include "../commall.h"
#include "resinst.h"
#include "strings.h"

static LOCDATA blackurq_desc =
{
	BLACKURQ_CONVERSATION, /* AlienConv */
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	BLACKURQ_PMAP_ANIM, /* AlienFrame */
	BLACKURQ_FONT, /* AlienFont */
	WHITE_COLOR_INIT, /* AlienTextFColor */
	BLACK_COLOR_INIT, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	BLACKURQ_COLOR_MAP, /* AlienColorMap */
	BLACKURQ_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	BLACKURQ_CONVERSATION_PHRASES, /* PlayerPhrases */
	7, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{
			7, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			13, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			20, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			23, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			26, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			33, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM
					| WAIT_TALKING, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			38, /* StartIndex */
			4, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 10, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
	},
	{ /* AlienTransitionDesc */
		29, /* StartIndex */
		2, /* NumFrames */
		0, /* AnimFlags */
		ONE_SECOND / 6, 0, /* FrameRate */
		0, 0, /* RestartRate */
		0, /* BlockMask */
	},
	{ /* AlienTalkDesc */
		29, /* StartIndex */
		4, /* NumFrames */
		RANDOM_ANIM, /* AnimFlags */
		ONE_SECOND / 10, 0, /* FrameRate */
		ONE_SECOND / 10, 0, /* RestartRate */
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
CombatIsInevitable (RESPONSE_REF R)
{
	BYTE NumVisits;

	int temp_info;
	
	SET_GAME_STATE (BATTLE_SEGUE, 1);

	if (PLAYER_SAID (R, bye))
	{
		if (GET_GAME_STATE (KOHR_AH_BYES) == 0)
			NPCPhrase (GOODBYE_AND_DIE);
		else
			NPCPhrase (DIE_HUMAN);

		SET_GAME_STATE (KOHR_AH_BYES, 1);
	}
	
	if (PLAYER_SAID (R, we_attack))
	{
		NPCPhrase (NO_WE_ATTACK);
	}
		
	if (PLAYER_SAID (R, murderous_bastards))
	{
		NPCPhrase (BRING_IT_ON);
		temp_info=GET_GAME_STATE (KOHR_AH_PLEAD);
		SET_GAME_STATE(KOHR_AH_PLEAD, (temp_info+1));
	}
	
	else if (PLAYER_SAID (R, give_up_1)
			|| PLAYER_SAID (R, give_up_extra_2)
			|| PLAYER_SAID (R, give_up_3))
	{
		NumVisits = GET_GAME_STATE (KOHR_AH_REASONS);
		switch (NumVisits++)
		{
			case 0:
				NPCPhrase (NO_GIVE_UP_1);
				break;
			case 1:
				NPCPhrase (NO_GIVE_UP_EXTRA_2);
				break;
			case 2:
				NPCPhrase (NO_GIVE_UP_3);
				--NumVisits;
				break;
		}
		SET_GAME_STATE (KOHR_AH_REASONS, NumVisits);
	}
}

static void
DieHuman (RESPONSE_REF R)
{
	//(void) R;  // ignored
	int temp_info;
	
	if (PLAYER_SAID (R, who_is_rath))
	{
		NPCPhrase (RATH_IS_GREAT);
		temp_info=GET_GAME_STATE (KOHR_AH_INFO);
		SET_GAME_STATE(KOHR_AH_INFO, (temp_info+1));
	}
	if (PLAYER_SAID (R, hmm_new_doctrine))
	{
		NPCPhrase (THIS_IS_NEW_DOCTRINE);
		temp_info=GET_GAME_STATE (KOHR_AH_INFO);
		SET_GAME_STATE(KOHR_AH_INFO, (temp_info+1));
	}
	if (PLAYER_SAID (R, wheres_talking_pet))
	{
		NPCPhrase (TALKING_PET_DEAD);
		temp_info=GET_GAME_STATE (KOHR_AH_PLEAD);
		SET_GAME_STATE(KOHR_AH_PLEAD, (temp_info+1));
	}
	if (PLAYER_SAID (R, give_up_2))
	{
		NPCPhrase (NO_GIVE_UP_2);
		DISABLE_PHRASE (give_up_2);
	}
		
	switch (GET_GAME_STATE (KOHR_AH_REASONS))
	{
		case 0:
			Response (give_up_1, CombatIsInevitable);
			break;
		case 1:
			if(PHRASE_ENABLED(give_up_2))
				Response (give_up_2, DieHuman);
			else
				Response (give_up_extra_2, CombatIsInevitable);
			break;
		case 2:
			Response (give_up_3, CombatIsInevitable);
			break;
	}
	switch (GET_GAME_STATE (KOHR_AH_INFO))
	{
		case 0:
			Response (who_is_rath, DieHuman);
			break;
		case 1:
			Response (hmm_new_doctrine, DieHuman);
			break;
	}
	switch (GET_GAME_STATE (KOHR_AH_PLEAD))
	{
		case 0:
			Response (wheres_talking_pet, DieHuman);
			break;
		case 1:
			Response (murderous_bastards, CombatIsInevitable);
			break;
	}
	Response (we_attack, CombatIsInevitable);
	Response (bye, CombatIsInevitable);
}

static void
Intro (void)
{
	BYTE NumVisits;

	NumVisits = GET_GAME_STATE (KOHR_AH_VISITS);
	switch (NumVisits++)
	{
		case 0:
			NPCPhrase (HELLO_AND_DIE_1);
			break;
		case 1:
			NPCPhrase (HELLO_AND_DIE_2);
			break;
		case 2:
			NPCPhrase (HELLO_AND_DIE_3);
			break;
		case 3:
			NPCPhrase (HELLO_AND_DIE_4);
			--NumVisits;
			break;
	}
	DieHuman ((RESPONSE_REF)0);
	SET_GAME_STATE (KOHR_AH_VISITS, NumVisits);
}

static COUNT
uninit_blackurq (void)
{
	return (0);
}

static void
post_blackurq_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_blackurq_comm (void)
{
	LOCDATA *retval;

	blackurq_desc.init_encounter_func = Intro;
	blackurq_desc.post_encounter_func = post_blackurq_enc;
	blackurq_desc.uninit_encounter_func = uninit_blackurq;

	blackurq_desc.AlienTextBaseline.x =	TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	blackurq_desc.AlienTextBaseline.y = 0;
	blackurq_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	SET_GAME_STATE (BATTLE_SEGUE, 1);
	
	retval = &blackurq_desc;

	return (retval);
}
