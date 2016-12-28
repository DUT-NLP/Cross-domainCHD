#include "tokenize.h"

vector<string> tokenize(string str, string sep_str/* =" " */)
{
	vector<string> return_value;
	if(str=="") return return_value;

	string fisrt_str;
	do 
	{
		fisrt_str=token_getfirst(str, sep_str);
		return_value.push_back(fisrt_str);
	} while (str!="");
	return return_value;
}

string token_getfirst(string& s_string, string sep_str)
{
	string return_value;
	unsigned int i=s_string.find(sep_str);
	if(string::npos==i)
	{
		return_value=s_string;
		s_string="";
	}
	else
	{
		return_value=s_string.substr(0,i);
		s_string=s_string.substr(i+sep_str.length());
	}
	return return_value;
}