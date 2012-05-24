/*
 *  conversation.h
 * 
 *  Created by Luke Somers on 2012-01-20, for Project 6014.
 */
#include "globdata.h" // for LOCDATA
#include "commglue.h" // for RESPONSE_REF


void ConversationManager (RESPONSE_REF R);

void cm_intro ();

void prep_conversation_module (char* who, LOCDATA *fill );
