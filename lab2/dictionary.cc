#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <sstream>
#include "word.h"
#include "dictionary.h"

using std::string;
using std::vector;
using std::unordered_set;
using std::ifstream;
using std::istringstream;
Dictionary::Dictionary() {
	ifstream in_file;
	string w;
	string delim = " ";
	
	in_file.open("words.txt");
	if (in_file.is_open()){
		while (getline(in_file, w, '\n'))
			{
				string s = w.substr(0, w.find(delim));
				string partial = w.substr(w.find(delim)+1, w.size());
				// partial 4 trigrams....
				string word = partial.substr(partial.find(delim)+1, partial.size());
				dict.insert(s);
				istringstream ss(word);
				string line;
				vector<string> strvec;
				while (getline(ss, line, ' ') ) {
					strvec.push_back(line);
					//std::cout<<line<<'\n';
				}
				vector<Word> wordvec = words[s.size()-1]; //this is an array of Vector<Word>
				Word word_obj(s,strvec);
				wordvec.push_back(word_obj);
			}
	}
	in_file.close();
}

bool Dictionary::contains(const string& word) const {
	return dict.find(word)!=dict.end();
}

vector<string> Dictionary::get_suggestions(const string& word) const {
	vector<string> suggestions;
	return suggestions;
}
void Dictionary::add_trigram_suggestions(vector<string> suggestions, const string& word){
	
}
