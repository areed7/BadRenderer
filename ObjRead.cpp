#include "ObjRead.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

void tokenizer(string input, vector<string>& parsedStr, char token){
    string res;
    for( int i = 0; i < input.size(); i++){
        
        //cout << "Equals Check : \"" << (char)input[i] << "\" == \"" << token << "\" " << (int)((char)input[i] == token) << endl;
        if((char)input[i] == token){
            parsedStr.push_back(res);
            res = "";
            continue;
        }
        if( i == input.size() -1){
            res.push_back(input[i]);
            parsedStr.push_back(res);
            return;
        }
        res.push_back(input[i]);
    }
}

void ReadObj(const char* name, vector<vert>& verts){
    ifstream objFile(name);
    string line;
    if( objFile.is_open()){
        while ( getline(objFile,line) )
        {
            if(line[0] == 'v'){
                vector<string> tokens;
                tokenizer(line, tokens, ' ');
                verts.push_back(vert(stod(tokens[1]), stod(tokens[2]), stod(tokens[3]), 1));
            }
            
        }
        objFile.close();
    }
}

/*
int main(){
    vector<vert> verts;
    ReadObj("teapot.obj", verts);
    //vector<string> parsedStr;
    //string test = "v -2.976687 1.920243 -0.081000";
    //tokenizer(test, parsedStr, ' ');

    for( int i = 0; i < 10; i++){
        cout << verts[i].x << " " << verts[i].y << " " << verts[i].z << endl;
    }
    return 0;
}*/