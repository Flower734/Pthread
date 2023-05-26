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
#include <arm_neon.h>
#define MAXSIZE 2000
using namespace std;
class _index {
public:
	int len = 0;
	vector<unsigned int> arr;
};
vector<_index> idx;
void sorted(int* list, vector<_index>& idx, int num) {
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
_index s;
_index temp;
pthread_t thread[4];

typedef struct {
	int t_id;//线程id
	int num_of_list;//待测的第几组list
	_index s0;
}threadParam_t;
threadParam_t param[4];
void* threadFunc_Level2(void* param) {
	threadParam_t* p = (threadParam_t*)param;
	int t_id = p->t_id;
	int num_of_list = p->num_of_list;
	int start = (s.arr.size() / 4) * t_id;
	int _end;
	int count = 0;
	if (t_id == 3) {
		_end = s.arr.size();
	}
	else {
		_end = start + (s.arr.size()) / 4;
	}
	_index s0;
	s0.arr.assign(s.arr.begin() + start, s.arr.begin() + _end);
	uint32_t length = ceil((_end - start) / 4) * 4;
	for (int m = _end; m < length; m++)
	{
		s0.arr[m] = 0;
		idx[num_of_list].arr[m] = 0;
	}

	for (int i = 0; i < _end - start; i++) {
		bool isexit = false;
		for (int t = 0; t < length; t += 4) {
			unsigned int res[4] = { 0 };
			uint32x4_t temp0 = vmovq_n_u32(s0.arr[count]);
			uint32x4_t temp1 = vld1q_u32(&idx[num_of_list].arr[t]);
			uint32x4_t result = vceqq_u32(temp0, temp1);
			vst1q_u32(res, result);
			if (res[0] == 1 || res[1] == 1 || res[2] == 1 || res[3] == 1) {
				isexit = true;
				break;
			};
		}
		if (isexit == false) {
			s0.arr[count] = -1;
		}
		count++;
	}
	vector<unsigned int>::iterator newEnd(remove(s0.arr.begin(), s0.arr.end(), -1));
	s0.arr.erase(newEnd);
	p->s0 = s0;
	return p;
}

_index& SVS(int* list, vector<_index>& idx, int num) {
	s = idx[list[0]];
	for (int i = 1; i < num; i++) {//num个列表求交
		int length = s.len;
		//在这里创建4个线程，分别把s分为四段后的数据与idx[list[i]].arr中的数比较，看是否存在
		//如果不存在，需要删除，则利用索引删除这个数
		for (int k = 0; k < 4; k++) {
			param[k].num_of_list = list[i];
			param[k].t_id = k;
			pthread_create(&thread[k], NULL, threadFunc_Level2, &param[k]);
		}
		void* res;
		for (int k = 0; k < 4; k++) {
			pthread_join(thread[k], &res);
			param[k] = *(threadParam_t*)res;
		}
		_index s2;
		for (int k = 0; k < 4; k++) {
			s2.arr.insert(s2.arr.end(), param[k].s0.arr.begin(), param[k].s0.arr.end());
		}
		s2.len = s2.arr.size();
		s = s2;
	}
	return s;
}
int main() {
	//读取二进制文件
	fstream file;
	file.open("ExpIndex", ios::binary | ios::in);
	if (!file.is_open()) {
		cout << "No file";
		return 0;

	}

	for (int i = 0; i < 2000; i++) {
		_index temp;
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
		stringstream ss; //输入流
		ss << line; //向流中传值
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

	struct timespec sts, ets;
	timespec_get(&sts, TIME_UTC);
	for (int i = 0; i < 5; i++) {
		int num = 0;
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}
		int* list = new int[num];//例子中的数
		for (int j = 0; j < num; j++) {
			list[j] = query[i][j];
		}
		//------ - 按表求交-------- -
		sorted(list, idx, num);
		SVS(list, idx, num);

	}
	timespec_get(&ets, TIME_UTC);
	time_t dsec = ets.tv_sec-sts.tv_sec;
	long dnsec = ets.tv_nsec-sts.tv_nsec;
  if(dnsec<0){
  dsec--;
  dnsec+=1000000000ll;
  }
	printf(" % llu. % 09llus\n", dsec, dnsec);

	return 0;
}