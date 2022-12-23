#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <queue>
// #include <map>

using namespace std;

// TYPEDEFS & STRUCTS
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

typedef struct terrainStep
{
  char height;
  coordinate_t coord;
  bool isVisited = false;
  uint64_t roundVisited = 0;
  terrainStep() {}
  terrainStep(char h, coordinate_t c) : height(h), coord(c) {}
  terrainStep(char h, int x, int y) : height(h), coord(x, y) {}
} terrainStep_t;

typedef vector<vector<terrainStep_t>> matrix2d_t;

// PROTOTYPES
matrix2d_t buildGridFromFile(string fName);
terrainStep_t *findStart(matrix2d_t *pGrid);
vector<terrainStep_t *> findHeight(matrix2d_t *pGrid, char height);
bool checkIfNextStepValid(char oldHeight, coordinate_t *pNext, matrix2d_t *pGrid);
void clearStepQueues(queue<terrainStep_t *> *pStepQueueActive, queue<terrainStep_t *> *pStepQueueInactive);
void resetGridVisitedStatus(matrix2d_t *pGrid);
uint64_t findShortestPath(matrix2d_t *pGrid, queue<terrainStep_t *> *pStepQueueActive, queue<terrainStep_t *> *pStepQueueInactive, bool verbose);

// CLASS DEFINITIONS

// FUNCTION DEFINITIONS
matrix2d_t buildGridFromFile(string fName)
{
  string s;
  matrix2d_t grid;
  int x = 0, y = 0;
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;
    vector<terrainStep_t> v;
    for (char c : s)
    {
      v.push_back(terrainStep_t(c, x, y));
      x++;
    }
    grid.push_back(v);
    y++;
    x = 0;
  }
  fsObj.close();
  return grid;
}

terrainStep_t *findStart(matrix2d_t *pGrid)
{
  terrainStep_t *pOutStep;
  bool isFound = false;
  // Scan top-down; left-right in grid to find start
  for (auto it1 = pGrid->begin(); it1 != pGrid->end(); ++it1)
  {
    for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
    {
      if (it2->height == 'S')
      {
        isFound = true;
        pOutStep = &(*it2);
        break;
      }
    }
    if (isFound)
      break;
  }
  return pOutStep;
}

vector<terrainStep_t *> findHeight(matrix2d_t *pGrid, char height)
{
  vector<terrainStep_t *> outVec;

  // Scan top-down; left-right in grid to find all point with matching height
  for (auto it1 = pGrid->begin(); it1 != pGrid->end(); ++it1)
  {
    for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
    {
      if (it2->height == height)
        outVec.push_back(&(*it2));
    }
  }
  return outVec;
}

bool checkIfNextStepValid(char oldHeight, coordinate_t *pNext, matrix2d_t *pGrid)
{
  bool isValid = false;
  char newHeight;

  if (oldHeight == 'S')
    oldHeight = 'a';

  do
  {
    // Check if new coordinate is out of bounds of the grid
    if ((pNext->x < 0) || (pNext->y < 0))
      break;
    if (pNext->x >= pGrid->front().size())
      break;
    if (pNext->y >= pGrid->size())
      break;

    // Check if next step is greater than 1 step in height from current
    newHeight = pGrid->at(pNext->y)[pNext->x].height;
    if (newHeight == 'E')
    {
      if (oldHeight >= 'y')
        isValid = true;
    }
    else if ((oldHeight + 1) >= newHeight)
    {
      isValid = true;
    }
  } while (false);

  return isValid;
}

void clearStepQueues(queue<terrainStep_t *> *pStepQueueActive, queue<terrainStep_t *> *pStepQueueInactive)
{
  while (!pStepQueueActive->empty())
    pStepQueueActive->pop();
  while (!pStepQueueInactive->empty())
    pStepQueueInactive->pop();
}

void resetGridVisitedStatus(matrix2d_t *pGrid)
{
  // Scan through grid and reset isVisited flag and round visited on all points
  for (auto it1 = pGrid->begin(); it1 != pGrid->end(); ++it1)
  {
    for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
    {
      (*it2).isVisited = false;
      (*it2).roundVisited = 0;
    }
  }
}

uint64_t findShortestPath(matrix2d_t *pGrid, queue<terrainStep_t *> *pStepQueueActive, queue<terrainStep_t *> *pStepQueueInactive, bool verbose = false)
{
  uint64_t pathLength = 0;
  bool hasPathBeenFound = false;
  terrainStep_t *pCurStep;
  coordinate_t nextCoord;
  queue<terrainStep_t *> *pStepQueueTmp;

  // Crawl through the grid, finding the optimal path to the end (brute force style)
  while (!pStepQueueActive->empty())
  {
    while (!pStepQueueActive->empty())
    {
      // Get a step off of the active queue
      pCurStep = pStepQueueActive->front();
      pStepQueueActive->pop();

      // Mark step as visited
      pCurStep->roundVisited = pathLength;

      // Check if current coordinate is the end
      if (pCurStep->height == 'E')
      {
        hasPathBeenFound = true;
        break;
      }

      // Check for valid paths in each direction, adding them to the queue for the next round
      // Up
      nextCoord = pCurStep->coord + coordinate_t(0, -1);
      if (checkIfNextStepValid(pCurStep->height, &nextCoord, pGrid))
      {
        if (!(*pGrid)[nextCoord.y][nextCoord.x].isVisited)
        {
          pStepQueueInactive->push(&(*pGrid)[nextCoord.y][nextCoord.x]);
          (*pGrid)[nextCoord.y][nextCoord.x].isVisited = true;
        }
      }
      // Down
      nextCoord = pCurStep->coord + coordinate_t(0, 1);
      if (checkIfNextStepValid(pCurStep->height, &nextCoord, pGrid))
      {
        if (!(*pGrid)[nextCoord.y][nextCoord.x].isVisited)
        {
          pStepQueueInactive->push(&(*pGrid)[nextCoord.y][nextCoord.x]);
          (*pGrid)[nextCoord.y][nextCoord.x].isVisited = true;
        }
      }
      // Left
      nextCoord = pCurStep->coord + coordinate_t(-1, 0);
      if (checkIfNextStepValid(pCurStep->height, &nextCoord, pGrid))
      {
        if (!(*pGrid)[nextCoord.y][nextCoord.x].isVisited)
        {
          pStepQueueInactive->push(&(*pGrid)[nextCoord.y][nextCoord.x]);
          (*pGrid)[nextCoord.y][nextCoord.x].isVisited = true;
        }
      }
      // Right
      nextCoord = pCurStep->coord + coordinate_t(1, 0);
      if (checkIfNextStepValid(pCurStep->height, &nextCoord, pGrid))
      {
        if (!(*pGrid)[nextCoord.y][nextCoord.x].isVisited)
        {
          pStepQueueInactive->push(&(*pGrid)[nextCoord.y][nextCoord.x]);
          (*pGrid)[nextCoord.y][nextCoord.x].isVisited = true;
        }
      }
    }

    // If path has been found, break the outer loop
    if (hasPathBeenFound)
      break;

    // Swap the inactive and active queues
    pStepQueueTmp = pStepQueueActive;
    pStepQueueActive = pStepQueueInactive;
    pStepQueueInactive = pStepQueueTmp;

    // Increase the path length
    pathLength++;

    // Notify of status
    if (verbose)
      cout << "-->Worked path " << pathLength << " | " << pStepQueueActive->size() << " steps in queue to be processed " << endl;
  }

  // Ensure a path was actually found and that the search wasn't exited due to a path blockage
  if (!hasPathBeenFound)
  {
    pathLength = 0xFFFFFFFFFFFFFFFF;
  }

  return pathLength;
}

uint64_t part1(string fName)
{
  uint64_t pathLength = 0;
  bool hasPathBeenFound = false;
  string s;
  matrix2d_t grid;
  terrainStep_t *pCurStep;
  coordinate_t nextCoord;
  queue<terrainStep_t *> stepQueue1, stepQueue2;
  queue<terrainStep_t *> *pStepQueueInactive, *pStepQueueActive, *pStepQueueTmp;

  // Get map overlay/grid
  grid = buildGridFromFile(fName);

  // Find start
  pCurStep = findStart(&grid);

  // Initialize the queue ping-pong pointers
  pStepQueueActive = &stepQueue1;
  pStepQueueInactive = &stepQueue2;

  // Add current step to the queue to be processed in the while loop
  pStepQueueActive->push(pCurStep);
  pCurStep->isVisited = true;

  // Find the shortest path
  pathLength = findShortestPath(&grid, pStepQueueActive, pStepQueueInactive);

  return pathLength;
}

uint64_t part2(string fName)
{
  uint64_t pathLength, shortestPathLength;
  bool hasPathBeenFound = false;
  string s;
  matrix2d_t grid;
  terrainStep_t *pCurStep;
  vector<terrainStep_t *> potentialStarts;
  coordinate_t nextCoord;
  queue<terrainStep_t *> stepQueue1, stepQueue2;
  queue<terrainStep_t *> *pStepQueueInactive, *pStepQueueActive, *pStepQueueTmp;

  // Get map overlay/grid
  grid = buildGridFromFile(fName);

  // Find start
  pCurStep = findStart(&grid);

  // Initialize the queue ping-pong pointers
  pStepQueueActive = &stepQueue1;
  pStepQueueInactive = &stepQueue2;

  // Add current step to the queue to be processed in the while loop
  pStepQueueActive->push(pCurStep);
  pCurStep->isVisited = true;

  // Find the shortest path from start and set it as the current shortest path length
  shortestPathLength = findShortestPath(&grid, pStepQueueActive, pStepQueueInactive);

  // Get all steps with 'a' height
  potentialStarts = findHeight(&grid, 'a');

  // Loop through all potential starting points, finding the shortest of the shortest path between them all
  for (terrainStep_t *pStep : potentialStarts)
  {
    // Reset grid isVisited bools
    resetGridVisitedStatus(&grid);

    // Clear the step queues if they aren't already
    clearStepQueues(pStepQueueActive, pStepQueueInactive);

    // Add current step to the queue to be processed in the while loop
    pStepQueueActive->push(pStep);
    pStep->isVisited = true;

    // Find the shortest path from start and check if it's the shortest of all starts, updating if so
    pathLength = findShortestPath(&grid, pStepQueueActive, pStepQueueInactive);
    if (pathLength < shortestPathLength)
    {
      shortestPathLength = pathLength;
    }
  }

  return shortestPathLength;
}

int main()
{
  uint64_t result;
  string outStr1 = "Shortest Path Length in ";
  string outStr2 = "Shortest Path Length of all possible starts in ";
  string fName;

  // -- PART 1
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