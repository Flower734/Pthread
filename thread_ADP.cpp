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
#define NUM_THREADS 7
using namespace std;
class _index{
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
bool operator<(const _index& s1, const _index& s2)
{
	return s1.len < s2.len; //从小到大排序
}
_index X;
_index temp;
pthread_t thread[NUM_THREADS];
pthread_mutex_t amutex=PTHREAD_MUTEX_INITIALIZER;
_index s;
typedef struct  {
	int t_id;//线程id
	int* num_of_list=0;//待测的第几组list
	int n;
	//bool isfound;
	//bool have;
}threadParam_t;
//threadParam_t param[NUM_THREADS];
_index* s0=new _index[NUM_THREADS];
vector<unsigned int> ans;
void* threadFunc_forADP(void* param) {
	threadParam_t* p = (threadParam_t*)param;
	int t_id = p->t_id; //线程ID
	int* num_of_list = p->num_of_list;  //待测列的编号
	int n = p->n;
    int start = (s.arr.size() / NUM_THREADS) * t_id;
	int _end;
	if (t_id == NUM_THREADS - 1) {
		_end = s.arr.size();
	}
	else {
		_end = start + (s.arr.size()) / NUM_THREADS;
	}
	s0[t_id].arr.assign(s.arr.begin() + start, s.arr.begin() + _end);
    s0[t_id].len=s0[t_id].arr.size();
    for(int i=0;i<s0[t_id].len;i++){
            //遍历每个list，判断s0[t_id].arr[i]是否出现在其他数组
            bool found=false;
        for(int j=1;j<n;j++){
            //从第1个数组开始遍历，如果第一个中就没有，则退出循环
            for(int k=0;k<idx[num_of_list[j]].arr.size();k++){
                if(s0[t_id].arr[i]==idx[num_of_list[j]].arr[k]){
                    found=true;
                    break;
                }
            }
            if(found==false){
                break;
            }
        }
        if(found==true){
            pthread_mutex_lock(&amutex);
            ans.push_back(s0[t_id].arr[i]);
            pthread_mutex_unlock(&amutex);
        }
    }
    s0[t_id].arr=ans;
    s0[t_id].len=s0[t_id].arr.size();
	return NULL;
}

_index& ADP(int* list, vector<_index>& idx, int num)
{
	s = idx[list[0]];
	threadParam_t param[NUM_THREADS];
	pthread_mutex_init(&amutex,NULL);
	for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
		param[t_id].t_id = t_id;
		param[t_id].num_of_list = list;
		param[t_id].n = num;
		pthread_create(&thread[t_id], NULL, threadFunc_forADP, &param[t_id]);
	}

	for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
		pthread_join(thread[t_id], NULL);
	}
	s.arr.clear();
	for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
		s.arr.insert(s.arr.end(), s0[t_id].arr.begin(), s0[t_id].arr.end());
	}

	return s;
}

int main() {
	//
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
		stringstream ss; //??????
		ss << line; //?????д??
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

	//struct timepspec sts,ets;
	//timespec_get(sts,TIME_UTC);
	struct timespec sts, ets;
	timespec_get(&sts, TIME_UTC);
	for (int i = 0; i < 5; i++) {
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
