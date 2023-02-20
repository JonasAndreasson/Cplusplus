#include <stdexcept>
#include <string>
#include <sstream>
template <class T>
T string_cast(std::string s){
    T t;
    std::istringstream is(s);
    is >> t;
    if (is.fail()){
        throw std::invalid_argument(s + " is not valid");
    }
    return t;
}
