#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

// TYPEDEFS
typedef struct coordinate
{
  int x = 0;
  int y = 0;
  coordinate() {}
  coordinate(int x, int y) : x(x), y(y) {}
  coordinate(const coordinate &c) : x(c.x), y(c.y) {}
  coordinate operator+(const coordinate &c) const { return coordinate(x + c.x, y + c.y); }
  coordinate operator-(const coordinate &c) const { return coordinate(x - c.x, y - c.y); }
  bool const operator==(const coordinate &c) const { return ((x == c.x) && (y == c.y)); }
  bool const operator<(const coordinate &c) const { return ((x < c.x) || (x == c.x && y < c.y)); }
} coordinate_t;

// PROTOTYPES
int signum(int x);
coordinate_t determineTailMovement(coordinate_t delta);

// FUNCTION DEFINITIONS
int signum(int x)
{
  return (((int)0 < x) - (x < (int)0));
}

coordinate_t parseMoveInstruction(char c)
{
  static map<char, coordinate> c2Move = {
      {'R', coordinate_t(1, 0)},
      {'U', coordinate_t(0, 1)},
      {'L', coordinate_t(-1, 0)},
      {'D', coordinate_t(0, -1)},
  };
  map<char, coordinate_t>::iterator it;
  coordinate_t retMove(0, 0);
  it = c2Move.find(c);
  if (it != c2Move.end())
    retMove = it->second;
  return retMove;
}

coordinate_t determineTailMovement(coordinate_t delta)
{
  static map<coordinate, coordinate> dirMap = {
      {coordinate_t(0, 0), coordinate_t(0, 0)},
      {coordinate_t(1, -1), coordinate_t(0, 0)},
      {coordinate_t(1, 0), coordinate_t(0, 0)},
      {coordinate_t(1, 1), coordinate_t(0, 0)},
      {coordinate_t(0, 1), coordinate_t(0, 0)},
      {coordinate_t(-1, 1), coordinate_t(0, 0)},
      {coordinate_t(-1, 0), coordinate_t(0, 0)},
      {coordinate_t(-1, -1), coordinate_t(0, 0)},
      {coordinate_t(0, -1), coordinate_t(0, 0)},
      {coordinate_t(2, -1), coordinate_t(1, -1)},
      {coordinate_t(2, 0), coordinate_t(1, 0)},
      {coordinate_t(2, 1), coordinate_t(1, 1)},
      {coordinate_t(1, 2), coordinate_t(1, 1)},
      {coordinate_t(0, 2), coordinate_t(0, 1)},
      {coordinate_t(-1, 2), coordinate_t(-1, 1)},
      {coordinate_t(-2, 1), coordinate_t(-1, 1)},
      {coordinate_t(-2, 0), coordinate_t(-1, 0)},
      {coordinate_t(-2, -1), coordinate_t(-1, -1)},
      {coordinate_t(-1, -2), coordinate_t(-1, -1)},
      {coordinate_t(0, -2), coordinate_t(0, -1)},
      {coordinate_t(1, -2), coordinate_t(1, -1)},
      {coordinate_t(-2, -2), coordinate_t(-1, -1)},
      {coordinate_t(-2, 2), coordinate_t(-1, 1)},
      {coordinate_t(2, -2), coordinate_t(1, -1)},
      {coordinate_t(2, 2), coordinate_t(1, 1)},
  };
  map<coordinate_t, coordinate_t>::iterator it;
  coordinate_t retMove(0, 0);
  it = dirMap.find(delta);
  if (it != dirMap.end())
    retMove = it->second;
  return retMove;
}

uint32_t part1(string fName)
{
  coordinate_t head(0, 0), tail(0, 0);
  coordinate_t move, delta;
  string s;
  char c;
  int iterations, i;
  vector<coordinate_t> headHist, tailHist, tailHistUnique;

  // Record starting history
  headHist.push_back(head);
  tailHist.push_back(tail);
  if (find(tailHistUnique.begin(), tailHistUnique.end(), tail) == tailHistUnique.end())
    tailHistUnique.push_back(tail);

  // Open file, enacting movement instructions specified within
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;
    iterations = stoi(s.substr(2));
    move = parseMoveInstruction(s[0]);
    for (i = 0; i < iterations; i++)
    {
      // Make the head/tail movement
      head = head + move;
      delta = head - tail;
      tail = tail + determineTailMovement(delta);
      // Record the history
      headHist.push_back(head);
      tailHist.push_back(tail);
      if (find(tailHistUnique.begin(), tailHistUnique.end(), tail) == tailHistUnique.end())
        tailHistUnique.push_back(tail);
    }
  }
  fsObj.close();

  return tailHistUnique.size();
}

uint32_t part2(string fName)
{
  coordinate_t rope[10] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
  coordinate_t move, delta;
  string s;
  char c;
  int iterations, i, j;
  vector<coordinate_t> hist[10], tailHistUnique;

  // Record starting history
  for (j = 0; j < 10; j++)
  {
    hist[j].push_back(rope[j]);
  }
  if (find(tailHistUnique.begin(), tailHistUnique.end(), rope[9]) == tailHistUnique.end())
    tailHistUnique.push_back(rope[9]);

  // Open file, enacting movement instructions specified within
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;
    iterations = stoi(s.substr(2));
    move = parseMoveInstruction(s[0]);
    for (i = 0; i < iterations; i++)
    {
      // Make the head/tail movement
      rope[0] = rope[0] + move;
      hist[0].push_back(rope[0]);
      for (j = 1; j < 10; j++)
      {
        delta = rope[j - 1] - rope[j];
        rope[j] = rope[j] + determineTailMovement(delta);
        hist[j].push_back(rope[j]);
      }
      // Record unique history of the tail
      if (find(tailHistUnique.begin(), tailHistUnique.end(), rope[9]) == tailHistUnique.end())
        tailHistUnique.push_back(rope[9]);
    }
  }
  fsObj.close();

  return tailHistUnique.size();
}

int main()
{
  uint32_t result;
  string outStr1 = "Number of unique positions visited by the tail in ";
  string outStr2 = "Number of unique positions visited by the last knot in ";
  string fName;

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
  result = part2(fName);
  cout << outStr2 + fName << " is " << result << endl;

  // Run on actual input
  fName = "input.txt";
  result = part2(fName);
  cout << outStr2 + fName << " is " << result << endl;

  return 0;
}