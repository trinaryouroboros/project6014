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

 
 // 	-DN DEC10		- added update_biounit_flags(void) that is activated when the 
 //						player sells bio-data to enable the easter egg when the player
 //						finds and sells data on all 25 new lifeform types
 
#include "../commall.h"
#include "resinst.h"
#include "strings.h"

#include "uqm/gameev.h"
#include "uqm/setup.h"
#include "uqm/shipcont.h"
#include "libs/inplib.h"
#include "libs/mathlib.h"
#include "assert.h"

// XXX: temporary, for SOL_X/SOL_Y
#include "uqm/hyper.h"

#define NUM_HISTORY_ITEMS 0
#define NUM_EVENT_ITEMS 1
#define NUM_ALIEN_RACE_ITEMS 0
#define NUM_TECH_ITEMS 0

static NUMBER_SPEECH_DESC melnorme_numbers_english;

static LOCDATA melnorme_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	MELNORME_PMAP_ANIM, /* AlienFrame */
	MELNORME_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH - 16, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_TOP, /* AlienTextValign */
	MELNORME_COLOR_MAP, /* AlienColorMap */
	MELNORME_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	MELNORME_CONVERSATION_PHRASES, /* PlayerPhrases */
	4, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{
			6, /* StartIndex */
			5, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 12, 0, /* FrameRate */
			ONE_SECOND * 4, ONE_SECOND * 4,/* RestartRate */
			(1 << 1), /* BlockMask */
		},
		{
			11, /* StartIndex */
			9, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 20, 0, /* FrameRate */
			ONE_SECOND * 4, ONE_SECOND * 4,/* RestartRate */
			(1 << 0), /* BlockMask */
		},
		{
			20, /* StartIndex */
			2, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 15, /* FrameRate */
			ONE_SECOND, ONE_SECOND * 3, /* RestartRate */
			0, /* BlockMask */
		},
		{
			22, /* StartIndex */
			2, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 10, ONE_SECOND / 15, /* FrameRate */
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
		5, /* NumFrames */
		0, /* AnimFlags */
		ONE_SECOND / 15, 0, /* FrameRate */
		ONE_SECOND / 12, 0, /* RestartRate */
		0, /* BlockMask */
	},
	&melnorme_numbers_english, /* AlienNumberSpeech - default */
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

static COUNT melnorme_digit_names[] =
{
	ENUMERATE_ZERO,
	ENUMERATE_ONE,
	ENUMERATE_TWO,
	ENUMERATE_THREE,
	ENUMERATE_FOUR,
	ENUMERATE_FIVE,
	ENUMERATE_SIX,
	ENUMERATE_SEVEN,
	ENUMERATE_EIGHT,
	ENUMERATE_NINE
};

static COUNT melnorme_teen_names[] =
{
	ENUMERATE_TEN,
	ENUMERATE_ELEVEN,
	ENUMERATE_TWELVE,
	ENUMERATE_THIRTEEN,
	ENUMERATE_FOURTEEN,
	ENUMERATE_FIFTEEN,
	ENUMERATE_SIXTEEN,
	ENUMERATE_SEVENTEEN,
	ENUMERATE_EIGHTEEN,
	ENUMERATE_NINETEEN
};

static COUNT melnorme_tens_names[] =
{
	0, /* invalid */
	0, /* skip digit */
	ENUMERATE_TWENTY,
	ENUMERATE_THIRTY,
	ENUMERATE_FOURTY,
	ENUMERATE_FIFTY,
	ENUMERATE_SIXTY,
	ENUMERATE_SEVENTY,
	ENUMERATE_EIGHTY,
	ENUMERATE_NINETY
};

static NUMBER_SPEECH_DESC melnorme_numbers_english =
{
	5, /* NumDigits */
	{
		{ /* 1000-999999 */
			1000, /* Divider */
			0, /* Subtrahend */
			NULL, /* StrDigits - recurse */
			NULL, /* Names - not used */
			ENUMERATE_THOUSAND /* CommonIndex */
		},
		{ /* 100-999 */
			100, /* Divider */
			0, /* Subtrahend */
			melnorme_digit_names, /* StrDigits */
			NULL, /* Names - not used */
			ENUMERATE_HUNDRED /* CommonIndex */
		},
		{ /* 20-99 */
			10, /* Divider */
			0, /* Subtrahend */
			melnorme_tens_names, /* StrDigits */
			NULL, /* Names - not used */
			0 /* CommonIndex - not used */
		},
		{ /* 10-19 */
			1, /* Divider */
			10, /* Subtrahend */
			melnorme_teen_names, /* StrDigits */
			NULL, /* Names - not used */
			0 /* CommonIndex - not used */
		},
		{ /* 0-9 */
			1, /* Divider */
			0, /* Subtrahend */
			melnorme_digit_names, /* StrDigits */
			NULL, /* Names - not used */
			0 /* CommonIndex - not used */
		}
	}
};

static StatMsgMode prevMsgMode;
void 
update_biounit_flags(void) {
	
	/*
	 * stuff for the Melnorme bio-data easter egg
	 * this sets a flag indicating what types of creature
	 * have been sold to the Melnorme. Once all types 
	 * have been found, activate the easter egg! 
	 * 
	 * DN 04JAN11
	 */
	int all_found_flag = 0;

	//probably should be done with a for loop and an array...  DN18JAN11
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_ECHINOSOL_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_FLORA_FLATULENSIS_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_HOPPING_HATCHLING_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_DIZZY_FNARBLE_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_FLAGELLUM_PEST_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_FLYING_OHAIRY_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_BOBBING_WHIBBIT_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_MUDDY_MORPHLEGM_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_ULTRAMOEBA_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_ELECTROPTERA_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_QUARTZERBACK_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_TUBERUS_HUMUNGUS_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_VENUS_FRYTRAP_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_WATCHFUL_WILLOW_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_XEROPHYTIC_AUTOVORE_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_MIGRATOR_BLIMP_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_TENTACLE_DUJOUR_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_VANISHING_VERMIN_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_TRIPAZOID_TUMBLER_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_DUMPY_DWEEJUS_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_RADIAL_ARACHNID_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_WACKODEMON_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_CRABBY_OCTOPUS_TYPE_FOUND);	
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_BLINKING_BEHOLDER_TYPE_FOUND);
	all_found_flag = all_found_flag + GET_GAME_STATE(MELNORME_CREEPING_HEAD_TYPE_FOUND);
		
	/*check to see if all bio_unit types have been found*/
	if (all_found_flag == 25 && GET_GAME_STATE(MELNORME_ALL_LIFE_TYPE_FOUND) != 2) {
		SET_GAME_STATE(MELNORME_ALL_LIFE_TYPE_FOUND, 1);
	}
	
}

static COUNT
DeltaCredit (SIZE delta_credit)
{
	COUNT Credit;
	
	Credit = MAKE_WORD (
						GET_GAME_STATE (MELNORME_CREDIT0),
						GET_GAME_STATE (MELNORME_CREDIT1)
						);
	if ((int)delta_credit >= 0 || ((int)(-delta_credit) <= (int)(Credit)))
	{
		Credit += delta_credit;
		SET_GAME_STATE (MELNORME_CREDIT0, LOBYTE (Credit));
		SET_GAME_STATE (MELNORME_CREDIT1, HIBYTE (Credit));
		LockMutex (GraphicsLock);
		DrawStatusMessage (NULL);
		UnlockMutex (GraphicsLock);
	}
	else
	{
		NPCPhrase (NEED_MORE_CREDIT0);
		NPCPhrase (delta_credit + (int)Credit);
		NPCPhrase (NEED_MORE_CREDIT1);
	}
	
	return (Credit);
}

BOOLEAN StripExplorer (COUNT fuel_required);

static void
ExitConversation (RESPONSE_REF R)
{
	if (PLAYER_SAID (R, no_trade_now))
	{
		NPCPhrase (OK_NO_TRADE_NOW_BYE);
		SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (PLAYER_SAID (R, spank_ass2))
	{
		NPCPhrase (BATTLE_MELNORME);
		SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (PLAYER_SAID (R, take_it))
	{
		switch (GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS)) {
			case CHMMR_EXPLORER_SHIP:
				StripExplorer(0);
				break;
			case PRECURSOR_BATTLESHIP:
				/* TODO: implement... */
				break;
		}
		NPCPhrase (HAPPY_TO_HAVE_RESCUED);
	}
	else if (PLAYER_SAID (R, leave_it))
	{
		SET_GAME_STATE (MELNORME_RESCUE_REFUSED, 1);
		NPCPhrase (MAYBE_SEE_YOU_LATER);
	}
	else if (PLAYER_SAID (R, no_help))
	{
		SET_GAME_STATE (MELNORME_RESCUE_REFUSED, 1);
		NPCPhrase (GOODBYE_AND_GOODLUCK);
	}
	else if (PLAYER_SAID (R, no_changed_mind))
	{
		NPCPhrase (GOODBYE_AND_GOODLUCK_AGAIN);
	}

}


static void
PurchaseMenu (RESPONSE_REF R);

static void
SellMenu (RESPONSE_REF R);

static void
TradeMenu (RESPONSE_REF R);

static void
ShipMarkSightingsMenu (RESPONSE_REF R)
{
	if (PLAYER_SAID (R, sell_ship_mark_sightings))
	{
		NPCPhrase (OFFER_CREDITS_FOR_SHIPMARK_INFO);
		Response (deal_shipmarks, ShipMarkSightingsMenu);
		Response (no_deal_shipmarks, SellMenu);
	}
	else if (PLAYER_SAID (R, deal_shipmarks))
	{
		NPCPhrase (ALTERNATIVE_REWARD_FOR_SHIPMARK_INFO);
		Response (credits_for_ship_mark_sightings, SellMenu);
		Response (info_for_ship_mark_sightings, SellMenu);
	}	
}

static void
BuyInfoMenu (RESPONSE_REF R)
{
	COUNT credit;
	SIZE needed_credit;
	
	credit = MAKE_WORD (GET_GAME_STATE (MELNORME_CREDIT0), GET_GAME_STATE (MELNORME_CREDIT1));
	
#define INFO_COST 75
	needed_credit = INFO_COST;
	
	if (PLAYER_SAID (R, buy_current_events))
	{
		BYTE stack = GET_GAME_STATE (MELNORME_EVENTS_INFO_STACK);
		assert (stack < NUM_EVENT_ITEMS);
		if ((int)credit >= (int)needed_credit)
		{
			/* TODO: If/when we add more current event info for sale, factor out
			 * a switch on stack here. */
			NPCPhrase (OK_BUY_EVENT_1);
			SET_GAME_STATE (MELNORME_EVENTS_INFO_STACK, stack+1);
		}
		DeltaCredit (-needed_credit);
	}
	else if (PLAYER_SAID (R, buy_alien_races))
	{
		BYTE stack = GET_GAME_STATE (MELNORME_ALIEN_INFO_STACK);
		assert(stack < NUM_ALIEN_RACE_ITEMS);
		if ((int)credit >= (int)needed_credit)
		{
			NPCPhrase (OK_BUY_ALIEN_RACE_1);
			SET_GAME_STATE (MELNORME_ALIEN_INFO_STACK, stack+1);
		}
		DeltaCredit (-needed_credit);
	}
	else if (PLAYER_SAID (R, buy_history))
	{
		BYTE stack = GET_GAME_STATE (MELNORME_HISTORY_INFO_STACK);
		assert(stack < NUM_HISTORY_ITEMS);
		if ((int)credit >= (int)needed_credit)
		{
			NPCPhrase (OK_BUY_HISTORY_1);
			SET_GAME_STATE (MELNORME_HISTORY_INFO_STACK, stack+1);
		}
		DeltaCredit (-needed_credit);
	}
	else if (PLAYER_SAID (R, buy_info))
	{
		if (GET_GAME_STATE (MELNORME_INFO_PROCEDURE))
			NPCPhrase (OK_BUY_INFO);
		else
		{
			NPCPhrase (BUY_INFO_INTRO);
			SET_GAME_STATE (MELNORME_INFO_PROCEDURE, 1);
		}
	}

	if (GET_GAME_STATE (MELNORME_EVENTS_INFO_STACK) < NUM_EVENT_ITEMS)
		Response (buy_current_events, BuyInfoMenu);
	if (GET_GAME_STATE (MELNORME_ALIEN_INFO_STACK) < NUM_ALIEN_RACE_ITEMS)
		Response (buy_alien_races, BuyInfoMenu);
	if (GET_GAME_STATE (MELNORME_HISTORY_INFO_STACK) < NUM_HISTORY_ITEMS)
		Response (buy_history, BuyInfoMenu);

	Response (done_buying_info, PurchaseMenu);
}


static void
BuyFuelMenu (RESPONSE_REF R)
{
	COUNT credit;
	SIZE needed_credit;
	BYTE slot;
	DWORD capacity;
	BOOLEAN doNotOfferFuel;
	
	credit = MAKE_WORD (GET_GAME_STATE (MELNORME_CREDIT0),GET_GAME_STATE (MELNORME_CREDIT1));
	capacity = FUEL_RESERVE;
	doNotOfferFuel = FALSE;
	
	if (GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS) == CHMMR_EXPLORER_SHIP)
	{
		capacity = EXPLORER_FUEL_CAPACITY;
	}	
	else
	{
		slot = NUM_MODULE_SLOTS - 1;
		
		do
		{
			if (GLOBAL_SIS (ModuleSlots[slot]) == FUEL_TANK || GLOBAL_SIS (ModuleSlots[slot]) == HIGHEFF_FUELSYS)
			{
				COUNT volume;
			
				volume = GLOBAL_SIS (ModuleSlots[slot]) == FUEL_TANK ? FUEL_TANK_CAPACITY : HEFUEL_TANK_CAPACITY;
				capacity += volume;
			}
		} while (slot--);
	}
	
	if (PLAYER_SAID (R, buy_fuel)
		|| PLAYER_SAID (R, buy_1_fuel)
		|| PLAYER_SAID (R, buy_5_fuel)
		|| PLAYER_SAID (R, buy_10_fuel)
		|| PLAYER_SAID (R, buy_25_fuel)
		|| PLAYER_SAID (R, fill_me_up))
	{
		needed_credit = 0;
		if (PLAYER_SAID (R, buy_1_fuel))
			needed_credit = 1;
		else if (PLAYER_SAID (R, buy_5_fuel))
			needed_credit = 5;
		else if (PLAYER_SAID (R, buy_10_fuel))
			needed_credit = 10;
		else if (PLAYER_SAID (R, buy_25_fuel))
			needed_credit = 25;
		else if (PLAYER_SAID (R, fill_me_up))
			needed_credit = (capacity - GLOBAL_SIS (FuelOnBoard) + FUEL_TANK_SCALE - 1) / FUEL_TANK_SCALE;
		
		if (needed_credit == 0)
		{
			if (!GET_GAME_STATE (MELNORME_FUEL_PROCEDURE))
			{
				NPCPhrase (BUY_FUEL_INTRO);
				SET_GAME_STATE (MELNORME_FUEL_PROCEDURE, 1);
			}
		}
		else
		{
			if (GLOBAL_SIS (FuelOnBoard) / FUEL_TANK_SCALE + needed_credit > capacity / FUEL_TANK_SCALE)
			{
				NPCPhrase (NO_ROOM_FOR_FUEL);
				goto TryFuelAgain;
			}
			
			if ((int)(needed_credit * (BIO_CREDIT_VALUE / 2)) <= (int)credit)
			{
				DWORD f;
				
				NPCPhrase (GOT_FUEL);
				
				f = (DWORD)needed_credit * FUEL_TANK_SCALE;
				LockMutex (GraphicsLock);
				while (f > 0x3FFFL)
				{
					DeltaSISGauges (0, 0x3FFF, 0);
					f -= 0x3FFF;
				}
				DeltaSISGauges (0, (SIZE)f, 0);
				UnlockMutex (GraphicsLock);
			}
			needed_credit *= (BIO_CREDIT_VALUE / 2);
		}
		if (needed_credit)
		{
			DeltaCredit (-needed_credit);
			if (GLOBAL_SIS (FuelOnBoard) >= capacity)
			{
				PurchaseMenu(0);
				doNotOfferFuel = TRUE;
			}
		}
	TryFuelAgain:
		if (!doNotOfferFuel)
		{
			NPCPhrase (HOW_MUCH_FUEL);
		
			Response (buy_1_fuel, BuyFuelMenu);
			Response (buy_5_fuel, BuyFuelMenu);
			Response (buy_10_fuel, BuyFuelMenu);
			Response (buy_25_fuel, BuyFuelMenu);
			Response (fill_me_up, BuyFuelMenu);
			Response (done_buying_fuel, PurchaseMenu);
		}
	}
		
}


static void
PurchaseMenu (RESPONSE_REF R)
{
	BYTE slot;
	DWORD capacity;
	
	(void) R; // satisfy compiler
	capacity = FUEL_RESERVE;
	
	if (GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS) == CHMMR_EXPLORER_SHIP)
		capacity = EXPLORER_FUEL_CAPACITY;
	else
	{
		slot = NUM_MODULE_SLOTS - 1;
		
		do
		{
			if (GLOBAL_SIS (ModuleSlots[slot]) == FUEL_TANK 
				|| GLOBAL_SIS (ModuleSlots[slot]) == HIGHEFF_FUELSYS)
			{
				COUNT volume;
				volume = GLOBAL_SIS (ModuleSlots[slot]) == FUEL_TANK
				? FUEL_TANK_CAPACITY : HEFUEL_TANK_CAPACITY;
				capacity += volume;
			}
		} while (slot--);
	}
	if (PLAYER_SAID (R, make_purchases)
			|| PLAYER_SAID (R, done_buying_info)
			|| PLAYER_SAID (R, done_buying_fuel))
	{
		NPCPhrase (WHAT_TO_BUY);
	}
	
	if (GET_GAME_STATE (MELNORME_EVENTS_INFO_STACK) < NUM_EVENT_ITEMS ||
			GET_GAME_STATE (MELNORME_ALIEN_INFO_STACK) < NUM_ALIEN_RACE_ITEMS ||
			GET_GAME_STATE (MELNORME_HISTORY_INFO_STACK) < NUM_HISTORY_ITEMS)
	{
		Response (buy_info, BuyInfoMenu);
	}

	if (GLOBAL_SIS (FuelOnBoard) < capacity)
		Response (buy_fuel, BuyFuelMenu);
	Response (done_buying, TradeMenu);
}

static void
SayHelloAndDownToBusiness ()
{
	BYTE stack;

	stack = (BYTE)(GET_GAME_STATE (MELNORME_YACK_STACK2));
	switch (stack++)
	{
		case 0:
			NPCPhrase (HELLO_AND_DOWN_TO_BUSINESS1);
			break;
		case 1:
			NPCPhrase (HELLO_AND_DOWN_TO_BUSINESS2);
			break;
		case 2:
			NPCPhrase (HELLO_AND_DOWN_TO_BUSINESS3);
			break;
		case 3:
			NPCPhrase (HELLO_AND_DOWN_TO_BUSINESS4);
			break;
		case 4:
			NPCPhrase (HELLO_AND_DOWN_TO_BUSINESS5);
			break;
		case 5:
			NPCPhrase (HELLO_AND_DOWN_TO_BUSINESS6);
			break;
		case 6:
			NPCPhrase (HELLO_AND_DOWN_TO_BUSINESS7);
			break;
		case 7:
			NPCPhrase (HELLO_AND_DOWN_TO_BUSINESS8);
			break;
		case 8:
			NPCPhrase (HELLO_AND_DOWN_TO_BUSINESS9);
			break;
    case 9:
			NPCPhrase (HELLO_AND_DOWN_TO_BUSINESS10);
			break;
	}
	stack = stack % 10; // Stack must be range [0,9].
	SET_GAME_STATE (MELNORME_YACK_STACK2, stack);

}

static void
SellMenu (RESPONSE_REF R)
{
	BYTE num_new_rainbows;
	UWORD rainbow_mask;
	SIZE added_credit;
	int what_to_sell_queued = 0;
	
	rainbow_mask = MAKE_WORD (GET_GAME_STATE (RAINBOW_WORLD0),GET_GAME_STATE (RAINBOW_WORLD1));
	num_new_rainbows = (BYTE)(-GET_GAME_STATE (MELNORME_RAINBOW_COUNT));
	
	while (rainbow_mask)
	{
		if (rainbow_mask & 1)
			++num_new_rainbows;
		
		rainbow_mask >>= 1;
	}
	
	if (PLAYER_SAID (R, no_deal_shipmarks))
	{
		NPCPhrase (OK_NO_DEAL_SHIPMARKS);
	}
	else if (!PLAYER_SAID (R, items_to_sell))
	{
		if (PLAYER_SAID (R, sell_life_data))
		{
			DWORD TimeIn;
			
			added_credit = GLOBAL_SIS (TotalBioMass) * BIO_CREDIT_VALUE;
			
			NPCPhrase (SOLD_LIFE_DATA1);
			NPCPhrase (-(int)GLOBAL_SIS (TotalBioMass));
			NPCPhrase (SOLD_LIFE_DATA2);
			NPCPhrase (-(int)added_credit);
			NPCPhrase (SOLD_LIFE_DATA3);
			
			/*
			 * if the bio-data easter egg hasn't been activated, 
			 * check the biounit flags
			 * -DN 18JAN11
			 */
			if (GET_GAME_STATE(MELNORME_ALL_LIFE_TYPE_FOUND) == 0) {
				update_biounit_flags(); 
			}
			
			/*
			 * a check for the MELNORME_ALL_LIFE_TYPE_FOUND
			 * flag after player sells bio-whatnot to the Melnorme. 
			 *
			 * DN 27DEC10
			 *
			 */
			if (GET_GAME_STATE(MELNORME_ALL_LIFE_TYPE_FOUND) == 1)
			{
				NPCPhrase (ALL_BIO_TYPES_FOUND);
				SET_GAME_STATE(MELNORME_ALL_LIFE_TYPE_FOUND, 2);
			}
			
			// queue WHAT_TO_SELL before talk-segue
			if (num_new_rainbows)
			{
				NPCPhrase (WHAT_TO_SELL);
				what_to_sell_queued = 1;
			}
			AlienTalkSegue (1);
			
			DrawCargoStrings ((BYTE)~0, (BYTE)~0);
			SleepThread (ONE_SECOND / 2);
			TimeIn = GetTimeCounter ();
			DrawCargoStrings ((BYTE)NUM_ELEMENT_CATEGORIES, (BYTE)NUM_ELEMENT_CATEGORIES);
			do
			{
				TimeIn = GetTimeCounter ();
				if (AnyButtonPress (TRUE))
				{
					DeltaCredit (GLOBAL_SIS (TotalBioMass) * BIO_CREDIT_VALUE);
					GLOBAL_SIS (TotalBioMass) = 0;
				}
				else
				{
					--GLOBAL_SIS (TotalBioMass);
					DeltaCredit (BIO_CREDIT_VALUE);
				}
				DrawCargoStrings ((BYTE)NUM_ELEMENT_CATEGORIES,(BYTE)NUM_ELEMENT_CATEGORIES);
			} while (GLOBAL_SIS (TotalBioMass));
			SleepThread (ONE_SECOND / 2);
			
			LockMutex (GraphicsLock);
			ClearSISRect (DRAW_SIS_DISPLAY);
			UnlockMutex (GraphicsLock);
		}
		else if (PLAYER_SAID (R, credits_for_ship_mark_sightings))
		{
			added_credit = (50 * BIO_CREDIT_VALUE);
			NPCPhrase (SHIP_MARK_DEAL);
			DeltaCredit (added_credit);
			SET_GAME_STATE(YEHAT_PRECURSOR_ARTIFACT, 3);
		}
		else if (PLAYER_SAID (R, info_for_ship_mark_sightings))
		{
			NPCPhrase (SHIP_MARK_INFORMATION);
			switch (GET_GAME_STATE(HINT_WORLD_LOCATION)) {
				case 0:
					NPCPhrase (HINT_PLANET_0);
					break;
				case 1:
					NPCPhrase (HINT_PLANET_1);
					break;
				case 2:
					NPCPhrase (HINT_PLANET_2);
					break;
				default:
					break;
			}
			NPCPhrase (SHIP_MARK_INFORMATION_2);
			SET_GAME_STATE(YEHAT_PRECURSOR_ARTIFACT, 3);
		}
		else if (PLAYER_SAID (R, sell_tzzrak_tzon_pics))
		{
			NPCPhrase (NO_WAY);
			SET_GAME_STATE(TZZRAK_TZON_PICS, 0);
		}
		else /* if (R == sell_rainbow_locations) */
		{
			added_credit = num_new_rainbows * (250 * BIO_CREDIT_VALUE);
			
			NPCPhrase (SOLD_RAINBOW_LOCATIONS1);
			NPCPhrase (-(int)num_new_rainbows);
			NPCPhrase (SOLD_RAINBOW_LOCATIONS2);
			NPCPhrase (-(int)added_credit);
			NPCPhrase (SOLD_RAINBOW_LOCATIONS3);
			
			num_new_rainbows += GET_GAME_STATE (MELNORME_RAINBOW_COUNT);
			SET_GAME_STATE (MELNORME_RAINBOW_COUNT, num_new_rainbows);
			num_new_rainbows = 0;
			
			DeltaCredit (added_credit);
		}
	}
	
	if (GLOBAL_SIS (TotalBioMass) 
		|| num_new_rainbows
		|| GET_GAME_STATE(YEHAT_PRECURSOR_ARTIFACT) == 2
		|| GET_GAME_STATE(TZZRAK_TZON_PICS) == 1)
	{
		if (!what_to_sell_queued)
			NPCPhrase (WHAT_TO_SELL);
		
		if (GLOBAL_SIS (TotalBioMass))
			Response (sell_life_data, SellMenu);
		if (num_new_rainbows)
			Response (sell_rainbow_locations, SellMenu);
		if (GET_GAME_STATE(YEHAT_PRECURSOR_ARTIFACT) == 2)
			Response (sell_ship_mark_sightings, ShipMarkSightingsMenu);
		if (GET_GAME_STATE(TZZRAK_TZON_PICS) == 1)
			Response (sell_tzzrak_tzon_pics, SellMenu);
		
		Response (done_selling, TradeMenu);
		Response (no_trade_now, ExitConversation);
	}
	else
	{
		if (PLAYER_SAID (R, items_to_sell))
		{
			NPCPhrase (NOTHING_TO_SELL);
			DISABLE_PHRASE (items_to_sell);
			TradeMenu (dummy);
		}
		else
		{
			Response (done_selling, TradeMenu);
			Response (no_trade_now, ExitConversation);
		}
	}
}

static COUNT rescue_fuel;
static SIS_STATE SIS_copy;

BOOLEAN
StripExplorer (COUNT fuel_required)
{
	COUNT worth, total, num_landers_sold;
	BYTE i, which_module;
	DWORD capacity;

	if (fuel_required == 0)
	{
		/* Player has agreed to rescue offer. */
		GlobData.SIS_state = SIS_copy;
		LockMutex (GraphicsLock);
		DeltaSISGauges (UNDEFINED_DELTA, rescue_fuel, UNDEFINED_DELTA);
		UnlockMutex (GraphicsLock);
		return TRUE;
	}

	assert (fuel_required > 0);
	
	/* Offer to trade planet landers for fuel. */
	SIS_copy = GlobData.SIS_state;

	/* The only thing of value which we can strip from the Explorer is
	the landers (or perhaps escorts). */
	capacity = EXPLORER_FUEL_CAPACITY;
	worth = fuel_required / FUEL_TANK_SCALE;
	total = 0;
	num_landers_sold = 0;

	for (i = 0; i < NUM_MODULE_SLOTS && SIS_copy.NumLanders > 0 && total < worth; ++i)
	{
		which_module = SIS_copy.ModuleSlots[i];
		if (which_module != PLANET_LANDER)
			continue;
		total += GLOBAL (ModuleCost[which_module]);
		SIS_copy.NumLanders--;
		++num_landers_sold;
	}

	if (total == 0)
	{
		/* Player has nothing of value, just give them fuel. */
		NPCPhrase (CHARITY);
		LockMutex (GraphicsLock);
		DeltaSISGauges (0, fuel_required, 0);
		UnlockMutex (GraphicsLock);
		return (FALSE);
	}
	else
	{
		/* Offer to sell them fuel for landers. */
		NPCPhrase (RESCUE_OFFER);
		rescue_fuel = fuel_required;
		if (rescue_fuel == capacity)
			NPCPhrase (RESCUE_TANKS);
		else
			NPCPhrase (RESCUE_HOME);

		NPCPhrase (ENUMERATE_ONE + num_landers_sold - 1);
		NPCPhrase (LANDERS);
	}

	return (TRUE);
}

static void
DoRescue ()
{
	SIZE dx, dy;
	COUNT fuel_required;
	BOOLEAN s = FALSE;

	dx = LOGX_TO_UNIVERSE (GLOBAL_SIS (log_x))
			- SOL_X;
	dy = LOGY_TO_UNIVERSE (GLOBAL_SIS (log_y))
			- SOL_Y;
	fuel_required = square_root (
			(DWORD)((long)dx * dx + (long)dy * dy)
			) + (2 * FUEL_TANK_SCALE);

	switch (GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS)) {
		case CHMMR_EXPLORER_SHIP:
			s = StripExplorer(fuel_required);
			break;
		case PRECURSOR_BATTLESHIP:
			/* TODO: implement... */
			break;
	}

	if (s)
	{
		Response (take_it, ExitConversation);
		Response (leave_it, ExitConversation);
	}

}

static void
TurnBridgePurple ()
{
	AlienTalkSegue((COUNT)~0);
	XFormColorMap (GetColorMapAddress (SetAbsColorMapIndex (CommData.AlienColorMap, 1)), ONE_SECOND / 2);
	AlienTalkSegue((COUNT)~0);
}

static void
DiscussRescue ()
{
	if (GET_GAME_STATE (MELNORME_RESCUE_REFUSED))
	{
		NPCPhrase (CHANGED_MIND);

		Response (yes_changed_mind, DoRescue);
		Response (no_changed_mind, ExitConversation);
	}
	else
	{
		BYTE num_rescues;

		num_rescues = GET_GAME_STATE (MELNORME_RESCUE_COUNT);
		switch (num_rescues)
		{
			case 0:
				NPCPhrase (RESCUE_EXPLANATION);
				break;
			case 1:
				NPCPhrase (RESCUE_AGAIN_1);
				break;
			case 2:
				NPCPhrase (RESCUE_AGAIN_2);
				break;
			case 3:
				NPCPhrase (RESCUE_AGAIN_3);
				break;
			case 4:
				NPCPhrase (RESCUE_AGAIN_4);
				break;
			case 5:
				NPCPhrase (RESCUE_AGAIN_5);
				break;
			}

		if (num_rescues < 5)
		{
			++num_rescues;
			SET_GAME_STATE (MELNORME_RESCUE_COUNT, num_rescues);
		}

		NPCPhrase (SHOULD_WE_HELP_YOU);

		TurnBridgePurple ();

		Response (yes_help, DoRescue);
		Response (no_help, ExitConversation);
	}

}

static BOOLEAN
PlayerNeedsRescue ()
{
	BYTE num_new_rainbows;
	UWORD rainbow_mask;
	COUNT Credit = DeltaCredit (0);

	rainbow_mask = MAKE_WORD (
			GET_GAME_STATE (RAINBOW_WORLD0),
			GET_GAME_STATE (RAINBOW_WORLD1)
			);
	num_new_rainbows = (BYTE)(-GET_GAME_STATE (MELNORME_RAINBOW_COUNT));

	while (rainbow_mask)
	{
		if (rainbow_mask & 1)
			++num_new_rainbows;

		rainbow_mask >>= 1;
	}

	return GLOBAL_SIS (FuelOnBoard) == 0
		  && GLOBAL_SIS (TotalBioMass) == 0
			&& Credit == 0
			&& num_new_rainbows == 0;
}

static void
TradeMenu (RESPONSE_REF R)
{
	if (PlayerNeedsRescue())
	{
		/* Player needs to be rescued... */
		DiscussRescue();
		return;
	}

	if (PLAYER_SAID (R, done_selling))
	{
		NPCPhrase (OK_DONE_SELLING);
	}
	else if (PLAYER_SAID (R, done_buying))
	{
		NPCPhrase (OK_DONE_BUYING);
	}
	else if (PLAYER_SAID (R, only_joke))
	{
		NPCPhrase (TRADING_INFO2);
		NPCPhrase (MORE_TRADING_INFO);
		NPCPhrase (BUY_OR_SELL);
		TurnBridgePurple ();
	}
	else if (PLAYER_SAID (R, i_remember))
	{
		NPCPhrase (RIGHT_YOU_ARE);
		NPCPhrase (BUY_OR_SELL);
		TurnBridgePurple ();
	}
	else if (PLAYER_SAID (R, how_to_trade))
	{
		NPCPhrase (TRADING_INFO1);
		NPCPhrase (MORE_TRADING_INFO);
		NPCPhrase (BUY_OR_SELL);
		TurnBridgePurple ();
	}
	else if (GET_GAME_STATE (MET_MELNORME) == 1 && !(PLAYER_SAID (R, dummy)))
	{
		SayHelloAndDownToBusiness();
		NPCPhrase (BUY_OR_SELL);
		TurnBridgePurple ();
	}
	Response (make_purchases, PurchaseMenu);
	if (PHRASE_ENABLED(items_to_sell))
		Response (items_to_sell, SellMenu);
	Response (no_trade_now, ExitConversation);
}

static void
Threaten (RESPONSE_REF R)
{
	if (PLAYER_SAID (R, spank_ass1))
	{
		NPCPhrase (UNWISE_CAPTAIN);
	}
		
	Response (spank_ass2, ExitConversation);
	Response (only_joke, TradeMenu);
	
}

static void
HowAreYou (RESPONSE_REF R)
{
	if (PLAYER_SAID (R, hi_doing_great))
	{
		NPCPhrase (DOING_GOOD_RESPONSE);
		NPCPhrase (WANT_TO_TRADE);
	}
	else if (PLAYER_SAID (R, doing_average))
	{
		NPCPhrase (DOING_AVERAGE_RESPONSE);
		NPCPhrase (WANT_TO_TRADE);
	}
	else if (PLAYER_SAID (R, not_good))
	{
		NPCPhrase (NOT_GOOD_RESPONSE);
		NPCPhrase (WANT_TO_TRADE);
	}

	Response (how_to_trade, TradeMenu);
	Response (i_remember, TradeMenu);
	Response (spank_ass1, Threaten);
}

static void
DoFirstMeeting (RESPONSE_REF R)
{
	(void) R; // satisfy compiler
	NPCPhrase (HELLO_NOW_DOWN_TO_BUSINESS1);

	Response (hi_doing_great, HowAreYou);
	Response (doing_average, HowAreYou);
	Response (not_good, HowAreYou);
}

static void
Intro (void) {
	prevMsgMode = SetStatusMessageMode (SMM_CREDITS);

	if (GET_GAME_STATE (MET_MELNORME) == 0)
	{
		SET_GAME_STATE (MET_MELNORME, 1);
		DoFirstMeeting (0);
	}
	else
	{
		TradeMenu(0);
	}
}

static COUNT
uninit_melnorme (void)
{
	return 0;
}

static void
post_melnorme_enc (void)
{
	LockMutex (GraphicsLock);
	if (prevMsgMode != SMM_UNDEFINED)
		SetStatusMessageMode (prevMsgMode);
	DrawStatusMessage (NULL);
	UnlockMutex (GraphicsLock);
}

LOCDATA*
init_melnorme_comm (void)
{
	LOCDATA *retval;

	melnorme_desc.init_encounter_func = Intro;
	melnorme_desc.post_encounter_func = post_melnorme_enc;
	melnorme_desc.uninit_encounter_func = uninit_melnorme;

	melnorme_desc.AlienTextBaseline.x = TEXT_X_OFFS + (SIS_TEXT_WIDTH >> 1);
	melnorme_desc.AlienTextBaseline.y = 0;
	melnorme_desc.AlienTextWidth = SIS_TEXT_WIDTH - 16;

	prevMsgMode = SMM_UNDEFINED;

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	//TODO ressurect?
	//AskedToBuy = FALSE;
	retval = &melnorme_desc;

	return (retval);
}
