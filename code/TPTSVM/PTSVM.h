#ifndef  _MY_PTSVM_H_
#define _MY_PTSVM_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <direct.h>
#include <io.h>
#include <Windows.h>
#include <vector>
#include <map>
#include <math.h>
using namespace std;

class AltNode
{
public:
	int index;
	double value;
	AltNode();
	AltNode(int index, double value);
	bool less(AltNode anode,int label);
};

class PTSVM
{
private:
	//����
	string *source_file_path_input;  //����ѵ�������ļ���·��
	string train_file_path_input;	//ѵ�������ļ���·��
	string test_file_path_input;	//���������ļ���·��
	int iter_N;	//	��������
	int semi_add_M;	//semi-supervised ������ÿ����ӵ�ʵ����
	string tempDir;	//ѵ����������ʱ�ļ����λ��
	int source_num; //����ѵ���������
	
	int *source_data_num; //����ѵ�����ݸ���
	int train_data_num;  //ѵ�����ݸ���
	int test_data_num;
//���ս�����
	vector<vector<double>> fresleft;
	vector<vector<double>> fresright;

	map<int,int> test_data_indexs;
	double weight_test;
	double weight_test_max;

	//�����������ʵı���	
	double train_err_by_des; 
	double train_err_by_whole;
	double des_err_by_des;
	double des_err_by_whole;
	double *des_err_by_one_source_des;
	double sum_weight_des;
	double sum_weight_train;

	vector<vector<int>> source_data_label;  //����ѵ������ʵ���ı�ǩ����
	vector<int> train_data_label;

	vector<vector<double>> weight_source;  //����ѵ������ʵ��Ȩ�ؼ���
	vector<double> weight_train;   //ѵ������ʵ��Ȩ�ؼ���

	double *Beita;


	//��ʱ�ļ�����
	string *source_test_data_file;
	string train_test_data_file;
	string test_test_data_file;
	string des_test_data_file;
	string semi_test_data_file;

	string whole_train_data_file;
	string *one_source_des_train_data_file;	
	string des_train_data_file;


	string whole_model_file;
	string *one_source_des_model_file;
	string des_model_file;

	string *source_result_by_whole_file;
	string *des_result_by_one_source_des_file;
	string des_result_by_whole_file;
	string des_result_by_des_file;
	string test_result_by_whole_file;
	string test_result_by_des_file;

	string svm_pre_tenstep_path;  //���������ļ���·�� �磺D:\\temp\\abs1\\1

	AltNode *altposHeap;
	AltNode *altnegHeap;

public:

	PTSVM();
	~PTSVM();

	void input(string train_file_path_input, string test_file_path_input,
						string svm_pre_tenstep_path,int source_num, int iter_N=50, int semi_add_M=20,
						double weight_test_max=1.0, string tempDir="F:\\PTSVMtemp1");
	void learn();
	void test();
	void init();
	void iter(int curT);
	string filename2path(string filename);
	void cropus();
	void testCropus();
	void trainCropus();
	string label2string(int label);
	void callSVM(int curT);
	void errCalculate();
	int PTSVM::res2label(double res);
	void reWeightSource();
	void reWeightTrain();
	void adjustTestData();
	void 	reWeightTest(int curT);

	void insertToAlt(AltNode* heap, int heap_size,int label, AltNode node);
	void Heapify(AltNode* heap, int heap_size, int cur, int label);

};



#endif