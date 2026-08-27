// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "app.h"

using namespace vgui;

void App::internalSetMouseArena(int, int, int, int, bool)
{
	// stub
}

long int App::getTimeMillis()
{
	return ( long int )GetTickCount();
}

void App::platTick()
{

}
