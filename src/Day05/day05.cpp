#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;


// TYPEDEFS
typedef struct
{
  unsigned int nMoves = 0;
  unsigned int source = 0;
  unsigned int dest = 0;
} stackOp_t;


// PROTOTYPES
vector<vector<char>> parseStacks(ifstream &fsObj);
stackOp_t parseInstruction(string s);


// FUNCTION DEFINITIONS
vector<vector<char>> parseStacks(ifstream &fsObj)
{
  string s;
  char c;
  bool endNow = false;
  bool isFirstLine = true;
  vector<vector<char>> dataVector;
  int nStacks = 0;

  while (!endNow)
  {
    // Read that line
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Column Index String found
    if (s.find("1") != string::npos)
    {
      endNow = true;
    }
    // Stack data found
    else
    {
      if (isFirstLine)
      {
        // Since space characters are used for formatting, length of string represents how many stacks are present
        nStacks = (s.length() - 3) / 4 + 1;
        isFirstLine = false;
        for (int i = 0; i < nStacks; i++)
        {
          vector<char> v;
          dataVector.push_back(v);
        }
      }
      // nStacks should be set now, so parse through the string and populate the stacks as appropriate
      for (int i = 0; i < nStacks; i++)
      {
        c = s[1 + i * 4];
        if (c != ' ')
        {
          dataVector[i].emplace(dataVector[i].begin(), c);
        }
      }
    }
  }
  return dataVector;
}

stackOp_t parseInstruction(string s)
{
  stackOp_t op;
  size_t pos1, pos2, pos3;
  // Minimize return calls w/ do-while(false)
  do
  {
    // Find delimiters
    pos1 = s.find("move ");
    if (pos1 == string::npos)
      break;
    pos2 = s.find("from");
    if (pos2 == string::npos)
      break;
    pos3 = s.find("to");
    if (pos3 == string::npos)
      break;
    op.nMoves = stoul(s.substr(5, pos2 - 5 - 1));
    op.source = stoul(s.substr(pos2 + 5, pos3 - (pos2 + 5) - 1));
    op.dest = stoul(s.substr(pos3 + 3));
  } while (false);

  return op;
}

string part1(string fName)
{
  vector<vector<char>> dataVector;
  string s;
  string outStr = "";
  stackOp_t myOp;

  // Open file, get stack data
  ifstream fsObj(fName);
  dataVector = parseStacks(fsObj);
  getline(fsObj, s); // Read blank line

  // Read and execute stack movement instructions
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    myOp = parseInstruction(s);

    for (int n = 0; n < myOp.nMoves; n++)
    {
      dataVector[myOp.dest - 1].push_back(dataVector[myOp.source - 1].back());
      dataVector[myOp.source - 1].pop_back();
    }
  }
  fsObj.close();

  // Get elements at top of each stack (technically back of the vectors)
  for (vector<char> v : dataVector)
  {
    outStr += v.back();
  }

  return outStr;
}

string part2(string fName)
{
  vector<vector<char>> dataVector;
  string s;
  string outStr = "";
  stackOp_t myOp;

  // Open file, get stack data
  ifstream fsObj(fName);
  dataVector = parseStacks(fsObj);
  getline(fsObj, s); // Read blank line

  // Read and execute stack movement instructions
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    myOp = parseInstruction(s);

    for (int n = myOp.nMoves; n > 0; n--)
    {
       // (vector.end() returns the iterator past the last object in the vector, so extra -1 is needed...)
      vector<char>::iterator it = dataVector[myOp.source - 1].end() - 1 -(n - 1);
      dataVector[myOp.dest - 1].push_back(*it);
      dataVector[myOp.source - 1].erase(it);
    }
  }
  fsObj.close();

  // Get elements at top of each stack (technically back of the vectors)
  for (vector<char> v : dataVector)
  {
    outStr += v.back();
  }

  return outStr;
}

int main()
{
  string result;

  // // -- PART 1
  cout << "==============PART1==============" << endl;
  // Run on example input
  result = part1("example.txt");
  cout << "Items at top of stack in example.txt are " << result << endl;

  // Run on actual input
  result = part1("input.txt");
  cout << "Items at top of stack in input.txt are " << result << endl;

  // -- PART 2
  cout << endl
       << "==============PART2==============" << endl;
  // Run on example input
  result = part2("example.txt");
  cout << "Items at top of stack in example.txt are " << result << endl;

  // Run on actual input
  result = part2("input.txt");
  cout << "Items at top of stack in input.txt are " << result << endl;

  return 0;
}