# Hole filling {#hole_filling}

Meshes sometimes contain holes as artifacts from previous modeling or reconstruction steps. PMP provides an algorithm for filling such holes. This algorithm fills holes in the mesh by creating an initial triangulation of the hole and applying subsequent refinement and smoothing steps to the newly created triangles.

\htmlonly
<iframe class="demo" src="/demos/hole_filling.html"></iframe>
\endhtmlonly

The function is pmp::fill_hole().

See \cite liepa_2003_filling for details.

\note This algorithm works on general polygon meshes.