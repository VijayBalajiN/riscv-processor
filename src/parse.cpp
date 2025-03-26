#include <string>
#include <fstream>  // For ifstream
#include <iostream>
#include <vector>
#include <tuple>

#include "parse.h"

using namespace std;

void readSpace(ifstream& file){
    string temp;
    while (file.peek()==' '){getline(file, temp, ' ');}
}

void readNext(ifstream& file, string& check, char delim){
    readSpace(file);
    getline(file, check, delim);
}


tuple<vector<string>, vector<string> > get_code(string filename){
    vector<string> bin_instruc, instructions;

    ifstream file;
    file.open(filename);

    string check1, check2, check3;

    while (file.peek()!=EOF){
        readNext(file, check1, ' ');
        readNext(file, check2, ' ');
        readNext(file, check3, '\n');
        if (check3[check3.length()-1]=='\r'){check3.pop_back();}
        bin_instruc.push_back(check2);
        instructions.push_back(check3);
    }


    return make_tuple(bin_instruc, instructions);
}