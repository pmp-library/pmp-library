# Development {#development}

[TOC]

# Contribution Guidelines {#contribution}

- criteria
- git best practices
- how to submit

# Testing and Code Coverage {#testing}

- how to write tests

# Coding Conventions {#codingConventions}

If you would like to contribute to surface_mesh please make sure
your code adheres to the following coding conventions.

## Naming {#naming}

### Types {#types}

The names of user-defined types such as classes, structs and enums use
`CamelCase` notation. This applies to acronyms as well. The names of persons
such as Cholesky or Delaunay are properly capitalized as well.

    class PolyLine { ... };
    enum RgbColors { red, green, blue };
    class SparseCholeskySolver { ... };

### Functions {#functions}

Function names are written using `camelCase` notation.

    class ExampleClassName
    {
        double exampleFunctionName(void);
    };

### Variables {#variables}

Variables are named in `camelCase` notation. Class member
variables are prefixed with `m_`.

    int globalsConsideredHarmful;

    class ExampleClass
    {
    protected:
        double m_memberVariable;
        static double m_staticVariable;
    };

### File Names {#fileNames}

File names follow the naming rules for user-defined types. Implementation files
end with `.cpp` and header files end with `.h`.

## Formatting {#formatting}

### Blocks {#blocks}

The expressions following an `if, else, while, do ... while` or `for` statement
should always be enclosed in braces. The braces enclosing a block should be
placed in the same column, on separate lines.

    if (fooBar == baz)
    {
        std::cout << "hurz" << std::endl;
    }
    else
    {
        std::cout << "asdf" << std::endl;
    }

### Comments {#comments}

C++-style comments should be used, i.e., `// My important comment.`

### Line Length {#lineLength}

Lines should not exceed more than 80 characters. There should only be one
statement per line.

### Indentation {#indentation}

Use spaces instead of tabs. Indent the code by four spaces for each
level of indentation. Avoid trailing whitespaces at the end of a
line as well as on empty lines.

## Misc {#misc}

This section describes some basic programming conventions developers should
adhere to.

- Use the <tt>\#pragma once</tt> compiler directive at the beginning of each
  header file in order to protect against multiple inclusion.

- Use the `surface_mesh` namespace in order to avoid conflicts. In source files, do not
  add an additional level of indentation due to the namespace:

        namespace surface_mesh {

        class ExampleClass
        {
        ...
        };

        }

- use is and has prefixes for boolean predicates
- consistent property naming
- consistent iterator / circulator naming
- using size_t where approriate
- localized variable scope
- const auto
- clang-format on / off
