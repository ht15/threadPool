#include<iostream>
#include<vector>
#include<stdio.h>
#include<algorithm>
using namespace std;

int main(){
	int n;
	cin>>n;
	vector<int> vec(n,0);
	int i=0;
	int m=n;
	while(n--){
		cin>>vec[i];
		i++;
	}
	int tmp1=0;
	int tmp2=0;
	sort(vec.begin(),vec.end(),[](const int &a,const int &b){return a>b;});
	tmp1=vec[0];
	for(int i=1;i<m;i++){
		if(tmp1>tmp2){
			tmp2+=vec[i];
		}else{
			tmp1+=vec[i];
		}
	}
	int res=max(tmp1,tmp2);
	cout<<res<<endl;

	return 0;
}
