/*
* Copyright (c) 2020-2021, Benjamin F. Zwick. All Rights reserved.
*
* This file is part of MVox.
*
* MVox is free software: you can redistribute it and/or modify it
* under the terms of the BSD 3-Clause License.
* See file LICENSE for details.
*/

#include "mvox/fileutil.hpp"

#include <cstring>

const char *file_ext(const char *filename)
{
   const char *dot = strrchr(filename, '.');
   if (!dot || dot == filename)
   {
      return "";
   }
   else
   {
      return dot + 1;
   }
}
