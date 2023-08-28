from setuptools import Extension, setup

module = Extension("kmeanssp", sources=['kmeans.c','kmeansmodule.c'])
setup(name='kmeanssp',
      version='1.0',
      description='Python wrapper for custom C kmeans module extension',
      ext_modules=[module])