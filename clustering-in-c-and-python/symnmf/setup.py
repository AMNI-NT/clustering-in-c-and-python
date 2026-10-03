from setuptools import Extension, setup

module = Extension('symnmf_module', sources=['symnmfmodule.c', 'symnmf.c'])

setup(
    name='symnmf_module',
    version='1.0',
    description='SymNMF module implemented in C',
    ext_modules=[module]
)