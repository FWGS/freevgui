#!/usr/bin/env python

from waflib.TaskGen import feature, after_method

def options(opt):
	pass

def configure(conf):
	if conf.env.DEST_OS != 'android':
		if conf.env.cxxshlib_PATTERN.startswith('lib'):
			conf.env.cxxshlib_PATTERN = conf.env.cxxshlib_PATTERN[3:]

def build(bld):
	if bld.env.DEST_OS == 'win32':
		platform = 'win32'
	else:
		platform = 'posix'

	bld.shlib(
		source   = bld.path.ant_glob([
			'*.cpp',
			'miniutl/utlmemory.cpp',
			'controls/*.cpp',
			'platform/common/*.cpp',
			'platform/%s/*.cpp' % platform,
		]),
		use = 'public',
		target   = 'vgui',
		features = 'cxx',
		includes = ['.', 'miniutl'],
		export_includes = '.',
		rpath    = '$ORIGIN',
		install_path = bld.env.LIBDIR,
		subsystem = bld.env.MSVC_SUBSYSTEM
	)
