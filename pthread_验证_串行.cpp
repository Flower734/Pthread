#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <time.h>
#include <string>
#include <string.h>
#include <sstream>
#include <algorithm>

#define MAXSIZE 2000
using namespace std;
class index {
public:
	int len = 0;
	vector<unsigned int> arr;
};

bool operator<(const index& s1, const index& s2)
{
	return s1.len < s2.len; 
}
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
		unsigned int e = idx_[0].arr[t];
		s = 1;
		for (int i = 1; i < num; i++) {
			found = false;
			for (int j = 0; j < idx_[i].len; j++) {
				if (idx_[i].arr[j] == e) {
					found = true;
					break;
				}
			}
			if (found == false) {
				break;
			}
		}
		if (found == true) {
			S.push_back(e);
		}
	}
	return S;
}
vector<index> idx;
int main() {

	fstream file;
	file.open("D://ExpIndex_Query//ExpIndex", ios::binary | ios::in);
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
	file.open("D://ExpIndex_Query//ExpQuery", ios::in);
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
	/*-----------------------------------------------------------------------------------------------------------*/

	clock_t start, end;
	start = clock();
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
		delete list;
	}
	end = clock();
	cout <<  (end - start) /5 << endl;
	return 0;
}

