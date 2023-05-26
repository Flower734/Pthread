#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <time.h>
#include <string>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <pthread.h>
#define MAXSIZE 2000
using namespace std;
class index{
public:
	int len = 0;
	vector<unsigned int> arr;
};
vector<index> idx;
void sorted(int* list, vector<index>& idx, int num) {
	for (int i = 0; i < num - 1; i++) {
		for (int j = 0; j < num - i - 1; j++) {
			if (idx[list[j]].len > idx[list[j + 1]].len) {
				int tmp = list[j];
				list[j] = list[j + 1];
				list[j + 1] = tmp;
			}
		}
	}
}
index s;
index temp;
pthread_t thread[4];

typedef struct  {
	int t_id;//�߳�id
	int num_of_list;//����ĵڼ���list
	index s0;
}threadParam_t;
threadParam_t param[4];
void* threadFunc_Level2(void* param) {
	threadParam_t* p = (threadParam_t*)param;
	int t_id = p->t_id;
	int num_of_list = p->num_of_list;
	int start = (s.arr.size() / 4) * t_id;
	int _end;
	if (t_id == 3) {
		_end = s.arr.size();
	}
	else {
		_end = start + (s.arr.size()) / 4;
	}
	index s0;
	s0.arr.assign(s.arr.begin() + start, s.arr.begin() + _end);
	for (int i =0; i < _end-start; i++) {
		bool isexit = false;
		for (int jj = 0; jj < idx[num_of_list].arr.size(); jj++) {
			if (s0.arr[i] == idx[num_of_list].arr[jj]) {
				isexit = true;
				break;
			}
		}
		if (isexit == false) {
			s0.arr[i]=-1;
		}
	}
	vector<unsigned int>::iterator newEnd(remove(s0.arr.begin(), s0.arr.end(), -1));
	s0.arr.erase(newEnd);
	p->s0 = s0;
	return p;
}

index& SVS(int* list, vector<index>& idx, int num) {
	s = idx[list[0]];
	for (int i = 1; i < num; i++) {//num���б���
		int length = s.len;
		//�����ﴴ��4���̣߳��ֱ��s��Ϊ�Ķκ��������idx[list[i]].arr�е����Ƚϣ����Ƿ����
		//��������ڣ���Ҫɾ��������������ɾ�������
		for (int k = 0; k < 4; k++) {
			param[k].num_of_list = list[i];
			param[k].t_id = k;
			pthread_create(&thread[k], NULL, threadFunc_Level2, &param[k]);
		}
		void* res;
		for (int k = 0; k < 4; k++) {
			pthread_join(thread[k], &res);
			param[k]=*(threadParam_t*)res;
		}
		index s2;
		for(int k = 0; k < 4; k++) {
			s2.arr.insert(s2.arr.end(), param[k].s0.arr.begin(), param[k].s0.arr.end());
		}
		s2.len = s2.arr.size();
		s = s2;
	}
	return s;
}
int main() {
	//��ȡ�������ļ�
	fstream file;
	file.open("ExpIndex", ios::binary | ios::in);
	if (!file.is_open()) {
		cout << "No file";
		return 0;

	}

	for (int i = 0; i < 2000; i++) {
		index temp;
		file.read((char*)&temp.len, sizeof(temp.len));
		for (int j = 0; j < (temp.len); j++)
		{
			unsigned int binary_temp;
			file.read((char*)&binary_temp, sizeof(binary_temp));
			temp.arr.push_back(binary_temp);
		}
		idx.push_back(temp);
	}
	file.close();
	/*---------------------------------------------------------------------------------------------------------*/
	file.open("ExpQuery", ios::in);
	int query[1000][5] = { 0 };
	string line;
	int count = 0;
	while (getline(file, line)) {
		stringstream ss; //������
		ss << line; //�����д�ֵ
		int a = 0;
		while (!ss.eof()) {
			int temp;
			ss >> temp;
			query[count][a] = temp;
			a++;
		}
		count++;
	}
	/*-----------------------------------------------------------------------------------------------------------*/

	//SVS

	clock_t start, end;
	start = clock();
	for (int i = 0; i < 5; i++) {
		int num = 0;
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}
		int* list = new int[num];//�����е���
		for (int j = 0; j < num; j++) {
			list[j] = query[i][j];
		}
		//------ - ������-------- -
		sorted(list, idx, num);
		SVS(list, idx, num);

	}
	end = clock();
	cout << "SVS���ĵ��¼���" << (end - start) / 5 << endl;

	return 0;
}
