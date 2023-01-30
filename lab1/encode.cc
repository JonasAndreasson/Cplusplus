#include "coding.h"
#include <iostream>
#include <fstream>
#include <string>


using std::cout;
using std::cin;
using std::string;
using std::ofstream;
using std::ifstream;
using std::getline;
int main(){
    string filename;
    string line;
    string ctext;
    ifstream myfile;
    ofstream encfile;
    cout << "Enter file name:"<<std::endl;
    cin >> filename;
    myfile.open(filename);
    if (myfile.is_open()){
        while( getline(myfile, line, '\n') ){
            for(auto& c : line) {
            ctext += encode(c);
        }
        }
    }//encode myfile
    myfile.close();
    encfile.open(filename+".enc");
    encfile << ctext;
    encfile.close();
}