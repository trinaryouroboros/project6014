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

// JMS 2009: Function call for initing orz space portal when encountering its enum label
// JMS 2010: -Function call for initing Lurg home system when encountering its enum label
//			 -Function call for initing those ones that mysteriously vanished in a hurry
//			 -Function call for initing Gamma Janus, the supposed precursor artifact

#include "gendef.h"
#include "encount.h"
#include "planets/generate.h"


extern GenerateFunctions generateDefaultFunctions;

extern GenerateFunctions generateAndrosynthFunctions;
extern GenerateFunctions generateBurvixeseFunctions;
extern GenerateFunctions generateChmmrFunctions;
extern GenerateFunctions generateColonyFunctions;
extern GenerateFunctions generateDruugeFunctions;
extern GenerateFunctions generateHint1Functions;
extern GenerateFunctions generateHint2Functions;
extern GenerateFunctions generateIlwrathFunctions;
extern GenerateFunctions generateLurgFunctions;
extern GenerateFunctions generateMelnormeFunctions;
extern GenerateFunctions generateMyconFunctions;
extern GenerateFunctions generateOrzFunctions;
extern GenerateFunctions generateOrzSpacePortalFunctions;
extern GenerateFunctions generatePkunkFunctions;
extern GenerateFunctions generateRainbowWorldFunctions;
extern GenerateFunctions generateSaMatraFunctions;
extern GenerateFunctions generateShofixtiFunctions;
extern GenerateFunctions generateShofixtiCrashSiteFunctions;
extern GenerateFunctions generateShofixtiColonyFunctions;
extern GenerateFunctions generateSlaveraceFunctions;
extern GenerateFunctions generateSlylandroFunctions;
extern GenerateFunctions generateSolFunctions;
extern GenerateFunctions generateSpathiFunctions;
extern GenerateFunctions generateSupoxFunctions;
extern GenerateFunctions generateSyreenFunctions;
extern GenerateFunctions generateThraddashFunctions;
extern GenerateFunctions generateTrapFunctions;
extern GenerateFunctions generateUtwigFunctions;
extern GenerateFunctions generateVaultFunctions;
extern GenerateFunctions generateVuxFunctions;
extern GenerateFunctions generateWreckFunctions;
extern GenerateFunctions generateYehatFunctions;
extern GenerateFunctions generateZoqFotPikFunctions;


const GenerateFunctions *
getGenerateFunctions (BYTE Index)
{
	switch (Index)
	{
		case SOL_DEFINED:
			return &generateSolFunctions;
		case SHOFIXTI_DEFINED:
			return &generateShofixtiFunctions;
		case START_COLONY_DEFINED:
			return &generateColonyFunctions;
		case SPATHI_DEFINED:
			return &generateSpathiFunctions;
		case MELNORME0_DEFINED:
		case MELNORME1_DEFINED:
		case MELNORME2_DEFINED:
		case MELNORME3_DEFINED:
		case MELNORME4_DEFINED:
		case MELNORME5_DEFINED:
		case MELNORME6_DEFINED:
		case MELNORME7_DEFINED:
		case MELNORME8_DEFINED:
			return &generateMelnormeFunctions;
		case CHMMR_DEFINED:
			return &generateChmmrFunctions;
		case SYREEN_DEFINED:
			return &generateSyreenFunctions;
		case MYCON_TRAP_DEFINED:
			return &generateTrapFunctions;
		case BURVIXESE_DEFINED:
			return &generateBurvixeseFunctions;
		case SLYLANDRO_DEFINED:
			return &generateSlylandroFunctions;
		case DRUUGE_DEFINED:
			return &generateDruugeFunctions;
		case BOMB_DEFINED:
		case UTWIG_DEFINED:
			return &generateUtwigFunctions;
		case AQUA_HELIX_DEFINED:
		case THRADD_DEFINED:
			return &generateThraddashFunctions;
		case SUN_DEVICE_DEFINED:
		case MYCON_DEFINED:
		case EGG_CASE0_DEFINED:
		case EGG_CASE1_DEFINED:
		case EGG_CASE2_DEFINED:
			return &generateMyconFunctions;
		case ANDROSYNTH_DEFINED:
			return &generateAndrosynthFunctions;
		case TAALO_PROTECTOR_DEFINED:
		case ORZ_DEFINED:
			return &generateOrzFunctions;
		case SHIP_VAULT_DEFINED:
			return &generateVaultFunctions;
		case URQUAN_WRECK_DEFINED:
			return &generateWreckFunctions;
		case MAIDENS_DEFINED:
		case VUX_BEAST_DEFINED:
		case VUX_DEFINED:
			return &generateVuxFunctions;
		case SAMATRA_DEFINED:
			return &generateSaMatraFunctions;
		case ZOQFOT_DEFINED:
			return &generateZoqFotPikFunctions;
		case YEHAT_DEFINED:
			return &generateYehatFunctions;
		case PKUNK_DEFINED:
			return &generatePkunkFunctions;
		case SUPOX_DEFINED:
			return &generateSupoxFunctions;
		case RAINBOW_DEFINED:
		case RAINBOW_KNOWN_DEFINED:  // JMS - hook to generating the Zeta Sextantis rainbow world which is already known to Melnorme
			return &generateRainbowWorldFunctions;
		case ILWRATH_DEFINED:
			return &generateIlwrathFunctions;
		case SLAVERACE_DEFINED:
			return &generateSlaveraceFunctions;
		case ORZ_SPACE_PORTAL_DEFINED: // JMS - hook to generating the contents of portal star system 
			return &generateOrzSpacePortalFunctions;
		case LURG_DEFINED:
			return &generateLurgFunctions;	 // JMS - hook to generating Lurg home system
		case SHOFIXTI_CRASH_SITE_DEFINED:
			return &generateShofixtiCrashSiteFunctions;	 // JMS - hook to generating Shofixti crash site
		case HINT_DEFINED:
			return &generateHint1Functions;	 // JMS - hook to generating hint for those ones mysteriously vanished in a hurry
		case HINT2_DEFINED:
			return &generateHint2Functions; // JMS - hook to generating hint in Gamma Janus for the supposed precursor artifact
		case SHOFIXTI_COLONY_DEFINED:
			return &generateShofixtiColonyFunctions;	 // JMS - hook to generating Shofixti colony
			break;
		default:
			return &generateDefaultFunctions;
	}
}

