#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <sstream>
#include "word.h"
#include "dictionary.h"
#include "process.cc"

using std::string;
using std::vector;
using std::unordered_set;
using std::ifstream;
using std::istringstream;
using std::min;
Dictionary::Dictionary() {
	ifstream in_file;
	string w;
	
	in_file.open("words.txt");
	if (in_file.is_open()){
		while (getline(in_file, w, '\n'))
			{
				string word;
				istringstream ss(w);
				vector<string> strvec;
				int n;
				ss >> word;
				dict.insert(word);
				ss >> n;
				string t;
				while (ss >> t){
					strvec.push_back(t);
				}
				if (word.size()<Dictionary::maxlen){
				Word word_obj(word,strvec);
				words[word.size()].push_back(word_obj); //this is an array of Vector<Word>
				} else {
					std::cout << "Too big!" << "\n";
				}
			}
	}
	in_file.close();
	std::cout << "Initialization complete! \n";
}

bool Dictionary::contains(const string& word) const {
	return dict.find(word)!=dict.end();
}

int Dictionary::edit_distance(const std::string& word, const std::string& suggested){
    int d[Dictionary::maxlen+1][Dictionary::maxlen+1];
    for (unsigned int i = 0; i <= word.size(); i++){
        for (unsigned int j = 0; j <= suggested.size(); j++){
            if (i == 0){
                d[0][j]=j;
            }
            else if (j == 0){
                d[i][0]=i;
            }
            else if (word[i-1] == suggested[j-1]){
                d[i][j] = min(d[i-1][j-1],d[i-1][j]+1);
                d[i][j] = min(d[i][j],d[i][j-1]+1); 
            }else {
                d[i][j] = min(d[i-1][j-1]+1,d[i-1][j]+1);
                d[i][j] = min(d[i][j],d[i][j-1]+1);
            }
        }
    }
    return d[word.size()][suggested.size()];
}

vector<string> Dictionary::get_suggestions(const string& word) const {
	vector<string> suggestions;
	add_trigram_suggestions(suggestions, word);
	rank_suggestions(suggestions, word);
	trim_suggestions(suggestions);
	return suggestions;
}
void Dictionary::add_trigram_suggestions(vector<string> &suggestions, const string& word)const{
	//contain atleast half the trigrams.
	vector<string> trig = trigram(word);
	unsigned int len_tri = int(trig.size()/2);
	if (trig.size()%2!=0){
		len_tri+=1;
	}
	std::sort(trig.begin(), trig.end());
	vector<Word> one_less = words[word.size()-1];
	
	for (Word w : one_less){
		if (w.get_matches(trig)>= len_tri){
			suggestions.push_back(w.get_word());
		}
	}
	vector<Word> same_len = words[word.size()];
	for (Word w : same_len){
		if (w.get_matches(trig)>= len_tri){
			suggestions.push_back(w.get_word());
		}
	}
	vector<Word> one_bigger = words[word.size()+1];
	for (Word w : one_bigger){
		if (w.get_matches(trig)>= len_tri){
			suggestions.push_back(w.get_word());
		}
	}
}

void Dictionary::trim_suggestions(std::vector<std::string> &suggestions)const{
	if (suggestions.size()>5){
		suggestions.resize(5);
	}

}

static bool sort_method(string i, string j, string word){
	return Dictionary::edit_distance(word, i)<Dictionary::edit_distance(word, j);
}

void Dictionary::rank_suggestions(vector<string> &suggestions, const string& word)const{
	using namespace std::placeholders;
	std::sort(suggestions.begin(),suggestions.end(), std::bind(sort_method, _1, _2, word));
}