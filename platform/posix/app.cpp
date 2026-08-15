// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include <time.h>
#include "app.h"

using namespace vgui;

void App::internalSetMouseArena(int, int, int, int, bool)
{
	// stub
}

long int App::getTimeMillis()
{
	struct timespec ts;

	clock_gettime( CLOCK_MONOTONIC, &ts );

	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void App::platTick()
{

}
