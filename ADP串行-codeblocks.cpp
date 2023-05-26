#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <sstream>
#define MAXSIZE 2000
//#include <omp.h>
//#define THREADS_NUM 4
using namespace std;

class index {
public:
	int len = 0;
	vector<unsigned int> arr;
};

index s;
index temp;
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
//index* s0=new index[THREADS_NUM];
//index& SVS_omp_baseonP(int* list, vector<index>& idx, int num) {
//	s = idx[list[0]];
//	int id;
//    int sum;
//	//#pragma omp parallel num_threads(THREADS_NUM)\
//     shared(s,idx,num,list),private(id)
//	{
//      id=omp_get_thread_num();
//      int start=s.arr.size()/THREADS_NUM*id;
//      int _end;
//      if (id == THREADS_NUM-1) {
//		    _end = s.arr.size();
//	    }
//	    else {
//		    _end = start + (s.arr.size()) / THREADS_NUM;
//	    }
//	    s0[id].arr.assign(s.arr.begin() + start, s.arr.begin() + _end);//s��ֵ����s0[id].array
//      for(int i=1;i<num;i++){
//	    for(int k=0;k<s0[id].arr.size();k++){
//            bool isexit=false;
//            for (int jj = 0; jj < idx[list[i]].arr.size(); jj++) {
//				if (s0[id].arr[k] < idx[list[i]].arr[jj])
//					break;
//			if (s0[id].arr[k] == idx[list[i]].arr[jj]) {
//				isexit = true;
//				break;
//			}
//		}
//		if (isexit == false) {
//			s0[id].arr[k]=-1;
//		}
//	    }
//	    vector<unsigned int>::iterator newEnd(remove(s0[id].arr.begin(), s0[id].arr.end(), -1));
//	    s0[id].arr.erase(newEnd);
//		}
//	}
//	for(int t_id=0;t_id<THREADS_NUM;t_id++){
//        s.arr.insert(s.arr.end(),s0[t_id].arr.begin(),s0[t_id].arr.end());
//	}
//	return s;
//}


index& SVS_serial(int* list, vector<index>& idx, int num) {
	index s0=idx[list[0]];
	for(int tool=0;tool<s0.arr.size();tool++)
        cout<<s0.arr[tool]<<" ";
    cout<<endl;

	for (int i = 1; i < num; i++) {
		for (int k = 0; k < s0.arr.size(); k++) {
			if (s0.arr[k] == -1)
				continue;
			bool isexit = false;
			for (int jj = 0; jj < idx[list[i]].arr.size(); jj++) {
				//if (s0.arr[k] < idx[list[i]].arr[jj])
				//break;
				if (s0.arr[k] == idx[list[i]].arr[jj]) {
					isexit = true;
					break;
				}
			}
			if (isexit == false) {
				s0.arr[k] = -1;
			}
		}
		/*vector<unsigned int>::iterator newEnd(remove(s0.arr.begin(), s0.arr.end(), -1));
		s0.arr.erase(newEnd);*/
	}

	return s0;
}

vector<index> idx;
int main() {

	fstream file;
	file.open("D://ExpIndex_Query//ExpIndex", ios::binary | ios::in);
	if (!file.is_open()) {
		cout << "No file";
		return 0;

	}
	cout << "infile" << endl;
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

	file.open("D://ExpIndex_Query//ExpQuery", ios::in);
	if (!file.is_open()) {
		cout << "No file";
		return 0;
	}
	cout << "infile" << endl;
	int query[1000][5] = { 0 };
	string line;
	int count = 0;
	while (getline(file, line)) {
		stringstream ss;
		ss << line;
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

	//struct timespec sts, ets;
	//timespec_get(&sts, TIME_UTC);
	clock_t start, _end;
	start = clock();
	for (int i = 0; i < 1; i++) {
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

		sorted(list, idx, num);
		//SVS_omp_baseonP(list, idx, num);
		index temp;
		temp=SVS_serial(list, idx, num);
		cout<<"start"<<endl;
		for(int i=0;i<temp.arr.size();i++)
            cout<<temp.arr[i]<<" ";
        cout<<endl;
		delete list;
	}
	_end = clock();
	cout << (_end - start) / 5;
	/*timespec_get(&ets, TIME_UTC);
	time_t dsec = ets.tv_sec - sts.tv_nsec;
	long dnsec = ets.tv_nsec - sts.tv_nsec;
	printf(" % llu. % 09llus\n", dsec, dnsec);*/
	return 0;
}

