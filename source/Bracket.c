/**
 * @file Bracket.c
 * @brief
 *
 * Copyright (c) 2020 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <logging/log.h>
#define LOG_LEVEL LOG_LEVEL_ERR
LOG_MODULE_REGISTER(Bracket);

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <stdlib.h>

#include "Bracket.h"

/******************************************************************************/
/* Local Constant, Macro and Type Definitions                                 */
/******************************************************************************/
/* iOS escapes '/' with '\' '/'
 * If this is intercepted here, then the buffer sizes don't need to be doubled
 * to handle a base-64 encoded image. */
#ifndef BRACKET_INTERCEPTS_ESCAPED_FORWARD_SLASH
#define BRACKET_INTERCEPTS_ESCAPED_FORWARD_SLASH 1
#endif

struct BracketObject {
	size_t size;
	size_t length;
	s32_t delta;
	bool entered;
	char buffer[];
};

BracketObj_t *Bracket_Initialize(size_t Size)
{
	size_t s = sizeof(BracketObj_t) + Size;
	BracketObj_t *ptr = (BracketObj_t *)k_calloc(s, sizeof(char));
	Bracket_Reset(ptr);
	ptr->size = Size;
	return ptr;
}

void Bracket_Reset(BracketObj_t *p)
{
	p->entered = false;
	p->length = 0;
	p->delta = 0;
}

bool Bracket_Match(BracketObj_t *p)
{
	return ((p->delta == 0) && p->entered);
}

bool Bracket_Entered(BracketObj_t *p)
{
	return p->entered;
}

size_t Bracket_Length(BracketObj_t *p)
{
	return p->length;
}

size_t Bracket_Size(BracketObj_t *p)
{
	return p->size;
}

int Bracket_Compute(BracketObj_t *p, char Character)
{
	if (Character == '{') {
		p->delta++;
		p->entered = true;
	}

	/* The LAIRD dongle in vSP mode sends status at the start and end of a
	 * connection (~300 bytes). If present, ignore that data. */
	if (p->entered) {
		if (Character == '}') {
			p->delta--;
		}

		p->buffer[p->length++] = Character;

#if BRACKET_INTERCEPTS_ESCAPED_FORWARD_SLASH
		if (p->length >= 2) {
			if (p->buffer[p->length - 2] == '\\' &&
			    p->buffer[p->length - 1] == '/') {
				p->buffer[p->length - 2] = '/';
				p->length -= 1;
			}
		}
#endif
	}

	if (Bracket_Match(p)) {
		return BRACKET_MATCH;
	} else if (p->length == p->size) {
		LOG_ERR("Not enough memory for bracket handler");
		Bracket_Reset(p);
		return -ENOMEM;
	} else if (p->entered) {
		return abs(p->delta);
	} else {
		return BRACKET_MATCHING_NOT_STARTED;
	}
}

size_t Bracket_Copy(BracketObj_t *p, void *pDest)
{
	if (pDest != NULL) {
		memcpy(pDest, p->buffer, p->length);
	}
	return p->length;
}
