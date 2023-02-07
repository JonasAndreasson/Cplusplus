#include <string>
#include <vector>
#include "word.h"

using std::vector;
using std::string;

Word::Word(const string& w, const vector<string>& t) {
	word = w;
	trigrams = t;
}

string Word::get_word() const {
	return word;
}

unsigned int Word::get_matches(const vector<string>& t) const {
	long unsigned int sum(0);
	unsigned int max_index = trigrams.size()-1;
	char last_tri_fc = trigrams[max_index][0];
	unsigned int current_tri_index = 0;
	//ell hel llo the = thello
	for (string s : t){
		if (s[0]>last_tri_fc){ //If it's later in the alphabet than
			return sum;		//our last one we can assume no ohter matches
		}
		while (current_tri_index<=max_index){ //<=4?
			if( s == trigrams[current_tri_index]){
				sum+=1;
				break;
			}
			else if (s[0]>trigrams[current_tri_index][0]){
				current_tri_index+=1;
			}
			else if (s[0]<trigrams[current_tri_index][0]){
				break;
			}
			else {
				break;
			}
		}
	}
	return sum;
}
