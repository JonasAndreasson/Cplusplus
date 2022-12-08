/* An example program that prints text to both standard out and standard error.
 */
#include <iostream>

int main()
{
    std::cout << "This text is written to stdout\n";
    std::cerr << "And this is written to stderr\n";
    std::cout << "More text to stdout\n";
    std::cerr << "And some more to stderr\n";
}
