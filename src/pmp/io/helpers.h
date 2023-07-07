// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/types.h"

template <typename T>
void tfread(FILE* in, const T& t)
{
    [[maybe_unused]] auto n_items = fread((char*)&t, 1, sizeof(t), in);
}

template <typename T>
void tfwrite(FILE* out, const T& t)
{
    [[maybe_unused]] auto n_items = fwrite((char*)&t, 1, sizeof(t), out);
}
