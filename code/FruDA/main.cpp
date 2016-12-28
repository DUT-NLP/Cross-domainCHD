#include "tokenize.h"
#include <iostream>
#include <fstream>
using namespace std;

void findmaxID(string filepath,int &maxF); 


int main(int argc, char* argv[])
{

/*	
	string sourcepath="D:\\a";
	string targettrainpath="D:\\b";
	string targettestpath="D:\\c";
*/

	string sourcepath=argv[1];
	string targettrainpath=argv[2];
	string targettestpath=argv[3];


	string uu="";
	if(argc>4) uu=argv[4];

	string resultpath="temp.result";
	
	//统计向量数，最大的特征id
	int maxF=0;
	findmaxID(sourcepath,maxF);
	findmaxID(targettrainpath,maxF);
	findmaxID(targettestpath,maxF);
	
	//特征变换
	string line;
	//生成trainfile
	ofstream trainfile("temp.train");
	//souce
	ifstream sourcefile(sourcepath.c_str());
	while(getline(sourcefile, line))
	{
		if(line=="") continue;
		trainfile<<line;
		vector<string> feaV=tokenize(line);
		for(unsigned int i =1; i<feaV.size(); i++)
		{
			trainfile<<" ";
			int feaID=atoi(tokenize(feaV[i], ":")[0].c_str());
			feaID+=maxF;
			trainfile<<feaID<<":1";
		}
		trainfile<<endl;
	}
	sourcefile.close();
	//targettrainfile
	ifstream targettrainfile(targettrainpath.c_str());
	while(getline(targettrainfile, line))
	{
		if(line=="") continue;
		trainfile<<line;
		vector<string> feaV=tokenize(line);
		for(unsigned int i =1; i<feaV.size(); i++)
		{
			trainfile<<" ";
			int feaID=atoi(tokenize(feaV[i], ":")[0].c_str());
			feaID+=2*maxF;
			trainfile<<feaID<<":1";
		}
		trainfile<<endl;
	}
	targettrainfile.close();

	//targettestfile
	if(uu=="u")
	{
		ifstream targettestfilet(targettestpath.c_str());
		while(getline(targettestfilet,line))
		{
			if(line=="") continue;
			trainfile<<"0";
			vector<string> feaV=tokenize(line);
			for(unsigned int i =1; i<feaV.size(); i++)
			{
				trainfile<<" ";
				int feaID=atoi(tokenize(feaV[i], ":")[0].c_str());
				feaID+=maxF;
				trainfile<<feaID<<":1";
			}
			for(unsigned int i =1; i<feaV.size(); i++)
			{
				trainfile<<" ";
				int feaID=atoi(tokenize(feaV[i], ":")[0].c_str());
				feaID+=2*maxF;
				trainfile<<feaID<<":-1";
			}
			trainfile<<endl;
		}
		targettestfilet.close();
	}
	
	trainfile.close();

	//生成testfile
	ofstream testfile("temp.test");
	ifstream targettestfile(targettestpath.c_str());
	while(getline(targettestfile,line))
	{
		if(line=="") continue;
		testfile<<line;
		vector<string> feaV=tokenize(line);
		for(unsigned int i =1; i<feaV.size(); i++)
		{
			testfile<<" ";
			int feaID=atoi(tokenize(feaV[i], ":")[0].c_str());
			feaID+=2*maxF;
			testfile<<feaID<<":1";
		}
		testfile<<endl;
	}
	targettestfile.close();
	testfile.close();
	
	//SVM
	string	cmdstr="svm_learn -t 1 -d 2 -c 1 temp.train temp.model";
	system(cmdstr.c_str());

	//测试
	cmdstr="svm_classify temp.test temp.model "+resultpath;
	system(cmdstr.c_str());
	//system("PAUSE");
	return 0;
}

void findmaxID(string filepath,int &maxF)
{
	ifstream infile(filepath.c_str());
	string line;
	while (getline(infile, line))
	{
		if(line=="") continue;
		vector<string> feav=tokenize(line," ");
		unsigned int e=feav.size()-1;
		string fea=feav[e];
		int feaID=atoi(tokenize(fea, ":")[0].c_str());
		if (feaID>maxF)
		{
			maxF=feaID;
		}
	}
	infile.close();
}
