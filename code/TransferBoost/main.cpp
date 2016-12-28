#include "PTSVM.h"

int main(int argc, char* argv[])
{
	PTSVM trasvm=PTSVM();	
	trasvm.input(argv[1],argv[2],argv[3],2,101,20);
//	trasvm.input("F:\\temp\\abs1\\1\\temp2_svm","F:\\temp\\abs1\\1\\temp3_svm","F:\\temp\\abs1\\1",2,101,20);
	trasvm.learn();
//	trasvm.test();


//for(int i = 0;i<4;i++)
//{
//	string ss="F:\\a";
//	ss=ss+to_string1(i+1)+".txt";
//		cout<<ss<<endl;
//	ifstream s((ss+to_string1(i+1)+".txt").c_str());
//	ofstream o((ss+to_string1(i+1)+"aa.txt").c_str());
//	s.close();
//	o.close();
//}

	return 0;
}