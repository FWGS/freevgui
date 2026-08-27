#!/usr/bin/env python

from waflib.TaskGen import feature, after_method

def options(opt):
	pass

def configure(conf):
	conf.load('fwgslib cxx11')
	conf.check_std('cxx11', mandatory = True)

	if conf.env.DEST_OS != 'android':
		if conf.env.cxxshlib_PATTERN.startswith('lib'):
			conf.env.cxxshlib_PATTERN = conf.env.cxxshlib_PATTERN[3:]

	if conf.env.USE_STATIC_FREEVGUI:
		conf.env.FREEVGUI_XASH_SUPPORT = True

# freevgui must match original vgui library name: vgui.so.
# however, Waf hardcodes -lvgui, failing vgui_support and client link to freevgui
@feature('fix_freevgui_link')
@after_method('process_use')
def fix_freevgui_link(self):
	if self.env.USE_STATIC_FREEVGUI or self.env.DEST_OS in ('win32', 'darwin', 'android'):
		return

	self.env.LIB = [':vgui.so' if lib == 'vgui' else lib for lib in self.env.LIB]

def build(bld):
	platform = 'win32' if bld.env.DEST_OS == 'win32' else 'posix'
	source = bld.path.ant_glob([
		'*.cpp',
		'controls/*.cpp',
		'platform/common/*.cpp',
		'platform/%s/*.cpp' % platform,
	])
	includes = ['.']
	defines = []

	if bld.env.FREEVGUI_XASH_SUPPORT:
		source += bld.path.ant_glob('platform/xash3d-fwgs/*.cpp')
		includes += ['../engine', '../../engine'] # this is horribly stupid, replace with exported includes later
		# rename the entry point to what the engine looks for in the client library
		if bld.env.USE_STATIC_FREEVGUI:
			defines += ['INTERNAL_VGUI_SUPPORT']

	install_path = None if bld.env.FREEVGUI_NO_INSTALL else bld.env.LIBDIR
	fn = bld.stlib if bld.env.USE_STATIC_FREEVGUI else bld.shlib

	linkflags = []

	# client libraries reference us by DT_NEEDED vgui.so entry. The original library however misses DT_SONAME but we set it
	# so the engine-preloaded vgui.so satisfies that lookup by an already loaded library, as DT_RUNPATH is not transitive and can't be relied upon here
	if not bld.env.USE_STATIC_FREEVGUI and bld.env.SONAME_ST:
		linkflags += (bld.env.SONAME_ST % (bld.env.cxxshlib_PATTERN % 'vgui')).split()

	fn(
		source = source,
		target = 'vgui',
		features = 'cxx',
		includes = includes,
		defines = defines,
		export_includes = '.',
		rpath = bld.env.DEFAULT_RPATH,
		use = 'yy_thunks',
		install_path = install_path,
		linkflags = linkflags,
		subsystem = bld.env.MSVC_SUBSYSTEM
	)
