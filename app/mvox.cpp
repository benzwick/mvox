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

#include <mfem.hpp>
#include <itkImageFileReader.h>

int main(int argc, char *argv[])
{
   // ----------------------------------------------------------------------
   // Initialize

   mfem::StopWatch timer;
   timer.Start();

   std::cout
      << "**********************************************************************" << std::endl
      << "*...._............._...._............._..............................*" << std::endl
      << "*...|_|._......._.|_|..|_|...........|_|.............................*" << std::endl
      << "*...|_||_|_..._|_||_|...|_|.........|_|......_.._......._......._....*" << std::endl
      << "*...|_|..|_|_|_|..|_|....|_|.......|_|...._.|_||_|._...|_|_..._|_|...*" << std::endl
      << "*...|_|....|_|....|_|.....|_|.....|_|....|_|......|_|....|_|_|_|.....*" << std::endl
      << "*...|_|...........|_|......|_|._.|_|.....|_|......|_|....._|_|_......*" << std::endl
      << "*...|_|...........|_|.......|_|_|_|......|_|._.._.|_|..._|_|.|_|_....*" << std::endl
      << "*...|_|...........|_|.........|_|...........|_||_|.....|_|.....|_|...*" << std::endl
      << "*....................................................................*" << std::endl
      << "**********************************************************************" << std::endl
      << "MVox Mesh Voxelizer "
      << "(version "
      << MVOX_VERSION_MAJOR << "."
      << MVOX_VERSION_MINOR << "."
      << MVOX_VERSION_PATCH ")"
      << std::endl
      << "Copyright (C) 2020 Benjamin F. Zwick"
      << std::endl;
   if (strcmp(MVOX_GIT_STRING, "") != 0)
   {
      std::cout << "Git: " << MVOX_GIT_STRING << std::endl;
   }

   // Warranty disclaimer
   std::cout
     << "\nThis program is distributed in the hope that it will be useful,\n"
     << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
     << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
     << "GNU General Public License for more details."
     << std::endl;

   // ----------------------------------------------------------------------
   // Options

   const char *box_file = "";        // box mesh output filename
   const char *output_file = "";     // voxelized mesh output filename
   const char *masks_file = "";      // input masks filename
   const char *attributes_file = ""; // input attributes filename

   bool visualization = false;

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
   args.AddOption(&output_file, "-o", "--output",
                  "Output mesh file to use (VTK or MFEM format).");
   args.AddOption(&box_file, "-b", "--box-output",
                  "Output bounding box mesh file to use (VTK or MFEM format).");
   args.AddOption(&masks_file, "-m", "--masks",
                  "Masks file to use (NRRD format).");
   args.AddOption(&attributes_file, "-a", "--attributes",
                  "Attributes file to use (NRRD format).");

   // Image parameters
   args.AddOption(&nx, "-nx", "--num-x-voxels",
                  "Number of voxels along x axis.");
   args.AddOption(&ny, "-ny", "--num-y-voxels",
                  "Number of voxels along y axis.");
   args.AddOption(&nz, "-nz", "--num-z-voxels",
                  "Number of voxels along z axis.");

   args.AddOption(&vx, "-vx", "--voxel-x-spacing",
                  "Voxel spacing along x axis.");
   args.AddOption(&vy, "-vy", "--voxel-y-spacing",
                  "Voxel spacing along y axis.");
   args.AddOption(&vz, "-vz", "--voxel-z-spacing",
                  "Voxel spacing along z axis.");

   // Miscellaneous options
   args.AddOption(&visualization, "-vis", "--visualization", "-no-vis",
                  "--no-visualization",
                  "Enable or disable GLVis visualization.");

   args.Parse();

   // Help
   if (args.Help())
   {
      std::cout << "\nMVox is a tool for generating volume meshes from image data.\n" << std::endl;

      std::cout << "Input:  NRRD files with masks and (optionally) attribute attributes (segmentation)" << std::endl;
      std::cout << "Output: VTK mesh file with attribute attributes" << std::endl;
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
   if (strcmp(masks_file, "") == 0 && strcmp(attributes_file, "") == 0)
   {
      std::cout << "Missing required option: --masks or --attributes (or both)\n" << std::endl;
      args.PrintUsage(std::cout);
      return 1;
   }
   else
   {
      if (strcmp(masks_file, "") == 0)
      {
         MVOX_WARNING( "Masks file not specified (using attributes file instead)." );
         masks_file = attributes_file;
      }
      else if (strcmp(attributes_file, "") == 0)
      {
         MVOX_WARNING( "Attributes file not specified (using masks file instead)." );
         attributes_file = masks_file;
      }
   }
   std::cout << "Masks file:      " << masks_file << std::endl;
   std::cout << "Attributes file: " << masks_file << std::endl;

   // Define ITK types
   using ImageType = itk::Image<short,3>;
   using ImageFileReaderType = itk::ImageFileReader<ImageType>;

   // Read masks
   ImageFileReaderType::Pointer masks_reader = ImageFileReaderType::New();
   std::cout << "Reading NRRD masks file:      '" << masks_file << "'... " << std::flush;
   masks_reader->SetFileName(masks_file);
   masks_reader->Update();
   ImageType::Pointer masks_image = masks_reader->GetOutput();
   std::cout << "done." << std::endl;

   // Read attributes
   ImageFileReaderType::Pointer attributes_reader = ImageFileReaderType::New();
   std::cout << "Reading NRRD attributes file: '" << attributes_file << "'... " << std::flush;
   attributes_reader->SetFileName(attributes_file);
   attributes_reader->Update();
   ImageType::Pointer attributes_image = attributes_reader->GetOutput();
   std::cout << "done." << std::endl;

   // ----------------------------------------------------------------------
   // Get information from *masks* image only

   std::cout << "\nMasks image information:" << std::endl;

   // Image size (number of voxels in x, y, z directions)
   ImageType::SizeType size = masks_image->GetLargestPossibleRegion().GetSize();
   std::cout << "Size: " << size << std::endl;
   if (nx == 0) nx = size[0];
   if (ny == 0) ny = size[1];
   if (nz == 0) nz = size[2];

   // Image spacing (voxel size)
   ImageType::SpacingType spacing = masks_image->GetSpacing();
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
   if (strcmp(box_file, "") != 0)
   {
      std::cout << "Saving mesh to file: '" << box_file << "'... " << std::flush;
      save_mesh(mesh, box_file);
      std::cout << "done." << std::endl;
   }

   // ----------------------------------------------------------------------
   // Create voxelized mesh

   short* masks = masks_image->GetBufferPointer();
   short* attributes = attributes_image->GetBufferPointer();

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
   for (int i = 0, j = 0; i < mesh.GetNE(); i++)
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
   if (strcmp(output_file, "") != 0)
   {
      std::cout << "Saving voxelized mesh to file: '" << output_file << "'... " << std::flush;
      save_mesh(vox, output_file);
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
