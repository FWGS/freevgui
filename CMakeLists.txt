#
# Copyright (c) 2016 Alibek Omarov
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required(VERSION 3.9)
project(FREEVGUI)

if(WIN32)
	set(FREEVGUI_PLATFORM win32)
else()
	set(FREEVGUI_PLATFORM posix)
endif()

file(GLOB FREEVGUI_SOURCES
	*.cpp
	controls/*.cpp
	platform/common/*.cpp
	platform/${FREEVGUI_PLATFORM}/*.cpp)

if(USE_STATIC_FREEVGUI)
	add_library(vgui STATIC ${FREEVGUI_SOURCES})
else()
	add_library(vgui SHARED ${FREEVGUI_SOURCES})
endif()

target_include_directories(vgui PRIVATE .)

set_target_properties(vgui PROPERTIES
	POSITION_INDEPENDENT_CODE 1
	BUILD_RPATH "$ORIGIN"
	INSTALL_RPATH "$ORIGIN")

# freevgui must match original vgui library name: vgui.so (no lib prefix).
if(NOT WIN32 AND NOT USE_STATIC_FREEVGUI)
	set_target_properties(vgui PROPERTIES PREFIX "")
endif()
