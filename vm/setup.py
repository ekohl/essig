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
    else:
        source = 'cli.c'
    
    extra_objects = ['vm.o', 'generated_simulator.o', 'interrupt_handler.o']
    climodule = Extension(
        'cli', 
        [source], 
        depends=extra_objects + ['cli.pxd'],
        extra_objects=extra_objects,
    )
    
    setup(
        name = 'Simulator CLI',
        ext_modules = [climodule],
        cmdclass=dict(build_ext=build_ext),
    )

if __name__ == '__main__':
    main()
