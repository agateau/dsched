#!/usr/bin/env python3
from setuptools import setup

import dsched

DESCRIPTION = 'dsched'

setup(name=dsched.__appname__,
      version=dsched.__version__,
      description=DESCRIPTION,
      author='Aurélien Gâteau',
      author_email='mail@agateau.com',
      license=dsched.__license__,
      platforms=['any'],
      url='http://github.com/agateau/dsched',
      install_requires=['arrow'],
      py_modules=['dsched'],
      entry_points={
          'gui_scripts': [
              'dsched = dsched:main',
          ],
      }
      )
