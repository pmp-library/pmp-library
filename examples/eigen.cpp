// Copyright 2023 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include <pmp/SurfaceMesh.h>
#include <pmp/io/io.h>

#include <Eigen/Dense>

// clang-format off
int main()
{
{
//! [eigen-construction]
// construct form Eigen
Eigen::Vector3d eigen_vector(1.0, 2.0, 3.0);
pmp::dvec3 pmp_vector(eigen_vector);
//! [eigen-construction]
}

{
//! [eigen-assignment]
// assign from Eigen
Eigen::Vector3d eigen_vector(1.0, 2.0, 3.0);
pmp::dvec3 pmp_vector;
// ...
pmp_vector = eigen_vector;
//! [eigen-assignment]
}

{
//! [eigen-cast]
// cast to Eigen
pmp::vec3 pmp_vector(1.0, 2.0, 3.0);
auto eigen_vector = static_cast<Eigen::Vector3f>(pmp_vector);
//! [eigen-cast]
(void)eigen_vector;
}
}
// clang-format on
