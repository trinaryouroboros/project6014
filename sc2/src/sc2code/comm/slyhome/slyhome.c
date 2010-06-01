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

#include "gameev.h"
#include "comm/commall.h"
#include "comm/slyhome/resinst.h"
#include "comm/slyhome/strings.h"


static LOCDATA slylandro_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	SLYLANDRO_PMAP_ANIM, /* AlienFrame */
	SLYLANDRO_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	SLYLANDRO_COLOR_MAP, /* AlienColorMap */
	SLYLANDRO_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	SLYLANDRO_CONVERSATION_PHRASES, /* PlayerPhrases */
	13, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{
			0, /* StartIndex */
			5, /* NumFrames */
			RANDOM_ANIM | COLORXFORM_ANIM, /* AnimFlags */
			ONE_SECOND / 8, ONE_SECOND * 5 / 8, /* FrameRate */
			ONE_SECOND / 8, ONE_SECOND * 5 / 8, /* RestartRate */
			0, /* BlockMask */
		},
		{
			1, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			6, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			11, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			16, /* StartIndex */
			6, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			22, /* StartIndex */
			8, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND / 15, 0, /* RestartRate */
			0, /* BlockMask */
		},
		{
			30, /* StartIndex */
			9, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 8) | (1 << 9), /* BlockMask */
		},
		{
			39, /* StartIndex */
			4, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			43, /* StartIndex */
			5, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 6), /* BlockMask */
		},
		{
			48, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 6), /* BlockMask */
		},
		{
			54, /* StartIndex */
			6, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 12), /* BlockMask */
		},
		{
			60, /* StartIndex */
			7, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 12), /* BlockMask */
		},
		{
			67, /* StartIndex */
			8, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			(1 << 10) | (1 << 11), /* BlockMask */
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
	(void) R;  // ignored
	SET_GAME_STATE (BATTLE_SEGUE, 0);

	if (PLAYER_SAID (R, not_fooling))
	{
		SET_GAME_STATE (BATTLE_SEGUE, 1);
		NPCPhrase (WHY_ATTACK);
	}
	else if(!(GET_GAME_STATE(SLYLANDRO_KOHRAH_PANIC)))
	{
		switch (GET_GAME_STATE (SLYLANDRO_HOME_VISITS))
		{
			case 1:
				NPCPhrase (GOOD_BYE_OK);
				break;
			default:
				NPCPhrase (GOOD_BYE_OK);
				break;
		}
	}
	else
	{
		switch (GET_GAME_STATE (SLYLANDRO_HOME_VISITS))
		{
			case 1:
				NPCPhrase (GOOD_BYE_PANIC);
				break;
			default:
				NPCPhrase (GOOD_BYE_PANIC);
				break;
		}
	}
}
/*
static void MainTalk (RESPONSE_REF R);

static void
HumanInfo (RESPONSE_REF R)
{
	BYTE InfoLeft;

	if (PLAYER_SAID (R, happy_to_tell_more))
	{
		NPCPhrase (TELL_MORE);

		SET_GAME_STATE (SLYLANDRO_STACK4, 1);
	}
	else if (PLAYER_SAID (R, would_you_like_to_know_more))
	{
		NPCPhrase (YES_TELL_MORE);
	}
	else if (PLAYER_SAID (R, we_come_from_earth))
	{
		NPCPhrase (OK_EARTH);

		SET_GAME_STATE (SLYLANDRO_KNOW_EARTH, 1);
	}
	else if (PLAYER_SAID (R, we_explore))
	{
		NPCPhrase (OK_EXPLORE);

		SET_GAME_STATE (SLYLANDRO_KNOW_EXPLORE, 1);
	}
	else if (PLAYER_SAID (R, we_fight_urquan))
	{
		NPCPhrase (URQUAN_NICE_GUYS);

		SET_GAME_STATE (SLYLANDRO_KNOW_URQUAN, 1);
	}
	else if (PLAYER_SAID (R, not_same_urquan))
	{
		NPCPhrase (PERSONALITY_CHANGE);

		SET_GAME_STATE (SLYLANDRO_KNOW_URQUAN, 2);
	}
	else if (PLAYER_SAID (R, we_gather))
	{
		NPCPhrase (MAYBE_INTERESTED);

		SET_GAME_STATE (SLYLANDRO_KNOW_GATHER, 1);
	}

	InfoLeft = FALSE;
	if (GET_GAME_STATE (SLYLANDRO_KNOW_URQUAN) == 1)
	{
		InfoLeft = TRUE;
		Response (not_same_urquan, HumanInfo);
	}
	if (!GET_GAME_STATE (SLYLANDRO_KNOW_EARTH))
	{
		InfoLeft = TRUE;
		Response (we_come_from_earth, HumanInfo);
	}
	if (!GET_GAME_STATE (SLYLANDRO_KNOW_EXPLORE))
	{
		InfoLeft = TRUE;
		Response (we_explore, HumanInfo);
	}
	if (!GET_GAME_STATE (SLYLANDRO_KNOW_URQUAN))
	{
		InfoLeft = TRUE;
		Response (we_fight_urquan, HumanInfo);
	}
	if (!GET_GAME_STATE (SLYLANDRO_KNOW_GATHER))
	{
		InfoLeft = TRUE;
		Response (we_gather, HumanInfo);
	}

	Response (enough_about_me, MainTalk);
	if (!InfoLeft)
	{
		SET_GAME_STATE (SLYLANDRO_STACK4, 2);
	}
}*/

static void
MainTalk (RESPONSE_REF R)
{
	BYTE i, stack2temp;
	
	if (PLAYER_SAID(R, close_call))
	{
		NPCPhrase(WONDERFUL_THING);
		SET_GAME_STATE(SLYLANDRO_STACK1, (GET_GAME_STATE (SLYLANDRO_STACK1)+1));
		SET_GAME_STATE(KNOW_SLYLANDRO_KOHRAH, 1);
		SET_GAME_STATE(SLYLANDRO_KOHRAH_PANIC, 0);
	}
	
	if (PLAYER_SAID(R, who_were_visitors))
	{
		NPCPhrase(THE_VISITORS_WERE);
		SET_GAME_STATE(SLYLANDRO_STACK1, (GET_GAME_STATE (SLYLANDRO_STACK1)+1));
	}

	if (PLAYER_SAID(R, pose_danger))
	{
		NPCPhrase(GOOD_GUYS);
		SET_GAME_STATE(SLYLANDRO_STACK1, (GET_GAME_STATE (SLYLANDRO_STACK1)+1));
	}
	
	if (PLAYER_SAID(R, up_no_good))
	{
		NPCPhrase(NO_GO_BACK);
		SET_GAME_STATE(SLYLANDRO_STACK1, (GET_GAME_STATE (SLYLANDRO_STACK1)+1));
		Response(your_funeral, MainTalk);
		DISABLE_PHRASE(better_go_back);
	}
	
	if (PLAYER_SAID(R, better_go_back))
	{
		SET_GAME_STATE(KNOW_SLYLANDRO_KOHRAH, 1);
		SET_GAME_STATE(SLYLANDRO_KOHRAH_PANIC, 0);
		SET_GAME_STATE(SLYLANDRO_STACK3, (GET_GAME_STATE (SLYLANDRO_STACK3)+1));
		NPCPhrase(NO_GO_BACK);
		Response(your_funeral, MainTalk);
		DISABLE_PHRASE(up_no_good);
		DISABLE_PHRASE(better_go_back);
	}
	
	if (PLAYER_SAID(R, your_funeral))
	{
		NPCPhrase(THANK_YOU);
	}
	
	if (PLAYER_SAID(R, sorry_mistake))
	{
		SET_GAME_STATE(SLYLANDRO_KOHRAH_PANIC, 0);
		NPCPhrase(MISTAKE_FORGIVEN);
	}
	
	if (PLAYER_SAID(R, ask_news_1))
	{
		DISABLE_PHRASE(ask_news_1);
		stack2temp = GET_GAME_STATE (SLYLANDRO_STACK2);
		
		switch (stack2temp)
		{
			case 0:
				NPCPhrase(GIVE_NEWS_1);
				break;
			default:
				NPCPhrase(GIVE_NEWS_2);
				stack2temp--;
				break;
		}
		
		stack2temp++;
		SET_GAME_STATE(SLYLANDRO_STACK2, stack2temp);
	}
	
	switch (GET_GAME_STATE (SLYLANDRO_STACK1))
	{
		case 0:
			if (!(GET_GAME_STATE(KNOW_SLYLANDRO_KOHRAH)))
				Response (not_fooling, ExitConversation);
			Response (close_call, MainTalk);
			break;
		case 1:
			if(!(GET_GAME_STATE(SLYLANDRO_KOHRAH_PANIC)))
				Response (who_were_visitors, MainTalk);
			break;
		case 2:
			if(!(GET_GAME_STATE(SLYLANDRO_KOHRAH_PANIC)))
				Response (pose_danger, MainTalk);
			break;
		case 3:
			if (PHRASE_ENABLED(up_no_good) && !(GET_GAME_STATE(SLYLANDRO_KOHRAH_PANIC)))
				Response (up_no_good, MainTalk);
			break;
	}
	
	if (GET_GAME_STATE(SLYLANDRO_STACK1)>0 
		&& PHRASE_ENABLED(ask_news_1)
		&& !(GET_GAME_STATE(SLYLANDRO_KOHRAH_PANIC)))
	{
		Response (ask_news_1, MainTalk);
	}
	
	if(GET_GAME_STATE(SLYLANDRO_STACK3)<1 && PHRASE_ENABLED(better_go_back))
		Response (better_go_back, MainTalk);
	
	if(GET_GAME_STATE(KNOW_SLYLANDRO_KOHRAH))
	{
		if (!(GET_GAME_STATE(SLYLANDRO_KOHRAH_PANIC)))
			Response (good_bye_ok, ExitConversation);
		else
		{
			Response (sorry_mistake, MainTalk);
			Response (good_bye_short, ExitConversation);
		}
	}
	else
		Response (good_bye_confused, ExitConversation);
}

static void
Intro (void)
{
	BYTE NumVisits;

	NumVisits = GET_GAME_STATE (SLYLANDRO_KOHRAH_MET_TIMES);
	
	if (!(GET_GAME_STATE(SLYLANDRO_KOHRAH_PANIC)))
	{
		switch (NumVisits++)
		{
			case 0:
				NPCPhrase (HELLO_1);
				break;
			case 1:
				NPCPhrase (HELLO_2);
				break;
			case 2:
				NPCPhrase (HELLO_3);
				break;
			case 3:
				NPCPhrase (HELLO_4);
				--NumVisits;
				break;
		}
	}
	else
	{
		switch (NumVisits++)
		{
			case 0:
				NPCPhrase (PANIC_HELLO_1);
				break;
			case 1:
				NPCPhrase (PANIC_HELLO_2);
				break;
			case 2:
				NPCPhrase (PANIC_HELLO_1);
				break;
			case 3:
				NPCPhrase (PANIC_HELLO_2);
				NumVisits -= 2;
				break;
		}
	}

	SET_GAME_STATE (SLYLANDRO_KOHRAH_MET_TIMES, NumVisits);
	MainTalk ((RESPONSE_REF)0);
}

static COUNT
uninit_slylandro (void)
{
	return (0);
}

static void
post_slylandro_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_slylandro_comm (void)
{
	LOCDATA *retval;

	slylandro_desc.init_encounter_func = Intro;
	slylandro_desc.post_encounter_func = post_slylandro_enc;
	slylandro_desc.uninit_encounter_func = uninit_slylandro;

	slylandro_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	slylandro_desc.AlienTextBaseline.y = 0;
	slylandro_desc.AlienTextWidth = SIS_TEXT_WIDTH;

	// JMS: Battle segue 'cause its impossible to tell are these guys kohr-ah or slylandros...
	SET_GAME_STATE (BATTLE_SEGUE, 1);
	
	// JMS: Zero the escape flag
	SET_GAME_STATE (ENEMY_ESCAPE_OCCURRED, 0);
	
	retval = &slylandro_desc;

	return (retval);
}
