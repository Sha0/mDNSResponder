/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2009 Apple Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

    Change History (most recent first):

$Log: safe_vproc.c,v $
Revision 1.1  2009/02/06 03:06:49  mcguire
<rdar://problem/5858533> Adopt vproc_transaction API in mDNSResponder

 */

#include <stdlib.h>
#include <assert.h>
#include <vproc.h>
#include "safe_vproc.h"
#include "mDNSDebug.h"

#ifdef VPROC_HAS_TRANSACTIONS
typedef struct __transaction_s
	{
	struct __transaction_s* next;
	vproc_transaction_t vt;
	} transaction_t;
	
static transaction_t* transactions = NULL;
#endif

void safe_vproc_transaction_begin(void)
	{
#ifdef VPROC_HAS_TRANSACTIONS
	if (vproc_transaction_begin)
		{
		transaction_t* t = malloc(sizeof(transaction_t));
		t->next = transactions;
		transactions = t;
		t->vt = vproc_transaction_begin(NULL);
		}
	else
		LogMsg("vproc_transaction support unavailable");
#else
	LogMsg("Compiled without vproc_transaction support");
#endif
	}
	
void safe_vproc_transaction_end(void)
	{
#ifdef VPROC_HAS_TRANSACTIONS
	if (vproc_transaction_end)
		{
		if (transactions)
			{
			transaction_t* t = transactions;
			transactions = t->next;
			vproc_transaction_end(NULL, t->vt);
			free(t);
			}
		else
			LogMsg("No transactions in list");
		}
	else
		assert(transactions == NULL);
#endif
	}
