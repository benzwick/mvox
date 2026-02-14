# Brain Tensors

This example creates a mesh from a brain label map with conductivity tensors
estimated from diffusion-weighted imaging.

## Input

The input is a brain label map where each voxel is assigned a tissue label
(e.g., gray matter, white matter, CSF):

![Axial view of label map](../../screenshots/brain_labelmap_axial.png)

![Sagittal view of label map](../../screenshots/brain_labelmap_sagittal.png)

![Coronal view of label map](../../screenshots/brain_labelmap_coronal.png)

## Output

The output is a hexahedral mesh of the brain with element attributes
corresponding to tissue labels:

![3D view of brain mesh](../../screenshots/brain_mesh_3d.png)

## Command

```bash
mvox --input-attributes labelmap.nrrd --input-tensors cond.nrrd --output-mesh brain.vtk
```

## Data

The data for this example is available at <https://zenodo.org/record/7687631>.

## References

- Zwick BF, Bourantas GC, Safdar S, Joldes GR, Hyde DE, Warfield SK, Wittek A, Miller K.
  Patient-specific solution of the electrocorticography forward problem in deforming brain.
  *NeuroImage*. 2022;263:119649.
  [DOI: 10.1016/j.neuroimage.2022.119649](https://doi.org/10.1016/j.neuroimage.2022.119649)

- Zwick BF, Safdar S, Bourantas GC, Joldes GR, Hyde DE, Warfield SK, Wittek A, Miller K.
  Image data and computational grids for computing brain shift and solving the electrocorticography forward problem.
  *Data Brief*. 2023;48:109122.
  [DOI: 10.1016/j.dib.2023.109122](https://doi.org/10.1016/j.dib.2023.109122)
