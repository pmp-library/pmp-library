# Smoothing {#smoothing}

Mesh smoothing effectively reduces noise in the input surface. Especially meshes coming from 3D scanning and reconstruction typically contain noise.

PMP provides two different methods for smoothing, explicit and implicit Laplacian smoothing.

Try for yourself:

\htmlonly
<iframe class="demo" src="/demos/smoothing.html"></iframe>
\endhtmlonly

## Explicit Laplacian smoothing

Explicit Laplacian can be controlled by the following parameters:

- `iterations` The number of iterations performed.
- `use_uniform_laplace` Use uniform or cotan Laplacian. Default: cotan.

See also pmp::explicit_smoothing().

## Implicit Laplacian smoothing

- `timestep` The time step taken.
- `iterations` The number of iterations performed.
- `use_uniform_laplace` Use uniform or cotan Laplacian.
- `rescale` Re-center and re-scale model after smoothing.

See pmp::implicit_smoothing().
