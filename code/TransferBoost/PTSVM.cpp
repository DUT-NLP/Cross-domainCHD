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

	whole_train_data_file="whole_train_data_file.train";	
	des_train_data_file="des_train_data_file.train";

	whole_model_file="whole_model_file.model";
	des_model_file="des_model_file.model";

	des_result_by_whole_file="des_result_by_whole_file.result";
	des_result_by_des_file="des_result_by_des_file.result";
	test_result_by_whole_file="test_result_by_whole_file.result";

}

PTSVM::~PTSVM()
{	
	delete[] source_file_path_input;
	delete[] source_data_num;
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
								int semi_add_M/* =20 */, string tempDir/* ="F:\\PTSVMtemp" */)
{	
	this->train_file_path_input=train_file_path_input;
	this->test_file_path_input=test_file_path_input;
	this->iter_N=iter_N;
	this->semi_add_M=semi_add_M;
	this->weight_test_max=weight_test_max;
	this->tempDir=tempDir;
	this->svm_pre_tenstep_path=svm_pre_tenstep_path;
	this->source_num=source_num;	

	source_file_path_input= new string[source_num];
	source_data_num= new int[source_num];
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
		while (source_data_instream>>label)  //ȡ����ǩ�ŵ�label��
		{
			getline(source_data_instream , line); //��ȡһ������
			source_data_num[i]++;                   //���㸨��ѵ�����ݸ���
			temp.push_back(label);
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
}

void PTSVM::cropus()
{
	trainCropus();
}

void PTSVM::trainCropus()
{
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~begin train~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
	ofstream whole_train_out_stream(filename2path(whole_train_data_file).c_str());
	ofstream des_train_out_stream(filename2path(des_train_data_file).c_str());
	
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
	}
	train_test_instream.close();			

	whole_train_out_stream.close();
	des_train_out_stream.close();
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

	//��Fone_source_des����������Dl
	for(int i = 0; i < source_num; i++)
	{	
		cout<<"/***************************des_by_one_source_des"<<i<<"******************************************/"<<endl;
		cmdstr="svm_classify "+filename2path(train_test_data_file)+" "
		+filename2path(one_source_des_model_file[i])+" "
		+filename2path(des_result_by_one_source_des_file[i]);
		system(cmdstr.c_str());
	}	
	//��Ft����������Dl
	cout<<"/************************des_by_des*********************************************/"<<endl;
	cmdstr="svm_classify "+filename2path(train_test_data_file)+" "
		+filename2path(des_model_file)+" "
		+filename2path(des_result_by_des_file);
	system(cmdstr.c_str());
	//��Fall����������Du
	cout<<"/************************test_by_whole*********************************************/"<<endl;
	cmdstr="svm_classify "+filename2path(test_test_data_file)+" "
		+filename2path(whole_model_file)+" "
		+filename2path(test_result_by_whole_file);
	system(cmdstr.c_str());
	//��Fall����������Dl
	cout<<"/************************train_by_whole*********************************************/"<<endl;
	cmdstr="svm_classify "+filename2path(train_test_data_file)+" "
		+filename2path(whole_model_file)+" "
		+filename2path(des_result_by_whole_file);
	system(cmdstr.c_str());
	//��Fall����������Ds
	for(int i = 0; i < source_num; i++)
	{
		cout<<"/***************************source"<<i<<"_by_whole******************************************/"<<endl;
		cmdstr="svm_classify "+filename2path(source_test_data_file[i])+" "
		+filename2path(whole_model_file)+" "
		+filename2path(source_result_by_whole_file[i]);
		system(cmdstr.c_str());
	}
	//if(i%10==0) copy test_result_by_whole_file test_detail
	//��һ���޸�
	if (curT%10==1 || curT<11)  //��������������ʱ����ļ���ŵ�����ļ�����
	{
		stringstream ss;
		ss<<svm_pre_tenstep_path<<"\\PTSVM_pre_iter"<<curT;
		string tempstr=ss.str();
		cmdstr="copy "+filename2path(test_result_by_whole_file)+" "+tempstr;
		system(cmdstr.c_str());
	}
	
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~callSVM�������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;

}


void PTSVM::errCalculate()
{
	ifstream des_result_by_des_instream(filename2path(des_result_by_des_file).c_str());
	ifstream des_result_by_whole_instream(filename2path(des_result_by_whole_file).c_str());
	sum_weight_train=0;
	train_err_by_des=0;
	train_err_by_whole=0;
	for(int i=0;i<source_num;i++)
	{
		des_err_by_one_source_des[i]=0;
	}	
	//����Ft��Dl�ϵĴ�����
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
	//����Fall��Dl�ϵĴ�����
	for (int i=0; i<train_data_num; i++)
	{
		des_result_by_whole_instream>>result;
		if (train_data_label[i]!=res2label(result))
		{
			train_err_by_whole+=weight_train[i];
		}
	}

	//����Fone_source_des��Dl�ϵĴ�����
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
	if(e>-0.0000001 && e <0.0000001) e = 0.00000001;
	if(e>0.5) e = 0.5;
	double beit=log((1-e)/e);
	for (int j=0; j<source_num; j++)
	{
		double dist=exp((train_err_by_des-des_err_by_one_source_des[j])/sum_weight_train); //����Ȩ��

		double result;
		ifstream source_result_instream(filename2path(source_result_by_whole_file[j]).c_str());
		for(int i=0;i<source_data_num[j]; i++)
		{
			source_result_instream>>result;
			if (source_data_label[j][i] != res2label(result))
			{
				weight_source[j][i] *= beit;
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
	if(e>-0.0000001 && e <0.0000001) e = 0.00000001;
	if(e>0.5) e = 0.5;
	double beit=log((1-e)/e);
	cout<<"e="<<e<<endl;

	double result;
	ifstream train_result_instream(filename2path(des_result_by_whole_file).c_str());
	for (int i=0;i<train_data_num; i++)
	{
		train_result_instream>>result;
		if(train_data_label[i] != res2label(result))
		{			
			weight_train[i] *=exp(beit);
		}
	}
	train_result_instream.close();
	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~reWeightTrain�������гɹ�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
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
