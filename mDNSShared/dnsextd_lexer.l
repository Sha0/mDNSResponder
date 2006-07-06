/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2006 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@

    Change History (most recent first):

$Log: dnsextd_lexer.l,v $
Revision 1.1  2006/07/06 00:09:05  cheshire
<rdar://problem/4472013> Add Private DNS server functionality to dnsextd


 */

%{
#include <string.h>
#include <stdio.h>
#include "dnsextd_parser.y.h"


extern YYSTYPE yylval;

#if defined(__APPLE__)
int yylineno = 1;
#endif

#define YY_NO_UNPUT

int  yylex(void);

static char*
StripQuotes
	(
	const char * string
	)
{
	char * dup;

	dup = strdup( string + 1);

	dup[ strlen( dup ) - 1 ] = '\0';

	return dup;
}


%}

%%

options								return OPTIONS;
listen-on							return LISTEN_ON;
nameserver							return NAMESERVER;
port								return PORT;
address								return ADDRESS;
llq									return LLQ;
public								return PUBLIC;
private								return PRIVATE;
key									return KEY;
allow-update						return ALLOWUPDATE;
allow-query							return ALLOWQUERY;
algorithm							return ALGORITHM;
secret								return SECRET;
zone                    			return ZONE;
type                    			return TYPE;
allow								return ALLOW;
\{                      			return OBRACE;
\}                      			return EBRACE;
;                       			return SEMICOLON;
IN									return IN;
\*									yylval.string = strdup(yytext);	return WILDCARD;
[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+		yylval.string = strdup(yytext);	return DOTTED_DECIMAL_ADDRESS;
[0123456789]+						yylval.number = atoi(yytext);	return NUMBER;
[a-zA-Z0-9]+(\.[a-zA-Z0-9]+)*		yylval.string = strdup(yytext);	return HOSTNAME;
[a-zA-Z0-9\.]+([a-zA-Z0-9\.]+)*		yylval.string = strdup(yytext);	return DOMAINNAME;
\"([^"\\\r\n]*(\\.[^"\\\r\n]*)*)\"	yylval.string = StripQuotes(yytext);	return QUOTEDSTRING;
[\/][\/].*							/* ignore C++ style comments */;
\n                      			yylineno++; /* ignore EOL */;
[ \t]+                  			/* ignore whitespace */;
%%
