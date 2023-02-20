#include <iostream>
#include <fstream>
#include <string>
#include "tag_remover.h"
int main(){
    std::ifstream htmltest("test.html");
    std::ifstream html_expected("test_expected.html");
    std::string proccessed;
    std::string expected;
    TagRemover tr(htmltest);
    tr.print(std::cout);
}