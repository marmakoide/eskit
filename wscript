#! /usr/bin/env python

VERSION = '1.2.3'
APPNAME = 'eskit'

top = '.'
out = 'build'


def options(context):
    context.load('compiler_c')

    context.add_option('--use_LAPACK', action='store_true', default=False, help='uses LAPACK')


def configure(context):
    context.load('compiler_c')

    context.env['VERSION'] = VERSION
    context.env.CCFLAGS = ['-std=c99', '-Wall', '-Wextra', '-O2', '-g']

    # Handle LAPACK usage
    context.env.use_LAPACK = context.options.use_LAPACK
    if context.env.use_LAPACK:
        context.env.CCFLAGS.append('-DUSE_LAPACK')


def build(context):
    # 1. The eskit library
    context.shlib(
        target = 'eskit',
        source = context.path.ant_glob('libeskit/src/*.c'),
        includes = 'libeskit/include'
    )

    libeskit_include_dir = context.path.find_dir('libeskit/include')
    context.install_files('${PREFIX}/include', libeskit_include_dir.ant_glob('**/*.h'), relative_trick = True)

    # 2. The test & benchmarking program
    lib_list = ['m']
    if context.env.use_LAPACK:
        lib_list.append('lapack')

    context.program(
        target = 'eskit-test',
        install_path = None, 
        source = context.path.ant_glob('test/src/*.c'),
        includes = 'test/include libeskit/include',
        lib = lib_list,
        libpath  = ['/usr/lib'],
        use = 'eskit'
    )

    # 3. The pkg-config file
    lib_list_str = '-leskit'
    lib_list_str += ''.join([' -l' + lib for lib in lib_list])

    cflags_str = ''
    if context.env.use_LAPACK:
        cflags_str += '-DUSE_LAPACK'

    context(
        source='eskit.pc.in',
        LIBS=lib_list_str,
        CFLAGS=cflags_str, 
        PREFIX=context.env.PREFIX, 
        NAME='eskit',
        VERSION=context.env['VERSION'],
    )
