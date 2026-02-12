# MVox Mesh Voxelizer

MVox is a tool for generating structured finite element meshes from image data
by converting image voxels to elements.

- **[Installing](#installing)** — install with GNU Guix or build from source
- **[Running](#running)** — usage examples and CLI options
- **[Examples](examples)** — example scripts and input data

| __Build Status__ | [![build](https://github.com/benzwick/mvox/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/benzwick/mvox/actions/workflows/c-cpp.yml) |
| :--- | :--- |

## Installing

MVox can be installed [with GNU Guix](#installing-with-gnu-guix)
or [built from source](#building-from-source).

### Installing with GNU Guix

MVox and all its dependencies can be installed with
[GNU Guix](https://guix.gnu.org) using the
[guix-mvox](https://github.com/benzwick/guix-mvox) channel.

To add the channel, include the following in
`~/.config/guix/channels.scm`:

```scheme
(cons (channel
       (name 'mvox)
       (url "https://github.com/benzwick/guix-mvox")
       (branch "main"))
      %default-channels)
```

Then pull and install:

    guix pull
    guix install mvox

See the [guix-mvox README](https://github.com/benzwick/guix-mvox#readme)
for more details.

### Building from source

MVox can be configured and installed using the
[CMake](https://cmake.org)
build system.

MVox depends on the
[MFEM](https://mfem.org)
and
[ITK](https://itk.org)
libraries which must be installed first,
either using [GNU Guix](#installing-dependencies-with-gnu-guix) or manually as described below.

#### Installing dependencies with GNU Guix

Use the [guix-mvox](https://github.com/benzwick/guix-mvox) channel
to provide all build dependencies in a development shell.

If the channel is already configured
(see [Installing with GNU Guix](#installing-with-gnu-guix)):

    guix shell -D mvox

Or from a local checkout of the
[guix-mvox](https://github.com/benzwick/guix-mvox) Guix channel repository:

    guix shell -D -L /path/to/guix-mvox mvox

This provides a development shell with MFEM, ITK, and all build tools.
Then build MVox as described in
[Building and installing MVox](#building-and-installing-mvox) below.

#### Installing dependencies on Linux

Install [CMake](https://cmake.org) and [ITK](https://itk.org)
using your package manager.
For instructions on how to install ITK see
https://docs.itk.org/en/latest/download.html.

For example, on Debian or Ubuntu:

    sudo apt-get install cmake libinsighttoolkit5-dev

On older distributions, use `libinsighttoolkit4-dev` instead.

Then build and install MFEM and MVox as described in
[Building and installing MFEM](#building-and-installing-mfem) and
[Building and installing MVox](#building-and-installing-mvox) below.

#### Building and installing MFEM

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

#### Building and installing MVox

Clone the repository using [Git](https://git-scm.com):

    git clone https://github.com/benzwick/mvox.git

Configure and make MVox in the
`../build` build directory relative to the
`./mvox` source directory:

    mkdir build
    cd build
    cmake -DMFEM_DIR=/opt/mfem/mfem-4.5 ../mvox
    make
    make install

The `MFEM_DIR` variable is optional
and can be used to specify the location
of the MFEM library.
When using [GNU Guix](#installing-dependencies-with-gnu-guix) for dependencies,
CMake will find MFEM automatically.

## Running

MVox is a CLI program.
For instructions on how to use MVox type `mvox --help`.

To create a mesh and symmetric tensors grid function:

    mvox -imask brain_mask.nrrd -iattr label.nrrd -itensor dti.nrrd -omesh mesh.mesh -sym -otensor dti.gf.gz

To view tensor components using GLVis:

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
