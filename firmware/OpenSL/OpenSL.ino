#include<iostream>
#include<string>
#include<stdlib.h>

struct Scan{
  unsigned short start;
  unsigned short end;
};

union Point{
  unsigned long lValue;
  unsigned short sValues[2];
};

void setup(){
  Serial.begin(9600);
}

void loop(){
  char lineNumberStr[5] = {0};
  
  int scanCount = 0;
  Scan scanline[1234] = {0};
  char buffer[1000] = {0};
  unsigned short lineNumber;
  
  while (Serial.available() > 0)
  {
    scanCount = 0;
    if(Serial.read()=='L')
    {
      Serial.print('L');
      int bytesRead = Serial.readBytesUntil('\n', buffer, 1000);
      Serial.println(buffer);
      //Scan Line Coming
      int byteI=0;
      while(byteI < 4 && byteI<bytesRead && buffer[byteI] != ':')
      {
         lineNumberStr[byteI] = buffer[byteI];
         ++byteI;
      }
      lineNumber = strtol(lineNumberStr, NULL, 16);
      ++byteI; //Skip over ':'
      
      while(byteI < bytesRead && buffer[byteI] != '\n')
      {
        char hexStr[9] = {0};
        Point p;
        Scan lScan;
        int tbyteRead = 0;
        while(byteI != ';' && tbyteRead < 8)
        {
          hexStr[tbyteRead] = buffer[byteI];
          ++tbyteRead;
          ++byteI;
        }
        p.lValue = (unsigned long)strtoul(hexStr, NULL, 16);
        lScan.start = p.sValues[1];
        lScan.end   = p.sValues[0];
       
        scanline[scanCount] = lScan;
        scanCount++;
        
        ++byteI; //SKip over ';'
      }
      
      Serial.print("L->");
      Serial.println(lineNumber);
      for(int i =0;
          i < scanCount;
          ++i)
      {
         Serial.print("S->");
         Serial.print(scanline[i].start);
         Serial.print(" E->");
         Serial.println(scanline[i].end);
      }
    }
  }
}
