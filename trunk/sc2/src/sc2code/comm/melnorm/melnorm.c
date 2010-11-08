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
#include "comm/melnorm/resinst.h"
#include "comm/melnorm/strings.h"

#include "gameev.h"
#include "setup.h"
#include "shipcont.h"
#include "libs/inplib.h"
#include "libs/mathlib.h"


#define NUM_HISTORY_ITEMS 9
#define NUM_EVENT_ITEMS 8
#define NUM_ALIEN_RACE_ITEMS 16
#define NUM_TECH_ITEMS 13

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
	{0 /*AlienFeaturesArray (alternative features) */
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
		DrawStatusMessage ((UNICODE *)~0);
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
	
	if (PLAYER_SAID (R, buy_info))
	{
		NPCPhrase (BUY_INFO_INTRO);
	}
	if (PLAYER_SAID (R, buy_current_events))
	{
		if ((int)credit >= (int)needed_credit)
		{
			NPCPhrase (OK_BUY_EVENT_1);
			DeltaCredit (-needed_credit);
		}
		else
		{
			DeltaCredit (-needed_credit);
		}
	}
	else if (PLAYER_SAID (R, buy_alien_races))
	{
		NPCPhrase (OK_BUY_ALIEN_RACE_1);
		NPCPhrase (OK_NO_TRADE_NOW_BYE);
	}
	else if (PLAYER_SAID (R, buy_history))
	{
		NPCPhrase (OK_BUY_HISTORY_1);
		NPCPhrase (OK_NO_TRADE_NOW_BYE);
	}

	Response (buy_current_events, BuyInfoMenu);
	Response (buy_alien_races, BuyInfoMenu);
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
	
	if(GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS) == 0)
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
	
	capacity = FUEL_RESERVE;
	
	if (GET_GAME_STATE(WHICH_SHIP_PLAYER_HAS) == 0)
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
	/*if (PLAYER_SAID (R, make_purchases)
		||PLAYER_SAID (R, done_buying_info)
		||PLAYER_SAID (R, done_buying_fuel))
	{*/
		NPCPhrase (WHAT_TO_BUY);
	//}
	
	Response (buy_info, BuyInfoMenu);

	if (GLOBAL_SIS (FuelOnBoard) < capacity)
		Response (buy_fuel, BuyFuelMenu);
	Response (done_buying, TradeMenu);
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
	
	if (GLOBAL_SIS (TotalBioMass) || num_new_rainbows || GET_GAME_STATE(YEHAT_PRECURSOR_ARTIFACT) == 2)
	{
		if (!what_to_sell_queued)
			NPCPhrase (WHAT_TO_SELL);
		
		if (GLOBAL_SIS (TotalBioMass))
			Response (sell_life_data, SellMenu);
		if (num_new_rainbows)
			Response (sell_rainbow_locations, SellMenu);
		if (GET_GAME_STATE(YEHAT_PRECURSOR_ARTIFACT) == 2)
			Response (sell_ship_mark_sightings, ShipMarkSightingsMenu);
		
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


static void
TradeMenu (RESPONSE_REF R)
{
	if (PLAYER_SAID (R, done_selling))
	{
		NPCPhrase (OK_DONE_SELLING);
	}
	else if (PLAYER_SAID (R, done_buying))
	{
		NPCPhrase (OK_DONE_BUYING);
	}
	else 
	{
		if (PLAYER_SAID (R, only_joke))
		{
			NPCPhrase (TRADING_INFO2);
			NPCPhrase (MORE_TRADING_INFO);
		}
		else if (PLAYER_SAID (R, i_remember))
		{
			NPCPhrase (RIGHT_YOU_ARE);
		}
		else if (PLAYER_SAID (R, how_to_trade))
		{
			NPCPhrase (TRADING_INFO1);
			NPCPhrase (MORE_TRADING_INFO);
		}
		else if(PLAYER_SAID (R, dummy))
		{
		}
		else if (GET_GAME_STATE (MET_MELNORME) == 1 && !(PLAYER_SAID (R, dummy)))
		{
			NPCPhrase (HELLO_NOW_DOWN_TO_BUSINESS2);
		}
		if (!(PLAYER_SAID (R, dummy)))
		{
			NPCPhrase (BUY_OR_SELL);
			AlienTalkSegue(1);
			XFormColorMap (GetColorMapAddress (SetAbsColorMapIndex (CommData.AlienColorMap, 1)), ONE_SECOND / 2);
			AlienTalkSegue((COUNT)~0);
		}
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
	NPCPhrase (HELLO_NOW_DOWN_TO_BUSINESS1);

	Response (hi_doing_great, HowAreYou);
	Response (doing_average, HowAreYou);
	Response (not_good, HowAreYou);
}

static void
Intro (void) {
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
	return (0);
}

static void
post_melnorme_enc (void)
{
	LockMutex (GraphicsLock);
	DrawStatusMessage (0);
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

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	//TODO ressurect?
	//AskedToBuy = FALSE;
	retval = &melnorme_desc;

	return (retval);
}
