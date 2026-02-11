# mvox Guix Channel

This [Guix channel](https://guix.gnu.org/manual/en/html_node/Channels.html)
provides packages for [MFEM](https://mfem.org) and
[mvox](https://github.com/benzwick/mvox).

## Packages

| Package | Version | Description |
|---------|---------|-------------|
| `mfem`  | 4.5.2   | Finite element methods library (shared, zlib, exceptions; no MPI) |
| `mvox`  | 1.0.0   | Mesh voxelizer for MFEM meshes (depends on MFEM and ITK) |

## Usage

### Add the channel

Add the following to `~/.config/guix/channels.scm`:

```scheme
(cons (channel
       (name 'mvox)
       (url "https://github.com/benzwick/mvox")
       (branch "master")
       (introduction
        ;; TODO: add channel introduction for authentication
        ))
      %default-channels)
```

Then pull:

```sh
guix pull
```

### Build packages

```sh
guix build mfem
guix build mvox
```

### Install mvox

```sh
guix install mvox
```

### Smoke test

```sh
mvox --help
```

## Development

Build from a local checkout without adding the channel:

```sh
guix build -L guix mfem
guix build -L guix mvox
```

Lint packages:

```sh
guix lint -L guix mfem mvox
```

## Slicer integration

This channel is designed to be composed with
[guix-systole](https://codeberg.org/guix-systole/guix-systole) to build a 3D
Slicer custom application with mvox as a loadable C++ module.  This integration
is planned for a future release.
