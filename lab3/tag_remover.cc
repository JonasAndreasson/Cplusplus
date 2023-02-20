#include <iostream>
#include <fstream>
#include <string>
#include "tag_remover.h"
using std::string;
using std::ifstream;
using std::ofstream;
using std::istream;
using std::ostream;

TagRemover::TagRemover(istream& in){
    bool is_tag = false;
    string temp;
    string empty("");
    while (std::getline(in, temp)){
        unsigned long int start_pos = temp.find_first_of('<');
        unsigned long int end_pos = temp.find_first_of('>');
        while (start_pos != string::npos || end_pos != string::npos){
            if (end_pos != string::npos){
                is_tag = false;
            }
            if (end_pos == string::npos){
                end_pos = temp.size();
                is_tag = true;
            }
            if (start_pos == string::npos){
                start_pos = 0;
            }
            
            if (start_pos != string::npos && end_pos != string::npos){
                temp.replace(start_pos, (end_pos+1)-start_pos, empty);
            }
            if (is_tag){
                temp.replace(start_pos, end_pos-start_pos, empty);
            }
            start_pos = temp.find_first_of('<');
            end_pos = temp.find_first_of('>');
        }
        if (is_tag){
            temp.replace(0, temp.size(), empty);
        } else {
            temp += "\n";
        }
        parsed += temp;
    }
    unsigned long int special = parsed.find("&lt;");
    while (special!=string::npos){
        parsed.replace(special, 4, "<");
        special = parsed.find("&lt;");
    }
    special=parsed.find("&gt;");
    while (special!=string::npos){
        parsed.replace(special, 4, ">");
        special=parsed.find("&gt;");
    }
    special = parsed.find("&nbsp;");
    while (special!=string::npos){
        parsed.replace(special, 6, " ");
        special = parsed.find("&nbsp;");
    }
    special =parsed.find("&amp;");
    while (special!=string::npos){
        parsed.replace(special, 5, "&");

        special =parsed.find("&amp;");
    }
    
}

void TagRemover::print(ostream& out){
    out << parsed << std::endl;
}