#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <vector>
#include <unordered_set>
#include "word.h"

class Dictionary {
	static constexpr int maxlen{25};
	std::vector<Word> words[maxlen];
public:
	Dictionary();
	bool contains(const std::string& word) const;
	std::vector<std::string> get_suggestions(const std::string& word) const;
private:
	std::unordered_set<std::string> dict;
	void add_trigram_suggestions(std::vector<std::string> suggestions, const string& word) const{
	
}
};

#endif
