#include <bits/stdc++.h>
using namespace std;
#define FILENAME "strlen.txt"

void readSpace(ifstream& file){
    string temp;
    while (file.peek()==' '){getline(file, temp, ' ');}
}

void readNext(ifstream& file, string& check, char delim){
    readSpace(file);
    getline(file, check, delim);
}
int main(){
    vector<string> bin_instruc, instructions;
    ifstream file;
    file.open(FILENAME);
    string check1, check3,temp;
    string check2;
    while (file.peek()!=EOF){
        readNext(file, check1, ' ');
        readNext(file, check2, ' ');
        readNext(file, check3, '\n');
        bin_instruc.push_back(check2);
        instructions.push_back(check3);
    }
    for (string instruc: bin_instruc){
        cout << instruc << "\n";
    }
    for (string instruc: instructions){
        cout << instruc << "\n";
    }
}