# Coding Style {#codingstyle}

This section describes some of the coding standards that are being used in the
pmp-library source tree. Although these standards should never be regarded as
strict requirements they are still important to establish a minimum level of
consistency through the code base as well as to facilitate code comprehension
for developers and library users. Therefore, if you would like to contribute
code to the pmp-library please make sure your code adheres to the conventions
outlined below.

## Naming

### Types

The names of user-defined types such as classes, structs and enums use
`CamelCase` notation. The names of persons such as Cholesky or Delaunay are
properly capitalized as well.

```cpp
class SurfaceMesh { ... };
struct Flags { ... };
enum TextureMode { ColdWarmTexture, CheckerboardTexture, OtherTexture };
```

### Functions

Function names are written using `snake_case`. All characters are
lowercase. Separate words by underscores.

```cpp
class MeshViewer
{
    bool load_mesh(const char* filename);
};
```

### Variables

Variable names use `snake_case` notation. All characters are lowercase. Separate
words with underscores. Class member variables have an underscore `_` suffix.

```cpp
int global_var;

class ExampleClass
{
protected:
    double member_variable_;
    static double static_member_;
};
```

_Exception:_ Public members of a `struct` holding just a group of variables may
omit the underscore suffix:

```cpp
struct NearestNeighbor
{
    Scalar dist;
    SurfaceMesh::Face face;
    Point nearest;
    int tests;
};
```

_Exception:_ For the sake of similarity with common mathematical notation, we
sometimes use uppercase letters, e.g., to denote matrices when solving a linear
system:

```cpp
Eigen::SparseMatrix<double> A(n, n);
Eigen::MatrixXd B(n, 3);
```

### File Names

File names follow the naming rules for user-defined types, i.e., they use
`CamelCase` naming. Implementation files end with `.cpp` and header files end
with `.h`.

_Exception_: Files for building an executable use an all lowercase name, e.g.,
`mpview.cpp` to build the `mpview` executable.

## Formatting

### Blocks

The expressions following an `if, else, while, do ... while` or `for` statement
should always be enclosed in braces. The braces enclosing a block should be
placed in the same column, on separate lines.

```cpp
if (foo == bar)
{
    std::cout << "baz" << std::endl;
}
else
{
    std::cout << "barbaz" << std::endl;
}
```

### Line Length

Lines should not exceed more than 80 characters. There should only be one
statement per line.

### Indentation

Use spaces instead of tabs. Indent the code by four spaces for each level of
indentation. Avoid trailing whitespace at the end of a line as well as on empty
lines.

## Miscellaneous

This section describes some basic programming conventions developers should
adhere to.

### Declaration Order

Group the sections of a class in the following order: public, protected,
private.

Use one section for each type of access specifier.

Omit empty sections.

Within a section use the following order:

1. typedefs and enums
2. constants
3. constructors and destructor
4. operators
5. methods
6. static methods
7. data members
8. static data members

### Comments

Use C++-style comments, i.e., `// my comment.`

### Doxygen Documentation Comments

We use [Doxygen](http://www.doxygen.org/index.html) to generate our API
documentation. All public types and interfaces should be properly documented.
This usually includes a short abstract not longer than a sentence as well as a
more detailed discussion of what the function does. We use `//!` for Doxygen
comments. The following is an example what a full documentation comment could
look like:

\verbatim
//! \brief Does foo.
//!
//! \details Does foo and nothing else. If \p use_bar argument is true uses the
//! bar method instead of foo.
//!
//! \param[in] use_bar toggle to switch method
//! \param[out] results filled with results from foo
//!
//! \return true on success.
\endverbatim

Use `\p` to reference (member) function parameters.

Another good practice is to document pre- and post-conditions for performing an
operation. Those can be documented using the `\pre` and `\post` Doxygen
[special commands](https://www.stack.nl/~dimitri/doxygen/manual/commands.html).

Use Doxygen comments only in the header file. Do not repeat the same information
in the implementation file. Instead, provide specific details on the
implementation at hand, i.e., how was the functionality implemented, and why was
it implemented in this manner?

### Include Guards

Use the

```cpp
#pragma once
```

compiler directive at the beginning of each header file in order to protect
against multiple inclusion. Although this is not officially part of the language
this feature is supported by all major compilers and is much more convenient
than conventional header guards.

### Header Include Order

Use the following order to include header files:

1. Related header
2. C standard library headers
3. C++ standard library headers
4. Other library headers
5. Project library headers

Separate each group by a blank line. Optional: Sort headers alphabetically
within a group.

### Include Style

Use quotes to include other project headers. Use the full relative path from the
project `src` directory. Example:

```cpp
#include "pmp/algorithms/SurfaceRemeshing.h"
```

### Namespace

Use the `pmp` namespace in order to avoid conflicts. In source files, do not add
an additional level of indentation due to the namespace:

```cpp
namespace pmp {
class ExampleClass
{
...
};
}
```

### Boolean Prefixes

Use meaningful prefixes for `bool` variables or functions returning booleans,
e.g., `has_colors()` for a function or `is_done` for a variable.

### Naming Consistency

Consistently name dynamic properties, e.g., `v:scalar` for vertex scalars or
`f:normal` for face normals. Similarly, consistently name iterators and
circulators by their entity type (`Vertex`, `Halfedge`, ...)

### Type Consistency

Try to avoid conversion issues by using consistent types, e.g., use
`std::size_t` when storing values from STL functions such as the `size()` member
function of a `std::vector`. Use the C++11 `auto` keyword to let the compiler
determine the proper types.

### Structs vs. Classes

Use plain structs for data objects providing nothing but a collection of other
data types, e.g., a collection of parameters passed to a functions. Such a
struct should not contain any further functionality than what is required for
construction, destruction, or initialization. In contrast to class member
variables, struct members do not have a underscore `_` suffix.

### Scoping

Localize variable scope and avoid declaring all variables at the beginning of a
function or code block.

### Prefer C++ over C

Give preference to C++ and STL constructs over C-style ones. Example: Use
`std::numeric_limits<float>\:\:max()` instead of `FLT_MAX`.

## Using clang-format

Please use the [clang-format](https://clang.llvm.org/docs/ClangFormat.html) tool
and the corresponding `.clang-format` configuration file from the repository to
properly format your code. We also provide a convenience CMake target to run
clang-format on all source files:

```shell
    make clang-format
```

This requires that the `clang-format` executable is found during CMake
configuration. The exact path to the executable can be specified using

```shell
    cmake -DCLANG_FORMAT_EXE=<path/to/executable> ..
```

In case you want to preserve the special formatting of a particular code block
such as a matrix intialization add the `// clang-format off` and `//
clang-format on` directives around this block:

```cpp
// clang-format off
Mat4<Scalar> m;
m(0, 0) = x[0]; m(0, 1) = x[1]; m(0, 2) = x[2]; m(0, 3) = -dot(x, eye);
m(1, 0) = y[0]; m(1, 1) = y[1]; m(1, 2) = y[2]; m(1, 3) = -dot(y, eye);
m(2, 0) = z[0]; m(2, 1) = z[1]; m(2, 2) = z[2]; m(2, 3) = -dot(z, eye);
m(3, 0) = 0.0;  m(3, 1) = 0.0;  m(3, 2) = 0.0;  m(3, 3) = 1.0;
// clang-format on
```

## See Also

The above coding standard is necessarily incomplete. Therefore, we list some
additional references and pointers to useful resources regarding C++ coding
standards and best practices:

* The [Effective C++ series](https://www.aristeia.com/books.html) of Scott Meyer
  contains a whealth of guidelines \cite meyers_2005_effective
  \cite meyers_2014_effective .
* [C++ Coding Standards](http://www.gotw.ca/publications/c++cs.htm) written by
  Herb Sutter and Andrei Alexandrescu describes concrete guidelines and best
  practices \cite sutter_2004_c++ .
* The [C++ Core Guidelines](https://github.com/isocpp/cppcoreguidelines) are an
  ongoing effort led by Bjarne Stroustrup to develop guidelines for effecitve
  modern C++ usage.
* Other coding standards:
  * [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html)
  * [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
  * [High Integrity C++ Coding Standard](http://www.codingstandard.com/section/index/)
