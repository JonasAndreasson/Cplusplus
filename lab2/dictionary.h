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
	void add_trigram_suggestions(std::vector<std::string> &suggestions, const std::string& word) const;
	void rank_suggestions(std::vector<std::string> &suggestions, const std::string& word)const;
	void trim_suggestions(std::vector<std::string> &suggestions)const;
	static int edit_distance(const std::string& word, const std::string& suggested);
private:
	std::unordered_set<std::string> dict;
	//static bool sort_method(std::string i, std::string j, std::string word);
	};

#endif
