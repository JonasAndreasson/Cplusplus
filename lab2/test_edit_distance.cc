/* A sketch of unit tests for an edit_distance function.
 *
 * The function is assumed to be
 *    int edit_distance(const std::string&, const std::string&);
 * and it is assumed to be declared in edit_distance.h
 */
#include "edit_distance.h"

#include <iostream>

bool do_test(const std::string& x, const std::string& y, int expected)
{
    auto actual = edit_distance(x, y);
    if (actual != expected) {
        std::cout << "*** WRONG: distance(" << x << ", " << y << ") was "
                  << actual << ", expected " << expected << '\n';
        return true;
    }
    return false;
}

int main()
{
    int res = do_test("foobar", "foobar", 0);
    res += do_test("x", "x", 0);
    res += do_test("baz", "bar", 1);
    res += do_test("separate", "seperate", 1);
    res += do_test("separate", "seprate", 1);
    res += do_test("separate", "sepeate", 2);
    res += do_test("separate", "desperate", 3);
    res += do_test("separate", "federate", 3);
    res += do_test("separate", "generate", 3);
    res += do_test("color", "colour", 1);
    res += do_test("color", "colr", 1);
    res += do_test("volor", "colour", 2);

    if (!res)
        std::cout << "\nPassed\n";
    else
        std::cout << "\nFailed " << res << " tests\n";
}
