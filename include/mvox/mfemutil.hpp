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

#include <mfem.hpp>

/// Save `mesh` depending on the `filename` extension.
void save_mesh(mfem::Mesh &mesh, const char *filename);

/// Save `gridfunction` depending on the `filename` extension.
void save_gridfunction(mfem::GridFunction &gridfunction, const char *filename);
