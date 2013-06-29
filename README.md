svgstream
=========

Scripts to stream svg files to a SL printer

Scan Line Syntax:
 L####:########;########;...;

 L#### - Scan Line Number (In Hex)
 ########; Scan Segment, start and end
  First four bytes are the start of the segment
  Next four bytes are end of the segment

 Can have any number of these in a line.

