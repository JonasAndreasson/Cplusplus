#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include "word.h"
#include "dictionary.h"

using std::string;
using std::vector;
using std::unordered_set;
using std::ifstream;

Dictionary::Dictionary() {
	ifstream in_file;
	string w;
	string delim = "";
	unordered_set<Word> dict;
	in_file.open("words.txt");
	if (in_file.is_open()){
		while (getline(in_file, w, '\n'))
			{
				string s = w.substr(0, w.find(delim));
				Word word_obj(s, {})
			}
	}
}

bool Dictionary::contains(const string& word) const {
	return true;
}

vector<string> Dictionary::get_suggestions(const string& word) const {
	vector<string> suggestions;
	return suggestions;
}
