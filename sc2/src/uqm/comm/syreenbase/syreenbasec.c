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

// JMS 2010: Totally new file for syreen starbase comm.

#include "../commall.h"
#include "resinst.h"
#include "strings.h"
#include "libs/sound/sound.h"
#include "libs/inplib.h"

#include "uqm/build.h"
#include "uqm/setup.h"
#include "uqm/shipcont.h"


static void SellMinerals (RESPONSE_REF R);

static LOCDATA syreenbase_desc =
{
	SYREENBASE_CONVERSATION, /* AlienConv */
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	SYREEN_BASE_PMAP_ANIM, /* AlienFrame */
	SYREEN_BASE_FONT, /* AlienFont */
	WHITE_COLOR_INIT, /* AlienTextFColor */
	BLACK_COLOR_INIT, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_BOTTOM, /* AlienTextValign */
	SYREEN_BASE_COLOR_MAP, /* AlienColorMap */
	SYREEN_BASE_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	SYREEN_BASE_CONVERSATION_PHRASES, /* PlayerPhrases */
	0, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{
			0, /* StartIndex */
			0, /* NumFrames */
			0, /* AnimFlags */
			0, 0, /* FrameRate */
			0, 0, /* RestartRate */
			0, /* BlockMask */
		}
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
	(void) R; // satisfy compiler
	NPCPhrase (COME_BACK_ANYTIME_LUCIELLE);
	SET_GAME_STATE (BATTLE_SEGUE, 0);
}


static void
AskMenu1 (RESPONSE_REF R)
{	
	/* Alien speech */


	if (PLAYER_SAID (R, not_hot))
	{
		NPCPhrase (MUCH_WOMAN);
		DISABLE_PHRASE (not_hot);
	}
	else if (PLAYER_SAID (R, huge_fan))
	{
		NPCPhrase (JEALOUS);
		DISABLE_PHRASE (huge_fan);
	}
			
	/* Human speech options */
	
	if (PHRASE_ENABLED (not_hot))
	{
		Response (not_hot, AskMenu1);
	}

	if (PHRASE_ENABLED (huge_fan))
	{
		Response (huge_fan, AskMenu1);
	}
	
	if (GLOBAL_SIS (TotalElementMass))
		Response (minerals_here, SellMinerals);

	if (PHRASE_ENABLED (must_be_going))
	{
		Response (must_be_going, ExitConversation);
	}
}



static void
SyreenResponse1 (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, i_ll_tell_her))
	{
		NPCPhrase (YOU_CUTIE);
	}
	else if (PLAYER_SAID (R, perfect_name))
	{
		NPCPhrase (THANKS_SUGAR);
	}

	Response (huge_fan, AskMenu1);
	Response (not_hot, AskMenu1);
	if (GLOBAL_SIS (TotalElementMass))
		Response (minerals_here, SellMinerals);
	Response (must_be_going, ExitConversation);
}


static void
NiceComplement (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, had_to_stop_by))
	{
		NPCPhrase (LUCY_SAYS_HI);
	}
	
	Response (i_ll_tell_her, SyreenResponse1);
	Response (perfect_name, SyreenResponse1);
	if (GLOBAL_SIS (TotalElementMass))
		Response (minerals_here, SellMinerals);
	Response (must_be_going, ExitConversation);
}

static void
AnyAssistance (RESPONSE_REF R)
{	
	if (PLAYER_SAID (R, am_i_lost))
	{
		NPCPhrase (TALK_TO_COMMANDER);
	}

	else if (PLAYER_SAID (R, holy_crap))
	{
		NPCPhrase (YOU_CUTIE);
	}

	else if  (PLAYER_SAID (R, hello))
	{
		NPCPhrase (WELCOME);
	}

	Response (huge_fan, AskMenu1);
	Response (not_hot, AskMenu1);
	if (GLOBAL_SIS (TotalElementMass))
		Response (minerals_here, SellMinerals);
	Response (must_be_going, ExitConversation);
}

static void
SellMinerals (RESPONSE_REF R)
{
	COUNT i, total;
	BOOLEAN Sleepy;

	total = 0;
	Sleepy = TRUE;
	for (i = 0; i < NUM_ELEMENT_CATEGORIES; ++i)
	{
		COUNT amount;
		DWORD TimeIn = 0;

		if (i == 0)
		{
			DrawCargoStrings ((BYTE)~0, (BYTE)~0);
			SleepThread (ONE_SECOND / 2);
			TimeIn = GetTimeCounter ();
			DrawCargoStrings ((BYTE)0, (BYTE)0);
		}
		else if (Sleepy)
		{
			DrawCargoStrings ((BYTE)(i - 1), (BYTE)i);
			TimeIn = GetTimeCounter ();
		}

		if ((amount = GLOBAL_SIS (ElementAmounts[i])) != 0)
		{
			total += amount * GLOBAL (ElementWorth[i]);
			do
			{
				if (!Sleepy || AnyButtonPress (TRUE) ||
						(GLOBAL (CurrentActivity) & CHECK_ABORT))
				{
					Sleepy = FALSE;
					GLOBAL_SIS (ElementAmounts[i]) = 0;
					GLOBAL_SIS (TotalElementMass) -= amount;
					LockMutex (GraphicsLock);
					DeltaSISGauges (0, 0, amount * GLOBAL (ElementWorth[i]));
					UnlockMutex (GraphicsLock);
					break;
				}
				
				--GLOBAL_SIS (ElementAmounts[i]);
				--GLOBAL_SIS (TotalElementMass);
				TaskSwitch ();
				TimeIn = GetTimeCounter ();
				DrawCargoStrings ((BYTE)i, (BYTE)i);
				LockMutex (GraphicsLock);
				ShowRemainingCapacity ();
				DeltaSISGauges (0, 0, GLOBAL (ElementWorth[i]));
				UnlockMutex (GraphicsLock);
			} while (--amount);
		}
		if (Sleepy) {
			SleepThreadUntil (TimeIn + (ONE_SECOND / 4));
			TimeIn = GetTimeCounter ();
		}
	}
	SleepThread (ONE_SECOND / 2);

	LockMutex (GraphicsLock);
	ClearSISRect (DRAW_SIS_DISPLAY);
	UnlockMutex (GraphicsLock);

	NPCPhrase (MINERALS);

	AskMenu1 (R);
}

static void
Intro (void)
{
	if (GET_GAME_STATE (SYREEN_MET) == 0)
	{
		SET_GAME_STATE (SYREEN_MET, 1);
	}

	NPCPhrase (LUCIELLE_GREETING);

	Response (holy_crap, AnyAssistance);
	Response (hello, AnyAssistance);
	Response (am_i_lost, AnyAssistance);
	Response (had_to_stop_by, NiceComplement);
}


static COUNT
uninit_syreen (void)
{
	return (0);
}

static void
post_syreen_enc (void)
{
	// nothing defined so far
}

LOCDATA*
init_syreenbase_comm (void)
{
	LOCDATA *retval;

	syreenbase_desc.init_encounter_func = Intro;
	syreenbase_desc.post_encounter_func = post_syreen_enc;
	syreenbase_desc.uninit_encounter_func = uninit_syreen;

	syreenbase_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	syreenbase_desc.AlienTextBaseline.y = 100 << RESOLUTION_FACTOR;
	syreenbase_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	retval = &syreenbase_desc;

	return (retval);
}
