#include "PTSVM.h"
//将int转化成string
string to_string(int n)
{
  int m=n;
  int i=0,j=0;
  char s[3];
  char ss[3];
  while(m>0)
  {
    s[i++]= m%10 + '0';
    m/=10;
  }
  s[i]='\0';
  i=i-1;
  while(i>=0)
  {
    ss[j++]=s[i--];
  }
  ss[j]='\0';
  return ss;
}
//测试时，测试语料不用加权重
//des指Dl和Dul
PTSVM::PTSVM()
{
	train_test_data_file="train_test_data_file.test";
	test_test_data_file="test_test_data_file.test";
	des_test_data_file="des_test_data_file.test";
	semi_test_data_file="semi_test_data_file.test";

	whole_train_data_file="whole_train_data_file.train";	
	des_train_data_file="des_train_data_file.train";

	whole_model_file="whole_model_file.model";
	des_model_file="des_model_file.model";

	des_result_by_whole_file="des_result_by_whole_file.result";
	des_result_by_des_file="des_result_by_des_file.result";
	test_result_by_whole_file="test_result_by_whole_file.result";
	test_result_by_des_file="test_result_by_des_file.result";		
}

PTSVM::~PTSVM()
{	
	delete[] altnegHeap;
	delete[] altposHeap;
	delete[] source_file_path_input;
	delete[] source_data_num;
	delete[] Beita;
	delete[] source_test_data_file;
	delete[] one_source_des_train_data_file;
	delete[] source_result_by_whole_file;
	delete[] one_source_des_model_file;
	delete[] des_result_by_one_source_des_file;
	delete[] des_err_by_one_source_des;
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~析构函数运行成功~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}



void PTSVM::input(string train_file_path_input, string test_file_path_input, 
								string svm_pre_tenstep_path, int source_num, int iter_N/* =50 */,
								int semi_add_M/* =20 */, double weight_test_max/* =1.0 */,
								string tempDir/* ="F:\\PTSVMtemp1" */)
{	
	this->train_file_path_input=train_file_path_input;
	this->test_file_path_input=test_file_path_input;
	this->iter_N=iter_N;
	this->semi_add_M=semi_add_M;
	this->weight_test_max=weight_test_max;
	this->tempDir=tempDir;
	this->svm_pre_tenstep_path=svm_pre_tenstep_path;
	this->source_num=source_num;
	
	altnegHeap= new AltNode[semi_add_M/2+1]; //存放选出来的semi_add_M/2个负例的堆
	altposHeap= new AltNode[semi_add_M/2+1]; //存放选出来的semi_add_M/2个正例的堆
	source_file_path_input= new string[source_num];
	source_data_num= new int[source_num];
	Beita= new double[source_num];
	source_test_data_file= new string[source_num];
	source_result_by_whole_file= new string[source_num];
	one_source_des_train_data_file= new string[source_num];
	one_source_des_model_file= new string[source_num];
	des_result_by_one_source_des_file= new string[source_num];
	des_err_by_one_source_des= new double[source_num];

	string beg = "\\temp1";
	string end = "_svm";
	string temp;
	int i;
	for(i=0;i<source_num;i++)
	{
		temp = svm_pre_tenstep_path + beg + to_string(i+1) + end;
		source_file_path_input[i]=temp;
		temp = "";
	}	

	beg = "source_test_data_file";
	end = ".test";
	for(i=0;i<source_num;i++)
	{
		temp = beg + to_string(i+1) + end;
		source_test_data_file[i]=temp;
		temp = "";
	}	

	beg = "source_result_by_whole_file";
	end = ".result";
	for(i=0;i<source_num;i++)
	{
		temp = beg + to_string(i+1) + end;
		source_result_by_whole_file[i]=temp;
		temp = "";
	}

	beg = "one_source_des_train_data_file";
	end = ".train";
	for(i=0;i<source_num;i++)
	{
		temp = beg + to_string(i+1) + end;
		one_source_des_train_data_file[i]=temp;
		temp = "";
	}

	beg = "one_source_des_model_file";
	end = ".model";
	for(i=0;i<source_num;i++)
	{
		temp = beg + to_string(i+1) + end;
		one_source_des_model_file[i]=temp;
		temp = "";
	}

	beg = "des_result_by_one_source_des_file";
	end = ".result";
	for(i=0;i<source_num;i++)
	{
		temp = beg + to_string(i+1) + end;
		des_result_by_one_source_des_file[i]=temp;
		temp = "";
	}


	//临时目录存在 ，返回
	if( 0==_access(tempDir.c_str() , 0 ) )
	{
		return;
	}
	//不存在则创建，如果创建失败则输出提示并强制退出
	if( -1 == _mkdir(tempDir.c_str()) )
	{
		cout<<"创建临时文件失败"<<endl;
		exit(1);
	}
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~input函数运行成功~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
	return;
}

void PTSVM::learn()
{
	init();  
	for (int curT=1; curT<=iter_N; curT++)
	{
		cout<<endl;
		cout<<endl;
		cout<<curT<<endl;
		iter(curT);
		cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~完成第"<<curT<<"次迭代~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
	}
	return;
}

void PTSVM::test()
{
	vector<int> Stand;
	ifstream DtestIn(filename2path(test_test_data_file).c_str());
	int label;
	string line;
	while(DtestIn>>label)
	{
		getline(DtestIn,line);
		Stand.push_back(label);
	}
	DtestIn.close();

	for (int n=0;n<iter_N;n++)
	{
		int recallright=0;
		int unrecall=0;
		int recallerr=0;

		for (int i=0;i<test_data_num;i++)
		{
			int standlabel=Stand[i];
			int reslabel=-1;
			int start=(n+1)/2;
			double left=0;
			double right=0;
			for(int j=start;j<=n;j++)
			{
				left+=fresleft[i][j];
				right+=fresright[i][j];
			}
			if(left>=right)
			{
				reslabel=1;
			}
			if(reslabel==1 && standlabel==1)	recallright++;
			if(reslabel==1 && standlabel==-1) recallerr++;
			if(reslabel==-1&& standlabel==1) unrecall++;
		}
		double p=(double) recallright/(recallright+recallerr);
		double r=(double)recallright/(recallright+unrecall);
		double f=p*r*2/(p+r);
		double a=(double) (test_data_num-unrecall-recallerr)/test_data_num;
		cout<<endl;
		cout<<endl;
		cout<<n<<endl;
		cout<<"A:"<<a<<"	P:"<<p<<"		R:"<<r<<"			F:"<<f<<endl;
	}
	return;
}

void PTSVM::init()
{	
	string line;
	int label;

	//拷贝辅助训练数据
	vector<int> temp;
	for(int i = 0; i < source_num; i++)
	{
		source_data_num[i]=0;  //辅助训练数据个数初始赋值为0
		ifstream source_data_instream(source_file_path_input[i].c_str()); //存放辅助训练数据文件
		ofstream source_data_outstream(filename2path(source_test_data_file[i]).c_str()); //存放辅助训练数据临时文件
//		cout<<filename2path(source_test_data_file[i]).c_str()<<endl;/////////////////////
		while (source_data_instream>>label)  //取出标签放到label中
		{
			getline(source_data_instream , line); //读取一行数据
			source_data_num[i]++;                   //计算辅助训练数据个数
			temp.push_back(label);
//			source_data_label.push_back(label);
			source_data_outstream<<label2string(label)<<line<<endl;  //存放辅助训练数据临时文件
		}
		source_data_label.push_back(temp);
		source_data_instream.close();
		source_data_outstream.close();
		temp.clear();
	}	

	//拷贝训练数据
	train_data_num=0;
	ifstream train_data_instream(train_file_path_input.c_str());
	ofstream train_data_outstream(filename2path(train_test_data_file).c_str());
	while (train_data_instream>>label)
	{
		getline(train_data_instream,line);
		train_data_num++;		
		train_data_label.push_back(label);
		train_data_outstream<<label2string(label)<<line<<endl;
	}
	train_data_instream.close();
	train_data_outstream.close();

	//拷贝测试数据
	test_data_num=0;
	ifstream test_data_instream(test_file_path_input.c_str());
	ofstream test_data_outstream(filename2path(test_test_data_file).c_str());
	while (getline(test_data_instream,line))
	{
		if (line=="")
		{
			continue;
		}
		test_data_num++;
		test_data_outstream<<line<<endl;
	}
	test_data_instream.close();
	test_data_outstream.close();

	// 删除之前的结果数据
	if( 0==_access(filename2path(test_result_by_whole_file).c_str() , 0 ) )
	{
		DeleteFileA(filename2path(test_result_by_whole_file).c_str());
	}

	//初始化权重
	vector<double> temp1;
	for(int i = 0; i < source_num; i++)
	{
		for (int j=0; j<source_data_num[i]; j++)
		{
			temp1.push_back(1.0);
		}
		weight_source.push_back(temp1);
		temp1.clear();
	}
	for(int i=0; i<train_data_num; i++)
	{
		weight_train.push_back(1.0);
	}

	//计算β
	for(int i = 0; i < source_num; i++)
	{
		Beita[i]=1.0 / ( 1.0+sqrt( 2 * log((double) source_data_num[i]) / iter_N ) );

	}	

	//===================
	vector<double> vtemp;
	for (int i=0;i<iter_N;i++)
	{
		vtemp.push_back(0);
	}
	for (int i=0;i<test_data_num;i++)
	{
		fresleft.push_back(vtemp);      //最终结果存放
		fresright.push_back(vtemp);
	}
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~init函数运行成功~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
	return;
}

void PTSVM::iter(int curT)
{
	cropus();
	callSVM(curT);
	errCalculate();
	reWeightSource();
	reWeightTrain();
	adjustTestData();
	reWeightTest(curT);
}

void PTSVM::cropus()
{
	testCropus();
	trainCropus();
}

void PTSVM::testCropus()
{
	ifstream test_result_instream(filename2path(test_result_by_whole_file).c_str());
	if(!test_result_instream) //如果test_result_by_whole_file.result文件不存在，表明是第一次迭代（因为在init函数中删除了），则返回
	{
		cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~第一次迭代~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
		return;
	}
	ifstream test_test_instream(filename2path(test_test_data_file).c_str());
	ofstream semi_test_outstream(filename2path(semi_test_data_file).c_str());

	int add_err=0;
	int index=0;
	string line;
	int label;

	while (test_test_instream>>label)
	{
		getline(test_test_instream,line);
		map<int,int>::iterator map_ite=test_data_indexs.find(index++);
		if(test_data_indexs.end()==map_ite)
		{
			continue;
		}
		if(map_ite->second != label)
		{
			add_err++;
		}
		semi_test_outstream<<label2string(map_ite->second)<<line<<endl;
	}

	cout<<"EEEEEEEEEE"<<add_err<<"/"<<test_data_indexs.size()<<endl;
	test_result_instream.close();
	test_test_instream.close();
	semi_test_outstream.close();
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~testCropus函数运行成功~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
	return;	

}

void PTSVM::trainCropus()
{
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~begin train~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
	ofstream whole_train_out_stream(filename2path(whole_train_data_file).c_str());
	ofstream des_train_out_stream(filename2path(des_train_data_file).c_str());
	ofstream des_test_out_stream(filename2path(des_test_data_file).c_str());

	whole_train_out_stream.precision(8);
	whole_train_out_stream.setf(ios::fixed);
	des_train_out_stream.precision(8);
	des_train_out_stream.setf(ios::fixed);

	int index=0;
	int label;
	string line;
	//将加了权重的辅助训练数据加入whole_train_data_file.train整个训练文件中,并准备one_source_des_train_data_file
	for(int i = 0; i < source_num; i++)
	{
		index = 0;
		ifstream source_test_instream(filename2path(source_test_data_file[i]).c_str());
		ofstream one_source_des_train_out_stream(filename2path(one_source_des_train_data_file[i]).c_str());
		one_source_des_train_out_stream.precision(8);
		one_source_des_train_out_stream.setf(ios::fixed);
		while(source_test_instream>>label)
		{
			getline(source_test_instream,line);
			whole_train_out_stream<<label2string(label)<<" cost:"<<weight_source[i][index]<<line<<endl;
			one_source_des_train_out_stream<<label2string(label)<<" cost:"<<weight_source[i][index++]<<line<<endl;
		}

		ifstream train_test_instream(filename2path(train_test_data_file).c_str());	
		index = 0;
		while (train_test_instream>>label)
		{
			getline(train_test_instream,line);
			one_source_des_train_out_stream<<label2string(label)<<" cost:"<<weight_train[index++]<<line<<endl;
		}
		train_test_instream.close();

		ifstream semi_test_instream(filename2path(semi_test_data_file).c_str());	
		while(semi_test_instream>>label)
		{
			getline(semi_test_instream,line);
			one_source_des_train_out_stream<<label2string(label)<<" cost:"<<weight_test<<line<<endl;
		}	
		semi_test_instream.close();

		source_test_instream.close();
		one_source_des_train_out_stream.close();
	}	
	//将加了权重的训练数据加入whole_train_data_file.train整个训练文件中，
	//并再分别放到des_test_data_file.train文件与des_test_data_file.test文件中
	index = 0;
	ifstream train_test_instream(filename2path(train_test_data_file).c_str());	
	while (train_test_instream>>label)
	{		
		getline(train_test_instream,line);
		whole_train_out_stream<<label2string(label)<<" cost:"<<weight_train[index]<<line<<endl;
		des_train_out_stream<<label2string(label)<<" cost:"<<weight_train[index++]<<line<<endl;		
		des_test_out_stream<<label2string(label)<<line<<endl;	
	}
	train_test_instream.close();			

	//将Dul中的实例加上权重并加入到whole_train_data_file.train整个训练文件中，
	//并再分别放到des_test_data_file.train文件与des_test_data_file.test文件中，第一次迭代，Dul无数据
	ifstream semi_test_instream(filename2path(semi_test_data_file).c_str());
	while(semi_test_instream>>label)
	{
		getline(semi_test_instream,line);
		whole_train_out_stream<<label2string(label)<<" cost:"<<weight_test<<line<<endl;
		des_train_out_stream<<label2string(label)<<" cost:"<<weight_test<<line<<endl;
		des_test_out_stream<<label2string(label)<<line<<endl;		
	}
	semi_test_instream.close();
	whole_train_out_stream.close();
	des_train_out_stream.close();
	des_test_out_stream.close();
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~trainCropus函数运行成功~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}

void PTSVM::callSVM(int curT)
{
	//训练得到Fall分类器
	cout<<"/********************************whole_train*************************************/"<<endl;
	string cmdstr="svm_learn "+filename2path(whole_train_data_file)+" "+filename2path(whole_model_file);
	system(cmdstr.c_str());
	//训练得到Ft分类器
	cout<<"/*************************************des_train********************************/"<<endl;
	cmdstr="svm_learn "+filename2path(des_train_data_file)+" "+filename2path(des_model_file);
	system(cmdstr.c_str());
	//训练得到Fone_source_des分类器
	for(int i = 0; i < source_num; i++)
	{
		cout<<"/*************************************one_source_des_train"<<i<<"********************************/"<<endl;
		cmdstr="svm_learn "+filename2path(one_source_des_train_data_file[i])+" "+filename2path(one_source_des_model_file[i]);
		system(cmdstr.c_str());

	}

	//注意：不用单独用Fall分类器测试Dl，因为已经用Fall分类器测试Dl和Dul了，
	//所以前面的train_data_num个就是用Fall分类器测试Dl的

	//用Fall分类器测试Ds
	for(int i = 0; i < source_num; i++)
	{
		cout<<"/***************************source"<<i<<"_by_whole******************************************/"<<endl;
		cmdstr="svm_classify "+filename2path(source_test_data_file[i])+" "
		+filename2path(whole_model_file)+" "
		+filename2path(source_result_by_whole_file[i]);
		system(cmdstr.c_str());
	}
	//用Fone_source_des分类器测试Dl和Dul
	for(int i = 0; i < source_num; i++)
	{	
		cout<<"/***************************des_by_one_source_des"<<i<<"******************************************/"<<endl;
		cmdstr="svm_classify "+filename2path(des_test_data_file)+" "
		+filename2path(one_source_des_model_file[i])+" "
		+filename2path(des_result_by_one_source_des_file[i]);
		system(cmdstr.c_str());
	}
	//用Fall分类器测试Dl和Dul
	cout<<"/************************des_by_whole*********************************************/"<<endl;
	cmdstr="svm_classify "+filename2path(des_test_data_file)+" "
		+filename2path(whole_model_file)+" "
		+filename2path(des_result_by_whole_file);
	system(cmdstr.c_str());
	//用Ft分类器测试Dl和Dul
	cout<<"/************************des_by_des*********************************************/"<<endl;
	cmdstr="svm_classify "+filename2path(des_test_data_file)+" "
		+filename2path(des_model_file)+" "
		+filename2path(des_result_by_des_file);
	system(cmdstr.c_str());
	//用Fall分类器测试Du
	cout<<"/************************test_by_whole*********************************************/"<<endl;
	cmdstr="svm_classify "+filename2path(test_test_data_file)+" "
		+filename2path(whole_model_file)+" "
		+filename2path(test_result_by_whole_file);
	system(cmdstr.c_str());

	//if(i%10==0) copy test_result_by_whole_file test_detail
	//下一步修改
	if (curT%10==1 || curT<11)  //将满足条件的临时结果文件存放到结果文件夹中
	{
		stringstream ss;
		ss<<svm_pre_tenstep_path<<"\\oldMulTPTSVM_pre_iter"<<curT;
		string tempstr=ss.str();
		cmdstr="copy "+filename2path(test_result_by_whole_file)+" "+tempstr;
		system(cmdstr.c_str());
	}
	//用Ft分类器测试Du
	cout<<"/************************test_by_des*******************************************/"<<endl;
	cmdstr="svm_classify "+filename2path(test_test_data_file)+" "
		+filename2path(des_model_file)+" "
		+filename2path(test_result_by_des_file);
	system(cmdstr.c_str());
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~callSVM函数运行成功~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;

}


void PTSVM::errCalculate()
{
	ifstream des_result_by_des_instream(filename2path(des_result_by_des_file).c_str());
	ifstream des_result_by_whole_instream(filename2path(des_result_by_whole_file).c_str());

	sum_weight_des=0;
	sum_weight_train=0;
	train_err_by_des=0;
	train_err_by_whole=0;
	des_err_by_des=0;
	des_err_by_whole=0;
	for(int i=0;i<source_num;i++)
	{
		des_err_by_one_source_des[i]=0;
	}	
	//计算Ft在Du和Dul上的错误率
	double result;
	for (int i=0; i<train_data_num; i++)
	{
		des_result_by_des_instream>>result;
		if (train_data_label[i]!=res2label(result))
		{					
			train_err_by_des+=weight_train[i];
		}
		sum_weight_train+=weight_train[i];
	}
	sum_weight_des+=sum_weight_train;
	des_err_by_des=train_err_by_des;
	for(map<int,int>::iterator ite=test_data_indexs.begin(); ite!=test_data_indexs.end(); ite++)
	{
		des_result_by_des_instream>>result;
		if(ite->second!=res2label(result))
		{
			des_err_by_des+=weight_test;
		}
		sum_weight_des+=weight_test;
	}
	//计算Fall在Dl上的错误率
	for (int i=0; i<train_data_num; i++)
	{
		des_result_by_whole_instream>>result;
		if (train_data_label[i]!=res2label(result))
		{
			train_err_by_whole+=weight_train[i];
		}
	}

	des_err_by_whole=train_err_by_whole;
	for(map<int,int>::iterator ite=test_data_indexs.begin(); ite!=test_data_indexs.end(); ite++)
	{
		des_result_by_whole_instream>>result;
		if(ite->second!=res2label(result))
		{
			des_err_by_whole+=weight_test;
		}
	}

	//计算Fone_source_des在Du和Dul上的错误率
	for (int j=0; j<source_num; j++)
	{
		ifstream des_result_by_one_source_des_instream(filename2path(des_result_by_one_source_des_file[j]).c_str());
		for (int i=0; i<train_data_num; i++)
		{
			des_result_by_one_source_des_instream>>result;
			if (train_data_label[i]!=res2label(result))
			{
				des_err_by_one_source_des[j]+=weight_train[i];
			}
		}		
		for(map<int,int>::iterator ite=test_data_indexs.begin(); ite!=test_data_indexs.end(); ite++)
		{
			des_result_by_one_source_des_instream>>result;
			if(ite->second!=res2label(result))
			{
				des_err_by_one_source_des[j]+=weight_test;
			}
		}
		des_result_by_one_source_des_instream.close();
	}	
	des_result_by_des_instream.close();
	des_result_by_whole_instream.close();
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~errCalculate函数运行成功~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}
//调整辅助训练数据权重
void PTSVM::reWeightSource()
{
	double e=(train_err_by_whole)/(sum_weight_train);
	
	cout<<"et: "<<e<<endl;
	if(e>-0.0000001 && e <0.0000001) e = 0.00000001;
	if(e>0.5) e = 0.5;
	double beit=e/(1.0-e);
	
	for (int j=0; j<source_num; j++)
	{
		double dist=exp((des_err_by_des-des_err_by_one_source_des[j])/sum_weight_des); //领域权重

		double result;
		ifstream source_result_instream(filename2path(source_result_by_whole_file[j]).c_str());
		for(int i=0;i<source_data_num[j]; i++)
		{
			source_result_instream>>result;
			if (source_data_label[j][i] != res2label(result))
			{
				weight_source[j][i] *= Beita[j];
			}
			weight_source[j][i] *= dist;
		}
		source_result_instream.close();
	}	
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~reWeightSource函数运行成功~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}
//调整训练数据权重
void PTSVM::reWeightTrain()
{
	double e=(train_err_by_whole)/(sum_weight_train);
	
	cout<<"et: "<<e<<endl;
	if(e>-0.0000001 && e <0.0000001) e = 0.00000001;
	if(e>0.5) e = 0.5;
	double beit=e/(1.0-e);
	
	double result;
	ifstream train_result_instream(filename2path(des_result_by_whole_file).c_str());
	for (int i=0;i<train_data_num; i++)
	{
		train_result_instream>>result;
		if(train_data_label[i] != res2label(result))
		{
			weight_train[i] /=beit;

		}
	}
	train_result_instream.close();
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~reWeightTrain函数运行成功~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}

void PTSVM::adjustTestData()
{
	int maxPos=semi_add_M/2;
	int maxNeg=maxPos;

	AltNode altini=AltNode(-1,0);
	for (int i=0; i<maxPos;i++)
	{
		altposHeap[i]=altini;
		altnegHeap[i]=altini;
	}

	int index=0;
	double result;
	ifstream test_result_instream(filename2path(test_result_by_whole_file).c_str());
	while(test_result_instream>>result)
	{
		map<int,int>::iterator map_ite=test_data_indexs.find(index);
		if(map_ite!=test_data_indexs.end())
		{
			if(res2label(result)!=map_ite->second || abs(result)<0.5 )
			{
				test_data_indexs.erase(map_ite);
			}
			else
			{
				index++;
				continue;
			}
		}
		
		if( abs(result)<1.0   &&  abs(result) > 0.5  )
		{
			AltNode inode=AltNode(index,result);
			if(1==res2label(result))
			{
				insertToAlt(altposHeap,maxPos,1,inode);
			}
			else
			{
				insertToAlt(altnegHeap,maxNeg,-1,inode);
			}
		}
		index++;
	}

	test_result_instream.close();

	int posNum=0;
	int negNum=0;
	for (int i=0; i<maxPos;i++)
	{
		if(-1!=altposHeap[i].index)
		{
			posNum++;
		}
		if(-1!=altnegHeap[i].index)
		{
			negNum++;
		}
	}
	if(negNum>posNum) negNum=posNum;
	if(posNum>negNum) posNum=negNum;


	for (int i=maxPos-1;  i>=0; i--)
	{
		if( posNum>0 && -1!=altposHeap[i].index)
		{
			posNum--;
			test_data_indexs[altposHeap[i].index]=1;
		}
		if(negNum>0 && -1!=altnegHeap[i].index)
		{
			negNum--;
			test_data_indexs[altnegHeap[i].index]=-1;
		}
	}
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~adjustTestData函数运行成功~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}

//调整测试数据权重
void PTSVM::reWeightTest(int curT)
{
	//weight_test=1;
	weight_test=weight_test_max*curT/iter_N*(1-train_err_by_whole/sum_weight_train);

	//测试语料结果记录
	double e=(train_err_by_whole)/(sum_weight_train);
	if(e>-0.0000001 && e <0.0000001) e = 0.00000001;
	if(e>0.5) e = 0.5;
	double beit=e/(1.0-e);
	
	double restemp;

	ifstream RestestIn(filename2path(test_result_by_whole_file).c_str());

	int resindex=0;
	while(RestestIn>>restemp)     
	{
		double rtemp=log(1/beit);
		fresright[resindex][curT-1]=0.5*rtemp;
		if(restemp>0)
		{
			fresleft[resindex][curT-1]=rtemp;
		}
		resindex++;
	}
	RestestIn.close();
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~reWeightTest函数运行成功~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}


string PTSVM::filename2path(string filename)  //将文件名加上临时文件存放的路径
{
	return tempDir+"\\"+filename;
}

string PTSVM::label2string(int label)
{
	if(label>0)
	{
		return "+1";
	}
	return "-1";
}

int PTSVM::res2label(double res)
{
	if (res > 0)
	{
		return 1;
	}
	return -1;
}


void PTSVM::insertToAlt(AltNode* heap, int heap_size,int label, AltNode node)
{
	if(heap[0].less(node,label))
	{
		heap[0]=node;
	}
	Heapify(heap,heap_size,0,label);
}

void PTSVM::Heapify(AltNode* heap, int heap_size, int cur, int label)
{
	int lchild=2*cur+1;
	int rchild=2*cur+2;
	int min=cur;
	if(lchild<heap_size && heap[lchild].less(heap[min], label) )
	{
		min=lchild;
	}
	if (rchild<heap_size && heap[rchild].less(heap[min],label) )
	{
		min=rchild;
	}
	if(min!=cur)
	{
		AltNode temp=heap[cur];
		heap[cur]=heap[min];
		heap[min]=temp;
		Heapify(heap,heap_size,min,label);
	}
}

/************************************************************************/
AltNode::AltNode()
{

}
AltNode::AltNode(int index, double value)
{
	this->index=index;
	this->value=value;
}

bool AltNode::less(AltNode anode,int label)
{
	if(1==label)
	{
		if(this->value < anode.value)
		{
			return true;
		}
		return false;
	}

	if(this->value > anode.value)
	{
		return true;
	}
	return false;
}

