#include <iostream>
#include <string>
#include <fstream>
#include <map>

using namespace std;

// TYPEDEFS

// PROTOTYPES

// FUNCTION DEFINITIONS

int part1(string fName)
{
  int x, z;
  int nCycle;
  int sigStrengthSum = 0;
  string s, instruction = "noop";
  uint8_t nCyclesToWork;
  map<string, uint8_t> cyclesPerInstruction{{"noop", 1}, {"addx", 2}};

  // Initialize "cpu"
  x = 1;
  nCycle = 1;
  nCyclesToWork = 0;

  // Open and scan through elf assembly
  ifstream fsObj(fName);
  while (true)
  {
    // Check to see if this is a cycle to record signal strength on
    if (!((nCycle - 20) % 40))
      sigStrengthSum += (x * nCycle);

    // Check if cpu needs to load an instruction and do so if needed
    if (!nCyclesToWork)
    {
      // Get instruction
      getline(fsObj, s);
      if (fsObj.eof())
        break;
      instruction = s.substr(0, 4);
      nCyclesToWork = cyclesPerInstruction[instruction];
    }

    // Check instructions
    if (instruction == "addx")
    {
      if (nCyclesToWork == 2)
        z = stoi(s.substr(5));
      else
        x += z;
    }
    // No-Op doens't need any action, so leaving statement off

    // Decrement work cycles to be done, increment total cycle count
    nCyclesToWork--;
    nCycle++;
  }
  fsObj.close();

  return sigStrengthSum;
}

string part2(string fName)
{
  int x, z;
  int nCycle;
  string outCrt;
  string s, instruction = "noop";
  uint8_t nCyclesToWork;
  map<string, uint8_t> cyclesPerInstruction{{"noop", 1}, {"addx", 2}};
  uint8_t row, col;
  string crt[7]; // Convenince to avoid overrun (could add a check at start of loop, but why do an extra IF each time through?)

  // Initialize "cpu"
  x = 1;
  nCycle = 1;
  nCyclesToWork = 0;

  // Open and scan through elf assembly
  ifstream fsObj(fName);
  while (true)
  {
    // Record the pixel for this cycle
    row = (nCycle - 1) / 40;
    col = (nCycle - 1) % 40;
    if ((col >= (x - 1)) && (col <= (x + 1)))
      crt[row] += "#";
    else
      crt[row] += ".";

    // Check if cpu needs to load an instruction and do so if needed
    if (!nCyclesToWork)
    {
      // Get instruction
      getline(fsObj, s);
      if (fsObj.eof())
        break;
      instruction = s.substr(0, 4);
      nCyclesToWork = cyclesPerInstruction[instruction];
    }

    // Check instructions
    if (instruction == "addx")
    {
      if (nCyclesToWork == 2)
        z = stoi(s.substr(5));
      else
        x += z;
    }
    // No-Op doens't need any action, so leaving statement off

    // Decrement work cycles to be done, increment total cycle count
    nCyclesToWork--;
    nCycle++;
  }
  fsObj.close();

  // Build the output crt image
  outCrt = "";
  for (row=0; row<6; row++)
  {
    outCrt += crt[row];
    outCrt += "\n";
  }

  return outCrt;
}

int main()
{
  uint32_t result;
  string outStr1 = "Signal Strength sum in ";
  string outStr2 = "CRT Screen in ";
  string fName;
  string result2;

  // // -- PART 1
  cout << "==============PART1==============" << endl;
  // Run on example input
  fName = "example.txt";
  result = part1(fName);
  cout << outStr1 + fName << " is " << result << endl;

  // Run on actual input
  fName = "input.txt";
  result = part1(fName);
  cout << outStr1 + fName << " is " << result << endl;

  // -- PART 2
  cout << endl
       << "==============PART2==============" << endl;
  // Run on example input
  fName = "example.txt";
  result2 = part2(fName);
  cout << outStr2 + fName << " looks like \n" << result2 << "\n\n" << endl;

  // Run on actual input
  fName = "input.txt";
  result2 = part2(fName);
  cout << outStr2 + fName << " looks like \n" << result2 << "\n\n" << endl;

  return 0;
}