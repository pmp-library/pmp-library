# Contributing {#contributing}

Contributions to the pmp-library are generally welcome. However, please keep in
mind that we develop the library besides our daily jobs and therefore might not
always find the time to quickly react to your requests and suggestions.

## Reporting Issues

In case you run into trouble using the pmp-library, please check for
existing [issues](https://github.com/pmp-library/pmp-library/issues). If your
problem is not already reported file a new issue and also provide some piece of
code and data to reproduce the behavior in question.

## Contributing Code

If you would like to contribute to the development of the pmp-library you should
start by [forking](https://help.github.com/articles/fork-a-repo) and creating
a [pull request](https://help.github.com/articles/creating-a-pull-request). Make
sure that your code follows the [Coding Style](codingstyle.html) guidelines. In
case you want to contribute a new algorithm make sure the code is properly
documented using Doxygen and is accompanied by a unit test, see below.

## Developer Certificate of Origin

Before we can integrate your contribution, we need your permission to do so. We
use a mechanism called
[Developer Certificate of Origin (DCO)](https://developercertificate.org/) for
this. The DCO is a legally binding agreement which asserts that:

1. You have the right to submit the contribution
2. You agree to the license used by the project (see [LICENSE.txt](https://github.com/pmp-library/pmp-library/blob/master/LICENSE.txt))

In order to agree to the [terms](https://developercertificate.org/) of the DCO,
you need to "sign off" every commit you wish to contribute by adding a line with
your full name and e-mail address at the end of the commit message:

```text
Signed-off-by: Jane Miller <jane.miller@example.com>
```

**Important**: You must use your real name for signing. We **do not** accept
anonymous contributions or submissions done under a pseudonym.

If you configured your Git `user.name` and `user.email` appropriately, you can
sign your commit automatically by

```sh
git commit -s
```

Alternatively, you may create an alias in your `.gitconfig` to append `-s` to
each invocation of `git commit`.

If you forgot to sign a previous commit, you can also amend the sign-off by running

```sh
git commit --amend -s
```

## Unit Testing

The pmp-library has a suite of unit tests that aims at making sure everything
works as expected. The unit tests are written
using [Google Test](https://github.com/google/googletest) and run during
continuous integration builds. See the also the `tests` sub-directory of the
repository. You can locally run the test suite from your build directory by
invoking the

```sh
make test
```

target. To obtain more detailed test output we recommend to invoke the Google
Test runner directly:

```sh
cd tests
./gtest_runner
```

## Code Coverage

We track the overall code coverage rate of our unit tests using
[gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html), which is part of
[GCC](https://gcc.gnu.org/). Generating the coverage report also requires the
[lcov](http://ltp.sourceforge.net/coverage/lcov.php) package to be installed.

To check the code coverage locally run cmake in `Debug` mode with the
`ENABLE_COVERAGE` flag set to `true`:

```sh
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=true ..
```

Rebuild and run the `coverage` target:

```sh
make && make coverage
```

This will run the test suite and collect the coverage data. A HTML report of the
results will be generated to `coverage/index.html`. We generally try to maintain
a high coverage rate of above 90%. Any code that you would like to contribute
should not decrease the coverage rate.
