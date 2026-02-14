#!/bin/bash
# Create a boxmesh using the dimensions of the input image
wget https://github.com/mhalle/spl-brain-atlas/raw/master/slicer/volumes/imaging/A1_grayT1-1mm_resample.nrrd
mvox -box -imask A1_grayT1-1mm_resample.nrrd -omesh mesh.mesh.gz
mvox -box -imask A1_grayT1-1mm_resample.nrrd -omesh boxmesh.vtk
