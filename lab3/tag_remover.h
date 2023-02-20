#ifndef TAG_REMOVER_H
#define TAG_REMOVER_H

#include <iostream>
#include <fstream>
#include <string>
class TagRemover {
    public:
    TagRemover(std::istream& in);
    void print(std::ostream& out);
    private:
    std::string parsed;
};
#endif