//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : Debug call stack
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// HAVE_PRAGMA_ONCE
// __cplusplus
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added alternative show leaks function. (Sep 15th 2005 Martin G�hmann)
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __DEBUGCALLSTACK_H
#define __DEBUGCALLSTACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"
#include <stdio.h>

void DebugCallStack_Open (void);
void DebugCallStack_Close (void);
void DebugCallStack_Dump (LogClass log_class);
void DebugCallStack_DumpFrom (LogClass log_class, size_t base_pointer);
void DebugCallStack_DumpAddress (LogClass log_class, size_t address);
void DebugCallStack_Save  (size_t *call_stack, int number, size_t Ebp);

void DebugCallStack_Show  (LogClass log_class, size_t *call_stack, int number);
void DebugCallStack_ShowToFile  (LogClass log_class, size_t *call_stack, int number, FILE *file);
void DebugCallStack_ShowToAltFile  (LogClass log_class, size_t *call_stack, int number, FILE *file);

#ifdef __cplusplus
class cDebugCallStackSet
{
public:
	cDebugCallStackSet(int depth);
	~cDebugCallStackSet();

	void Add();
	void Dump(const char *filename);

private:
	int m_maxNumStacks;
	size_t m_caller;
	int m_depth;
	int m_blockSize;
	int m_numStacks;
	size_t *m_stacks;
	size_t *m_curStack;
};
#endif

#ifdef __cplusplus
}
#endif

#endif
