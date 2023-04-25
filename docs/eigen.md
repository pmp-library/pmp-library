# Interfacing with Eigen {#eigen}

PMP supports some level of interoperability with Eigen. The pmp::Matrix and pmp::Vector classes can be constructed and assigned from Eigen matrix and vector types. In addition, it possible to cast the pmp::Matrix and pmp::Vector classes to Eigen.

Here is an example for constructing a PMP `dvec3` from an `Eigen::Vector3d`:

\snippet eigen.cpp eigen-construction

Here is an example for assigning a PMP `dvec3` from an `Eigen::Vector3d`:

\snippet eigen.cpp eigen-assignment

Finally, here is an example for casting a PMP `vec3` from an `Eigen::Vector3f`:

\snippet eigen.cpp eigen-cast

See the reference documentation for pmp::Matrix and pmp::Vector for more details.
