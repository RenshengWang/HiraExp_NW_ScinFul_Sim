/*
Author: R.Z. Wang   Date: Oct 23, 2014
Function: Read Configure file, whose format is fixed
( one key-index corresponses one value, the key-index is unique;)
( with "! " as the comment line beginning.)

How to use: 
with map<string,string> as the storage data structure,the value that you 
get is string, so you have to transfer the string format to int or double, if you need.
*/

#ifndef Config_H
#define Config_H 1

#include "iostream"
#include "fstream"
#include "sstream"
#include "string.h"
#include "stdlib.h"
#include "map"

using namespace std;

class Config
{
public:
  Config();
  ~Config();

public:
  static map<string,string> Config_Data;
  
  static Config* Config_Man;
  static Config* Get_Config_Man();
  
//  static void Read_ConfigFile_oneDimesion(string FileName_tem);
  static void Read_ConfigFile(string FileName_tem);
  static void Print_Ini_Info();

  string Get_ConfigValue_string(string Paras);  //{ return Config_Data[Paras.c_str()]; }
  int Get_ConfigValue_int(string Paras); //{ return atoi(Config_Data[Paras.c_str()].c_str()); }
  double Get_ConfigValue_double(string Paras); //{ return atof(Config_Data[Paras.c_str()].c_str()); }
  bool Get_ConfigValue_stringArray(string Paras,int lower_tem,int upper_tem,string* Value_tem);
  bool Get_ConfigValue_intArray(string Paras,int lower_tem,int upper_tem,int* Value_tem);
  bool Get_ConfigValue_doubleArray(string Paras,int lower_tem,int upper_tem,double* Value_tem);
};

#endif
