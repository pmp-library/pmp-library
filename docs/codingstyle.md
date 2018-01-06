# Coding Style {#codingstyle}

If you would like to contribute to the pmp-library please make sure your source
code adheres to the following coding conventions. Please use
the [clang-format](https://clang.llvm.org/docs/ClangFormat.html) tool and the
corresponding `.clang-format` configuration file from the repository to properly
format your code. We also provide a convenience CMake target to run clang-format
on all source files:

    $ make clang-format

This requires that the `clang-format` executable is found during CMake
configuration. The exact path to the executable can be specified using

    $ cmake -DCLANG_FORMAT_EXE=<path/to/executable> ..

Look for a line like this

    -- clang-format found: /usr/bin/clang-format

in the CMake configuration output.

## Naming

### Types

The names of user-defined types such as classes, structs and enums use
`CamelCase` notation. The names of persons such as Cholesky or Delaunay are
properly capitalized as well.

~~~~{.cpp}
    class PolyLine { ... };
    enum RGBColors { red, green, blue };
    class SparseCholeskySolver { ... };
~~~~

### Functions

Function names are written using `camelCase` notation starting with a lowercase
letter.

~~~~{.cpp}
    class ExampleClassName
    {
        double exampleFunctionName(void);
    };
~~~~

### Variables

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

### File Names

File names follow the naming rules for user-defined types. Implementation files
end with `.cpp` and header files end with `.h`.

## Formatting

### Blocks

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

### Comments

Use C++-style comments, i.e., `// My important comment.` Use `//!` for doxygen
special comments.

### Line Length

Lines should not exceed more than 80 characters. There should only be one
statement per line.

### Indentation

Use spaces instead of tabs. Indent the code by four spaces for each
level of indentation. Avoid trailing whitespaces at the end of a
line as well as on empty lines.

## Miscellaneous

This section describes some basic programming conventions developers should
adhere to.

- Use the <tt>\#pragma once</tt> compiler directive at the beginning of each
  header file in order to protect against multiple inclusion.

- Use the `pmp` namespace in order to avoid conflicts. In source files, do not
  add an additional level of indentation due to the namespace:

~~~~{.cpp}
        namespace pmp {

        class ExampleClass
        {
        ...
        };

        }
~~~~

- Use meaningful prefixes for `bool` variables or functions returning booleans,
  e.g., `hasColors()` or `isDone`.
- Consistently name dynamic properties, e.g., "v:scalar" for vertex scalars or
  "f:normal" for face normals.
- Consistently name iterators and circulators by their type
- Use `std::size_t` where appropriate, e.g., storing values from STL functions
  such as the `size()` member function of a `std::vector`
- Localize variable scope and avoid declaring all variables at the beginning of
  a function or code block.
- In case you want to preserve the special formatting of a particular code block
  such as a matrix intialization add the `// clang-format off` and
  `// clang-format on` directives around this block.
