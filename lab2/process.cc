#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>


using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::ifstream;
using std::ofstream;
using std::vector;

vector<string> trigram(string s);

int main() {
    ifstream in_file;
    string word;
    ofstream out_file;
    in_file.open("words");
    out_file.open("words.txt");
    if (in_file.is_open()){
        while ( getline(in_file, word, '\n') ){
            if (word.size()>=3){
                vector<string> trigrams = trigram(word);
                std::sort(trigrams.begin(), trigrams.end());
                out_file << word <<" "<< trigrams.size();
                for (string w : trigrams){
                    out_file << " ";
                    out_file << w;
                }
                out_file << "\n";
            }
        }
    }
    out_file.close();
    in_file.close();
}

vector<string> trigram(string word){
    vector<string> v = {};
    for (int i = 1; i<word.size()-1; i++){
        char temp[3];
        temp[0] = tolower(word[i-1]);
        temp[1] = tolower(word[i]);
        temp[2] = tolower(word[i+1]);
        v.push_back(string(temp));
    }
    return v;
}