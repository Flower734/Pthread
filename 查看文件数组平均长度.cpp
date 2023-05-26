#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <sstream>
using namespace std;
class index {
public:
	int len = 0;
	vector<unsigned int> arr;
};
vector<index> idx;
int query[1000][5] = { 0 };

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
	else {
		cout << "infile" << endl;

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


	}
	cout<<"start"<<endl;
	int count=0;

	for(int i=0;i<=1000;i++)
    {
        cout<<idx[i].len<<" ";
        if(idx[i].len==30000)
            count++;
    }
    cout<<endl;
    cout<<count<<endl;
    cout<<"end"<<endl;

    int sum[1000]={0};

    for(int i=0;i<1000;i++)
    {
        for(int j=0;j<5;j++)
        {
            if(query[i][j]!=0)
                sum[i]++;
        }
        cout<<sum[i]<<" ";
    }
cout<<endl;
int t1=0;
int t2=0;int t3=0;int t4=0;int t5=0;
for(int i=0;i<1000;i++)
{
    if(sum[i]==3)
        t3++;
    if(sum[i]==1)
        t1++;
    if(sum[i]==2)
        t2++;
    if(sum[i]==4)
        t4++;
    if(sum[i]==5)
        t5++;

}

cout<<t1<<endl;
cout<<t2<<endl;
cout<<t3<<endl;
cout<<t4<<endl;
cout<<t5<<endl;

return 0;

}
