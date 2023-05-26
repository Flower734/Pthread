#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <arm_neon.h>
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <pthread.h>
#define NUM_THREADS 4
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
bool operator<(const index& s1, const index& s2)
{
	return s1.len < s2.len; //从小到大排序
}
index X;
index temp;
pthread_t thread[NUM_THREADS];

typedef struct  {
	int t_id;//线程id
	int num_of_list;//待测的第几组list
	int e; //用于比较的数
	index s0;
	bool isfound = true;
	bool have;
}threadParam_t;
threadParam_t param[NUM_THREADS];

void* threadFunc_forADP(void* param) {
	threadParam_t* p = (threadParam_t*)param;
	if(p->have==true){
	int t_id = p->t_id; //线程ID
	int num_of_list = p->num_of_list;  //待测列的编号

	bool found = p->isfound;

    for(int i = 0;i<p->s0.len; i++)
    {
        found = false;
        if (p->e==p->s0.arr[i])
        {
            found = true;
            break;
        }
    }

    p->isfound=found;}
	return p;
}




vector<unsigned int> S;
vector<unsigned int>& ADP(int* list, vector<index>& idx, int num)
{
	int s = 1;
	bool found = true;
	vector<index> idx_;
	for (int i = 0; i < num; i++)
	{
		idx_.push_back(idx[list[i]]);
	}
	sort(idx_.begin(), idx_.end());

	for (int t = 0; t < idx_[0].len; t++)
	{
	    s = 1;
		unsigned int e = idx_[0].arr[t];
		while (s < num && found == true)
		{


            for (int k = 0; k < NUM_THREADS; k++, s++) {
                if(s < num){
                param[k].num_of_list = list[s]; //待测的第s组list
                param[k].t_id = k;
                param[k].e = e;
                param[k].s0 = idx_[s];
                param[k].isfound=found;
                param[k].have=true;
                pthread_create(&thread[k], NULL, threadFunc_forADP, &param[k]);
                }
                else param[k].have=false;
            }
		    void* res;
            for (int k = 0; k < num-1; k++) {
                pthread_join(thread[k], &res);
                param[k]=*(threadParam_t*)res;
            }
            for(int k = 0; k< NUM_THREADS;k++)
                if(param[k].isfound==false)
                    found=false;

		}

		if (s >= num  && found == true)
			S.push_back(e);
	}

	return S;
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
	for (int i = 0; i < 1000; i++) {
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

		ADP(list, idx, num); 
		delete list;
   if((i+1)%200==0){
     timespec_get(&ets, TIME_UTC);
	   time_t dsec = ets.tv_sec-sts.tv_sec;
	   long dnsec = ets.tv_nsec-sts.tv_nsec;
     if(dnsec<0){
       dsec--;
       dnsec+=1000000000ll;
     }
	   printf(" % lld. % 09llds\n", dsec, dnsec);
       cout<<endl;
   }
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
