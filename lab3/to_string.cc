#include <sstream>
#include <string>
using std::istringstream;
using std::ostringstream;
template <typename T>
std::string toString(T& t){
    ostringstream os;
    os << t;
    return os.str();
}