//=============================================================================
//!
//! @file "FrameworkStubs.c"
//!
//! @brief Weak implentations for functions required by framework.
//!
//! @copyright Copyright 2020 Laird
//!            All Rights Reserved.
//=============================================================================

//=============================================================================
// Includes
//=============================================================================
#include "Framework.h"

//=============================================================================
// Local Constant, Macro and Type Definitions
//=============================================================================

//=============================================================================
// Global Data Definitions
//=============================================================================

//=============================================================================
// Local Data Definitions
//=============================================================================

//=============================================================================
// Local Function Prototypes
//=============================================================================

//=============================================================================
// Global Function Definitions
//=============================================================================
__weak void Framework_AssertionHandler(char *file, int line)
{
	UNUSED_PARAMETER(file);
	UNUSED_PARAMETER(line);
}

__weak bool Framework_InterruptContext(void)
{
	return k_is_in_isr();
}

__weak void Framework_SystemReset(void)
{
	return;
}

__weak DispatchResult_t Framework_UnknownMsgHandler(FwkMsgReceiver_t *pMsgRxer,
						    FwkMsg_t *pMsg)
{
	UNUSED_PARAMETER(pMsgRxer);
	UNUSED_PARAMETER(pMsg);
	FRAMEWORK_ASSERT(FORCED);
	return DISPATCH_ERROR;
}

//=============================================================================
// Local Function Definitions
//=============================================================================

// end
