/*
* Copyright (C) 2020 Benjamin F. Zwick
*
* This file is part of MVox.
*
* MVox is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* MVox is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with MVox.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "mvox.hpp"

#include <fstream>
#include <iostream>
#include <limits>

#include <mfem.hpp>
#include <itkImageFileReader.h>

int main(int argc, char *argv[])
{
   // Constants
   constexpr auto output_precision = std::numeric_limits<double>::max_digits10;

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
      << "Copyright (C) 2020 Benjamin F. Zwick"
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
     << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
     << "GNU General Public License for more details."
     << std::endl;

   // ----------------------------------------------------------------------
   // Options

   const char *box_ofile = "";        // box mesh output filename
   const char *mesh_ofile = "";       // voxelized mesh output filename
   const char *tensors_ofile = "";    // output tensors filename
   const char *masks_ifile = "";      // input masks filename
   const char *attributes_ifile = ""; // input attributes filename
   const char *tensors_ifile = "";    // input tensors filename

   bool visualization = false;
   bool symmetric = false;

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
   args.AddOption(&box_ofile,
                  "-obox", "--output-box",
                  "Output bounding box mesh file to use (VTK or MFEM format).");
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

      std::cout << "Input:  NRRD files with masks and (optionally) attribute attributes (segmentation)" << std::endl;
      std::cout << "Output: VTK or MFEM mesh file with attributes" << std::endl;
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
   using TensorPixelType = itk::DiffusionTensor3D<float>;
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
   std::cout << "Reading NRRD tensors file: '" << tensors_ifile << "'... " << std::flush;
   tensors_reader->SetFileName(tensors_ifile);
   tensors_reader->Update();
   TensorImageType::Pointer tensors_image = tensors_reader->GetOutput();
   std::cout << "done." << std::endl;

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

   // Physical size in x, y, z directions
   double sx = vx * nx;
   double sy = vy * ny;
   double sz = vz * nz;
   std::cout << "Physical dimensions: "
             << "[" << sx << ", " << sy << ", " << sz << "]\n" << std::endl;

   // Total number of voxels
   int num_voxels = nx * ny * nz;

   // ----------------------------------------------------------------------
   // Create box mesh

   std::cout << "Generating bounding box mesh with: "
             << nx*ny*nz << " elements... " << std::flush;
   mfem::Mesh mesh(nx, ny, nz, mfem::Element::HEXAHEDRON, false,
                   sx, sy, sz, false);
   std::cout << "done." << std::endl;

   std::cout << "\nBounding box mesh information:" << std::endl;
   mesh.PrintInfo();

   // Save box mesh to file
   if (strcmp(box_ofile, "") != 0)
   {
      std::cout << "Saving mesh to file: '" << box_ofile << "'... " << std::flush;
      save_mesh(mesh, box_ofile);
      std::cout << "done." << std::endl;
   }

   // ----------------------------------------------------------------------
   // Create voxelized mesh

   short* masks = masks_image->GetBufferPointer();
   short* attributes = attributes_image->GetBufferPointer();
   TensorPixelType* tensors = tensors_image->GetBufferPointer();

   std::cout << "Setting element attributes and counting "
             << "number of elements to keep... " << std::flush;
   int ne_keep = 0;
   int ne_discard = 0;
   int num_bad_voxels = 0;
   for (int i = 0; i < mesh.GetNE(); i++)
   {
      int mask = masks[i];
      int mat = attributes[i];
      if (mask > 0 && mat < 1)
      {
         num_bad_voxels++;
         // We require the element attribute to be strictly positive
         // so enforce it by highlighting the invalid elements with a
         // value that is lower than the possible minimum input value.
         mat = SHRT_MIN - 1;
      }
      mesh.SetAttribute(i, mat);
      mask > 0 ? ne_keep++ : ne_discard++;
#if MVOX_DEBUG
      if (mask <= 0)
      {
         std::cout << i << " " << mask << " : ";
      }
#endif
   }
   std::cout << "done." << std::endl;
   if (num_bad_voxels > 0)
   {
      MVOX_WARNING( num_bad_voxels << " voxels have non-positive values." );
   }

   // Initialize voxelized mesh
   mfem::Mesh vox(dim, mesh.GetNV(), ne_keep);

   // Copy the vertices
   std::cout << "Copying vertices... " << std::flush;
   for (int i = 0; i < mesh.GetNV(); i++)
   {
      vox.AddVertex(mesh.GetVertex(i));
   }
   std::cout << "done." << std::endl;

   std::cout << "Creating elements... " << std::flush;
   for (int i = 0, j = 0; i < num_voxels; i++)
   {
      if (masks[i] > 0)
      {
         vox.AddElement(mesh.GetElement(i)->Duplicate(&vox));
         vox.SetAttribute(j, mesh.GetAttribute(i));
         j++;
      }
   }
   std::cout << "done." << std::endl;
   std::cout << "Number of elements included: " << ne_keep << std::endl;
   std::cout << "Number of elements excluded: " << ne_discard << std::endl;

   // Free memory of the box mesh that is no longer required
   mesh.Clear();

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

   // Save tensors to file
   if (strcmp(tensors_ofile, "") != 0)
   {
      std::cout << "Saving tensors to file: '" << tensors_ofile << "'... " << std::flush;

      // Create ouput file stream
      std::ostream *tofs;
      if (strcmp(file_ext(tensors_ofile), "gz") == 0) // compressed MFEM mesh
      {
#ifdef MFEM_USE_ZLIB
         // See https://github.com/mfem/mfem/pull/638/files
         tofs = new mfem::ofgzstream(tensors_ofile, "zwb9");
#else
         MFEM_ABORT( "Cannot compress file because MFEM was built without ZLIB" );
#endif
      }
      else
      {
         tofs = new std::ofstream (tensors_ofile, std::ofstream::out);
      }
      tofs->precision(output_precision);

      tensors_gf.Save(*tofs);
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
