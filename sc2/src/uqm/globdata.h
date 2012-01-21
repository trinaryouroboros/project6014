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

// JMS 2009: -Added new game states for "new" race - humans
//			 -Added IN_ORZSPACE flag for saved game summaries
//			 -Added new game state for Vela factory visit
// JMS 2010: -Added new game state for which ship is the flagship
//			 -Added new device: Black orb
//			 -Added new device: Temporal wrapper
//			 -Increased max number of comm screen animations for one race from 20 to 30
//			 -Added BLACK_ORB_STATE game state for handling the events upon finding the black orb.
//			 -Added BLACK_ORB_CUTSCENE activity enum for initiating the cutscene
//			 -Changed Animflags size from BYTE to COUNT (Possibility to add more flags later on.)
//			 -Lots of new game states... see the bottom of the ADD_GAME_STATE list.			
//
// DN DEC10		-added game state flags for Melnorme Bio-Data easter egg

#ifndef _GLOBDATA_H
#define _GLOBDATA_H

#include "clock.h"
#include "libs/gfxlib.h"
#include "libs/reslib.h"
#include "libs/sndlib.h"
#include "sis.h"
#include "velocity.h"
#include "commanim.h"


#define MAX_FEATURE_OPTIONS 5

typedef struct
{
	COUNT StartIndex;
			// Index of the first possible image
	BYTE NumFrames;
			// Number of distinct possible images for the feature.

	DWORD BlockMaskArray[MAX_FEATURE_OPTIONS];
			// Array of the bit masks of indices of all
			// animations that are incompatible with each
			// option for this feature.
} FEATURE_DESC;

// general numbers-speech generator info
// should accomodate most common base-10 languages
// many languages require various plural forms
// for digit names like "hundred"
// possibly needs reworking for others
typedef struct
{
	// an array of these structs must be in ascending remainder order
	// terminate the array with Divider == 0

	// digit divider, i.e. 1, 10, 100, etc.
	int Divider;
	// maximum remainder for this name
	// name will be used if Number % Divider <= MaxRemainder
	int MaxRemainder;
	// string table index for this name
	// i.e. "hundred" in English
	COUNT StrIndex;
} SPEECH_DIGITNAME;

typedef struct
{
	// digit divider, i.e. 1, 10, 100, etc.
	int Divider;
	// digit sub, i.e. 10 for teens
	// subtracted from the value to get an index into StrDigits
	int Subtrahend;
	// ptr to 10 indices for this digit
	// index is string table ptr when > 0
	//       is invalid (should not happen) or
	//       is a a 'skip digit' indicator when == 0 
	// StrDigits can be NULL, in which case
	// the value is interpreted recursively
	COUNT *StrDigits;
	// digit Names, can be NULL, in which case
	// CommonNameIndex is used
	SPEECH_DIGITNAME *Names;
	// common digit name string table index
	// i.e. "hundred" in English
	COUNT CommonNameIndex;
} SPEECH_DIGIT;

// this accomodates up to "billions" in english
#define MAX_SPEECH_DIGITS 7

typedef struct
{
	// slots used in Digits array
	COUNT NumDigits;
	// slots for each digit in numbers
	// teens is exception
	// 0-9, 10-19, ..20-90, ..100-900, etc.
	SPEECH_DIGIT Digits[MAX_SPEECH_DIGITS];
} NUMBER_SPEECH_DESC;
typedef NUMBER_SPEECH_DESC *NUMBER_SPEECH;

typedef DWORD LDAS_FLAGS;
#define LDASF_NONE           ((LDAS_FLAGS)      0 )
#define LDASF_USE_ALTERNATE  ((LDAS_FLAGS)(1 << 0))


// BW: had to move that from commglue.h to here because now LOCDATA features it
typedef enum {
	ARILOU_CONVERSATION,
	CHMMR_CONVERSATION,
	COMMANDER_CONVERSATION,
	ORZ_CONVERSATION,
	PKUNK_CONVERSATION,
	SHOFIXTI_CONVERSATION,
	SPATHI_CONVERSATION,
	SUPOX_CONVERSATION,
	THRADD_CONVERSATION,
	UTWIG_CONVERSATION,
	VUX_CONVERSATION,
	YEHAT_CONVERSATION,
	MELNORME_CONVERSATION,
	DRUUGE_CONVERSATION,
	ILWRATH_CONVERSATION,
	MYCON_CONVERSATION,
	SLYLANDRO_CONVERSATION,
	UMGAH_CONVERSATION,
	URQUAN_CONVERSATION,
	ZOQFOTPIK_CONVERSATION,
	SYREEN_CONVERSATION,
	BLACKURQ_CONVERSATION,
	ANDROSYNTH_CONVERSATION,	// JMS
	BAUL_CONVERSATION,			// JMS
	FOONFOON_CONVERSATION,		// JMS
	LURG_CONVERSATION,			// JMS
	HUMAN_CONVERSATION,			// JMS
	SLYLANDRO_HOME_CONVERSATION,
	YEHAT_REBEL_CONVERSATION,
	TRANSPORT_CONVERSATION,		// JMS
	SYREENBASE_CONVERSATION,
	SYREENHOME_CONVERSATION,
	YEHATPKUNK_CONVERSATION,  // BW
	SHOFIXTICOLONY_CONVERSATION,// JMS
	INVALID_CONVERSATION,
} CONVERSATION;

typedef struct
{
	CONVERSATION AlienConv;
	void (*init_encounter_func) (void);
			/* Called when entering communications */
	void (*post_encounter_func) (void);
			/* Called when leaving communications or combat normally */
	COUNT (*uninit_encounter_func) (void);
			/* Called when encounter is done for cleanup */

	RESOURCE AlienFrameRes;
	RESOURCE AlienFontRes;
	Color AlienTextFColor, AlienTextBColor;
	POINT AlienTextBaseline;
	COUNT AlienTextWidth;
	TEXT_ALIGN AlienTextAlign;
	TEXT_VALIGN AlienTextValign;
	RESOURCE AlienColorMapRes;
	RESOURCE AlienSongRes;
	RESOURCE AlienAltSongRes;
	LDAS_FLAGS AlienSongFlags;

	RESOURCE ConversationPhrasesRes;

	COUNT NumAnimations;
	ANIMATION_DESC AlienAmbientArray[MAX_ANIMATIONS];

	// Transition animation to/from talking state;
	// the first frame is neutral (sort of like YOYO_ANIM)
	ANIMATION_DESC AlienTransitionDesc;
	// Talking animation, like RANDOM_ANIM, except random frames
	// always alternate with a neutral frame;
	// the first frame is neutral
	ANIMATION_DESC AlienTalkDesc;

	NUMBER_SPEECH AlienNumberSpeech;

	FRAME AlienFrame;
	FONT AlienFont;
	COLORMAP AlienColorMap;
	MUSIC_REF AlienSong;
	STRING ConversationPhrases;
	
	COUNT NumFeatures;
	FEATURE_DESC AlienFeatureArray[MAX_ANIMATIONS];
	COUNT AlienFeatureChoice[MAX_ANIMATIONS];
} LOCDATA;

typedef struct
{
        RESOURCE StarbaseFrameRes;
	COUNT NumAnimations;
	ANIMATION_DESC StarbaseAmbientArray[MAX_ANIMATIONS];

	FRAME StarbaseFrame;
	COLORMAP StarbaseColorMap;
	
} SBDATA;


enum
{
	PORTAL_SPAWNER_DEVICE = 0,
	TALKING_PET_DEVICE,
	UTWIG_BOMB_DEVICE,
	SUN_EFFICIENCY_DEVICE,
	BLACK_ORB_DEVICE,	// JMS: Replaces ROSY_SPHERE_DEVICE
	AQUA_HELIX_DEVICE,
	SHIELD_BUSTER_DEVICE,	// JMS: Replaces CLEAR_SPINDLE_DEVICE
	ULTRON_0_DEVICE,
	ULTRON_1_DEVICE,
	ULTRON_2_DEVICE,
	ULTRON_3_DEVICE,
	TEMPORAL_WRAPPER_DEVICE,	// JMS: Replaces MAIDENS_DEVICE
	UMGAH_HYPERWAVE_DEVICE,
	BURVIX_HYPERWAVE_DEVICE,
	DATA_PLATE_1_DEVICE,
	DATA_PLATE_2_DEVICE,
	DATA_PLATE_3_DEVICE,
	TAALO_PROTECTOR_DEVICE,
	EGG_CASING0_DEVICE,
	EGG_CASING1_DEVICE,
	EGG_CASING2_DEVICE,
	SYREEN_SHUTTLE_DEVICE,
	VUX_BEAST_DEVICE,
	DESTRUCT_CODE_DEVICE,
	URQUAN_WARP_DEVICE,
	ARTIFACT_2_DEVICE,
	ARTIFACT_3_DEVICE,
	LUNAR_BASE_DEVICE,

	NUM_DEVICES
};

#define YEARS_TO_KOHRAH_VICTORY 4

#define START_GAME_STATE enum {
#define ADD_GAME_STATE(SName,NumBits) SName, END_##SName = SName + NumBits - 1,
#define END_GAME_STATE NUM_GAME_STATE_BITS };

START_GAME_STATE
#include "gamestate.inc"
END_GAME_STATE

#undef ADD_GAME_STATE

// See game state THRADDASH_BODY_COUNT
#define THRADDASH_BODY_THRESHOLD 25

// See game state WHICH_SHIP_PLAYER_HAS
#define CHMMR_EXPLORER_SHIP 0
#define PRECURSOR_SERVICE_VEHICLE 1
#define PRECURSOR_BATTLESHIP 2



// Values for GAME_STATE.glob_flags:
#define COMBAT_SPEED_SHIFT 7
#define COMBAT_SPEED_MASK (((1 << 2) - 1) << COMBAT_SPEED_SHIFT)
#define NUM_COMBAT_SPEEDS 4
#define MUSIC_DISABLED (1 << 3)
#define SOUND_DISABLED (1 << 4)
#define VOICE_DISABLED (1 << 5)
#define CYBORG_ENABLED (1 << 6)

enum
{
	SUPER_MELEE = 0, /* Is also used while in the main menu */
	IN_LAST_BATTLE,
	IN_ENCOUNTER,
	IN_HYPERSPACE /* in Hyperspace or Quasispace */,
	IN_INTERPLANETARY,
	WON_LAST_BATTLE,

	/* The following four are only used when displaying save game
	 * summaries */
	IN_QUASISPACE,
	IN_PLANET_ORBIT,
	IN_STARBASE,
	IN_ORZSPACE,	// JMS: In *below* (Orz space) or not
	
	BLACK_ORB_CUTSCENE, // JMS: For initiating cutscene after finding the black orb
//#define BLACK_ORB_CUTSCENE SUPER_MELEE
	
	CHECK_PAUSE = MAKE_WORD (0, (1 << 0)),
	IN_BATTLE = MAKE_WORD (0, (1 << 1)),
	START_ENCOUNTER = MAKE_WORD (0, (1 << 2)),
	START_INTERPLANETARY = MAKE_WORD (0, (1 << 3)),
	CHECK_LOAD = MAKE_WORD (0, (1 << 4)),
	CHECK_RESTART = MAKE_WORD (0, (1 << 5)),
	CHECK_ABORT = MAKE_WORD (0, (1 << 6)),
};
typedef UWORD ACTIVITY;

typedef struct
{
	UWORD glob_flags;
			// See above for the meaning of the bits.

	BYTE CrewCost, FuelCost;
	BYTE ModuleCost[NUM_MODULES];
	BYTE ElementWorth[NUM_ELEMENT_CATEGORIES];

	PRIMITIVE *DisplayArray;
	ACTIVITY CurrentActivity;

	CLOCK_STATE GameClock;

	POINT autopilot;
    DPOINT autopilotOrzSpaceCenter;
    SDWORD autopilotOrzSpaceRadius;
	POINT ip_location;
	STAMP ShipStamp;
	UWORD ShipFacing;
	BYTE ip_planet;
	BYTE in_orbit;
	VELOCITY_DESC velocity;

	DWORD BattleGroupRef;
	QUEUE avail_race_q;
			/* List of all the races in the game with information
			 * about their ships, and what player knows about their
			 * fleet, center of SoI, status, etc.
			 * queue element is FLEET_INFO */
	QUEUE npc_built_ship_q;
			/* Non-player-character list of ships (during encounter)
			 * queue element is SHIP_FRAGMENT */
	QUEUE ip_group_q;
			/* List of groups present in solarsys (during IP);
			 * queue element is IP_GROUP */
	QUEUE encounter_q;
			/* List of HyperSpace encounters (black globes);
			 * queue element is ENCOUNTER */
	QUEUE built_ship_q;
			/* List of SIS escort ships;
			 * queue element is SHIP_FRAGMENT */

	BYTE GameState[(NUM_GAME_STATE_BITS + 7) >> 3];
} GAME_STATE;

typedef struct
{
	SIS_STATE SIS_state;
	GAME_STATE Game_state;
} GLOBDATA;

extern GLOBDATA GlobData;
#define GLOBAL(f) GlobData.Game_state.f
#define GLOBAL_SIS(f) GlobData.SIS_state.f

#define MAX_ENCOUNTERS  16
#define MAX_BATTLE_GROUPS 32

//#define STATE_DEBUG
	
extern BYTE getGameState (int startBit, int endBit);
extern void setGameState (int startBit, int endBit, BYTE val
#ifdef STATE_DEBUG
		, const char *name
#endif
		);

#define GET_GAME_STATE(SName) getGameState ((SName), (END_##SName))
#ifdef STATE_DEBUG
#	define SET_GAME_STATE(SName, val) \
			setGameState ((SName), (END_##SName), (val), #SName)
#else
#	define SET_GAME_STATE(SName, val) \
			setGameState ((SName), (END_##SName), (val))
#endif

extern DWORD getGameState32 (int startBit);
extern void setGameState32 (int startBit, DWORD val
#ifdef STATE_DEBUG
		, const char *name
#endif
		);

#define GET_GAME_STATE_32(SName) getGameState32 ((SName))
#ifdef STATE_DEBUG
#	define SET_GAME_STATE_32(SName, val) \
			setGameState32 ((SName), (val), #SName)
#else
#	define SET_GAME_STATE_32(SName, val) \
			setGameState32 ((SName), (val))
#endif

	
extern CONTEXT RadarContext;

extern void FreeSC2Data (void);
extern BOOLEAN LoadSC2Data (void);

extern void InitGlobData (void);

extern BOOLEAN InitGameStructures (void);
extern void UninitGameStructures (void);

extern void DumpAllGameStates(void);

#endif /* _GLOBDATA_H */

