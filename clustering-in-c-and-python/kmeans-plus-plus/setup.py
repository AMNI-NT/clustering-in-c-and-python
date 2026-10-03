from setuptools import Extension, setup

module = Extension("kmeanssp", sources=['kmeansmodule.c'])
setup(name='kmeanssp',
     version='1.0',
     description='Python wrapper for kmeans C extension',
     ext_modules=[module])
