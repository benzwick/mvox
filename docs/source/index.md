# MVox Mesh Voxelizer

MVox is a tool for generating structured finite element meshes from image data
by converting image voxels to elements.

MVox reads medical image data in [NRRD](https://teem.sourceforge.net/nrrd/) format
(label maps, masks, tensor fields) and generates finite element meshes
compatible with [MFEM](https://mfem.org).

## Examples

```{toctree}
:maxdepth: 2

examples/boxmesh
examples/brain-tensors
```

## Links

- [Source code](https://github.com/benzwick/mvox)
- [MFEM](https://mfem.org) — finite element library
- [ITK](https://itk.org) — image processing library
