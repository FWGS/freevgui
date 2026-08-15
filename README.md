## FreeVGUI

Free and open source re-implementation of Half-Life 1 GUI library. Aims to be
compatible with original in public API and optionally in private API as well.
Intended to be used within Xash3D FWGS engine to provide cross-platform
features of VGUI1 but in theory can be used in GoldSrc mods to fix various bugs
in panels code.

### How it was written

FreeVGUI shares no code with the original library, nor with any other
re-implementation of it. It was written in two separate steps.

First, a specification was created: one file per public class, giving the class
hierarchy, the memory layout, the vtable slot order and a textual description
of what every method observably does. It was put together from the debug
information left in the shipped binaries: import library's export list, `pahole` layout dumps,
and also from third party research such as Nagist's https://github.com/nagist/vgui_dll.
It deliberately carries no implementation source.

Second, the library was written from that specification alone, by reproducing
the described behaviour - never by transcribing anything. Where the spec was
silent or ambiguous, the answer was found by observing the original binary, not
by copying from another codebase. Nothing is derived from HLSDK headers or from
the Source SDK.

### Building

For now, only building as subproject of Xash3D FWGS is supported.

### Licensing

FreeVGUI is licensed under 3-clause BSD. Every source file carries an SPDX
identifier, see `LICENSE` for the full text.

Copyright (c) 2019-2026 Alibek Omarov.

Nagist's `lib-src/vgui/` work, licensed under 3-clause BSD, was one of the
sources the specification was compiled from. No code from it was taken, but
FreeVGUI would not exist without that research, so the copyright notice is
carried here and in `LICENSE`.

Copyright (c) 2016-2020 Nagist.
