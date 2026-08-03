// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#ifndef VGUI_INTERNAL_H
#define VGUI_INTERNAL_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define Q_min( a, b ) (((a) < (b)) ? (a) : (b))
#define Q_max( a, b ) (((a) > (b)) ? (a) : (b))
#define bound( min, num, max ) ((num) >= (min) ? ((num) < (max) ? (num) : (max)) : (min))

#endif // VGUI_INTERNAL_H
