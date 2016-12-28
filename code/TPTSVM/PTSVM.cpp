#include "PTSVM.h"
//��intת����string
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
//����ʱ���������ϲ��ü�Ȩ��
//desָDl��Dul
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
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~�����������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
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
	
	altnegHeap= new AltNode[semi_add_M/2+1]; //���ѡ������semi_add_M/2�������Ķ�
	altposHeap= new AltNode[semi_add_M/2+1]; //���ѡ������semi_add_M/2�������Ķ�
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


	//��ʱĿ¼���� ������
	if( 0==_access(tempDir.c_str() , 0 ) )
	{
		return;
	}
	//�������򴴽����������ʧ���������ʾ��ǿ���˳�
	if( -1 == _mkdir(tempDir.c_str()) )
	{
		cout<<"������ʱ�ļ�ʧ��"<<endl;
		exit(1);
	}
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~input�������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
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
		cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~��ɵ�"<<curT<<"�ε���~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
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

	//��������ѵ������
	vector<int> temp;
	for(int i = 0; i < source_num; i++)
	{
		source_data_num[i]=0;  //����ѵ�����ݸ�����ʼ��ֵΪ0
		ifstream source_data_instream(source_file_path_input[i].c_str()); //��Ÿ���ѵ�������ļ�
		ofstream source_data_outstream(filename2path(source_test_data_file[i]).c_str()); //��Ÿ���ѵ��������ʱ�ļ�
//		cout<<filename2path(source_test_data_file[i]).c_str()<<endl;/////////////////////
		while (source_data_instream>>label)  //ȡ����ǩ�ŵ�label��
		{
			getline(source_data_instream , line); //��ȡһ������
			source_data_num[i]++;                   //���㸨��ѵ�����ݸ���
			temp.push_back(label);
//			source_data_label.push_back(label);
			source_data_outstream<<label2string(label)<<line<<endl;  //��Ÿ���ѵ��������ʱ�ļ�
		}
		source_data_label.push_back(temp);
		source_data_instream.close();
		source_data_outstream.close();
		temp.clear();
	}	

	//����ѵ������
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

	//������������
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

	// ɾ��֮ǰ�Ľ������
	if( 0==_access(filename2path(test_result_by_whole_file).c_str() , 0 ) )
	{
		DeleteFileA(filename2path(test_result_by_whole_file).c_str());
	}

	//��ʼ��Ȩ��
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

	//�����
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
		fresleft.push_back(vtemp);      //���ս�����
		fresright.push_back(vtemp);
	}
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~init�������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
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
	if(!test_result_instream) //���test_result_by_whole_file.result�ļ������ڣ������ǵ�һ�ε�������Ϊ��init������ɾ���ˣ����򷵻�
	{
		cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~��һ�ε���~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
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
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~testCropus�������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
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
	//������Ȩ�صĸ���ѵ�����ݼ���whole_train_data_file.train����ѵ���ļ���,��׼��one_source_des_train_data_file
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
	//������Ȩ�ص�ѵ�����ݼ���whole_train_data_file.train����ѵ���ļ��У�
	//���ٷֱ�ŵ�des_test_data_file.train�ļ���des_test_data_file.test�ļ���
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

	//��Dul�е�ʵ������Ȩ�ز����뵽whole_train_data_file.train����ѵ���ļ��У�
	//���ٷֱ�ŵ�des_test_data_file.train�ļ���des_test_data_file.test�ļ��У���һ�ε�����Dul������
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
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~trainCropus�������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}

void PTSVM::callSVM(int curT)
{
	//ѵ���õ�Fall������
	cout<<"/********************************whole_train*************************************/"<<endl;
	string cmdstr="svm_learn "+filename2path(whole_train_data_file)+" "+filename2path(whole_model_file);
	system(cmdstr.c_str());
	//ѵ���õ�Ft������
	cout<<"/*************************************des_train********************************/"<<endl;
	cmdstr="svm_learn "+filename2path(des_train_data_file)+" "+filename2path(des_model_file);
	system(cmdstr.c_str());
	//ѵ���õ�Fone_source_des������
	for(int i = 0; i < source_num; i++)
	{
		cout<<"/*************************************one_source_des_train"<<i<<"********************************/"<<endl;
		cmdstr="svm_learn "+filename2path(one_source_des_train_data_file[i])+" "+filename2path(one_source_des_model_file[i]);
		system(cmdstr.c_str());

	}

	//ע�⣺���õ�����Fall����������Dl����Ϊ�Ѿ���Fall����������Dl��Dul�ˣ�
	//����ǰ���train_data_num��������Fall����������Dl��

	//��Fall����������Ds
	for(int i = 0; i < source_num; i++)
	{
		cout<<"/***************************source"<<i<<"_by_whole******************************************/"<<endl;
		cmdstr="svm_classify "+filename2path(source_test_data_file[i])+" "
		+filename2path(whole_model_file)+" "
		+filename2path(source_result_by_whole_file[i]);
		system(cmdstr.c_str());
	}
	//��Fone_source_des����������Dl��Dul
	for(int i = 0; i < source_num; i++)
	{	
		cout<<"/***************************des_by_one_source_des"<<i<<"******************************************/"<<endl;
		cmdstr="svm_classify "+filename2path(des_test_data_file)+" "
		+filename2path(one_source_des_model_file[i])+" "
		+filename2path(des_result_by_one_source_des_file[i]);
		system(cmdstr.c_str());
	}
	//��Fall����������Dl��Dul
	cout<<"/************************des_by_whole*********************************************/"<<endl;
	cmdstr="svm_classify "+filename2path(des_test_data_file)+" "
		+filename2path(whole_model_file)+" "
		+filename2path(des_result_by_whole_file);
	system(cmdstr.c_str());
	//��Ft����������Dl��Dul
	cout<<"/************************des_by_des*********************************************/"<<endl;
	cmdstr="svm_classify "+filename2path(des_test_data_file)+" "
		+filename2path(des_model_file)+" "
		+filename2path(des_result_by_des_file);
	system(cmdstr.c_str());
	//��Fall����������Du
	cout<<"/************************test_by_whole*********************************************/"<<endl;
	cmdstr="svm_classify "+filename2path(test_test_data_file)+" "
		+filename2path(whole_model_file)+" "
		+filename2path(test_result_by_whole_file);
	system(cmdstr.c_str());

	//if(i%10==0) copy test_result_by_whole_file test_detail
	//��һ���޸�
	if (curT%10==1 || curT<11)  //��������������ʱ����ļ���ŵ�����ļ�����
	{
		stringstream ss;
		ss<<svm_pre_tenstep_path<<"\\oldMulTPTSVM_pre_iter"<<curT;
		string tempstr=ss.str();
		cmdstr="copy "+filename2path(test_result_by_whole_file)+" "+tempstr;
		system(cmdstr.c_str());
	}
	//��Ft����������Du
	cout<<"/************************test_by_des*******************************************/"<<endl;
	cmdstr="svm_classify "+filename2path(test_test_data_file)+" "
		+filename2path(des_model_file)+" "
		+filename2path(test_result_by_des_file);
	system(cmdstr.c_str());
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~callSVM�������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;

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
	//����Ft��Du��Dul�ϵĴ�����
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
	//����Fall��Dl�ϵĴ�����
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

	//����Fone_source_des��Du��Dul�ϵĴ�����
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
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~errCalculate�������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}
//��������ѵ������Ȩ��
void PTSVM::reWeightSource()
{
	double e=(train_err_by_whole)/(sum_weight_train);
	
	cout<<"et: "<<e<<endl;
	if(e>-0.0000001 && e <0.0000001) e = 0.00000001;
	if(e>0.5) e = 0.5;
	double beit=e/(1.0-e);
	
	for (int j=0; j<source_num; j++)
	{
		double dist=exp((des_err_by_des-des_err_by_one_source_des[j])/sum_weight_des); //����Ȩ��

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
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~reWeightSource�������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}
//����ѵ������Ȩ��
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
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~reWeightTrain�������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
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
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~adjustTestData�������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}

//������������Ȩ��
void PTSVM::reWeightTest(int curT)
{
	//weight_test=1;
	weight_test=weight_test_max*curT/iter_N*(1-train_err_by_whole/sum_weight_train);

	//�������Ͻ����¼
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
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~reWeightTest�������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}


string PTSVM::filename2path(string filename)  //���ļ���������ʱ�ļ���ŵ�·��
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

