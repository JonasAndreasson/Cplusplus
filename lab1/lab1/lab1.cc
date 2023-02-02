#include <iostream>
#include "Lab1.h"
#include "coding.h"
#include "list.h"



using std::cout;
using std::endl;

/* A small example of a project built using CMake.
 * The Simple_VERSION_* variables are included
 * to show how a header file with configuration macros
 * can be generated from the CMakeLists.txt.
 */
int main()
{
    cout << "This is version " << Simple_VERSION_MAJOR << "." <<
        Simple_VERSION_MINOR << "\n";
    cout << "Hello, world!\n";
    cout << encode('c') << endl;
    List();
}


