/*
* Copyright (c) 2020-2021, Benjamin F. Zwick. All Rights reserved.
*
* This file is part of MVox.
*
* MVox is free software: you can redistribute it and/or modify it
* under the terms of the BSD 3-Clause License.
* See file LICENSE for details.
*/

#include <mfem.hpp>

/// Save `mesh` depending on the `filename` extension.
void save_mesh(mfem::Mesh &mesh, const char *filename);

/// Save `gridfunction` depending on the `filename` extension.
void save_gridfunction(mfem::GridFunction &gridfunction, const char *filename);
