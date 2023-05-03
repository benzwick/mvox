# MVox Mesh Voxelizer

MVox is a tool for generating structured finite element meshes from image data
by converting image voxels to elements.

| __Build Status__ | [![build](https://github.com/benzwick/mvox/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/benzwick/mvox/actions/workflows/c-cpp.yml) |
| :--- | :--- |

## Downloading

Clone the repository using [Git](https://git-scm.com):

    git clone https://github.com/benzwick/mvox.git

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
However, to use on-the-fly gzip compression of mesh and grid function files,
the zlib library option is required (`-DMFEM_USE_ZLIB=YES`).

For example, configure and make MFEM 4.5 in the
`../mfem-build` build directory relative to the
`./mfem-4.5.2` source directory:

    wget https://github.com/mfem/mfem/archive/refs/tags/v4.5.2.tar.gz
    tar -zxf v4.5.2.tar.gz
    mkdir mfem-build
    cd mfem-build
    cmake -DCMAKE_INSTALL_PREFIX=/opt/mfem/mfem-4.5 -DMFEM_USE_ZLIB=YES ../mfem-4.5.2
    make -j4
    make install

#### ITK

For instructions on how to install ITK see
https://itk.org/Wiki/ITK/Source#Linux_Package_Distributions.

For example, to install ITK on Debian or Ubuntu:

    sudo apt-get install libinsighttoolkit4-dev

### Building MVox from source

Configure and make MVox in the
`../build` build directory relative to the
`./mvox` source directory:

    cd ..
    mkdir build
    cd build
    cmake -DMFEM_DIR=/opt/mfem/mfem-4.5 ../mvox
    make
    make install

The `MFEM_DIR` variable is optional
and can be used to specify the location
of the MFEM library.

## Running

MVox is a CLI program.
For instructions on how to use MVox type `mvox --help`.

To create a mesh and symmetric tensors grid function:

    mvox -imask brain_mask.nrrd -iattr label.nrrd -itensor dti.nrrd -omesh mesh.mesh -sym -otensor dti.gf.gz

To view a tensor components using GLVis:

    glvis -m mesh.mesh -g dti.gf.gz -gc 0
    
Example scripts and input data files can be found
in the [examples](examples) directory.

## Copying

MVox is free software: you can redistribute it and/or modify it
under the terms of the BSD 3-Clause License.
See file [LICENSE](LICENSE) for details.

## Citing

If you use MVox for your research,
please consider adding the following citation:

- Zwick, B.F. (2020)
  MVox Mesh Voxelizer.
  Available at: https://github.com/benzwick/mvox

BibTeX:

    @Misc{zwick_2020_mvox,
      author =   {Benjamin F. Zwick},
      title =    {{MVox} {M}esh {V}oxelizer},
      howpublished = {\url{https://github.com/benzwick/mvox}},
      year = 2020
    }
