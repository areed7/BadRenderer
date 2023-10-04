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

void ReadObj(const char* name, Mesh& m){
    ifstream objFile(name);
    string line;
    if( objFile.is_open()){
        while ( getline(objFile,line) )
        {
            if(line[0] == 'v'){
                vector<string> tokens;
                tokenizer(line, tokens, ' ');
                m.verts.push_back(Vert(stod(tokens[1]), stod(tokens[2]), stod(tokens[3]), 1));
            }

            if(line[0] == 'f'){
                vector<string> tokens;
                tokenizer(line, tokens, ' ');
                vector<int> face;
                if(line.find('/') == std::string::npos){
                    for( int i = 1; i < tokens.size(); i++){
                        face.push_back(stoi(tokens[i])-1);
                    }
                    m.faces.push_back(face);
                } else{
                    for( int i = 1; i < tokens.size(); i++){
                        vector<string> all_info_vert; //Index 0 should be face index.
                        tokenizer(tokens[i], all_info_vert, '/');
                        face.push_back(stoi(all_info_vert[0])-1);
                    }
                    m.faces.push_back(face);
                }
            }
        }
        objFile.close();
    }
}

/*
int main(){
    mesh teapot;
    ReadObj("teapot.obj", teapot);
    //vector<string> parsedStr;
    //string test = "v -2.976687 1.920243 -0.081000";
    //tokenizer(test, parsedStr, ' ');

    for( int fi = 0; fi < teapot.faces.size(); fi++){
        vector<int> tmp = teapot.faces[fi];
        for (int vi = 0; vi < tmp.size(); vi++){
            std::cout << tmp[vi] << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}*/