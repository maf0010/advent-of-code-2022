#include <iostream>
#include <string>
#include <fstream>
#include <map>

using namespace std;

void parseStr2arr(string s, int a[2], int b[2])
{
  // String is formatted as "n-m,x-y", with n,m,x,y being various-digit integers
  size_t pos1 = 0, pos2;
  pos2 = s.find("-", pos1);
  a[0] = stoi(s.substr(pos1, pos2 - pos1));
  pos1 = pos2 + 1;
  pos2 = s.find(",", pos1);
  a[1] = stoi(s.substr(pos1, pos2 - pos1));
  pos1 = pos2 + 1;
  pos2 = s.find("-", pos1);
  b[0] = stoi(s.substr(pos1, pos2 - pos1));
  b[1] = stoi(s.substr(pos2 + 1));
}

int identifyContainment(int a[2], int b[2])
{
  int isContained = 0;
  // If a.lowerBound == b.lowerBound, then guarenteed one dataset encases the other
  if (a[0] == b[0])
    isContained = 1;
  // If a.lowerBound < b.lowerBound, possible containment if a.upperBound >= b.upperBound
  else if ((a[0] < b[0]) && (a[1] >= b[1]))
    isContained = 1;
  // If b.lowerBound < a.lowerBound, possible containment if b.upperBound >= a.upperBound
  else if ((b[0] < a[0]) && (b[1] >= a[1]))
    isContained = 1;
  return isContained;
}

int identifyOverlap(int a[2], int b[2])
{
  int isOverlap = 0;
  // If a.lowerBound == b.lowerBound, then guarenteed overlap
  if (a[0] == b[0])
    isOverlap = 1;
  // If a.lowerBound < b.lowerBound, possible overlap if a.upperBound >= b.lowerBound
  else if ((a[0] < b[0]) && (a[1] >= b[0]))
    isOverlap = 1;
  // If b.lowerBound < a.lowerBound, possible containment if b.upperBound >= a.lowerBound
  else if ((b[0] < a[0]) && (b[1] >= a[0]))
    isOverlap = 1;
  return isOverlap;
}

int part1(string fName)
{
  string s;
  int a[2], b[2];
  int nContainments = 0;

  // Open and scan through the file
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Parse string to numerical arrays
    parseStr2arr(s, a, b);

    // Determine if a,b contain each other, adding to running total
    nContainments += identifyContainment(a, b);
  }
  fsObj.close();

  return nContainments;
}

int part2(string fName)
{
  string s;
  int a[2], b[2];
  int nOverlaps = 0;

  // Open and scan through the file
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Parse string to numerical arrays
    parseStr2arr(s, a, b);

    // Determine if a,b overlap each other, adding to running total
    nOverlaps += identifyOverlap(a, b);
  }
  fsObj.close();

  return nOverlaps;
}

int main()
{
  int result;

  // -- PART 1
  cout << "==============PART1==============" << endl;
  // Run on example input
  result = part1("example.txt");
  cout << "Number of containments from example.txt is " << result << endl;

  // Run on actual input
  result = part1("input.txt");
  cout << "Number of containments from input.txt is " << result << endl;

  // -- PART 2
  cout << endl
       << "==============PART2==============" << endl;
  // Run on example input
  result = part2("example.txt");
  cout << "Number of overlaps from example.txt is " << result << endl;

  // Run on actual input
  result = part2("input.txt");
  cout << "Number of overlaps from input.txt is " << result << endl;

  return 0;
}