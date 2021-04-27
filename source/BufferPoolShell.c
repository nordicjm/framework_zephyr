/**
 * @file BufferPoolShell.c
 * @brief
 *
 * Copyright (c) 2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <zephyr.h>
#include <shell/shell.h>

#include "BufferPool.h"

/******************************************************************************/
/* Local Function Prototypes                                                  */
/******************************************************************************/
static int bp_stats(const struct shell *shell, size_t argc, char **argv);

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
SHELL_STATIC_SUBCMD_SET_CREATE(sub_bp,
			       SHELL_CMD(stats, NULL, "Print buffer pool stats",
					 bp_stats),
			       SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(bp, &sub_bp, "Buffer Pool", NULL);

/******************************************************************************/
/* Local Function Definitions                                                 */
/******************************************************************************/
static int bp_stats(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	const uint8_t POOL_INDEX = 0;

	struct bp_stats *stats = BufferPool_GetStats(POOL_INDEX);

	if (stats != NULL) {
		shell_print(shell, "Buffer Pool %u", POOL_INDEX);
		shell_print(shell, "stats initialized     %u",
			    stats->initialized);
		shell_print(shell, "space available       %d",
			    stats->space_available);
		shell_print(shell, "min space available   %d",
			    stats->min_space_available);
		shell_print(shell, "min alloced size      %d", stats->min_size);
		shell_print(shell, "max alloced size      %d", stats->max_size);
		shell_print(shell, "total allocs          %d", stats->allocs);
		shell_print(shell, "current allocations   %d",
			    stats->cur_allocs);
		shell_print(shell, "max allocations       %d",
			    stats->max_allocs);

#if CONFIG_BUFFER_POOL_WINDOW_SIZE > 0
		shell_print(shell, "List of recently allocated sizes:");
		size_t i;
		for (i = 0; i < CONFIG_BUFFER_POOL_WINDOW_SIZE - 1; i++) {
			shell_fprintf(shell, SHELL_NORMAL, "%u, ",
				      stats->window[i]);
		}
		shell_fprintf(shell, SHELL_NORMAL, "%u\n", stats->window[i]);
#endif
	} else {
		shell_error(shell, "Buffer pool not found");
	}
	return 0;
}
