#!/usr/bin/env python
""" eFEL setup """

"""
Copyright (c) 2015, EPFL/Blue Brain Project

 This file is part of eFEL <https://github.com/BlueBrain/eFEL>

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 3.0 as published
 by the Free Software Foundation.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
"""

try:
    from setuptools import setup, Extension
except ImportError:
    from distutils.core import setup, Extension  # pylint: disable=E0611,F0401

import pip.req
import os

execfile("efel/version.py")

BASEDIR = os.path.dirname(os.path.abspath(__file__))

cppcore_dir = os.path.join('efel', 'cppcore')
cppcore_sources = ['cppcore.cpp',
                   'Utils.cpp',
                   'LibV1.cpp',
                   'LibV2.cpp',
                   'LibV3.cpp',
                   'LibV4.cpp',
                   'LibV5.cpp',
                   'FillFptrTable.cpp',
                   'DependencyTree.cpp',
                   'efel.cpp',
                   'cfeature.cpp',
                   'mapoperations.cpp']
cppcore_sources = [
    os.path.join(
        cppcore_dir,
        filename) for filename in cppcore_sources]

cppcore = Extension('efel.cppcore',
                    sources=cppcore_sources,
                    include_dirs=['efel/cppcore/'])
setup(
    name="efel",
    version=VERSION,
    install_requires=['numpy>=1.6'],
    packages=['efel'],
    include_package_data=True,
    author="Werner Van Geit",
    author_email="werner.vangeit@epfl.ch",
    description="Electrophys Feature Extraction Library",
    license="LGPLv3",
    keywords=(
        'feature',
        'extraction',
        'electrophysiology',
        'BlueBrainProject'),
    url="http://bluebrain.epfl.ch",
    classifiers=[
        'Development Status :: 4 - Beta',
        'Environment :: Console',
        'License :: OSI Approved :: GNU Lesser General Public '
        'License v3 (LGPLv3)',
        'Operating System :: POSIX',
        'Topic :: Scientific/Engineering',
        'Topic :: Utilities'],
    package_data={
        '': [
            'DependencyV5.txt',
            'VERSION.txt',
            'GITHASH.txt']},
    ext_modules=[cppcore])
