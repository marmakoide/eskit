#! /usr/bin/env python

import Options
import os.path



# the following two variables are used by the target "waf dist"
VERSION = '1.2.2'
APPNAME = 'eskit'

# these variables are mandatory ('/' are converted automatically)
top = '.'
out = 'build'



def set_options(context):
	context.tool_options('compiler_cc')

	context.add_option('--use_LAPACK', action='store_true', default=False, help='uses LAPACK')



def configure(context):
	context.check_tool('compiler_cc')
	context.check_tool('misc')

	context.env['VERSION'] = VERSION
	context.env.CCFLAGS = ['-std=c99', '-Wall', '-Wextra', '-O2', '-g']
	
	# Handle LAPACK usage
	context.env.use_LAPACK = Options.options.use_LAPACK
	if context.env.use_LAPACK:
		context.env.CCFLAGS.append('-DUSE_LAPACK')


def build(context):
	# 1. The eskit library
	context(
		target = 'eskit',
		features = 'cc cshlib',
		source = context.path.ant_glob('libeskit/src/*.c'),
		includes = 'libeskit/include')

	context.install_files('${PREFIX}/include', 'libeskit/include/eskit.h')
	context.install_files('${PREFIX}/include/eskit', 'libeskit/include/eskit/*.h')

	# 2. The test & benchmarking program
	libList = ['m']
	if context.env.use_LAPACK:
		libList.append('lapack')

	context(
		target = 'eskit-test',
		features = 'cc cprogram',
		install_path = '../test', 
		source = context.path.ant_glob('test/src/*.c'),
		includes = 'test/include libeskit/include',
		lib = libList,
		libpath  = ['/usr/lib'],
		uselib_local = 'eskit')

	# 3. The pkg-config file
	libListStr = '-leskit'
	libListStr += ''.join([' -l' + lib for lib in libList])

	cflagsStr = ''
	if context.env.use_LAPACK:
		cflagsStr += '-DUSE_LAPACK'

	obj = context.new_task_gen('subst')
	obj.source = 'eskit.pc.in'
	obj.target = 'eskit.pc'
	obj.dict   = {
		'LIBS': libListStr, 
		'CFLAGS': cflagsStr, 
		'PREFIX': context.env.PREFIX, 
		'NAME': 'eskit', 
		'VERSION': context.env['VERSION'] }

	context.install_files('${PREFIX}/lib/pkgconfig', 'eskit.pc')
