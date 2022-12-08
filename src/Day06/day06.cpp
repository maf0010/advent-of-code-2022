#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

// TYPEDEFS

// PROTOTYPES
void scanStream(ifstream &fsObj, char *buf, uint8_t bufDepth, uint32_t scanDepth);
uint8_t checkRepeatingChars(char *buf, uint8_t bufDepth);

// FUNCTION DEFINITIONS
void scanStream(ifstream &fsObj, char *buf, uint8_t bufDepth, uint32_t scanDepth = 1)
{
  for (uint32_t i = 0; i < scanDepth; i++)
  {
    for (uint8_t j = 0; j < (bufDepth - 1); j++)
    {
      buf[j] = buf[j + 1];
    }
    fsObj.get(buf[bufDepth - 1]);
  }
}

uint8_t checkRepeatingChars(char *buf, uint8_t bufDepth)
{
  uint8_t scansToBumpRepeater = 0;
  bool repeatFound = false;
  uint8_t i, j;
  for (i = 0; i < (bufDepth - 1); i++)
  {
    for (j = i + 1; j < bufDepth; j++)
    {
      if (buf[i] == buf[j])
      {
        repeatFound = true;
        break;
      }
    }
    if (repeatFound)
    {
      break;
    }
  }
  if (repeatFound)
  {
    scansToBumpRepeater = i + 1;
  }
  return scansToBumpRepeater;
}

int part1(string fName)
{
  char buf[4];
  const uint8_t bufDepth = 4;
  int scanCounter = 4;
  uint8_t nScansToBump;

  // Open file, populate buffer with first 4
  ifstream fsObj(fName);
  scanStream(fsObj, buf, bufDepth, bufDepth);

  // Read and execute stack movement instructions
  while (true)
  {
    // Check for repeaters and get number of scans to bump it out if applicable
    nScansToBump = checkRepeatingChars(buf, bufDepth);
    if (!nScansToBump)
      break;

    // Scan through buffer if not at EOF
    if (fsObj.eof())
    {
      cout << "EOF Reached without finding pattern!" << endl;
      scanCounter = -1;
      break;
    }
    scanStream(fsObj, buf, bufDepth, nScansToBump);
    scanCounter += nScansToBump;
  }
  fsObj.close();

  return scanCounter;
}

int part2(string fName)
{
  char buf[14];
  const uint8_t bufDepth = 14;
  int scanCounter = 14;
  uint8_t nScansToBump;

  // Open file, populate buffer with first 14
  ifstream fsObj(fName);
  scanStream(fsObj, buf, bufDepth, bufDepth);

  // Read and execute stack movement instructions
  while (true)
  {
    // Check for repeaters and get number of scans to bump it out if applicable
    nScansToBump = checkRepeatingChars(buf, bufDepth);
    if (!nScansToBump)
      break;

    // Scan through buffer if not at EOF
    if (fsObj.eof())
    {
      cout << "EOF Reached without finding pattern!" << endl;
      scanCounter = -1;
      break;
    }
    scanStream(fsObj, buf, bufDepth, nScansToBump);
    scanCounter += nScansToBump;
  }
  fsObj.close();

  return scanCounter;
}

int main()
{
  int result;

  // // -- PART 1
  cout << "==============PART1==============" << endl;
  // Run on example input
  result = part1("example.txt");
  cout << "Characters processed before start of packet pattern detected in example.txt is " << result << endl;

  // Run on actual input
  result = part1("input.txt");
  cout << "Characters processed before start of packet pattern detected in input.txt is " << result << endl;

  // -- PART 2
  cout << endl
       << "==============PART2==============" << endl;
  // Run on example input
  result = part2("example.txt");
  cout << "Characters processed before start of message pattern detected in example.txt is " << result << endl;

  // Run on actual input
  result = part2("input.txt");
  cout << "Characters processed before start of message pattern detected in input.txt is " << result << endl;

  return 0;
}