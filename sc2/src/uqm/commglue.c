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

// JMS 2009: -Added earthling ship and Androsynth dialogue screen hooks
// JMS 2010: -Removed unnecessary init_commander_comm
//			 -Added transport ship dialogue screen hook

#include "commglue.h"

#include "races.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include "libs/log.h"

int NPCNumberPhrase (int number, UNICODE **ptrack);

/**
 * Gets the content for index and loads it into the current conversation,
 * using the voice track cb if provided, or making one itself if not.
 * 
 * The indices are:
 * - 1 000 000 : captain's name
 *   - 999 999 : ship's name
 *   - 999 998 : ship's location
 * numbers from -999 997 to 998 997 : display the name of your alliance
 * (not sure why this one option needs 1001 indices)
 * numbers from -998 996 to -1 : alien number speech (sign is flipped)
 *           0 : say and do nothing
 * positive numbers : entries from the current alien conversation's string table 
 */
void
NPCPhrase_cb (int index,  TFB_TrackCB cb)
{
	UNICODE *pStr, numbuf[400];
	void *pClip, *pTimeStamp;

	switch (index)
	{
		case GLOBAL_PLAYER_NAME:
			pStr = GLOBAL_SIS (CommanderName);
			pClip = 0;
			pTimeStamp = 0;
			break;
		case GLOBAL_SHIP_NAME:
			pStr = GLOBAL_SIS (ShipName);
			pClip = 0;
			pTimeStamp = 0;
			break;
		case GLOBAL_PLAYER_LOCATION:
		{
			SIZE dx, dy;
			COUNT adx, ady;

			dx = LOGX_TO_UNIVERSE (GLOBAL_SIS (log_x)) - 333;
			adx = dx >= 0 ? dx : -dx;
			dy = 9812 - LOGY_TO_UNIVERSE (GLOBAL_SIS (log_y));
			ady = dy >= 0 ? dy : -dy;
			sprintf (numbuf,
					"%+04d.%01u,%+04d.%01u",
					(SIZE)(dy / 10), (COUNT)(ady % 10),
					(SIZE)(dx / 10), (COUNT)(adx % 10));
			pStr = numbuf;
			pClip = 0;
			pTimeStamp = 0;
			break;
		}
		case 0:
		{
			return;
		}
		default:
			if (index < 0)
			{
				if (index > UNREASONABLE_NUMBER)
				{
					if (CommData.AlienNumberSpeech)
					{
						NPCNumberPhrase (-index, NULL);
						return;
					}
					sprintf (numbuf, "%d", -index);
				}
				else
				{
					COUNT i;
					STRING S;

					index -= GLOBAL_ALLIANCE_NAME;

					i = GET_GAME_STATE (NEW_ALLIANCE_NAME);
					S = SetAbsStringTableIndex (CommData.ConversationPhrases, (index - 1) + i);
					strcpy (numbuf, (UNICODE *)GetStringAddress (S));
					if (i == 3)
						strcat (numbuf, GLOBAL_SIS (CommanderName));
				}
				pStr = numbuf;
				pClip = 0;
				pTimeStamp = 0;
			}
			else
			{
				pStr = (UNICODE *)GetStringAddress (
						SetAbsStringTableIndex (CommData.ConversationPhrases, index - 1)
						);
				pClip = GetStringSoundClip (
						SetAbsStringTableIndex (CommData.ConversationPhrases, index - 1)
						);
				pTimeStamp = GetStringTimeStamp (
						SetAbsStringTableIndex (CommData.ConversationPhrases, index - 1)
						);
			}
			break;
	}

	if (GLOBAL (glob_flags) & VOICE_DISABLED || pClip == NULL)
		pClip = "noname.ogg";
	SpliceTrack (pClip, pStr, pTimeStamp, cb);
}

int
NPCNumberPhrase (int number, UNICODE **ptrack)
{
#define MAX_NUMBER_TRACKS 20
	NUMBER_SPEECH speech = CommData.AlienNumberSpeech;
	COUNT i;
	int queued = 0;
	int toplevel = 0;
	UNICODE *TrackNames[MAX_NUMBER_TRACKS];
	UNICODE numbuf[60];

	if (!speech)
		return 0;

	if (!ptrack)
	{
		toplevel = 1;
		sprintf (numbuf, "%d", number);
		ptrack = TrackNames;
	}

	for (i = 0; i < speech->NumDigits; ++i)
	{
		SPEECH_DIGIT* dig = speech->Digits + i;
		int quot;

		quot = number / dig->Divider;
	
		if (quot == 0)
			continue;
		quot -= dig->Subtrahend;
		if (quot < 0)
			continue;

		if (dig->StrDigits)
		{
			COUNT index;

			assert (quot < 10);
			index = dig->StrDigits[quot];
			if (index == 0)
				continue;
			index -= 1;

			*ptrack++ = GetStringSoundClip (SetAbsStringTableIndex (
					CommData.ConversationPhrases, index
					));
			queued++;
		}
		else
		{
			int ctracks = NPCNumberPhrase (quot, ptrack);
			ptrack += ctracks;
			queued += ctracks;
		}

		if (dig->Names != 0)
		{
			SPEECH_DIGITNAME* name;

			for (name = dig->Names; name->Divider; ++name)
			{
				if (number % name->Divider <= name->MaxRemainder)
				{
					*ptrack++ = GetStringSoundClip (
							SetAbsStringTableIndex (
							CommData.ConversationPhrases,
							(COUNT) (name->StrIndex - 1)
							));
					queued++;
					break;
				}
			}
		}
		else if (dig->CommonNameIndex != 0)
		{
			*ptrack++ = GetStringSoundClip (SetAbsStringTableIndex (
					CommData.ConversationPhrases,
					(COUNT) (dig->CommonNameIndex - 1)
					));
			queued++;
		}

		number %= dig->Divider;
	}

	if (toplevel)
	{
		if (queued == 0)
		{	// nothing queued, say "zero"
			*ptrack++ = GetStringSoundClip (SetAbsStringTableIndex (
					CommData.ConversationPhrases,
					speech->Digits[speech->NumDigits - 1].StrDigits[0]
					));
			
		}
		*ptrack++ = NULL; // term
		
		SpliceMultiTrack (TrackNames, numbuf);
	}
	
	return queued;
}

void
GetAllianceName (UNICODE *buf, RESPONSE_REF name_1)
{
	COUNT i;
	STRING S;

	i = GET_GAME_STATE (NEW_ALLIANCE_NAME);
	S = SetAbsStringTableIndex (CommData.ConversationPhrases, (name_1 - 1) + i);
	// XXX: this should someday be changed so that the function takes
	//   the buffer size as an argument
	strcpy (buf, (UNICODE *)GetStringAddress (S));
	if (i == 3)
	{
		strcat (buf, GLOBAL_SIS (CommanderName));
		strcat (buf, (UNICODE *)GetStringAddress (SetRelStringTableIndex (S, 1)));
	}
}

void
construct_response (UNICODE *buf, int R /* promoted from RESPONSE_REF */, ...)
{
	UNICODE *buf_start = buf;
	UNICODE *name;
	va_list vlist;
	
	va_start (vlist, R);
	
	do
	{
		COUNT len;
		STRING S;
		
		S = SetAbsStringTableIndex (CommData.ConversationPhrases, R - 1);
		
		strcpy (buf, (UNICODE *)GetStringAddress (S));
		
		len = strlen (buf);
		
		buf += len;
		
		name = va_arg (vlist, UNICODE *);
		
		if (name)
		{
			len = strlen (name);
			strncpy (buf, name, len);
			buf += len;
			
			/*
			if ((R = va_arg (vlist, RESPONSE_REF)) == (RESPONSE_REF)-1)
				name = 0;
			*/
					
			R = va_arg(vlist, int);
			if (R == ((RESPONSE_REF) -1))
				name = 0;
		}
	} while (name);
	va_end (vlist);
	
	*buf = '\0';

	// XXX: this should someday be changed so that the function takes
	//   the buffer size as an argument
	if ((buf_start == shared_phrase_buf) &&
			(buf > shared_phrase_buf + sizeof (shared_phrase_buf)))
	{
		log_add (log_Fatal, "Error: shared_phrase_buf size exceeded,"
				" please increase!\n");
		exit (EXIT_FAILURE);
	}
}

LOCDATA*
init_race (CONVERSATION comm_id)
{
	switch (comm_id)
	{
		case ANDROSYNTH_CONVERSATION:		// BY JMS - Hook to Androsynth dialogue screen
			return init_androsynth_comm ();
		case ARILOU_CONVERSATION:
			return init_arilou_comm ();
		case BLACKURQ_CONVERSATION:
			return init_blackurq_comm ();
		case CHMMR_CONVERSATION:
			return init_chmmr_comm ();
		case COMMANDER_CONVERSATION:
			// JMS: Commented out the unnecessary init_commander_comm
			//if (!GET_GAME_STATE (STARBASE_AVAILABLE))
			//	return init_commander_comm ();
			//else
			return init_starbase_comm ();
		case DRUUGE_CONVERSATION:
			return init_druuge_comm ();
		case HUMAN_CONVERSATION:			// BY JMS - Hook to earthling ship dialogue screen
			return init_human_comm ();
		case ILWRATH_CONVERSATION:
			return init_ilwrath_comm ();
		case LURG_CONVERSATION:				// BY JMS - Hook to Lurg ship dialogue screen
			return init_lurg_comm ();
		case MELNORME_CONVERSATION:
			return init_melnorme_comm ();
		case MYCON_CONVERSATION:
			return init_mycon_comm ();
		case ORZ_CONVERSATION:
			return init_orz_comm ();
		case PKUNK_CONVERSATION:
			return init_pkunk_comm ();
		case SHOFIXTI_CONVERSATION:
			return init_shofixti_comm ();
		case SHOFIXTICOLONY_CONVERSATION:
			return init_shofixticolony_comm (); // BY JMS - Hook to Shofixti colony dialogue screen
		case SLYLANDRO_CONVERSATION:
			return init_slyland_comm ();
		case SLYLANDRO_HOME_CONVERSATION:
			return init_slylandro_comm ();
		case SPATHI_CONVERSATION:
			return init_spahome_comm ();
		case SUPOX_CONVERSATION:
			return init_supox_comm ();
		case SYREEN_CONVERSATION:
			return init_syreen_comm ();
		case SYREENBASE_CONVERSATION:
			return init_syreenbase_comm ();
		case SYREENHOME_CONVERSATION:
			return init_syreenhome_comm ();
		case THRADD_CONVERSATION:
			return init_thradd_comm ();
		case TRANSPORT_CONVERSATION:			// BY JMS - Hook to transport ship dialogue screen
			return init_transport_comm ();
		case UMGAH_CONVERSATION:
			return init_umgah_comm ();
		case URQUAN_CONVERSATION:
			return init_urquan_comm ();
		case UTWIG_CONVERSATION:
			return init_utwig_comm ();
		case VUX_CONVERSATION:
			return init_vux_comm ();
		case YEHAT_REBEL_CONVERSATION:
			return init_rebel_yehat_comm ();
		case YEHAT_CONVERSATION:
			return init_yehat_comm ();
		case YEHATPKUNK_CONVERSATION:
			return init_yehatpkunk_comm ();
		case ZOQFOTPIK_CONVERSATION:
			return init_zoqfot_comm ();
		default:
			return init_chmmr_comm ();
	}
}
