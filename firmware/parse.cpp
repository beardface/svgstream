#include<iostream>
#include<string>
#include<stdlib.h>
#include<vector>
using namespace std;

struct Scan{
  unsigned short start;
  unsigned short end;
};

int main ()
{
std::string cmd = "L13FF:03120323;0F230F24;F323F343;";

cout<<cmd<<std::endl;

unsigned short lineNumber = (unsigned short)strtol(cmd.substr(cmd.find("L")+1,cmd.find(":")-cmd.find("L")-1).c_str(),NULL,16);

std::vector<Scan> scanline;
int frontVal = cmd.find(":");
int nextVal = cmd.find(";");
while(nextVal != 0)
{
   Scan scan;
   scan.start = (unsigned short)strtol(cmd.substr(frontVal+1, nextVal-frontVal-5).c_str(), NULL, 16);
   scan.end   = (unsigned short)strtol(cmd.substr(nextVal-4,4).c_str(), NULL, 16);

   frontVal=nextVal;

   scanline.push_back(scan);

   if(nextVal == (int)cmd.length()-1) nextVal=0;
   else nextVal=cmd.find(";",nextVal+1);
}

cout << "Num Scans in Line ("<<lineNumber<<"): "<<scanline.size()<<std::endl;
for(unsigned int i = 0;
    i<scanline.size();
    ++i)
{
   cout <<scanline[i].start<<","<<scanline[i].end<<std::endl;
}
    
return 0;
}
