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
#define NUM_THREADS 4
using namespace std;
using std::vector;


class index {
public:
	int len = 0;
	vector<unsigned int> arr;
};


bool operator<(const index& s1, const index& s2)
{
	return s1.len < s2.len; //��С��������
}
vector<index> idx;
vector<index> idx_;

bool operator<(const index& s1, const index& s2)
{
	return s1.len < s2.len; //从小到大排序
}

pthread_t thread[NUM_THREADS];
sem_t sem_main;
sem_t sem_workerstart[NUM_THREADS];//每个线程专有信号量
sem_t sem_workerend[NUM_THREADS];


struct threadParam_t {
	int t_id;//线程id
	int* num_of_list;//待测的第几组list
	int num;
	bool isfound = false;
	index s0;
};
threadParam_t param[NUM_THREADS];

vector<unsigned int> temp_s[NUM_THREADS];
void* threadFunc_forADP(void* param) {
	threadParam_t* p = (threadParam_t*)param;
	bool found = true;
	for (int t = 0; t < p->s0.arr.size(); t++)
	{
		
		int s = 1;
		unsigned int e = idx_[0].arr[t];
		while (s != p->num && found == true)
		{
			for (int i = 0; i < idx_[s].len; i++)
			{
				found = false;
				if (e == idx_[s].arr[i])
				{
					found = true;
					break;
				}
			}
			s = s + 1;
		}
		if (s == p->num && found == true)
			temp_s[p->t_id].push_back(e);
	}
	sem_post(&sem_main);//唤醒主线程，合并
	return p;
}

vector<unsigned int> S;
vector<unsigned int>& ADP(int* list, vector<index>& idx, int num)
{
	sem_init(&sem_main, 0, 0);//初始化
	for (int i = 0; i < NUM_THREADS; i++) {
		sem_init(&sem_workerstart[i], 0, 0);
		sem_init(&sem_workerend[i], 0, 0);
	}
	int s = 1;
	bool found = true;
	int count = 0;
	
	for (int i = 0; i < num; i++)
	{
		idx_.push_back(idx[list[i]]);
	}
	sort(idx_.begin(), idx_.end());

	int space = idx_[0].arr.size() / 4;
	int _end;
	for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
		param[t_id].t_id = t_id;
		param[t_id].num_of_list = list;
		param[t_id].num = num;
		int start = space * t_id;
		if (t_id == 3) {
			_end = idx_[0].arr.size();
		}
		else {
			_end = start + space;
		}
		param[t_id].s0.arr.assign(idx_[0].arr.begin() + start, idx_[0].arr.begin() + _end);
		pthread_create(&thread[t_id], NULL, threadFunc_forADP, &param[t_id]);//创建线程，开始工作
	}
	for (int t_id = 0; t_id < NUM_THREADS; t_id++) {//唤醒工作线程
		sem_post(&sem_workerstart[t_id]);
	}
	for (int t_id = 0; t_id < NUM_THREADS; t_id++) {//主线程睡眠，等待子线程唤醒
		sem_wait(&sem_main);
	}
	for (int t_id = 0; t_id < NUM_THREADS; t_id++) {//vector结果合并
		S.insert(S.end(), temp_s[t_id].begin(), temp_s[t_id].end());
	}
	for (int t_id = 0; t_id < NUM_THREADS; t_id++) {//join
		pthread_join(thread[t_id], NULL);
	}
	sem_destroy(&sem_main);//销毁
	for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
		sem_destroy(&sem_workerstart[t_id]);
		sem_destroy(&sem_workerend[t_id]);
	}

	return S;
}
vector<index> idx;
int main() {
	//读取二进制文件
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
	file.close();
	/*-----------------------------------------------------------------------------------------------------------*/
	

	struct timespec sts, ets;
	timespec_get(&sts, TIME_UTC);
	for (int i = 0; i < 100; i++) {
		int num = 0;
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}
		int* list = new int[num];
		for (int j = 0; j < num; j++) {
			list[j] = query[i][j];
		}

		ADP(list, idx, num);
	}
	timespec_get(&ets, TIME_UTC);
	time_t dsec = ets.tv_sec-sts.tv_sec;
	long dnsec = ets.tv_nsec-sts.tv_nsec;
  	if(dnsec<0){
  	dsec--;
  	dnsec+=1000000000ll;
  	}
	printf(" % lld. % 09llds\n", dsec, dnsec);
	return 0;
}
