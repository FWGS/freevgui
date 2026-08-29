// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "platform/common/font.h"

using namespace vgui;

BaseFontPlat *vgui::FontPlat_CreateSystem( const char *, int, int, float, int, bool, bool, bool, bool )
{
	// no system font backend here yet
	return nullptr;
}
