#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tuple>
#include <algorithm>

using namespace std;

string trim(string s){
    bool start=true;
    string temp;
    int i=0;
    for (; i<s.length(); i++){
        if (s[i]==' '){
            continue;
        }
        else {
            start=false;
            break;
        }
    }
    while ( i<s.length()){
        temp.push_back(s[i]);
        i++;
    }
    for (int i=temp.length()-1; i>=0; i--){
        if (temp[i]==' '){
            temp.pop_back();
        }
        else{break;}
    }
    return temp;
}

void print_csv(vector <string> v){
    fstream file;
    file.open("build/output.csv");
    int length = 0;

    for (int i = 0; i < v.size(); i++) {
        length =((count(v[i].begin(),v[i].end(), ';')+1) > length ? (count(v[i].begin(),v[i].end(), ';')+1) : length);
    }
    // cout << length;
    for (int i=0; i<length; i++){
        if (i==0){file << "Instruction Number,";}
        else{
            file << "C" << i << ",";
        }
    }
    file << "\n";
    for (int i =1; i<=v.size(); i++){
        
        string data=v[i-1];
        stringstream ss;
        ss <<data;
        string temp;
        bool first=true;
        while (getline(ss, temp, ';')){
            if (! first){file << ",";}
            else {first =false;}
            temp=trim(temp);
            if (temp ==""){
                file << "-";
            }
            else{
                file << temp;
            }
        }
        file << '\n';
    }
}
