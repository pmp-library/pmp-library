# Decimation {#decimation}

Mesh decimation reduces the number of elements in a mesh while preserving additional attributes such as surface approximation error or triangle quality. Our implementation performs incremental greedy mesh decimation based on halfedge collapses.

The function is pmp::decimate().

See \cite kobbelt_1998_general and \cite garland_1997_surface for more details.

\note This algorithm only works on triangle meshes.

## Parameters

The decimation function can be controlled by the following parameters:

- `n_vertices`: Control the target number of vertices
- `aspect_ratio`: Threshold for the quality of the triangles in the mesh.
- `edge_length`: Specify a minimum target edge length.
- `max_valence`: Control the maximum number of incident edges per vertex.
- `normal_deviation`: Control the maximum deviation of normals.
- `hausdorff_error`: The maximum deviation from the original surface.
- `seam_threshold`: Threshold for detecting texture seams.
- `seam_angle_deviation`: The maximum texture seam deviation.

Try for yourself:

\htmlonly
<iframe class="demo" src="/demos/decimation.html"></iframe>
\endhtmlonly

## Selections

The decimation algorithm supports selections. You can select a subset of all vertices in the mesh to perform the simplification. This is done using the boolean `v:selected` vertex property.
