/*
 *  conversation.c
 * 
 *  Created by Luke Somers on 2012-01-20, for Project 6014.
 *
 */

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


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "libs/strings/strintrn.h"
#include "libs/strlib.h"
#include "libs/uio.h"

#include "conversation.h"
#include "comm.h"
#include "options.h"

#include <glib.h>

# define GET_LINE (get_line (line, sizeof (line), hook))
/* with its whitespace trimmed and any '//'-style comments stripped */
/* if there's no such line, return NULL */

/*
 Concerning members  'GSList *lines;':
 
 The format of this is alternating.
 
 Odd element: index, stored directly by casting to pointer, of text in string table, or '0' for nothing.
 Even element: char* pointing to string that needs to be evaluated, or '0' for nothing.
 
 Indices in string table start at 1.
 */


struct ConditionsConsequences_s { /* all these lists are type char* */
	GSList *needed;
	GSList *blocking;
	GSList *consequences;
};
typedef struct ConditionsConsequences_s ConditionsConsequences;

struct DeclarationGoodies_s {
	char *name;
	ConditionsConsequences *cc;
};
typedef struct DeclarationGoodies_s DeclarationGoodies;

struct ConversationGreeting_s {
	GSList *lines; // see note above
	char *proceed;
	ConditionsConsequences *cc;
};
typedef struct ConversationGreeting_s ConversationGreeting;

struct ConversationOption_s {
	char *name;
	char *say;  // a string that needs to be substituted-into.
	GSList *lines; // see note above
	char *proceed;
	ConditionsConsequences *cc;
};
typedef struct ConversationOption_s ConversationOption;

struct ConversationNode_s {
	char *name;
	GSList *options; /* the data type is ConversationOption*, with that a pointer, not array */
	ConditionsConsequences *cc;
};
typedef struct ConversationNode_s ConversationNode;

struct ConversationText_s {
	char *name;
	GSList* lines; // see note above
	ConditionsConsequences *cc;
};
typedef struct ConversationText_s ConversationText;

struct ConversationModel_s {
	GSList *initializers;
	GSList *greetings;
	GSList *nodes;
	GSList *texts;
	const char *name;
	STRING_TABLE table;
};
typedef struct ConversationModel_s ConversationModel;


struct StringTableBuilder_s {
	GSList *list; // in 'mommy', of type STRING_TABLE_ENTRY_DESC. In 'baby', of type char
	int length;
	STRING_TABLE parent; // not used by 'baby' string table builders
};
typedef struct StringTableBuilder_s StringTableBuilder;

/*
 perhaps ConversationModel should contain the pointer to the whole file and its length,
 so that when the time comes to free it, the disposal is very easy. Then we don't need to
 reallocate anything, just insert nulls and set the pointers appropriately. There will be
 a little bloat for the comments and the keywords, but it could be better overall.
 */

# define ERR1 "In file \n"
# define ERR2 "Found a line where we were expecting a keyword 'greeting', 'node', or 'text':"
# define ERR3 "Check for a dialog line beginning with '.' - if so, put a {} before it. It'll be invisible."

# define ERR4 "A body of text (not necessarily a text item) was not terminated with a '.'"


/* could also maintain a line number for pinpointing errors,
 in which case we'll need to include a pointer to that in uio_Stream *hook. */

static DeclarationGoodies initCode (char *line);
static GSList *extractSubstitutions(StringTableBuilder *stb, uio_Stream *hook, char **lastLine);
static ConversationGreeting *parseGreeting (char *line, uio_Stream *hook, StringTableBuilder *stb);
static ConversationNode *parseNode (char *line);
static ConversationOption *parseOption (char *line, uio_Stream *hook, StringTableBuilder *stb);
static ConversationText *parseTextItem (char *line, uio_Stream *hook, StringTableBuilder *stb);
static void validateNode (ConversationNode *node);
static int add_string (StringTableBuilder *stb, char *line);
static char *get_line (char *buf, int len, uio_Stream *hook);
static int checkForCase (const char* str);
static int plusesAndMinuses (const char *expression);
static char *nextPlusOrBinaryMinus (const char *string);
static void applyConsequences (ConditionsConsequences *cc, ConversationModel *cm);
static void setVal (char *name, int newval, ConversationModel *cm);
// static guint hash_djb2i(gconstpointer *p);
static GSList *getOptionList(ConversationNode* cn, ConversationModel *cm);
static ConversationNode *getNode (char *name, ConversationModel *cm);
static ConversationText *getTextItem (char *name, ConversationModel *cm);
static gboolean checkConditions (ConditionsConsequences* cc, ConversationModel* cm);
static int arithmetic(char *expression, ConversationModel *cm);
static int innerArithmetic (char *expression, ConversationModel *cm);
static int evaluatePrimitive (char* expression, ConversationModel *cm);
static int import_string (StringTableBuilder *baby, StringTableBuilder *mommy);
static void displaySubstituted (GSList *things);
static int myRandom();


// GLOBAL INTERNAL STATE
static GSList *currentOptions;
static ConversationModel *currentModel;
static GHashTable *transientState;
static char *source;
static int percent;
// END GLOBALS


/*
 
 overall status:
 ALL: untested, probably won't compile. 'okay' means just that I don't KNOW
 of any errors or incompleteness.
 
 -- parsing:
 √ added lines to string tables and store indices instead of storing in
 objects as strings
 
 -- conversation logic:
 once the above is handled, call the appropriate usual functions on it. Will
 be easy, I hope?
 add in state gets and sets
 
 -- substitution and arithmetic:
 need to incorporate callbacks to speech generation (if nonnull)
 
 
 Every other aspect of each category is fine.
 */
 
//34567890123456789012345678901234567890123456789012345678901234567890123456

// ~~~~~~~~~~ PARSING ~~~~~~~~
/**
 * Handles parsing a whole conversation file.
 * 
 * status: OKAY?
 */
static ConversationModel
*parseConversationModel (const char* who )
{
	char *path = NULL; 
	uio_Stream *hook;
	char line[1024];
	StringTableBuilder stb;
	GSList *readout, *head;
	int i;
	ConversationModel *out;
	ConversationNode *currentNode;
	DeclarationGoodies dg;
	
	memset (&stb, 0, sizeof (stb));
	
  /* set up input system here - */
	
	path = g_strdup_printf ("comm/%s/%s.txt", who, who);
	if (source)
		g_free(source);
	source = path;
	hook = res_OpenResFile (contentDir, path, "rb");
	if (hook == NULL)
	{
//		log_add (log_warning, "Warning: Can't open '%s'", path);
		g_free (path);
		source = NULL;
		return NULL; // haven't opened, so don't need to goto err.
	}
	
	if (LengthResFile (hook) == 0)
	{
//		log_add (log_warning, "Warning: Trying to load empty file '%s'.", path);
		goto err;
	}
	
	currentNode = NULL;
	out = g_new(ConversationModel, 1);
	out->name = who;
	
	while (GET_LINE)
	{
		
		switch (line[0])
		{ /* at this level of parsing, we only expect to
												encounter one of 3 keywords */
				/* also, you may consider using a switch AND an 'if' weird practice,
				 but having the break helps with error control. I miss 'throw'. */
			case 't':
				if (strncmp(line, "text", 4) != 0)
					break; /* switch, not while. Basically, go to the error handler. */
				out->texts = g_slist_append (out->texts,
							parseTextItem ( line,  hook, &stb ));
				continue; /* the while, not the switch. */
			case 'i':
				if (strncmp(line, "init", 4) != 0)
					break;
				dg = initCode(line);
				out->initializers = g_slist_append(out->initializers, dg.cc);
			case 'g':
				if (strncmp(line, "greeting", 8) != 0)
					break;
				out->greetings = g_slist_append (out->greetings,
							parseGreeting( line, hook, &stb ));
				continue;
			case 'n':
				if (strncmp(line, "node", 4) != 0)
					break;
				if (currentNode != NULL)
					validateNode (currentNode);
				currentNode = parseNode ( line);
				out->nodes = g_slist_append (out->nodes, currentNode);
        continue;
			case 'o':
				if (strncmp (line, "option", 6) != 0)
					break;
				if (currentNode == NULL){
/*					log_add (g_strdup_sprintf (
								"%s\nOption declared before first node declaration!",
								source));
 */
				} else
					currentNode->options = g_slist_append (currentNode->options,
								parseOption ( line, hook, &stb));
				continue;
		}
		// Getting here means an error!
		printf ("%s%s\n%s\n%s\n%s\n", ERR1, source, ERR2, line, ERR3);
	} /* end while over file contents */

	out->table = g_new (STRING_TABLE_DESC, 1);
	out->table->strings = g_new(STRING_TABLE_ENTRY_DESC, stb.length+1);
	out->table->size = stb.length;
	
	readout = head = g_slist_reverse(stb.list);
	for (i = 1; i <= stb.length; i++, readout = g_slist_next(readout))
	{
		out->table->strings[i] = *((STRING_TABLE_ENTRY_DESC*)(readout->data));
		g_free(readout->data);
	}
	g_slist_free(stb.list);
	g_slist_free(head);
	
  err:
	res_CloseResFile (hook);
	return out;
} /* end parseConversation(...) */



/**
 * Handles parsing a greeting, once you know that the line begins with
 * 'greeting'.
 * 
 * param stb: the string table builder. A GSList and the indices so far
 * 
 * status: OK
 */
static ConversationGreeting
*parseGreeting (char *line, uio_Stream *hook, StringTableBuilder *stb)
{
	DeclarationGoodies dg;
	ConversationGreeting *out = g_new(ConversationGreeting, 1);

	
	dg = initCode(line);
	out->cc = dg.cc;
/*	if (dg->name != NULL)
		log_add (g_strdup_printf ("%s%s\nGreeting had a name. They don't get names.",
					ERR1, source));
*/

	out->lines = extractSubstitutions (stb, hook, &line);
	
	
/*	if (line == NULL)
		log_add (g_strdup_printf ("%s%s\n%s", ERR1, source, ERR4));
 */
  // not exceptional case - we're looking at a '.' line
	out->proceed = g_strdup (line+1);
	g_free(line);

	return out;
} /* end parseGreeting(...) */

/**
 * gives the pointer-to-GSList encoding the alien dialog in the manner described in conversation.h
 *
 * @param lastLine the address of a pointer-to-char. This will set this address to point to the last line.
 *
 * Status: OK?
 */
static GSList
*extractSubstitutions (StringTableBuilder *stb, uio_Stream *hook , char **lastLine)
{
	GSList *build = NULL; // list of alternating string table indices and (char*) strings to codes.
	GSList *flip; // for reversing the order of build
	StringTableBuilder currentText;
	char  *line, *bra, *ket; // ket normally points to the character AFTER a ']'
	int brackets;
	int index;
	
	
	while (GET_LINE)
	{
		if ( line[0] == '.') { // all done!
			index = import_string(&currentText, stb);
			build = g_slist_prepend(build, GINT_TO_POINTER(index));
			break;
		}
		
		if ((bra = strchr(ket, '[')))
		{
			
			// now organize that bracket entry!
			ket = line;
			do
			{
				// TERMINATE PREVIOUS STRING, IF ANY
				*bra = '\0';
				if ((ket == bra) && (currentText.length == 0)) {
					// the second condition is in case '[' starts a new line following text
					build = g_slist_prepend(build, 0);
				}
				else
				{
					// now that we know where the end of the 
					if (ket != bra)
						add_string(&currentText, ket); // top it off
					index = import_string(&currentText, stb); // this clears out currentText
					build = g_slist_prepend(build, GINT_TO_POINTER(index));
				}
				// DONE TERMINATING PREVIOUS STRING
				
				brackets = 1;
				for (ket = bra+1; *ket; ket++)
				{
					if (*ket == '[') 
						brackets++;
					else if (*ket == ']')
					{
						brackets--;
						if (brackets == 0) {
							*ket++ = '\0'; // set it null and restore the 'after' property.
							
							build = g_slist_prepend(build, g_strdup(bra));
							
							break;
						} // end if we're all out of brackets
					} // end if it was a closing bracket
				} // end for each character before the brackets are closed
			} 
			while ((bra = strchr(ket, '['))); // end while we find brackets on this line
			
		}
		else // there were no brackets. Simple case!
		{
			add_string(&currentText, line);
		}
	} // end for all lines
	
	if (lastLine)
		*lastLine = line;
	else
		g_free(line);

	
	flip = g_slist_reverse(build);
	g_slist_free(build);
	return flip;
}

/**
 * Handles parsing a node, once you know that the line begins with 'node'.
 * Does not attempt to load up the options within the node.
 * 
 * status: OK
 */
static ConversationNode*
parseNode(char* line)
{
  DeclarationGoodies dg;
	ConversationNode *out;
	
	out = g_new(ConversationNode, 1);
	
	dg = initCode(line);
	out->cc = dg.cc;
	out->name = dg.name;
	
/*	if (name == NULL)
		log_add(g_strdup_printf("%s%s\nNode provided with no valid name!\n%s\n\
					Names go right after the declaration, \n\
					and must start with an alphanumeric symbol.",
					ERR1, source, line));
 */
  return out;
} /* end parseNode(...) */

/**
 * Checks that the passed node is properly formed.
 *
 * status: Partially implemented; moderately useful as it stands.
 * Nothing blocking.
 */
static void
validateNode (ConversationNode* node)
{

 if (g_slist_length(node->options) == 0)
	{
		printf ("%s%s\nNode provided with no options!",
					ERR1, source);
	}
	
	/* TODO: check that each option name occurs at least twice */
	/* TODO: check that no named option occurs after an option of the
	   same name with no conditions */
	/*
	 TODO: check that each option has no transparently invalid evaluations:
	 "[", "]" - mismatched brackets
	 */
}

/**
 * Handles parsing an option, once you know that the line begins with
 * 'option'.
 * 
 * status: OK
 */
static ConversationOption*
parseOption (char* line, uio_Stream *hook, StringTableBuilder *stb)
{
	DeclarationGoodies dg; // not DeclarationGoodies*
	ConversationOption *out = g_new (ConversationOption, 1);
	StringTableBuilder lines;
	
	lines.list = NULL;
	lines.length = 0;
	
	dg = initCode (line);
	out->cc = dg.cc;
	out->name = dg.name;
	
	out->lines = extractSubstitutions(stb, hook, &line);
	
	out->proceed = g_strdup (line+1);
	g_free(line);
	
	return out;
} /* end parseOption(...) */

/**
 * parses a 'text' item, once you know that the line begins with 'text'.
 *
 * status: OK
 */
static ConversationText*
parseTextItem (char* line, uio_Stream *hook, StringTableBuilder *stb)
{
	ConversationText *out;
	DeclarationGoodies dg;
	StringTableBuilder lines;
	
	lines.list = NULL;
	lines.length = 0;
	
	dg = initCode(line);
	out->cc = dg.cc;
	out->name = dg.name;
	
	out->lines = extractSubstitutions(stb, hook, NULL);
	/*
	if (line == NULL) log_add (g_strdup_printf ("%s\n%s\n%s", ERR1, source, ERR4));
	*/
	return out;
}

/**
 * parses the opening line of an item (greeting, node, option, text),
 * sorting the conditions and consequences, identifying the name (if any).
 * Modifies the passed string. 
 *
 * status: OK
 */
static DeclarationGoodies /* yes, return by value */
initCode (char* line )
{
	char *token;
	DeclarationGoodies out;
	ConditionsConsequences *cc;
	
	memset (&out, 0, sizeof (out));
	cc = g_new0 (ConditionsConsequences, 1);
	out.cc = cc;
	token = strpbrk (line, " \t");
	if (!token) return out;
	
	line = g_strdup (line);
	token = strtok (line, " \t"); // looking at the keyword. Move past that…
	token = strtok (NULL, " \t");
	
	if (isalnum (token[0]))
	{
		out.name = g_strdup (token);
		token = strtok (NULL, " \t");
	}
	
	while (token)
	{
		if ('=' == token[0])
		{
			cc->needed = g_slist_append (cc->needed, g_strdup (token+1));
		}
		else if ('<' == token[0])
		{
			cc->needed = g_slist_append (cc->needed, g_strdup (token+1));
			token[0] = '-';
			cc->consequences =
						g_slist_append (cc->consequences, g_strdup (token));
		}
		else if ('!' == token[0])
		{
			cc->blocking = g_slist_append (cc->blocking, g_strdup (token+1));
		}
		else if ('>' == token[0])
		{
			cc->blocking = g_slist_append (cc->blocking, g_strdup (token+1));
			token[0] = '+';
			cc->consequences =
						g_slist_append (cc->consequences, g_strdup (token));
		}
		else
		{ // cases +, -, ~
			cc->consequences =
						g_slist_append (cc->consequences, g_strdup (token));
		}
		
		token = strtok(NULL, " \t");
	} // end while more tokens
	return out;
}

/**
 * returns a version of the passed string without comments
 * marked with "//" or initial and ending spaces.
 * Copies the passed string.
 * If line is entirely comment or space, returns NULL.
 * 
 * status: OK
 */
static char*
trim(const char* src)
{
	int i, len;
	char *buf;

	
	buf = strstr (src, "//");
	if (buf != NULL)
	{
		buf[0] = '\0'; // terminate string	
		len = buf-src;
	}
	
	for (i = len-1; i >= 0 && isspace(src[i]); i--)
	{;	} /* end walkback */
	
	if (i == -1)
		return NULL; // empty!
	
	len = i+1;
	
	for (i = 0; i < len && isspace (src[i]); i++)
	{;	}
	len -= i;
	buf = g_new (char, len );
	strncpy (buf, src+i, len );

	return buf;
} /* end trim */

/**
 * Gets the next line from the stream given, trims it, skips any blank
 * lines
 * 
 * status: OK
 */
static char
*get_line (char* buf, int len, uio_Stream *hook)
{
	char *out, *tmp;
	do {
	  tmp = uio_fgets (buf, len, hook);
		if (tmp == NULL) return NULL;
	  out = trim (tmp);
	  free (tmp);
	}	while (out == NULL);
	
	return out;
}

/**
 * the improved version of the djb2 hash function of Dan Bernstein
 * (original used + instead of ^ )
 */
/*
static unsigned guint
hash_djb2i (gconstpointer *p)
{
	unsigned long hash = 5381;
	gint c;
	char *str = (char*) p;
 
	while ((c = *str++) != '\0')
		hash = ((hash << 5) + hash) ^ c;

	return (guint)hash;
}
*/

/**
 * wraps strncmp(a, b, 100) so it returns TRUE or FALSE instead of either
 * "0 or something else".
 */
/*
 static gboolean
strings_equal(const char *a, const char *b)
{
  return strncmp(a, b, 100) == 0 ? TRUE:FALSE;
}
*/
						
/**
 * Adds the line for gathering-up.
 * NOT intended to feed the master string table,
 * JUST for adding strings to the temp list.
 *
 * @param line This will be copied with g_strdup and stored.
 * @return the number of characters in this string table so far.
 * 
 * status: OKAY
 */
static int
add_string (StringTableBuilder *stb, char *line)
{
	stb->list = g_slist_prepend(stb->list, g_strdup(line));
	return stb->length += strlen(line)+1;
}

/**
 * Handles importing an individual STRING_TABLE_ENTRY_DESC object to
 * the parent string table, via StringTableBuilder objects.
 * This disposes of the baby (freeing the GSList elements and
 * setting all values to 0), but leaves the mommy in place.
 *
 * status: OKAY
 */
static int import_string (StringTableBuilder *baby, StringTableBuilder *mommy) {
	GSList *walk;
	char *build;
	STRING_TABLE_ENTRY_DESC *nova;
	int linelength;
	
	
	nova = g_new (STRING_TABLE_ENTRY_DESC, 1);
	nova->data = g_new (char, baby->length);
	nova->length = baby->length;
	nova->parent = mommy->parent;
	nova->index = ++(mommy->length);
	
	build = nova->data + nova->length; // start at the end since we were prepending
	
	for (walk = baby->list; walk; walk = g_slist_next (walk)) {
		linelength = strlen (walk->data);
		build -= linelength+1;
		strncpy (build, walk->data, linelength);
		g_free (walk->data);
		build[linelength] = '\0';
	}
	g_slist_free (baby->list);
	mommy->list = g_slist_prepend (mommy->list, nova);
	
	baby->list = NULL;
	baby->length = 0;
	
	return nova->index;
}

// ~~~~~~~~ CONVERSATION LOGIC ~~~~~~~~~

/**
 * If a race will be using this comm system, put this function in a
 * race's init_foo_comm function,
 * and pass the race's name as the argument.
 */
void prep_conversation_module (char* who, LOCDATA *fill ) {
	GSList *inits;
	
	if (transientState) {
		g_hash_table_remove_all (transientState);
	} else {
		transientState = g_hash_table_new_full ( &g_str_hash, &g_str_equal, &g_free, NULL);
	}
	
	currentModel = parseConversationModel (who);
	fill->init_encounter_func = cm_intro;
   fill->conversationPhrases = currentModel->table->strings;
   // The other members of currentModel->table are superfluous, I guess?
   // The size value is mainly used to set indices, but the indices are already set.
   // I still feel nervous about this.
   
	
	percent = myRandom();
	
	for (inits = currentModel->initializers; inits; inits = g_slist_next(inits)) {
		if (TRUE == checkConditions(inits->data, currentModel)) {
			applyConsequences(inits->data, currentModel);
			break;
		}
	}
		
} // end init_conversation

#define MAX_FAIR_RAND ((RAND_MAX / 100)*100)

static int myRandom() {
	int out = rand();
	while (out > MAX_FAIR_RAND) out = rand();
	return out % 100;
}

static gboolean
checkConditions(ConditionsConsequences* cc, ConversationModel* cm)
{
  GSList *conds;
  int bit;
	GSList *tmp;
	tmp = g_slist_append (tmp, "");
	for (conds = cc->needed; conds; conds = g_slist_next (conds))
	{
		bit = arithmetic ((char*)(conds->data), cm);
		if (!bit)
			 return FALSE;
	}
	for (conds = cc->blocking; conds; conds = g_slist_next (conds))
	{
		bit = arithmetic ((char*)(conds->data), cm);
		if (bit)
			return FALSE;
	}
	return TRUE;
}

static int
getGreeting(ConversationModel* cm)
{
	GSList *greetings = cm->greetings;
	ConversationGreeting *cg;
	int index = 0;
	do
	{
		cg = (ConversationGreeting*)greetings->data;
		if (checkConditions (cg->cc, cm) == TRUE)
			return index;
		index++;
	}
	while ((greetings = g_slist_next (greetings)) != NULL);
	return 0;
}

/**
 * The use as init_encounter_func
 */
void
cm_intro ()
{
	ConversationManager ( -1 - getGreeting (currentModel));
}

/**
 * status: PARTIALLY IMPLEMENTED
 *
 * Missing:
 * display directives
 * text substitutions
 */
void
ConversationManager (RESPONSE_REF R)
{	
	GSList *walk;
	char *nextNode;
	RESPONSE_REF i = 1;
    ConversationNode *currentNode;
	
	if (R > 0)
	{ 
		ConversationOption *taken = g_slist_nth (currentOptions, R-1)->data;
		applyConsequences (taken->cc, currentModel);
		nextNode = taken->proceed;
		walk = taken->lines;
		g_slist_free (currentOptions); // not free_full, since these are held elsewhere
	}
	else
	{
		ConversationGreeting *cg = (g_slist_nth (currentModel->greetings, -1 - R ))->data;
		applyConsequences (cg->cc, currentModel);
		nextNode = cg->proceed;
		walk = cg->lines;
	}
	
  displaySubstituted(walk);

	if (0 == strncmp(nextNode, "done", 5))
	{
		SET_GAME_STATE (BATTLE_SEGUE, 0);
		return;
	}
	if (0 == strncmp(nextNode, "fight", 6))
	{
		SET_GAME_STATE (BATTLE_SEGUE, 1);
		return;
	}
		
	currentNode = getNode (nextNode, currentModel);
	
	currentOptions = getOptionList (currentNode, currentModel);
	
	for (walk = currentOptions; walk; walk = walk->next)
	{
		ConversationOption* opt = walk->data;
		DoResponsePhrase (i++, &ConversationManager, (UNICODE*)(opt->say));
	}
	
} // end ConversationManager

static void
displaySubstituted(GSList *walk) {
	ConversationText *ct;
	
	while (walk)
	{
	  NPCPhrase (GPOINTER_TO_INT(walk->data));
		walk = g_slist_next(walk);
		if (walk == NULL)
			return;
		ct = getTextItem(walk->data, currentModel);
		if (ct) 
			displaySubstituted(ct->lines); // recurse into text items
		else
  		NPCPhrase (-arithmetic(walk->data, currentModel));
		
		walk = g_slist_next(walk);
	}	
}

/**
 * Determines the set of conversation options.
 * 
 * status: OK
 */
static GSList*
getOptionList(ConversationNode* cn, ConversationModel *cm)
{
	GSList *walk, *acceptedNames;
	GSList *out = acceptedNames = NULL;
	ConversationOption *step;
	
	for (walk = cn->options; walk; walk = g_slist_next (walk))
	{
		step = walk->data;
    if (step->name != NULL)
		{
			if (g_slist_find_custom (acceptedNames, step->name, g_strcmp0))
				continue;
		}
		if (checkConditions (step->cc, cm) == FALSE)
			continue;
		out = g_slist_append (out, step);
		if (step->name != NULL)
			acceptedNames = g_slist_prepend (acceptedNames, step->name);
	}
	return out;
}

/**
 * Finds the appropriate node to go to next.
 *
 * status: OK
 */
static ConversationNode*
getNode (char* name, ConversationModel* cm)
{
	GSList *walk;
	if (name == NULL)
		return NULL;
	for (walk = cm->nodes; walk; walk = g_slist_next(walk))
	{
		ConversationNode *node = (ConversationNode*)walk->data;
		if (g_strcmp0(node->name, name) != 0)
			continue;
		if (checkConditions (node->cc, cm) == TRUE)
			return node;
	}
	return NULL;
}

/**
 * Finds the appropriate text to use.
 * Does not substitute any variables or text items to find the
 * text name, just looks it up.
 *
 * status: OK
 */
static ConversationText*
getTextItem (char* name, ConversationModel* cm)
{
	GSList *walk;
	if (name == NULL)
		return NULL;
	for (walk = cm->texts; walk; walk = g_slist_next(walk))
	{
		ConversationText *node = (ConversationText*)(walk->data);
		if (g_strcmp0(node->name, name) != 0)
			continue;
		if (checkConditions (node->cc, cm) == TRUE)
			return node;
	}
	return NULL;
}

/**
 * Applies the consequences of an item being taken.
 * 
 * status: OKAY
 */
static void
applyConsequences (ConditionsConsequences* cc, ConversationModel *cm)
{
	GSList *walk;
	char *con, *LHS, *RHS, *cmp, *assemble;
	int newval, statement;
	int offset;
	
	for (walk = cc->consequences; walk; walk = g_slist_next(walk))
	{
		con = walk->data;
		
		cmp = strpbrk(con, "=<>"); // take the FIRST comparator
		if (cmp)
		{ // explicit comparison or assignment - numeric flags
			offset = cmp-con;
			LHS = strncpy( g_new (char, offset+1), con, offset);
			LHS[offset] = '\0';
			RHS = cmp + 1;
		}
		else
		{ // case of implicit assignment - typically boolean flags
			LHS = con;
			cmp = "=";
			RHS = "1";
		}
		newval = 0;
		assemble = g_strdup_printf ("%s%s%s", con, cmp, RHS);
		statement = arithmetic (assemble, cm);

		switch (*con)
		{
			case '~':
				if (statement)	break; // leave newval as 0
					// else fall through and set the new value
			case '+':
				if (statement)
					return; // setting an already satisfied condition? Do nothing.
				// otherwise...
				newval = arithmetic (RHS, cm);

				switch (*cmp)
			  {
					case '<': newval--; break;
					case '>': newval++; // break;
//					case '=':
				}
//			case '-': // leave newval as 0
//			default:
		} // end switch on style of consequence
		setVal (LHS, newval, cm);
	} // end for each consequence
}	// end applyConsequences(...)


static void
setVal (char *name, int newval, ConversationModel *cm)
{
	if (*name == '%') {
		percent = myRandom();
		return;
	}
	switch (checkForCase (name))
	{
		case 2: // only lower-case
			g_hash_table_insert (transientState, name, GINT_TO_POINTER(newval));
			return;
		case 3: // mixed case
			SetGameStateByName (g_strdup_printf ("%s[%s]", cm->name, name), (BYTE)newval);
			return;
	}
	SetGameStateByName (name, (BYTE)newval);
} // end setVal(...)

// ~~~~~~~ SUBSTITUTION AND ARITHMETIC ~~~~~~~~~




/**
 * This handles arithmetic without grouping. If it doesn't find any
 * arithmetic or numbers, returns 0.
 * This alters the passed string.
 *
 * Recognized operators, in order of increasing precedence are:
 * = (equals) and < (less than) and > (greater than),
 * + (add) and - (subtract),
 * / (divide),
 * * (multiply),
 * √ (radical binary operator) and ^ (exponential)
 *
 * The separation of * from / enables expressions like (a*b)/(c*d)
 * to do without the parentheses with need for
 * neither double division nor the explicit groupings.
 *
 * The same does NOT apply to addition and subtraction, which are
 * performed strictly left to right:
 * if you want a-(b+c), you can just write a-b-c.
 * 
 * The √ operator a√b is what b^[1/a] would be if 1/a weren't going
 * to be int-trimmed.
 * 
 * Operations / and later resolve RIGHT to LEFT, so a√b^c is made
 * bigger, then smaller (good for avoiding int-trimming)
 *
 * status: OKAY
 */
static int arithmetic(char* expression, ConversationModel* cm) {
	char *divider, *out, *next;
	char operator, nextop;
	int left, right;
	
	if (*expression == '\0')
		return 0;
	
	divider = strpbrk (out, "=<>");
	if (divider)
	{
		operator = *divider;
		*divider = '\0';
		
		left = arithmetic (expression, cm);
		
		while (divider != NULL)
		{
			next = strpbrk (out, "=<>");
			if (next)
			{
				nextop = *next;
			  *next = '\0';
			}
			right = arithmetic (divider+1, cm);
			
			switch (operator)
			{
				case '=': left = (left == right)? 1: 0; break;
				case '<': left = (left <  right)? 1: 0; break;
				case '>': left = (left >  right)? 1: 0; break;
				default:  left = 0;
			}
			operator = nextop;
			divider = next;
		}
	}

	// lowest priority, so split on this and evaluate each side before going.
  divider = nextPlusOrBinaryMinus (expression);
	if (divider)
	{
		operator = *divider;
		*divider = '\0';
		
		left = innerArithmetic (expression, cm);
		
		while (divider != NULL)
		{ // identify and close off the next element of the sum
			next = nextPlusOrBinaryMinus (divider+1); 
			if (next)
			{
				nextop = *next;
			  *next = '\0';
			}
			
			right = innerArithmetic (divider+1, cm);
			
			left = (operator == '+') ? left+right : left-right;
			
			operator = nextop;
			divider = next;
		}
		return left;
	}
	
	return innerArithmetic (expression, cm);
	
} // end arithmetic(...)

/**
 * returns the pointer to the next + or -, NOT counting any unary - signs.
 * If there aren't any, returns NULL.
 *
 * status: OK
 */
static char *
nextPlusOrBinaryMinus (const char *expression)
{
	char* out;
	
	out = strpbrk(out, "-+");
	if (out == expression)
		out = strpbrk (out+1, "-+"); // manually make sure we don't back out of the string
	while (out && strchr ("+-*/√^", *(out - 1)) )
	{ // while the character before this - sign is another operator...
		out = strpbrk (out+1, "-+");
	}
	
	return out;
}

static int
innerArithmetic (char* expression, ConversationModel *cm)
{
	char *divider;
	char operator;
	int left, right;
	
	divider = strpbrk (expression, "/");
	if (divider != NULL)
	{
		*divider = '\0';
		
		left = innerArithmetic (expression, cm);
		right = innerArithmetic (divider + 1, cm);
		
		return left/right;
	}
	
	divider = strpbrk (expression, "*");
	if (divider != NULL)
	{
		*divider = '\0';
		left = innerArithmetic (expression, cm);
		right = innerArithmetic (divider + 1, cm);
		return left*right;
	}
	
	// highest priority. The left-side is a number or nothing.
	divider = strpbrk (expression, "^√"); 
	
	if (divider != NULL)
	{
		operator = *divider;
		*divider = '\0';
		left = innerArithmetic (expression, cm);
		right = innerArithmetic (divider + 1, cm);
		
		return (operator == '^')?
					(int)(pow (left, right)) :
					(int)(pow (right, 1.0 / left));
	}
	return evaluatePrimitive (expression, cm);
}

/**
 * Gets a number, either parsing it as int or by getting from
 * GetGameState or transientState, as appropriate
 * Alters the expression passed.
 *
 * status: OK but needs an implementation of GetGameState
 */
static int
evaluatePrimitive (char* expression, ConversationModel *cm)
{
	int out, jump;
	
	if (*expression == '%')
		return percent;
	
	// clear sequential - signs
	jump = plusesAndMinuses (expression);
	if (jump < 0)
	{
		expression -= jump;
		expression--;
		expression[0] = '-';
	}
	else
	{
		expression += jump;
	}
	
	if (strlen(expression) == 1 && expression[0] == '0')
		return 0; // check for an ACTUAL 0 instead of atoi bombing out.
	
	out = atoi (expression);
	if (out != 0)
		return out;
	switch (checkForCase (expression))
	{
		case 2: // only lower-case
			return (int)(g_hash_table_lookup (transientState, expression));
		case 3: // mixed case
			return (int)(GetGameStateByName (g_strdup_printf ("%s[%s]", cm->name, expression)));
	}
	return ((int)(GetGameStateByName (expression)));
}

/**
 * returns the leading - and + signs… 
 * negative if the number of - signs is odd.
 * 
 * status: OK
 */
static int
plusesAndMinuses (const char* expression)
{
  int out, odd;
	out = odd = 0;
	for ( ; expression; expression++, out++)
	{
		if (*expression == '-')
			odd++;
		else if (*expression != '+')
			break;
	}
	if (odd & 1) return -out;
	return out;
}

/**
 * returns a bitmask with the corresponding values:
 * 1 -> there is an upper-case letter
 * 2 -> there is a lower-case letter
 * 
 * status: OK
 */
static int
checkForCase (const char* str)
{
  int out;
	const char *walk;
	walk = str;
	for (walk = str; *walk != '\0'; walk++)
	{
		if (*walk >= 'A' && *walk <='Z')
		{
			out |= 1;
		}
		else if (*walk >= 'a' && *walk <= 'z')
		{
			out |= 2;
		}
		if (out == 3) return out;
	}
  return out;
}
