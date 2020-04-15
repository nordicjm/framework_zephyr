/**
 * @file Bracket.h
 * @brief Handles bracket matching on a JSON serial stream.
 * A packet framer and rudimentary parser.
 *
 * Copyright (c) 2020 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BRACKET_H__
#define __BRACKET_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <zephyr/types.h>
#include <stddef.h>

/******************************************************************************/
/* Global Constants, Macros and Type Definitions                              */
/******************************************************************************/
#define BRACKET_MATCH 0
#define BRACKET_MATCHING_NOT_STARTED 1

typedef struct BracketObject BracketObj_t;

/******************************************************************************/
/* Global Function Prototypes                                                 */
/******************************************************************************/
/**
 * @brief Allocates memory for and initializes a bracket handler.
 *
 * @param Size of the buffer
 *
 * @retval pointer to bracket object
 */
BracketObj_t *Bracket_Initialize(size_t Size);

/**
 * @brief Resets bracket (delta and index).
 *
 * @param p is a pointer to bracket object
 */
void Bracket_Reset(BracketObj_t *p);

/**
 * @brief Accessor Function
 *
 * @param p is a pointer to bracket object
 *
 * @retval true if bracket delta is 0, false otherwise
 */
bool Bracket_Match(BracketObj_t *p);

/**
 * @brief Accessor Function
 *
 * @param p is a pointer to bracket object
 *
 * @retval true if the first '{' has been found after bracket was reset.
 */
bool Bracket_Entered(BracketObj_t *p);

/**
 * @brief Accessor Function
 *
 * @param p is a pointer to bracket object
 *
 * @retval the number of used bytes in the bracket.
 */
size_t Bracket_Length(BracketObj_t *p);

/**
 * @brief Accessor Function
 *
 * @param p is a pointer to bracket object
 *
 * @retval the total number of bytes in the bracket.
 */
size_t Bracket_Size(BracketObj_t *p);

/**
 * @brief Counts the number of { } matching pairs in a JSON stream.
 * If the first bracket has been found then the character is added to the
 * buffer.
 *
 * @param p is a pointer to bracket object
 * @param Character is the next character to process.
 *
 * @retval 0 for bracket match (at least one '{' was found).
 * @retval > 0 the delta between { and }. This will be 1 if first '{'
 * hasn't been found.
 * @retval -ENOMEM if there wasn't enough memory to process incoming stream
 */
int Bracket_Compute(BracketObj_t *p, char Character);

/**
 * @brief Copies the current buffer into the destination (pDest) if non-NULL.
 *
 * @param pDest if NULL, then nothing is copied but length is still valid.
 *
 * @retval The number of bytes in the buffer (length).
 */
size_t Bracket_Copy(BracketObj_t *p, void *pDest);

#ifdef __cplusplus
}
#endif

#endif /* __BRACKET_H__ */
