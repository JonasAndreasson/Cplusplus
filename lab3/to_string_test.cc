#include <sstream>
#include <string>
#include <stdexcept>
#include "to_string.cc"
#include "type_casting.cc"
#include "date.h"

using std::cout;
int main(){
    double d = 1.234;
    Date today;
    cout << toString(d) <<"\n";
    cout << toString(today) << std::endl;
    try {
    int i = string_cast<int>("123");
    Date date = string_cast<Date>("2015-1-10");
    cout << i << "\n";
    cout << date << std::endl;
    } catch (std::invalid_argument& e){
        cout << "Error: " << e.what() << std::endl;
    }

}