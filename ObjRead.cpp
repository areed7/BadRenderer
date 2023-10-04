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
                //std::cout << "Line: " << line << std::endl;
                tokenizer(line, tokens, ' ');
                //std::cout << "\tTokens: " << tokens[0] << ", " << tokens[1] << ", " << tokens[2] << std::endl;
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
                        //std::cout << "\tToken["<<i<<"]: " << all_info_vert[0] << ", " << all_info_vert[1] << ", " << all_info_vert[2] << std::endl;
                        face.push_back(stoi(all_info_vert[0])-1);
                    }
                    m.faces.push_back(face);
                    //std::cout << "Face Index: " << face[0] << " " << face[1] << " " << face[2] << std::endl;
                }
            }
        }
        objFile.close();
    }
}

/*
int main(){
    Mesh teapot;
    ReadObj("cube.obj", teapot);
    //vector<string> parsedStr;
    //string test = "v -2.976687 1.920243 -0.081000";
    //tokenizer(test, parsedStr, ' ');

   
    return 0;
}*/