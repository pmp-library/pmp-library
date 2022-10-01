// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/Types.h"

template <typename T>
void tfread(FILE* in, const T& t)
{
    size_t n_items = fread((char*)&t, 1, sizeof(t), in);
    PMP_ASSERT(n_items > 0);
}

template <typename T>
void tfwrite(FILE* out, const T& t)
{
    size_t n_items = fwrite((char*)&t, 1, sizeof(t), out);
    PMP_ASSERT(n_items > 0);
}
