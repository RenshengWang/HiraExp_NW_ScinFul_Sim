#include "Config.hh"

//the map initial
map<string,string> Config::Config_Data;
Config* Config::Config_Man = 0;

Config::Config()
{;}

Config::~Config()
{;}

Config* Config::Get_Config_Man()
{
  if(Config_Man==0)
  {
    Config_Man = new Config();
    Config_Man->Read_ConfigFile("./Config/Initial.config");
    for(int i=0;i<10000;i++)
    {
      char FileName_tem[200];
      sprintf(FileName_tem,"Config_File_%d_0",i);
      if(Config_Data[FileName_tem].size()!=0)
      {
        Config_Man->Read_ConfigFile(Config_Data[FileName_tem].c_str());
      }
    }
  }
  
  return Config_Man;
}

void Config::Read_ConfigFile(string FileName_tem)
{
  ifstream infile(FileName_tem.c_str());
  cout<<"--> Reading File: "<<FileName_tem<<" ..."<<endl;
  char buf[1000];
  int buflen = 1000;
  while(infile)
  {
    infile.clear(ios::goodbit);
    infile.getline(buf,buflen);
    string buf_Word[1000]; int buf_Dimension = 0;

    for(int i=0;buf[i]!='\0';i++)
    {
      if(buf[i]=='!' || buf[i]=='#') { break; }
      //this line transfer the char array to the stream, which is more easier to identify the single word. 
      istringstream buf_stream(&(buf[i]));
      string word_tem;
      while(buf_stream>>word_tem)
      {
        buf_Word[buf_Dimension] = word_tem;
        buf_Dimension++;
      }
      cout<<"  "<<buf_Word[0]<<":  ";
      for(int j=1;j<buf_Dimension;j++)
      {
        char Name_tem[200];
        sprintf(Name_tem,"%s_%d",buf_Word[0].c_str(),j-1);
        if(Config_Data[Name_tem].size()==0)
        {
          Config_Data[Name_tem] = buf_Word[j];
          cout<<buf_Word[j]<<"  ";
        }
        else
        {
          cout<<Name_tem<<"already existed, the previous value is: "<<Config_Data[Name_tem]<<endl;
        }
      }
      cout<<endl;
      break;
    }
  }
  cout<<"Check the Information that you set, If right, click Enter, or Ctrl+c to kill this program!"<<endl;
  if(atoi(Config_Data["Is_Batch_mode_0"].c_str())==0){ getchar(); }
}

string Config::Get_ConfigValue_string(string Paras) 
{ 
  if(Config_Data[(Paras+"_0").c_str()].size()==0)
  {
    cout<<Paras<<"NOT existed! "<<endl;
    getchar();
    return "NotExisted";
  }
  else
  { return Config_Data[(Paras+"_0").c_str()]; }
  
}

int Config::Get_ConfigValue_int(string Paras) 
{ 
  if(Config_Data[(Paras+"_0").c_str()].size()==0)
  {
    cout<<Paras<<"NOT existed! "<<endl;
    getchar();
    return -999;
  }
  else
  { return atoi(Config_Data[(Paras+"_0").c_str()].c_str()); }  
}

double Config::Get_ConfigValue_double(string Paras) 
{ 
  if(Config_Data[(Paras+"_0").c_str()].size()==0)
  {
    cout<<Paras<<"NOT existed! "<<endl;
    getchar();
    return -999;
  }
  else
  { return atof(Config_Data[(Paras+"_0").c_str()].c_str()); }
}

bool Config::Get_ConfigValue_stringArray(string Paras,int lower_tem,int upper_tem,string* Value_tem)
{
  int ValueNum = 0; bool IsAllExisted = 1;
  for(int i=lower_tem;i<=upper_tem;i++)
  {
    char Name_tem[200];
    sprintf(Name_tem,"%s_%d",Paras.c_str(),i);
    if(Config_Data[Name_tem].size()==0)
    { cout<<Name_tem<<" NOT existed!"<<endl; Value_tem[ValueNum] = "NotExisted"; getchar(); IsAllExisted=0;}
    else
    { Value_tem[ValueNum] = Config_Data[Name_tem]; }
    ValueNum++;
  }
  if(IsAllExisted==1){ return 1; }
  else{ return 0; }
}

bool Config::Get_ConfigValue_intArray(string Paras,int lower_tem,int upper_tem,int* Value_tem)
{
  int ValueNum = 0; bool IsAllExisted = 1;
  for(int i=lower_tem;i<=upper_tem;i++)
  {
    char Name_tem[200];
    sprintf(Name_tem,"%s_%d",Paras.c_str(),i);
    if(Config_Data[Name_tem].size()==0)
    { cout<<Name_tem<<" NOT existed!"<<endl; Value_tem[ValueNum] = -999; getchar(); IsAllExisted=0;}
    else
    { Value_tem[ValueNum] = atoi((Config_Data[Name_tem]).c_str()); }
    ValueNum++;
  }
  if(IsAllExisted==1){ return 1; }
  else{ return 0; }
}

bool Config::Get_ConfigValue_doubleArray(string Paras,int lower_tem,int upper_tem,double* Value_tem)
{
  int ValueNum = 0; bool IsAllExisted = 1;
  for(int i=lower_tem;i<=upper_tem;i++)
  {
    char Name_tem[200];
    sprintf(Name_tem,"%s_%d",Paras.c_str(),i);
    if(Config_Data[Name_tem].size()==0)
    { cout<<Name_tem<<" NOT existed!"<<endl; Value_tem[ValueNum] = -999; getchar(); IsAllExisted=0;}
    else
    { Value_tem[ValueNum] = atof((Config_Data[Name_tem]).c_str()); }
    ValueNum++;
  }
  if(IsAllExisted==1){ return 1; }
  else{ return 0; }
  
}

void Config::Print_Ini_Info()
{
  cout<<"-->Print Initial Info: "<<endl;
  map<string,string>::iterator Config_Data_Itr;
  for(Config_Data_Itr=Config_Data.begin();Config_Data_Itr!=Config_Data.end();++Config_Data_Itr)
  {
    cout<<"  "<<Config_Data_Itr->first<<"      "<<Config_Data_Itr->second<<endl;
  }
}

/*
void Config::Read_ConfigFile_oneDimesion(string FileName_tem)
{
  cout<<"--> Reading Initial File: "<<FileName_tem<<endl;
  ifstream infile(FileName_tem.c_str());
  if(!infile.good())
  {
    cout<<FileName_tem<<" NOT Exist!"<<endl; getchar(); return;
  }
  
  string tag; string Value_tem;
  while(infile)
  {
    infile>>tag;
    if(tag=="!") { infile.ignore(999,'\n'); }
    else
    {
      infile>>Value_tem;
      if(Config_Data[tag.c_str()].size()==0)
      {
        Config_Data[tag.c_str()]=Value_tem;
        cout<<tag<<"----"<<Value_tem<<endl;
      }
      else
      {
        cout<<tag<<"already existed, the previous value is: "<<Config_Data[tag.c_str()]<<endl;
      }
      infile.ignore(999,'\n'); 
      char first; infile.get(first);
      infile.putback(first);
    }
  }
  cout<<"Check the Information that you set, If right, click Enter, or Ctrl+c to kill this program!"<<endl;
  getchar();
}
*/
