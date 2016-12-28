#ifndef _M_TOKENLIZE_H_
#define  _M_TOKENLIZE_H_
#include <string>
#include <vector>
using namespace std;

string token_getfirst(string& s_string, string sep_str);
vector<string> tokenize(string str, string sep_str=" ");

#endif 

