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

// JMS 2010: -Removed the arc welder anims that created black spots on top of earth graphics.
//			 -Added rudimentary dialogue regarding other races and lost shofixti mission

#include "comm/commall.h"
#include "comm/comandr/resinst.h"
#include "comm/starbas/strings.h"

#include "build.h"
#include "setup.h"
#include "shipcont.h"
#include "libs/graphics/gfx_common.h"
#include "libs/inplib.h"
#include "libs/mathlib.h"
#include "libs/inplib.h"
#include "libs/sound/sound.h"


//static void TellMission (RESPONSE_REF R);
static void SellMinerals (RESPONSE_REF R);


static LOCDATA commander_desc =
{
	NULL, /* init_encounter_func */
	NULL, /* post_encounter_func */
	NULL, /* uninit_encounter_func */
	COMMANDER_PMAP_ANIM, /* AlienFrame */
	COMMANDER_FONT, /* AlienFont */
	WHITE_COLOR, /* AlienTextFColor */
	BLACK_COLOR, /* AlienTextBColor */
	{0, 0}, /* AlienTextBaseline */
	0, /* SIS_TEXT_WIDTH, */ /* AlienTextWidth */
	ALIGN_CENTER, /* AlienTextAlign */
	VALIGN_MIDDLE, /* AlienTextValign */
	COMMANDER_COLOR_MAP, /* AlienColorMap */
	COMMANDER_MUSIC, /* AlienSong */
	NULL_RESOURCE, /* AlienAltSong */
	0, /* AlienSongFlags */
	STARBASE_CONVERSATION_PHRASES, /* PlayerPhrases */
	3, /* NumAnimations */
	{ /* AlienAmbientArray (ambient animations) */
		{ /* Blink */
			1, /* StartIndex */
			3, /* NumFrames */
			YOYO_ANIM, /* AnimFlags */
			ONE_SECOND / 15, 0, /* FrameRate */
			0, ONE_SECOND * 8, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* Running light */
			10, /* StartIndex */
			30, /* NumFrames */
			CIRCULAR_ANIM, /* AnimFlags */
			ONE_SECOND / 40, 0, /* FrameRate */
			ONE_SECOND * 2, 0, /* RestartRate */
			0, /* BlockMask */
		},
		{ /* Flagship picture */
			95, /* StartIndex */
			1, /* NumFrames */
			0, /* AnimFlags */
			0, 0, /* FrameRate */
			0, 0, /* RestartRate */
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
		4, /* StartIndex */
		6, /* NumFrames */
		0, /* AnimFlags */
		ONE_SECOND / 10, ONE_SECOND / 15, /* FrameRate */
		ONE_SECOND * 7 / 60, ONE_SECOND / 12, /* RestartRate */
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

static DWORD CurBulletinMask;

static void
ByeBye (RESPONSE_REF R)
{
	(void) R;  // ignored

	CurBulletinMask |= GET_GAME_STATE_32 (STARBASE_BULLETS0);
	SET_GAME_STATE_32 (STARBASE_BULLETS0, CurBulletinMask);

	/* if (R == goodbye_starbase_commander) */
	if (GET_GAME_STATE (CHMMR_BOMB_STATE) >= 2)
		NPCPhrase (GOOD_LUCK_AGAIN);
	else
	{
		RESPONSE_REF pStr0, pStr1;
		
		switch ((BYTE)TFB_Random () & 7)
		{
			case 0:
				pStr0 = NORMAL_GOODBYE_A0;
				pStr1 = NORMAL_GOODBYE_A1;
				break;
			case 1:
				pStr0 = NORMAL_GOODBYE_B0;
				pStr1 = NORMAL_GOODBYE_B1;
				break;
			case 2:
				pStr0 = NORMAL_GOODBYE_C0;
				pStr1 = NORMAL_GOODBYE_C1;
				break;
			case 3:
				pStr0 = NORMAL_GOODBYE_D0;
				pStr1 = NORMAL_GOODBYE_D1;
				break;
			case 4:
				pStr0 = NORMAL_GOODBYE_E0;
				pStr1 = NORMAL_GOODBYE_E1;
				break;
			case 5:
				pStr0 = NORMAL_GOODBYE_F0;
				pStr1 = NORMAL_GOODBYE_F1;
				break;
			case 6:
				pStr0 = NORMAL_GOODBYE_G0;
				pStr1 = NORMAL_GOODBYE_G1;
				break;
			case 7:
				pStr0 = NORMAL_GOODBYE_H0;
				pStr1 = NORMAL_GOODBYE_H1;
				break;
		}

		NPCPhrase (pStr0);
	}
}

static void AlienRaces (RESPONSE_REF R);

static void
AllianceInfo (RESPONSE_REF R)
{
#define ALLIANCE_CHMMR (1 << 0)
#define ALLIANCE_SHOFIXTI (1 << 1)
#define ALLIANCE_YEHATPKUNK (1 << 2)
#define ALLIANCE_ARILOU (1 << 3)
#define ALLIANCE_SYREEN (1 << 4)
#define ALLIANCE_SPATHI (1 << 5)
#define ALLIANCE_ORZ	(1 << 6)

#define ALLIANCE_SUPOXUTWIG (1 << 0)
#define ALLIANCE_ZOQFOT (1 << 1)
	
	static BYTE AllianceMask = 0;
	static BYTE AllianceMask2 = 0;

	if (PLAYER_SAID (R, alliance_races))
	{
		NPCPhrase (WHICH_ALLY);
		AllianceMask = 0;
		AllianceMask2 = 0;
	}
	else if (PLAYER_SAID (R, chmmr))
	{
		NPCPhrase (CHMMR_INFO_SB);
		AllianceMask |= ALLIANCE_CHMMR;
	}
	else if (PLAYER_SAID (R, shofixti))
	{
		NPCPhrase (SHOFIXTI_INFO_SB);
		AllianceMask |= ALLIANCE_SHOFIXTI;
	}
	else if (PLAYER_SAID (R, yehat_and_pkunk))
	{
		NPCPhrase (YEHAT_AND_PKUNK_INFO_SB);
		AllianceMask |= ALLIANCE_YEHATPKUNK;
	}
	else if (PLAYER_SAID (R, arilou))
	{
		NPCPhrase (ARILOU_INFO_SB);
		AllianceMask |= ALLIANCE_ARILOU;
	}
	else if (PLAYER_SAID (R, syreen))
	{
		NPCPhrase (SYREEN_INFO_SB);
		AllianceMask |= ALLIANCE_SYREEN;
	}
	else if (PLAYER_SAID (R, spathi))
	{
		NPCPhrase (SPATHI_INFO_SB);
		AllianceMask |= ALLIANCE_SPATHI;
	}
	else if (PLAYER_SAID (R, orz))
	{
		NPCPhrase (ORZ_INFO_SB);
		AllianceMask |= ALLIANCE_ORZ;
	}
	else if (PLAYER_SAID (R, supox_and_utwig))
	{
		NPCPhrase (SUPOX_UTWIG_INFO_SB);
		AllianceMask2 |= ALLIANCE_SUPOXUTWIG;
	}
	else if (PLAYER_SAID (R, zoqfot))
	{
		NPCPhrase (ZOQFOT_INFO_SB);
		AllianceMask2 |= ALLIANCE_ZOQFOT;
	}

	if (!(AllianceMask & ALLIANCE_CHMMR))
		Response (chmmr, AllianceInfo);
	if (!(AllianceMask & ALLIANCE_SHOFIXTI))
		Response (shofixti, AllianceInfo);
	if (!(AllianceMask & ALLIANCE_YEHATPKUNK))
		Response (yehat_and_pkunk, AllianceInfo);
	if (!(AllianceMask & ALLIANCE_ARILOU))
		Response (arilou, AllianceInfo);
	if (!(AllianceMask & ALLIANCE_SYREEN))
		Response (syreen, AllianceInfo);
	if (!(AllianceMask & ALLIANCE_SPATHI))
		Response (spathi, AllianceInfo);
	if (!(AllianceMask & ALLIANCE_ORZ))
		Response (orz, AllianceInfo);
	if (!(AllianceMask2 & ALLIANCE_SUPOXUTWIG))
		Response (supox_and_utwig, AllianceInfo);
	if (!(AllianceMask2 & ALLIANCE_ZOQFOT))
		Response (zoqfot, AllianceInfo);
	Response (enough_allies, AlienRaces);
}

static void
HostileInfo (RESPONSE_REF R)
{
#define HOSTILE_URQUAN (1 << 0)
#define HOSTILE_KOHRAH (1 << 1)
#define HOSTILE_VUX (1 << 2)
#define HOSTILE_MYCON (1 << 3)
#define HOSTILE_ANDROSYNTH (1 << 4)
#define HOSTILE_ILWRATH (1 << 5)
#define HOSTILE_THRADDASH (1 << 6)
	
	static BYTE HostileMask = 0;

	if (PLAYER_SAID (R, hostile_races))
	{
		NPCPhrase (WHICH_ENEMY);
		HostileMask = 0;
	}
	else if (PLAYER_SAID (R, urquan))
	{
		NPCPhrase (UR_QUAN_INFO_SB);
		HostileMask |= HOSTILE_URQUAN;
	}
	else if (PLAYER_SAID (R, kohrah))
	{
		NPCPhrase (KOHR_AH_INFO_SB);
		HostileMask |= HOSTILE_KOHRAH;
	}
	else if (PLAYER_SAID (R, vux))
	{
		NPCPhrase (VUX_INFO_SB);
		HostileMask |= HOSTILE_VUX;
	}
	else if (PLAYER_SAID (R, mycon))
	{
		NPCPhrase (MYCON_INFO_SB);
		HostileMask |= HOSTILE_MYCON;
	}
	else if (PLAYER_SAID (R, androsynth))
	{
		NPCPhrase (ANDROSYNTH_INFO_SB);
		HostileMask |= HOSTILE_ANDROSYNTH;
	}
	else if (PLAYER_SAID (R, ilwrath))
	{
		NPCPhrase (ILWRATH_INFO_SB);
		HostileMask |= HOSTILE_ILWRATH;
	}
	else if (PLAYER_SAID (R, thraddash))
	{
		NPCPhrase (THRADDASH_INFO_SB);
		HostileMask |= HOSTILE_THRADDASH;
	}

	if (!(HostileMask & HOSTILE_URQUAN))
		Response (urquan, HostileInfo);
	if (!(HostileMask & HOSTILE_KOHRAH))
		Response (kohrah, HostileInfo);
	if (!(HostileMask & HOSTILE_VUX))
		Response (vux, HostileInfo);
	if (!(HostileMask & HOSTILE_MYCON))
		Response (mycon, HostileInfo);
	if (!(HostileMask & HOSTILE_ANDROSYNTH))
		Response (androsynth, HostileInfo);
	if (!(HostileMask & HOSTILE_ILWRATH))
		Response (ilwrath, HostileInfo);
	if (!(HostileMask & HOSTILE_THRADDASH))
		Response (thraddash, HostileInfo);
	Response (enough_enemies, AlienRaces);
}

static void
NeutralInfo (RESPONSE_REF R)
{
#define NEUTRAL_DRUUGE (1 << 0)
#define NEUTRAL_UMGAH (1 << 1)
#define NEUTRAL_MELNORME (1 << 2)
#define NEUTRAL_SLYLANDRO (1 << 3)
	static BYTE NeutralMask = 0;
	static BYTE MelnormeInfoState = 0;
	
	if (PLAYER_SAID (R, neutral_races))
	{
		NPCPhrase (WHICH_NEUTRAL);
		NeutralMask = 0;
	}
	else if (PLAYER_SAID (R, druuge))
	{
		NPCPhrase (DRUUGE_INFO_SB);
		NeutralMask |= NEUTRAL_DRUUGE;
	}
	else if (PLAYER_SAID (R, umgah))
	{
		NPCPhrase (UMGAH_INFO_SB);
		NeutralMask |= NEUTRAL_UMGAH;
	}
	else if (PLAYER_SAID (R, melnorme))
	{
		NPCPhrase (MELNORME_INFO_SB);
		NeutralMask |= NEUTRAL_MELNORME;
		++MelnormeInfoState;
	}
	else if (PLAYER_SAID (R, where_melnorme_planet))
	{
		NPCPhrase (MELNORME_INFO_SB_2);
		++MelnormeInfoState;
	}
	else if (PLAYER_SAID (R, slylandro))
	{
		NPCPhrase (SLYLANDRO_INFO_SB);
		NeutralMask |= NEUTRAL_SLYLANDRO;
	}
	
	if (!(NeutralMask & NEUTRAL_UMGAH))
		Response (umgah, NeutralInfo);
	if (!(NeutralMask & NEUTRAL_DRUUGE))
		Response (druuge, NeutralInfo);
	if (!(NeutralMask & NEUTRAL_MELNORME))
		Response (melnorme, NeutralInfo);
	if (MelnormeInfoState == 1)
		Response (where_melnorme_planet, NeutralInfo);
	if (!(NeutralMask & NEUTRAL_SLYLANDRO))
		Response (slylandro, NeutralInfo);
	Response (enough_neutral, AlienRaces);
}

static void NormalStarbase (RESPONSE_REF R);

static void
AlienRaces (RESPONSE_REF R)
{
#define RACES_ALLIANCE (1 << 0)
#define RACES_HOSTILE (1 << 1)
#define RACES_NEUTRAL (1 << 2)
	static BYTE RacesMask = 0;

	if (PLAYER_SAID (R, info_on_races))
	{
		NPCPhrase (WHICH_ALIEN);
		RacesMask = 0;
	}
	else if (PLAYER_SAID (R, enough_allies))
	{
		NPCPhrase (ENOUGH_ALLIES_OK);
		RacesMask |= RACES_ALLIANCE;
	}
	else if (PLAYER_SAID (R, enough_enemies))
	{
		NPCPhrase (ENOUGH_HOSTILES_OK);
		RacesMask |= RACES_HOSTILE;
	}
	else if (PLAYER_SAID (R, enough_neutral))
	{
		NPCPhrase (ENOUGH_NEUTRAL_OK);
		RacesMask |= RACES_NEUTRAL;
	}

	if (!(RacesMask & RACES_ALLIANCE))
	{
		Response (alliance_races, AllianceInfo);
	}
	if (!(RacesMask & RACES_HOSTILE))
	{
		Response (hostile_races, HostileInfo);
	}
	if (!(RacesMask & RACES_NEUTRAL))
	{
		Response (neutral_races, NeutralInfo);
	}
	
	Response (enough_info_races, NormalStarbase);
}

static BOOLEAN
DiscussDevices (BOOLEAN TalkAbout)
{
	COUNT i, VuxBeastIndex, PhraseIndex;
	BOOLEAN Undiscussed;

	if (TalkAbout)
		NPCPhrase (DEVICE_HEAD);
	PhraseIndex = 2;

	VuxBeastIndex = 0;
	Undiscussed = FALSE;
	for (i = 0; i < NUM_DEVICES; ++i)
	{
		RESPONSE_REF pStr;

		pStr = 0;
		switch (i)
		{
			case ARTIFACT_2_DEVICE:
				if (GET_GAME_STATE (ARTIFACT_2_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_ARTIFACT_2))
				{
					pStr = ABOUT_ARTIFACT_2;
					SET_GAME_STATE (DISCUSSED_ARTIFACT_2, TalkAbout);
				}
				break;
			case ARTIFACT_3_DEVICE:
				if (GET_GAME_STATE (ARTIFACT_3_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_ARTIFACT_3))
				{
					pStr = ABOUT_ARTIFACT_3;
					SET_GAME_STATE (DISCUSSED_ARTIFACT_3, TalkAbout);
				}
				break;
			case SUN_EFFICIENCY_DEVICE:
				if (GET_GAME_STATE (SUN_DEVICE_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_SUN_EFFICIENCY))
				{
					pStr = ABOUT_SUN;
					SET_GAME_STATE (DISCUSSED_SUN_EFFICIENCY, TalkAbout);
				}
				break;
			case UTWIG_BOMB_DEVICE:
				if (GET_GAME_STATE (UTWIG_BOMB_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_UTWIG_BOMB))
				{
					pStr = ABOUT_BOMB;
					SET_GAME_STATE (DISCUSSED_UTWIG_BOMB, TalkAbout);
				}
				break;
			case ULTRON_0_DEVICE:
				if (GET_GAME_STATE (ULTRON_CONDITION) == 1
						&& !GET_GAME_STATE (DISCUSSED_ULTRON))
				{
					pStr = ABOUT_ULTRON_0;
					SET_GAME_STATE (DISCUSSED_ULTRON, TalkAbout);
				}
				break;
			case ULTRON_1_DEVICE:
				if (GET_GAME_STATE (ULTRON_CONDITION) == 2
						&& !GET_GAME_STATE (DISCUSSED_ULTRON))
				{
					pStr = ABOUT_ULTRON_1;
					SET_GAME_STATE (DISCUSSED_ULTRON, TalkAbout);
				}
				break;
			case ULTRON_2_DEVICE:
				if (GET_GAME_STATE (ULTRON_CONDITION) == 3
						&& !GET_GAME_STATE (DISCUSSED_ULTRON))
				{
					pStr = ABOUT_ULTRON_2;
					SET_GAME_STATE (DISCUSSED_ULTRON, TalkAbout);
				}
				break;
			case ULTRON_3_DEVICE:
				if (GET_GAME_STATE (ULTRON_CONDITION) == 4
						&& !GET_GAME_STATE (DISCUSSED_ULTRON))
				{
					pStr = ABOUT_ULTRON_3;
					SET_GAME_STATE (DISCUSSED_ULTRON, TalkAbout);
				}
				break;
			case TALKING_PET_DEVICE:
				if (GET_GAME_STATE (TALKING_PET_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_TALKING_PET))
				{
					pStr = ABOUT_TALKPET;
					SET_GAME_STATE (DISCUSSED_TALKING_PET, TalkAbout);
				}
				break;
			case AQUA_HELIX_DEVICE:
				if (GET_GAME_STATE (AQUA_HELIX_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_AQUA_HELIX))
				{
					pStr = ABOUT_HELIX;
					SET_GAME_STATE (DISCUSSED_AQUA_HELIX, TalkAbout);
				}
				break;
			case UMGAH_HYPERWAVE_DEVICE:
				if (GET_GAME_STATE (UMGAH_BROADCASTERS_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_UMGAH_HYPERWAVE))
				{
					pStr = ABOUT_UCASTER;
					SET_GAME_STATE (DISCUSSED_UMGAH_HYPERWAVE, TalkAbout);
				}
				break;
#ifdef NEVER
			case DATA_PLATE_1_DEVICE:
				if (GET_GAME_STATE (DATA_PLATE_1_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_DATA_PLATE_1))
				{
					pStr = ABOUT_DATAPLATE_1;
					SET_GAME_STATE (DISCUSSED_DATA_PLATE_1, TalkAbout);
				}
				break;
			case DATA_PLATE_2_DEVICE:
				if (GET_GAME_STATE (DATA_PLATE_2_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_DATA_PLATE_2))
				{
					pStr = ABOUT_DATAPLATE_2;
					SET_GAME_STATE (DISCUSSED_DATA_PLATE_2, TalkAbout);
				}
				break;
			case DATA_PLATE_3_DEVICE:
				if (GET_GAME_STATE (DATA_PLATE_3_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_DATA_PLATE_3))
				{
					pStr = ABOUT_DATAPLATE_3;
					SET_GAME_STATE (DISCUSSED_DATA_PLATE_3, TalkAbout);
				}
				break;
#endif /* NEVER */
			case TAALO_PROTECTOR_DEVICE:
				if (GET_GAME_STATE (TAALO_PROTECTOR_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_TAALO_PROTECTOR))
				{
					pStr = ABOUT_SHIELD;
					SET_GAME_STATE (DISCUSSED_TAALO_PROTECTOR, TalkAbout);
				}
				break;
			case EGG_CASING0_DEVICE:
				if (GET_GAME_STATE (EGG_CASE0_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_EGG_CASING0))
				{
					pStr = ABOUT_EGGCASE_0;
					SET_GAME_STATE (DISCUSSED_EGG_CASING0, TalkAbout);
					SET_GAME_STATE (DISCUSSED_EGG_CASING1, TalkAbout);
					SET_GAME_STATE (DISCUSSED_EGG_CASING2, TalkAbout);
				}
				break;
			case EGG_CASING1_DEVICE:
				if (GET_GAME_STATE (EGG_CASE1_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_EGG_CASING1))
				{
					pStr = ABOUT_EGGCASE_0;
					SET_GAME_STATE (DISCUSSED_EGG_CASING0, TalkAbout);
					SET_GAME_STATE (DISCUSSED_EGG_CASING1, TalkAbout);
					SET_GAME_STATE (DISCUSSED_EGG_CASING2, TalkAbout);
				}
				break;
			case EGG_CASING2_DEVICE:
				if (GET_GAME_STATE (EGG_CASE2_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_EGG_CASING2))
				{
					pStr = ABOUT_EGGCASE_0;
					SET_GAME_STATE (DISCUSSED_EGG_CASING0, TalkAbout);
					SET_GAME_STATE (DISCUSSED_EGG_CASING1, TalkAbout);
					SET_GAME_STATE (DISCUSSED_EGG_CASING2, TalkAbout);
				}
				break;
			case SYREEN_SHUTTLE_DEVICE:
				if (GET_GAME_STATE (SYREEN_SHUTTLE_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_SYREEN_SHUTTLE))
				{
					pStr = ABOUT_SHUTTLE;
					SET_GAME_STATE (DISCUSSED_SYREEN_SHUTTLE, TalkAbout);
				}
				break;
			case VUX_BEAST_DEVICE:
				if (GET_GAME_STATE (VUX_BEAST_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_VUX_BEAST))
				{
					pStr = ABOUT_VUXBEAST0;
					SET_GAME_STATE (DISCUSSED_VUX_BEAST, TalkAbout);
				}
				break;
			case DESTRUCT_CODE_DEVICE:
				if (GET_GAME_STATE (DESTRUCT_CODE_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_DESTRUCT_CODE))
				{
					pStr = ABOUT_DESTRUCT;
					SET_GAME_STATE (DISCUSSED_DESTRUCT_CODE, TalkAbout);
				}
				break;
			case PORTAL_SPAWNER_DEVICE:
				if (GET_GAME_STATE (PORTAL_SPAWNER_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_PORTAL_SPAWNER))
				{
					pStr = ABOUT_PORTAL;
					SET_GAME_STATE (DISCUSSED_PORTAL_SPAWNER, TalkAbout);
				}
				break;
			case URQUAN_WARP_DEVICE:
				if (GET_GAME_STATE (PORTAL_KEY_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_URQUAN_WARP))
				{
					pStr = ABOUT_WARPPOD;
					SET_GAME_STATE (DISCUSSED_URQUAN_WARP, TalkAbout);
				}
				break;
			case BURVIX_HYPERWAVE_DEVICE:
				if (GET_GAME_STATE (BURV_BROADCASTERS_ON_SHIP)
						&& !GET_GAME_STATE (DISCUSSED_BURVIX_HYPERWAVE))
				{
					pStr = ABOUT_BCASTER;
					SET_GAME_STATE (DISCUSSED_BURVIX_HYPERWAVE, TalkAbout);
				}
				break;
		}

		if (pStr)
		{
			if (TalkAbout)
			{
				if (PhraseIndex > 2)
					NPCPhrase (BETWEEN_DEVICES);
				NPCPhrase (pStr);
				if (pStr == ABOUT_VUXBEAST0)
				{
					VuxBeastIndex = ++PhraseIndex;
					NPCPhrase (ABOUT_VUXBEAST1);
				}
			}
			PhraseIndex += 2;
		}
	}

	if (TalkAbout)
	{
		NPCPhrase (DEVICE_TAIL);

		if (VuxBeastIndex)
		{
			for (i = 1; i < VuxBeastIndex; ++i)
				AlienTalkSegue (i);
			i = CommData.AlienTalkDesc.NumFrames;
			CommData.AlienTalkDesc.NumFrames = 0;
			AlienTalkSegue (VuxBeastIndex);
			CommData.AlienTalkDesc.NumFrames = i;
			AlienTalkSegue ((COUNT)~0);
		}
	}

	return (PhraseIndex > 2);
}

static BOOLEAN
CheckTiming (COUNT month_index, COUNT day_index)
{
	COUNT mi, year_index;
	BYTE days_in_month[12] =
	{
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
	};

	mi = GET_GAME_STATE (STARBASE_MONTH);
	year_index = START_YEAR;

	day_index += GET_GAME_STATE (STARBASE_DAY);
	while (day_index > days_in_month[mi - 1])
	{
		day_index -= days_in_month[mi - 1];
		if (++mi > 12)
		{
			mi = 1;
			++year_index;
		}
	}

	month_index += mi;
	year_index += (month_index - 1) / 12;
	month_index = ((month_index - 1) % 12) + 1;

	return (year_index < GLOBAL (GameClock.year_index)
			|| (year_index == GLOBAL (GameClock.year_index)
			&& (month_index < GLOBAL (GameClock.month_index)
			|| (month_index == GLOBAL (GameClock.month_index)
			&& day_index < GLOBAL (GameClock.day_index)))));
}

static void
NormalStarbase (RESPONSE_REF R)
{	
	static BYTE ExplorerInfoState = 0;
	static BYTE LandingInfoState = 0;
	static BYTE NewsState = 0;
	
	if (R == 0)
	{
		if(!(GET_GAME_STATE(STARBASE_VISITED_FOR_FIRST_TIME)))
		{
			SET_GAME_STATE (STARBASE_MONTH,
					GLOBAL (GameClock.month_index));
			SET_GAME_STATE (STARBASE_DAY,
					GLOBAL (GameClock.day_index));
			NPCPhrase (FIRST_HELLO);
			SET_GAME_STATE(STARBASE_VISITED_FOR_FIRST_TIME, 1);
		}
		else
		{
			RESPONSE_REF pStr0, pStr1;

			switch ((BYTE)TFB_Random () & 7)
			{
				case 0:
					pStr0 = NORMAL_HELLO_A0;
					pStr1 = NORMAL_HELLO_A1;
					break;
				case 1:
					pStr0 = NORMAL_HELLO_B0;
					pStr1 = NORMAL_HELLO_B1;
					break;
				case 2:
					pStr0 = NORMAL_HELLO_C0;
					pStr1 = NORMAL_HELLO_C1;
					break;
				case 3:
					pStr0 = NORMAL_HELLO_D0;
					pStr1 = NORMAL_HELLO_D1;
					break;
				case 4:
					pStr0 = NORMAL_HELLO_E0;
					pStr1 = NORMAL_HELLO_E1;
					break;
				case 5:
					pStr0 = NORMAL_HELLO_F0;
					pStr1 = NORMAL_HELLO_F1;
					break;
				case 6:
					pStr0 = NORMAL_HELLO_G0;
					pStr1 = NORMAL_HELLO_G1;
					break;
				case 7:
					pStr0 = NORMAL_HELLO_H0;
					pStr1 = NORMAL_HELLO_H1;
					break;
			}
			NPCPhrase (pStr0);
		}

		SET_GAME_STATE (STARBASE_VISITED, 1);
	}
	
	////// ANSWERS
	// Answer returning from AlienRaces
	if (PLAYER_SAID (R, enough_info_races))
		NPCPhrase (ENOUGH_INFO_RACES_OK);
	
	// Answer about landing on alliance home planets.
	else if (PLAYER_SAID (R, lame_landing_question_1))
	{
		NPCPhrase (LAME_LANDING_ANSWER_1);
		++LandingInfoState;
	}
	else if (PLAYER_SAID (R, lame_landing_question_2))
	{
		NPCPhrase (LAME_LANDING_ANSWER_2);
		++LandingInfoState;
	}
	else if (PLAYER_SAID (R, lame_landing_question_3))
	{
		NPCPhrase (LAME_LANDING_ANSWER_3);
		DISABLE_PHRASE(lame_landing_question_1);
		LandingInfoState = 0;
	}
	
	// Answer about ship
	else if (PLAYER_SAID (R, explorer_info))
	{
		NPCPhrase (ABOUT_EXPLORER_SHIP);
		++ExplorerInfoState;
	}
	else if (PLAYER_SAID (R, explorer_drawbacks))
	{
		NPCPhrase (ABOUT_EXPLORER_SHIP_2);
		++ExplorerInfoState;
	}
	else if (PLAYER_SAID (R, explorer_run))
	{
		NPCPhrase (ABOUT_EXPLORER_SHIP_3);
		DISABLE_PHRASE(explorer_info);
		ExplorerInfoState = 0;
	}
	
	// Answer news
	else if (PLAYER_SAID (R, current_news))
	{
		NPCPhrase (SHOFIXTI_MISSING);
		DISABLE_PHRASE(current_news);
		++NewsState;
	}
	// Answer map limits
	else if (PLAYER_SAID (R, lame_map_limit_question))
	{
		NPCPhrase (LAME_MAP_LIMIT_ANSWER);
		DISABLE_PHRASE(lame_map_limit_question);
		--NewsState;
	}
	
	///////QUESTIONS
	// Ask news
	if (PHRASE_ENABLED (current_news))
		Response (current_news, NormalStarbase);
	// Ask about map limitations
	else if (NewsState==1 && PHRASE_ENABLED (lame_map_limit_question))
		Response (lame_map_limit_question, NormalStarbase);
	
	// Minerals to offload
	if (GLOBAL_SIS (TotalElementMass))
		Response (have_minerals, SellMinerals);
	
	// Ask about ship
	if (ExplorerInfoState == 0 && PHRASE_ENABLED (explorer_info))
		Response (explorer_info, NormalStarbase);
	else if (ExplorerInfoState == 1)
		Response (explorer_drawbacks, NormalStarbase);
	else if (ExplorerInfoState == 2)
		Response (explorer_run, NormalStarbase);
	
	// Ask about landing on alliance home planets
	if (LandingInfoState == 0 && PHRASE_ENABLED (lame_landing_question_1))
		Response (lame_landing_question_1, NormalStarbase);
	else if (LandingInfoState == 1)
		Response (lame_landing_question_2, NormalStarbase);
	else if (LandingInfoState == 2)
		Response (lame_landing_question_3, NormalStarbase);
	
	// Devices
	//if (DiscussDevices (FALSE))
	//	Response (new_devices, NormalStarbase);
	
	// Ask about aliens and other info
	Response (info_on_races, AlienRaces);
	
	// Leave
	Response (goodbye_commander, ByeBye);
}

static void
SellMinerals (RESPONSE_REF R)
{
	COUNT i, total;
	BOOLEAN Sleepy;
	RESPONSE_REF pStr1, pStr2;

	total = 0;
	Sleepy = TRUE;
	for (i = 0; i < NUM_ELEMENT_CATEGORIES; ++i)
	{
		COUNT amount;
		DWORD TimeIn;

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
				if (!Sleepy || AnyButtonPress (TRUE))
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
// DrawStorageBays (FALSE);

	if (total < 1000)
	{
		total = GET_GAME_STATE (LIGHT_MINERAL_LOAD);
		switch (total++)
		{
			case 0:
				pStr1 = LIGHT_LOAD_A0;
				pStr2 = LIGHT_LOAD_A1;
				break;
			case 1:
				pStr1 = LIGHT_LOAD_B0;
				pStr2 = LIGHT_LOAD_B1;
				break;
			case 2:
				pStr1 = LIGHT_LOAD_C0;
				pStr2 = LIGHT_LOAD_C1;
				break;
			case 3:
				pStr1 = LIGHT_LOAD_D0;
				pStr2 = LIGHT_LOAD_D1;
				break;
			case 4:
				pStr1 = LIGHT_LOAD_E0;
				pStr2 = LIGHT_LOAD_E1;
				break;
			case 5:
				pStr1 = LIGHT_LOAD_F0;
				pStr2 = LIGHT_LOAD_F1;
				break;
			case 6:
				--total;
				pStr1 = LIGHT_LOAD_G0;
				pStr2 = LIGHT_LOAD_G1;
				break;
		}
		SET_GAME_STATE (LIGHT_MINERAL_LOAD, total);
	}
	else if (total < 2500)
	{
		total = GET_GAME_STATE (MEDIUM_MINERAL_LOAD);
		switch (total++)
		{
			case 0:
				pStr1 = MEDIUM_LOAD_A0;
				pStr2 = MEDIUM_LOAD_A1;
				break;
			case 1:
				pStr1 = MEDIUM_LOAD_B0;
				pStr2 = MEDIUM_LOAD_B1;
				break;
			case 2:
				pStr1 = MEDIUM_LOAD_C0;
				pStr2 = MEDIUM_LOAD_C1;
				break;
			case 3:
				pStr1 = MEDIUM_LOAD_D0;
				pStr2 = MEDIUM_LOAD_D1;
				break;
			case 4:
				pStr1 = MEDIUM_LOAD_E0;
				pStr2 = MEDIUM_LOAD_E1;
				break;
			case 5:
				pStr1 = MEDIUM_LOAD_F0;
				pStr2 = MEDIUM_LOAD_F1;
				break;
			case 6:
				--total;
				pStr1 = MEDIUM_LOAD_G0;
				pStr2 = MEDIUM_LOAD_G1;
				break;
		}
		SET_GAME_STATE (MEDIUM_MINERAL_LOAD, total);
	}
	else
	{
		total = GET_GAME_STATE (HEAVY_MINERAL_LOAD);
		switch (total++)
		{
			case 0:
				pStr1 = HEAVY_LOAD_A0;
				pStr2 = HEAVY_LOAD_A1;
				break;
			case 1:
				pStr1 = HEAVY_LOAD_B0;
				pStr2 = HEAVY_LOAD_B1;
				break;
			case 2:
				pStr1 = HEAVY_LOAD_C0;
				pStr2 = HEAVY_LOAD_C1;
				break;
			case 3:
				pStr1 = HEAVY_LOAD_D0;
				pStr2 = HEAVY_LOAD_D1;
				break;
			case 4:
				pStr1 = HEAVY_LOAD_E0;
				pStr2 = HEAVY_LOAD_E1;
				break;
			case 5:
				pStr1 = HEAVY_LOAD_F0;
				pStr2 = HEAVY_LOAD_F1;
				break;
			case 6:
				--total;
				pStr1 = HEAVY_LOAD_G0;
				pStr2 = HEAVY_LOAD_G1;
				break;
		}
		SET_GAME_STATE (HEAVY_MINERAL_LOAD, total);
	}

	NPCPhrase (pStr1);
	if (speechVolumeScale == 0.0f)
	{
		NPCPhrase (SPACE);
		NPCPhrase (GLOBAL_PLAYER_NAME);
	}
	NPCPhrase (pStr2);

	NormalStarbase (R);
}

static void
Intro (void)
{
	NormalStarbase (0);
}

static COUNT
uninit_starbase (void)
{
	return (0);
}

static void
post_starbase_enc (void)
{
}

LOCDATA*
init_starbase_comm ()
{
	LOCDATA *retval;

	commander_desc.init_encounter_func = Intro;
	commander_desc.post_encounter_func = post_starbase_enc;
	commander_desc.uninit_encounter_func = uninit_starbase;

	commander_desc.AlienTextWidth = 143;
	commander_desc.AlienTextBaseline.x = 164;
	commander_desc.AlienTextBaseline.y = 20;

	// use alternate Starbase track if available
	commander_desc.AlienAltSongRes = STARBASE_ALT_MUSIC;
	commander_desc.AlienSongFlags |= LDASF_USE_ALTERNATE;

	CurBulletinMask = 0;
	SET_GAME_STATE (BATTLE_SEGUE, 0);
	retval = &commander_desc;

	return (retval);
}
