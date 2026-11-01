
#include <pmp/predicates.h>

extern "C" {
#include <predicates/predicates.h>
}

namespace pmp {

namespace detail {

// ensure that predicates are initialized before use
inline void ensure_predicates_initialized()
{
    // function-local static variable to ensure thread-safe one time initialization
    static const bool initialized = []() {
        exactinit();
        return true;
    }();

    (void)initialized;
}
} // namespace detail

Orientation orient3d(const dvec3& a, const dvec3& b, const dvec3& c,
                     const dvec3& d)
{
    detail::ensure_predicates_initialized();

    const double det = ::orient3d(a.data(), b.data(), c.data(), d.data());

    if (det > 0.0)
        return Orientation::Positive;

    if (det < 0.0)
        return Orientation::Negative;

    return Orientation::Coplanar;
}

} // namespace pmp
