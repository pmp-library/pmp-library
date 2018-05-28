# Coding Style {#codingstyle}

This section describes some of the coding standards that are being used in the
pmp-library source tree. Although these standards should never be regarded as
strict requirements they are still important to establish a minimum level of
consistency through the code base as well as to facilitate code comprehension
for developers and library users. Therefore, if you would like to contribute
code to the pmp-library please make sure your code adheres to the conventions
outlined below.

# Naming

## Types

The names of user-defined types such as classes, structs and enums use
`CamelCase` notation. The names of persons such as Cholesky or Delaunay are
properly capitalized as well.

~~~~{.cpp}
    class PolyLine { ... };
    enum RGBColors { red, green, blue };
    class SparseCholeskySolver { ... };
~~~~

## Functions

Function names are written using `camelCase` notation starting with a lowercase
letter.

~~~~{.cpp}
    class ExampleClassName
    {
        double exampleFunctionName(void);
    };
~~~~

## Variables

Variables are named in `camelCase` notation. Class member variables are prefixed
with `m_`.

~~~~{.cpp}
    int globalsConsideredHarmful;

    class ExampleClass
    {
    protected:
        double m_memberVariable;
        static double m_staticVariable;
    };
~~~~

## File Names

File names follow the naming rules for user-defined types. Implementation files
end with `.cpp` and header files end with `.h`.

# Formatting

## Blocks

The expressions following an `if, else, while, do ... while` or `for` statement
should always be enclosed in braces. The braces enclosing a block should be
placed in the same column, on separate lines.

~~~~{.cpp}
    if (fooBar == baz)
    {
        std::cout << "hurz" << std::endl;
    }
    else
    {
        std::cout << "asdf" << std::endl;
    }
~~~~

## Line Length

Lines should not exceed more than 80 characters. There should only be one
statement per line.

## Indentation

Use spaces instead of tabs. Indent the code by four spaces for each
level of indentation. Avoid trailing whitespaces at the end of a
line as well as on empty lines.

# Miscellaneous

This section describes some basic programming conventions developers should
adhere to.

## Comments

Use C++-style comments, i.e., `// my comment.`

### Doxygen Documentation Comments

We use [Doxygen](http://www.doxygen.org/index.html) to generate our API
documentation. All public types and interfaces should be properly
documented. This usually includes a short abstract not longer than a sentence as
well as a more detailed discussion of what the function does. We use `//!` for
doxygen comments. The following is an example what a full documentation comment
could look like:

\verbatim
//! \brief Does foo.
//!
//! \details Does foo and nothing else. If \p useBar argument is true uses the
//! bar method instead of foo.
//!
//! \param[in] useBar toggle to switch method
//! \param[out] results filled with results from foo
//!
//! \returns true on success.
\endverbatim

Another good practice is to document pre- and post-conditions for performing an
operation. Those can be documented using the `\pre` and `\post` Doxygen
[special commands](https://www.stack.nl/~dimitri/doxygen/manual/commands.html).

Use Doxygen comments only in the header file. Do not repeat the same information
in the implementation file. Instead, provide specific details on the
implementation at hand, i.e., how was the functionality implemented, and why was
it implemented in this manner?

## Include Guards

Use the <tt>\#pragma once</tt> compiler directive at the beginning of each
header file in order to protect against multiple inclusion. Although this is not
officially part of the language this feature is supported by all major
compilers and is much more convenient than conventional header guards.

## Namespace

Use the `pmp` namespace in order to avoid conflicts. In source files, do not add
an additional level of indentation due to the namespace:

~~~~{.cpp}
        namespace pmp {

        class ExampleClass
        {
        ...
        };

        }
~~~~

## Boolean Prefixes

Use meaningful prefixes for `bool` variables or functions returning booleans,
e.g., `hasColors()` or `isDone`.

## Naming Consistency

Consistently name dynamic properties, e.g., "v:scalar" for vertex scalars or
"f:normal" for face normals. Similarly, consistently name iterators and
circulators by their entity type (Vertex, Halfedge, ...)

## Type Consistency

Try to avoid conversion issues by using consistent types, e.g., use
`std::size_t` when storing values from STL functions such as the `size()` member
function of a `std::vector`. Use the C++11 `auto` keyword to let the compiler
determine the proper types.

## Structs vs. Classes

Use plain structs for data objectes providing nothing but a collection of other
data types, e.g., a collection of parameters passed to a functions. Such a
struct should not contain any further functionality than what is required for
construction, destruction, or initialization. In contrast to class member
variables, struct members do not have a `m_` prefix.

## Scoping

Localize variable scope and avoid declaring all variables at the beginning of a
function or code block.

# Using clang-format

Please use the [clang-format](https://clang.llvm.org/docs/ClangFormat.html) tool
and the corresponding `.clang-format` configuration file from the repository to
properly format your code. We also provide a convenience CMake target to run
clang-format on all source files:

    $ make clang-format

This requires that the `clang-format` executable is found during CMake
configuration. The exact path to the executable can be specified using

    $ cmake -DCLANG_FORMAT_EXE=<path/to/executable> ..

Look for a line like this

    -- clang-format found: /usr/bin/clang-format

in the CMake configuration output.

In case you want to preserve the special formatting of a particular code block
such as a matrix intialization add the `// clang-format off` and `//
clang-format on` directives around this block:

~~~~{.cpp}
    // clang-format off
    Mat4<Scalar> m;
    m(0, 0) = x[0]; m(0, 1) = x[1]; m(0, 2) = x[2]; m(0, 3) = -dot(x, eye);
    m(1, 0) = y[0]; m(1, 1) = y[1]; m(1, 2) = y[2]; m(1, 3) = -dot(y, eye);
    m(2, 0) = z[0]; m(2, 1) = z[1]; m(2, 2) = z[2]; m(2, 3) = -dot(z, eye);
    m(3, 0) = 0.0;  m(3, 1) = 0.0;  m(3, 2) = 0.0;  m(3, 3) = 1.0;
    // clang-format on
~~~~

# See Also

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
