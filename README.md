## FreeVGUI

Free and open source re-implementation of Half-Life 1 GUI library. Aims to be
compatible with original in public API and optionally in private API as well.
Intended to be used within Xash3D FWGS engine to provide cross-platform
features of VGUI1 but in theory can be used in GoldSrc mods to fix various bugs
in panels code.

Sources and headers were carefully restored from debug information and third
party research like Nagist's lib-src/vgui/ code (omitting any derivation from
HLSDK headers).

### Building

For now, only building as subproject of Xash3D FWGS is supported.

### Licensing

This code partially derived from original Nagist's work, licensed under
3-clause BSD.

Copyright (c) 2016-2020 Nagist.

This code also incorporates GameNetworkingSockets code (see MiniUTL library),
licensed under 3-clause BSD too.

Copyright (c) 2018, Valve Software.

As this code depends on Xash3D FWGS's libpublic library, which is licensed
under GNU GPL v3, FreeVGUI is also licensed under GNU GPL v3.

Copyright (c) 2007 Uncle Mike
Copyright (c) 2019-2024 Alibek Omarov.
