import os
from distutils.core import setup, Extension

try:
    from Cython.Distutils import build_ext
    havecython = True
except ImportError:
    from distutils.command.build_ext import build_ext
    havecython = False

libdir = os.path.dirname(os.path.abspath(__file__))

def main():
    if havecython:
        source = 'cli.pyx'
        depends = ['cli.pxd', 'vm.h']
    else:
        source = 'cli.c'
        depends = ['vm.h']
    
    climodule = Extension(
        'cli', 
        [source], 
        depends=depends,
        extra_objects=['vm.o', 'dummy_simulator.o'],
    )
    
    setup(
        name = 'Simulator CLI',
        ext_modules = [climodule],
        cmdclass=dict(build_ext=build_ext),
    )

if __name__ == '__main__':
    main()
    