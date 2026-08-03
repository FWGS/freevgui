#!/usr/bin/env python

from waflib.TaskGen import feature, after_method

def options(opt):
	pass

def configure(conf):
	if conf.env.DEST_OS != 'android':
		if conf.env.cxxshlib_PATTERN.startswith('lib'):
			conf.env.cxxshlib_PATTERN = conf.env.cxxshlib_PATTERN[3:]

# freevgui must match original vgui library name: vgui.so.
# however, Waf hardcodes -lvgui, failing vgui_support and client link to freevgui
@feature('fix_freevgui_link')
@after_method('process_use')
def fix_freevgui_link(self):
	if self.env.DEST_OS in ('win32', 'darwin', 'android'):
		return

	self.env.LIB = [':vgui.so' if lib == 'vgui' else lib for lib in self.env.LIB]

def build(bld):
	platform = 'win32' if bld.env.DEST_OS == 'win32' else 'posix'

	bld.shlib(
		source = bld.path.ant_glob([
			'*.cpp',
			'miniutl/utlmemory.cpp',
			'controls/*.cpp',
			'platform/common/*.cpp',
			'platform/%s/*.cpp' % platform,
		]),
		target = 'vgui yy_thunks',
		features = 'cxx',
		includes = ['.', 'miniutl'],
		export_includes = '.',
		rpath = '$ORIGIN',
		install_path = bld.env.LIBDIR,
		subsystem = bld.env.MSVC_SUBSYSTEM
	)
