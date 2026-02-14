# Box Mesh

This example creates a box mesh using the dimensions of an input brain atlas volume.
The mesh is a structured hexahedral grid that fills the bounding box of the image.

## Input

The input is a T1-weighted brain MRI volume from the
[SPL Brain Atlas](https://github.com/mhalle/spl-brain-atlas):

![Axial view of input volume](../../screenshots/boxmesh_input_axial.png)

![Sagittal view of input volume](../../screenshots/boxmesh_input_sagittal.png)

![Coronal view of input volume](../../screenshots/boxmesh_input_coronal.png)

## Output

The output is a hexahedral mesh covering the bounding box of the input volume:

![3D view of box mesh](../../screenshots/boxmesh_mesh_3d.png)

## Command

```bash
mvox -box -imask A1_grayT1-1mm_resample.nrrd -omesh boxmesh.vtk
```
