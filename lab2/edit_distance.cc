#include <iostream>
#include <string>
#include <algorithm>
#include "edit_distance.h"

using std::min;
int edit_distance(const std::string& word, const std::string& suggested){
    int d[25+1][25+1];
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