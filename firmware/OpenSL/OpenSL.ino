#include<iostream>
#include<string>
#include<stdlib.h>

#include "TimerOne.h"

struct Scan{
  unsigned short start;
  unsigned short end;
};

struct ScanLine{
  unsigned short  xsegmentCnt;
  Scan xsegment[5];
  unsigned short y;
};

bool newScan = false;
unsigned short lineNumber;

volatile unsigned long scanline_index;
volatile unsigned long scanline_count;
volatile ScanLine scans[100] = {0};
int scanCount = 0;

volatile unsigned short xGalvoPos = 0; // use volatile for shared variables
volatile unsigned short yGalvoPos = 0; // use volatile for shared variables
volatile unsigned short MIN_GALVO_POS = 0;
volatile unsigned long scanTurnCount = 0;
volatile int scanDir = 1;

//Print Quality and Speed Settings
volatile int TIMER_SPEED = 10; //TIMER_SPEED/16MhZ -- interrupt for updating galvo Pos
volatile int RESOLUTION = 30; //Lower number == higher resolution, slower prints.
volatile int NUM_SCANS = 1;   //How many times to scan a single X line.  More scans == stronger cure & cost of speed.
volatile int XLEAD_DISTANCE = 300; //Pre / Post travel distance to smooth out X scans

union Point{
  unsigned long lValue;
  unsigned short sValues[2];
};

void initialize_scan_test(void)
{
  //Small Box to Scan
   unsigned short yStart = 20000;
   for(int y = 0; y <= 50; y++)
   {
       ScanLine line;
       if(y<=10 || y >= 40)
       {
         scans[scanline_index].xsegmentCnt = 1;
         scans[scanline_index].xsegment[0].start=20000;
         scans[scanline_index].xsegment[0].end  =25000;
         scans[scanline_index].y = yStart+(y*100);
       }
       else
       {
         scans[scanline_index].xsegmentCnt = 2;
         scans[scanline_index].xsegment[0].start=20000;
         scans[scanline_index].xsegment[0].end  =21000;
         scans[scanline_index].xsegment[1].start=24000;
         scans[scanline_index].xsegment[1].end  =25000;
         scans[scanline_index].y = yStart+(y*100);
         
       }
       
       scanline_index++;
   }
   
   MIN_GALVO_POS = 20000;
   scanline_count = scanline_index;
   scanline_index = 0;
}

void updateGalvos_timed(void)
{
    if(xGalvoPos > scans[scanline_index].xsegment[scans[scanline_index].xsegmentCnt-1].end+XLEAD_DISTANCE)
    {
      scanDir = -1;
      scanTurnCount = scanTurnCount + 1;
    }
    
    xGalvoPos = xGalvoPos + scanDir*RESOLUTION;
    
    //TODO Add logic to check to see if xGalvoPos is inside of a segment
    //if yes, turn on the laser
    //if no, turn off the laser
    
    if(scanDir == -1)
    {
      if(xGalvoPos < MIN_GALVO_POS)
      {
         scanDir = 1;
         scanTurnCount = scanTurnCount + 1;
      }
    }
    
    if(scanTurnCount >= NUM_SCANS)
    {
      scanTurnCount = 0;
      scanline_index++;
      if(scanline_index >= scanline_count)
      {
        xGalvoPos = 0;
        scanTurnCount = 0;
        //Layer Complete
        Timer1.stop();
      }
      else
      {
        if(scans[scanline_index].xsegmentCnt > 0)
        {
           xGalvoPos=scans[scanline_index].xsegment[0].start-XLEAD_DISTANCE;
           yGalvoPos=scans[scanline_index].y;
        }
      }
    }
    
      
}

void setup(){
  initialize_scan_test();
  
  Timer1.initialize(TIMER_SPEED);
  Timer1.attachInterrupt(updateGalvos_timed); // blinkLED to run every 0.15 seconds
  
  Serial.begin(9600);
}
/*
void parseCommand()
{
  char lineNumberStr[5] = {0};
  char buffer[1000] = {0}; 
  newScan = false;
  if (Serial.available() > 0)
  {
    if(Serial.read()=='L')
    {
      newScan = true;
      scanCount = 0;
      int bytesRead = Serial.readBytesUntil('\n', buffer, 1000);
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
       
        scans[scanCount] = lScan;
        scanCount++;
        
        ++byteI; //SKip over ';'
      }
    } 
  } 
}
*/
void loop(){
   unsigned long scanTurnCountCopy = 0;
   unsigned short xGalvoPosCopy = 0;
   unsigned short yGalvoPosCopy = 0;
   
   /* TODO Fix Scan Line Storage
   parseCommand();
   
   if(newScan)
   {      
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
   */
   delay(100);
  
   noInterrupts();
   scanTurnCountCopy = scanTurnCount;
   xGalvoPosCopy = xGalvoPos;
   yGalvoPosCopy = yGalvoPos;
   interrupts();
   
   Serial.print("Scans: ");
   Serial.print(scanTurnCount);
   Serial.print(" XPOS: ");
   Serial.print(xGalvoPosCopy);
   Serial.print(" YPOS: ");
   Serial.println(yGalvoPosCopy);
   
}
