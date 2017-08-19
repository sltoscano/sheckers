// Copyright: Steven Toscano

#pragma once

#ifdef DEBUG

// autodetect CRT memory leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// init CRT memory leak detection before main is called
struct init {
	init() { _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); }
} _init;

#endif // DEBUG
