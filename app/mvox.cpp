/*
* Copyright (c) 2020-2021, Benjamin F. Zwick. All Rights reserved.
*
* This file is part of MVox.
*
* MVox is free software: you can redistribute it and/or modify it
* under the terms of the BSD 3-Clause License.
* See file LICENSE for details.
*/

#include "mvox.hpp"

#include <fstream>
#include <iostream>
#include <limits>

#include <mfem.hpp>
#include <itkImageFileReader.h>

int main(int argc, char *argv[])
{
   // ----------------------------------------------------------------------
   // Initialize

   mfem::StopWatch timer;
   timer.Start();

   std::cout
      <<
      "**********************************************************************\n"
      "*...._............._...._............._..............................*\n"
      "*...|_|._......._.|_|..|_|...........|_|.............................*\n"
      "*...|_||_|_..._|_||_|...|_|.........|_|......_.._......._......._....*\n"
      "*...|_|..|_|_|_|..|_|....|_|.......|_|...._.|_||_|._...|_|_..._|_|...*\n"
      "*...|_|....|_|....|_|.....|_|.....|_|....|_|......|_|....|_|_|_|.....*\n"
      "*...|_|...........|_|......|_|._.|_|.....|_|......|_|....._|_|_......*\n"
      "*...|_|...........|_|.......|_|_|_|......|_|._.._.|_|..._|_|.|_|_....*\n"
      "*...|_|...........|_|.........|_|...........|_||_|.....|_|.....|_|...*\n"
      "*....................................................................*\n"
      "**********************************************************************\n"
      << "MVox Mesh Voxelizer "
      << std::endl
      << "Copyright (c) 2020-2021, Benjamin F. Zwick. All Rights reserved."
      << std::endl
      << "Version: "
      << MVOX_VERSION_MAJOR << "."
      << MVOX_VERSION_MINOR << "."
      << MVOX_VERSION_PATCH
      << std::endl;

   // Warranty disclaimer
   std::cout
     << "\nThis program is distributed in the hope that it will be useful,\n"
     << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
     << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
     << "See file LICENSE for more details."
     << std::endl;

   // ----------------------------------------------------------------------
   // Options

   const char *mesh_ofile = "";       // voxelized mesh output filename
   const char *tensors_ofile = "";    // output tensors filename
   const char *masks_ifile = "";      // input masks filename
   const char *attributes_ifile = ""; // input attributes filename
   const char *tensors_ifile = "";    // input tensors filename

   bool visualization = false;
   bool symmetric = false;
   bool boxmesh = false;

   const int dim = 3;

   // Voxel or element size
   double vx = 0;
   double vy = 0;
   double vz = 0;

   // Number of voxels or elements in x, y, z directions
   int nx = 0;
   int ny = 0;
   int nz = 0;

   // ----------------------------------------------------------------------
   // Parse command-line options

   mfem::OptionsParser args(argc, argv);

   // Files
   args.AddOption(&mesh_ofile,
                  "-omesh", "--output-mesh",
                  "Output mesh file to use (VTK or MFEM format).");
   args.AddOption(&tensors_ofile,
                  "-otensor", "--output-tensors",
                  "Output tensors file to use (MFEM format).");
   args.AddOption(&masks_ifile,
                  "-imask", "--input-mask",
                  "Masks file to use (NRRD format).");
   args.AddOption(&attributes_ifile,
                  "-iattr", "--input-attributes",
                  "Attributes file to use (NRRD format).");
   args.AddOption(&tensors_ifile,
                  "-itensor", "--input-tensors",
                  "Tensors file to use (NRRD format).");
   args.AddOption(&symmetric,
                  "-sym", "--symmetric-tensors",
                  "-no-sym", "--no-symmetric-tensors",
                  "Enable or disable symmetric tensor output.");
   args.AddOption(&boxmesh,
                  "-box", "--box-mesh",
                  "-no-box", "--no-box-mesh",
                  "Create boxmesh using image dimensions.");

   // Image parameters
   args.AddOption(&nx,
                  "-nx", "--num-x-voxels",
                  "Number of voxels along x axis.");
   args.AddOption(&ny,
                  "-ny", "--num-y-voxels",
                  "Number of voxels along y axis.");
   args.AddOption(&nz,
                  "-nz", "--num-z-voxels",
                  "Number of voxels along z axis.");

   args.AddOption(&vx,
                  "-vx", "--voxel-x-spacing",
                  "Voxel spacing along x axis.");
   args.AddOption(&vy,
                  "-vy", "--voxel-y-spacing",
                  "Voxel spacing along y axis.");
   args.AddOption(&vz,
                  "-vz", "--voxel-z-spacing",
                  "Voxel spacing along z axis.");

   // Miscellaneous options
   args.AddOption(&visualization,
                  "-vis", "--visualization",
                  "-no-vis", "--no-visualization",
                  "Enable or disable GLVis visualization.");

   args.Parse();

   // Help
   if (args.Help())
   {
      std::cout << "\nMVox is a tool for generating volume meshes from image data.\n" << std::endl;

      std::cout << "Input:  NRRD image files with mask and (optionally) attributes and tensors." << std::endl;
      std::cout << "Output: MFEM or VTK mesh file with attributes "
                << "and (optionally) MFEM grid function file with tensors." << std::endl;
   }

   // Check args and exit on error
   std::cout << std::endl;
   if (!args.Good())
   {
      args.PrintUsage(std::cout);
      return 1;
   }

   args.PrintOptions(std::cout);
   std::cout << std::endl;

   // ----------------------------------------------------------------------
   // Read image input files

   // If only one of masks or attributes file is specified use that as both.
   if (strcmp(masks_ifile, "") == 0 && strcmp(attributes_ifile, "") == 0)
   {
      std::cout << "Missing required option: --masks or --attributes (or both)\n" << std::endl;
      args.PrintUsage(std::cout);
      return 1;
   }
   else
   {
      if (strcmp(masks_ifile, "") == 0)
      {
         MVOX_WARNING( "Masks file not specified (using attributes file instead)." );
         masks_ifile = attributes_ifile;
      }
      else if (strcmp(attributes_ifile, "") == 0)
      {
         MVOX_WARNING( "Attributes file not specified (using masks file instead)." );
         attributes_ifile = masks_ifile;
      }
   }
   std::cout << "Masks file:      " << masks_ifile << std::endl;
   std::cout << "Attributes file: " << masks_ifile << std::endl;

   // Define ITK types
   // Scalar images
   using ShortImageType = itk::Image<short,3>;
   using ShortImageFileReaderType = itk::ImageFileReader<ShortImageType>;
   // Tensor images
   using TensorPixelType = itk::DiffusionTensor3D<double>;
   using TensorImageType = itk::Image<TensorPixelType,3>;
   using TensorImageFileReaderType = itk::ImageFileReader<TensorImageType>;

   // Read masks
   ShortImageFileReaderType::Pointer masks_reader = ShortImageFileReaderType::New();
   std::cout << "Reading NRRD masks file:      '" << masks_ifile << "'... " << std::flush;
   masks_reader->SetFileName(masks_ifile);
   masks_reader->Update();
   ShortImageType::Pointer masks_image = masks_reader->GetOutput();
   std::cout << "done." << std::endl;

   // Read attributes
   ShortImageFileReaderType::Pointer attributes_reader = ShortImageFileReaderType::New();
   std::cout << "Reading NRRD attributes file: '" << attributes_ifile << "'... " << std::flush;
   attributes_reader->SetFileName(attributes_ifile);
   attributes_reader->Update();
   ShortImageType::Pointer attributes_image = attributes_reader->GetOutput();
   std::cout << "done." << std::endl;

   // TODO:
   // Read scalars
   // Read vectors

   // Read tensors (full 3x3 or symmetric matrix with 6 components in nrrd)
   // NOTE: image format: http://teem.sourceforge.net/nrrd/format.html#kinds
   // "3D-symmetric-matrix"  6  Unique components of a 3D symmetric matrix: Mxx Mxy Mxz Myy Myz Mzz
   // "3D-matrix"            9  Components of 3D matrix:                    Mxx Mxy Mxz Myx Myy Myz Mzx Mzy Mzz
   TensorImageFileReaderType::Pointer tensors_reader = TensorImageFileReaderType::New();
   TensorImageType::Pointer tensors_image;
   if (strcmp(tensors_ifile, "") == 0)
   {
      if (strcmp(tensors_ofile, "") != 0)
      {
         MVOX_ERROR( "Tensor output requested but tensor input file not specified." );
         return 1;
      }
      // else all good because no input and no output
   }
   else
   {
      if (strcmp(tensors_ofile, "") == 0)
      {
         MVOX_ERROR( "Tensor input exists but tensor output file not specified." );
         return 1;
      }
      else // both input and ouput tensor files specified:
      {
         std::cout << "Reading NRRD tensors file: '" << tensors_ifile << "'... " << std::flush;
         tensors_reader->SetFileName(tensors_ifile);
         tensors_reader->Update();
         tensors_image = tensors_reader->GetOutput();
         std::cout << "done." << std::endl;
      }
   }

   // Image data
   short* masks = masks_image->GetBufferPointer();
   short* attributes = attributes_image->GetBufferPointer();
   TensorPixelType* tensors;
   if (strcmp(tensors_ifile, "") != 0)
   {
      tensors = tensors_image->GetBufferPointer();
   }

   // ----------------------------------------------------------------------
   // Get information from *masks* image only

   std::cout << "\nMasks image information:" << std::endl;

   // Image size (number of voxels in x, y, z directions)
   ShortImageType::SizeType size = masks_image->GetLargestPossibleRegion().GetSize();
   std::cout << "Size: " << size << std::endl;
   if (nx == 0) nx = size[0];
   if (ny == 0) ny = size[1];
   if (nz == 0) nz = size[2];

   // Image spacing (voxel size)
   ShortImageType::SpacingType spacing = masks_image->GetSpacing();
   std::cout << "Spacing: " << spacing << std::endl;
   if (vx == 0) vx = spacing[0];
   if (vy == 0) vy = spacing[1];
   if (vz == 0) vz = spacing[2];

   // Image directions (not the same as NRRD space directions)
   ShortImageType::DirectionType direction = masks_image->GetDirection();
   std::cout << "Direction:\n" << direction << std::endl;

   // Image origin
   ShortImageType::PointType image_origin = masks_image->GetOrigin();
   std::cout << "Origin: " << image_origin << std::endl;

   // Physical size in x, y, z directions
   double sx = vx * nx;
   double sy = vy * ny;
   double sz = vz * nz;
   std::cout << "Physical dimensions: "
             << "[" << sx << ", " << sy << ", " << sz << "]\n" << std::endl;

   // Total number of voxels
   int num_voxels = nx * ny * nz;
   // Number of vertices for the image box mesh (final number may be lower)
   int box_num_vertices = (nx+1) * (ny+1) * (nz+1);

   // ----------------------------------------------------------------------
   // Create voxelized mesh
   // Similar to mfem::Mesh::Make3D in mfem/mesh/mesh.cpp

   std::cout << "Generating image mesh grid with: "
             << box_num_vertices << " vertices." << std::endl;

   // Initialize voxelized mesh
   mfem::Mesh vox(dim, box_num_vertices, 0);

   // Shift mesh origin by half a voxel
   // See https://itk.org/ItkSoftwareGuide.pdf, section 4.1.4
   ShortImageType::PointType mesh_origin;
   {
      using MatrixType = itk::Matrix<double, dim, dim>;
      using VectorType = itk::Vector<double, dim>;
      MatrixType spacing_matrix;
      spacing_matrix.Fill(0.0);
      spacing_matrix(0,0) = spacing[0];
      spacing_matrix(1,1) = spacing[1];
      spacing_matrix(2,2) = spacing[2];
      VectorType offset;
      offset.Fill(-0.5);
      mesh_origin = image_origin + (direction * spacing_matrix * offset);
   }

   // Set vertices and the corresponding coordinates for entire box
   std::cout << "Adding vertices... " << std::flush;
   {
      int x, y, z;
      double coord[3];
#define COORD(I, O, D, S) (O[I] + D(I,0)*S[0]*x + D(I,1)*S[1]*y + D(I,2)*S[2]*z)
      for (z = 0; z <= nz; z++)
      {
         for (y = 0; y <= ny; y++)
         {
            for (x = 0; x <= nx; x++)
            {
               coord[0] = COORD(0, mesh_origin, direction, spacing);
               coord[1] = COORD(1, mesh_origin, direction, spacing);
               coord[2] = COORD(2, mesh_origin, direction, spacing);
               vox.AddVertex(coord);
            }
         }
      }
   }
   std::cout << "done." << std::endl;

   // Set elements and the corresponding indices of vertices only if mask > 0
   // using lexicographic ordering (i.e. sfc_ordering = false in Mesh::Make3D)
   std::cout << "Adding elements... " << std::flush;
   int ne_keep = 0;
   int ne_discard = 0;
   int num_bad_voxels = 0;
   {
      int i, j, x, y, z;
      int ind[8];
#define VTX(XC, YC, ZC) ((XC)+((YC)+(ZC)*(ny+1))*(nx+1))
      for (i = 0, j = 0, z = 0; z < nz; z++)
      {
         for (y = 0; y < ny; y++)
         {
            for (x = 0; x < nx; x++)
            {
               int mask = boxmesh ? 1 : masks[i];
               int attr = boxmesh ? 1 : attributes[i];
               if (mask > 0)
               {
                  if (attr < 1)
                  {
                     num_bad_voxels++;
                     // We require the element attribute to be strictly positive
                     // so enforce it by highlighting the invalid elements with a
                     // value that is lower than the possible minimum input value.
                     attr = SHRT_MIN - 1;
                  }
                  ind[0] = VTX(x  , y  , z  );
                  ind[1] = VTX(x+1, y  , z  );
                  ind[2] = VTX(x+1, y+1, z  );
                  ind[3] = VTX(x  , y+1, z  );
                  ind[4] = VTX(x  , y  , z+1);
                  ind[5] = VTX(x+1, y  , z+1);
                  ind[6] = VTX(x+1, y+1, z+1);
                  ind[7] = VTX(  x, y+1, z+1);
                  vox.AddHex(ind, 1);
                  vox.SetAttribute(j, attr);
                  j++;
               }
#if MVOX_DEBUG
               if (mask <= 0)
               {
                  std::cout << i << " " << mask << " : ";
               }
#endif
               i++;
            }
         }
      }
      ne_keep = j;
      ne_discard = num_voxels - ne_keep;
   }
   std::cout << "done." << std::endl;

   if (num_bad_voxels > 0)
   {
      MVOX_WARNING( num_bad_voxels << " voxels have non-positive values." );
   }

   std::cout << "Number of voxels included: " << ne_keep << std::endl;
   std::cout << "Number of voxels excluded: " << ne_discard << std::endl;

   std::cout << "Finalizing topology of voxelized mesh... " << std::flush;
   vox.FinalizeTopology();
   std::cout << "done." << std::endl;

   std::cout << "Finalizing voxelized mesh... " << std::flush;
   vox.Finalize();
   std::cout << "done." << std::endl;

   std::cout << "Removing unused vertices... " << std::flush;
   vox.RemoveUnusedVertices();
   std::cout << "done." << std::endl;

   std::cout << "\nVoxelized mesh information:" << std::endl;
   vox.PrintInfo();

   // Save voxelized mesh to file
   if (strcmp(mesh_ofile, "") != 0)
   {
      std::cout << "Saving voxelized mesh to file: '" << mesh_ofile << "'... " << std::flush;
      save_mesh(vox, mesh_ofile);
      std::cout << "done." << std::endl;
   }

   // ----------------------------------------------------------------------
   // Tensors

   if (strcmp(tensors_ifile, "") != 0)
   {
      // Define a finite element space on the mesh
      int vdim = symmetric ? 6 : 9;
      mfem::L2_FECollection tensors_fec(0, dim);
      mfem::FiniteElementSpace tensors_fespace(&vox, &tensors_fec, vdim);

      // Define grid functions for tensor components
      mfem::GridFunction tensors_gf(&tensors_fespace);

      std::cout << "Assigning tensor values... " << std::flush;
      for (int vi = 0, ei = 0; vi < num_voxels; vi++)
      {
         if (masks[vi] > 0)
         {
            if (symmetric)
            {
               tensors_gf(tensors_fespace.DofToVDof(ei, 0)) = tensors[vi](0,0); // Mxx
               tensors_gf(tensors_fespace.DofToVDof(ei, 1)) = tensors[vi](0,1); // Mxy
               tensors_gf(tensors_fespace.DofToVDof(ei, 2)) = tensors[vi](0,2); // Mxz
               tensors_gf(tensors_fespace.DofToVDof(ei, 3)) = tensors[vi](1,1); // Myy
               tensors_gf(tensors_fespace.DofToVDof(ei, 4)) = tensors[vi](1,2); // Myz
               tensors_gf(tensors_fespace.DofToVDof(ei, 5)) = tensors[vi](2,2); // Mzz
               // Ensure that tensor is really symmetric
               if (tensors_gf(tensors_fespace.DofToVDof(ei, 1)) != tensors[vi](1,0) ||
                   tensors_gf(tensors_fespace.DofToVDof(ei, 2)) != tensors[vi](2,0) ||
                   tensors_gf(tensors_fespace.DofToVDof(ei, 4)) != tensors[vi](2,1))
               {
                  MFEM_ABORT("Tensor at voxel " << vi << " is not symmetric!");
               }
            }
            else
            {
               tensors_gf(tensors_fespace.DofToVDof(ei, 0)) = tensors[vi](0,0); // Mxx
               tensors_gf(tensors_fespace.DofToVDof(ei, 1)) = tensors[vi](0,1); // Mxy
               tensors_gf(tensors_fespace.DofToVDof(ei, 2)) = tensors[vi](0,2); // Mxz
               tensors_gf(tensors_fespace.DofToVDof(ei, 3)) = tensors[vi](1,0); // Myx
               tensors_gf(tensors_fespace.DofToVDof(ei, 4)) = tensors[vi](1,1); // Myy
               tensors_gf(tensors_fespace.DofToVDof(ei, 5)) = tensors[vi](1,2); // Myz
               tensors_gf(tensors_fespace.DofToVDof(ei, 6)) = tensors[vi](2,0); // Mzx
               tensors_gf(tensors_fespace.DofToVDof(ei, 7)) = tensors[vi](2,1); // Mzy
               tensors_gf(tensors_fespace.DofToVDof(ei, 8)) = tensors[vi](2,2); // Mzz
            }
            ei++;
         }
      }
      MFEM_ASSERT(ei == vox.GetNE(), "Mismatch between number of tensors and elements");
      std::cout << "done." << std::endl;

      // Save tensors to file
      std::cout << "Saving tensors to file: '" << tensors_ofile << "'... " << std::flush;
      save_gridfunction(tensors_gf, tensors_ofile);
      std::cout << "done." << std::endl;
   }

   // ----------------------------------------------------------------------
   // Send the mesh by socket to a GLVis server

   if (visualization)
   {
      std::cout << "Visualizing the voxelized mesh... " << std::flush;

      // Avoid excessive memory usage with large meshes
      if (ne_keep > 5e6)
      {
         MFEM_ABORT( "Too big to visualize with GLVis." );
      }

      char vishost[] = "localhost";
      int  visport   = 19916;
      mfem::socketstream sock(vishost, visport);
      sock.precision(8);
      sock << "mesh\n" << vox << std::flush;

      std::cout << "done." << std::endl;
   }

   // ----------------------------------------------------------------------
   // Finalize

   std::cout << "Time elapsed: " << timer.RealTime() << " s" << std::endl;

   std::cout << "Success!" << std::endl;

   return 0;
}
