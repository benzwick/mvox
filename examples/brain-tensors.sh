#!/bin/bash
# Create a mesh from a brain label map with conductivity tensors estimated from diffusion-weighted imaging
# The data for this example is available at https://zenodo.org/record/7687631
# For further details, see the following publications:
# - Zwick BF, Bourantas GC, Safdar S, Joldes GR, Hyde DE, Warfield SK, Wittek A, Miller K. Patient-specific solution of the electrocorticography forward problem in deforming brain. NeuroImage. 2022;263:119649, DOI: 10.1016/j.neuroimage.2022.119649.
# - Zwick BF, Safdar S, Bourantas GC, Joldes GR, Hyde DE, Warfield SK, Wittek A, Miller K. Image data and computational grids for computing brain shift and solving the electrocorticography forward problem. Data Brief. 2023;48:109122, DOI: 10.1016/j.dib.2023.109122.
mvox --input-attributes labelmap.nrrd --input-tensors cond.nrrd --output-mesh mesh.mesh.gz --output-tensors cond.gf.gz
mvox --input-attributes labelmap.nrrd --input-tensors cond.nrrd --output-mesh brain.vtk --output-tensors cond-brain.gf.gz
