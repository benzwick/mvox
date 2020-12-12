# MVox Mesh Voxelizer

[![pipeline status](https://gitlab.com/benzwick/mvox/badges/master/pipeline.svg)](https://gitlab.com/benzwick/mvox/-/commits/master)
[![coverage report](https://gitlab.com/benzwick/mvox/badges/master/coverage.svg)](https://gitlab.com/benzwick/mvox/-/commits/master)
[![documentation (placeholder)](https://img.shields.io/badge/docs-latest-blue.svg)](https://benzwick.gitlab.io/mvox/)

MVox is a tool for generating finite element meshes from image data.

## Downloading

Clone the repository using [Git](https://git-scm.com):

    git clone https://gitlab.com/benzwick/mvox.git

## Installing

MVox can be configured and installed using the
[CMake](https://cmake.org)
build system.

### Dependencies

MVox depends on the
[MFEM](https://mfem.org)
and
[ITK](https://itk.org)
libraries which must be installed first.

#### MFEM

For instructions on how to install MFEM see
https://github.com/mfem/mfem/blob/master/INSTALL.

For use with MVox it is sufficient to configure MFEM
with the default options.

#### ITK

For instructions on how to install ITK see
https://itk.org/Wiki/ITK/Source#Linux_Package_Distributions.

For example, to install ITK on Ubuntu 20.04:

    sudo apt-get install cmake
    sudo apt-get install libinsighttoolkit4-dev

### Building MVox from source

Configure and make MVox in the
`../build` build directory relative to the
`./mvox` source directory:

    cd ..
    mkdir build
    cd build
    cmake -DMFEM_DIR=/opt/mfem/mfem-4.2 ../mvox
    make

The `MFEM_DIR` variable is optional
and can be used to specify the location
of the MFEM library.

## Running

MVox is a CLI program.
For instructions on how to use MVox type `mvox --help`.

Example scripts and input data files can be found
in the [examples](examples) directory.

## Copying

MVox is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MVox is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

## Citing

If you use MVox for your research,
please consider adding the following citation:

- Zwick, B.F. (2020)
  MVox Mesh Voxelizer.
  Available at: https://gitlab.com/benzwick/mvox

BibTeX:

    @Misc{zwick_2020_mvox,
      author =   {Benjamin F. Zwick},
      title =    {{MVox} {M}esh {V}oxelizer},
      howpublished = {\url{https://gitlab.com/benzwick/mvox}},
      year = 2020
    }
