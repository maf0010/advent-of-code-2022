#include <iostream>
#include <string>
#include <fstream>
#include <vector>

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

typedef enum spaceFill
{
  AIR = 0,
  ROCK = 1,
  SAND = 2,
  SOURCE = 3,
} spaceFill_t;

typedef struct gridItem
{
  coordinate_t coord;
  bool isOccupied = false;
  spaceFill_t fillType = AIR;
  gridItem() {}
  gridItem(coordinate_t c) : coord(c) {}
  gridItem(int x, int y) : coord(x, y) {}
} gridItem_t;

typedef vector<vector<gridItem_t>> matrix2d_t;

// PROTOTYPES
int signum(int x);
void getRockFilledArea(string fName, vector<coordinate_t> &rockArea, int &minX, int &maxX, int &maxY);
vector<gridItem_t> makeNewGridVector(int nElements, int xStart, int yStart, bool varyX);
void buildScanGrid(matrix2d_t &scanGrid, vector<coordinate_t> &rockArea, int minX, int maxX, int maxY);
void addBottomToGrid(matrix2d_t &scanGrid, int yStepsToBottom);
void printScanGrid(matrix2d_t &scanGrid, int minX, int maxX, int maxY);
bool simulateSandUnit(matrix2d_t &scanGrid, coordinate_t sourceLoc, int minX, int maxX, int maxY);
bool simulateSandUnitPart2(matrix2d_t &scanGrid, coordinate_t sourceLoc, int minX, int maxX, int maxY, int &fullMinX, int &fullMaxX, matrix2d_t &overLeftT, matrix2d_t &overRightT);

// CLASS DEFINITIONS

// FUNCTION DEFINITIONS
int signum(int x)
{
  return (((int)0 < x) - (x < (int)0));
}

void getRockFilledArea(string fName, vector<coordinate_t> &rockArea, int &minX, int &maxX, int &maxY)
{
  string s;
  size_t pos1, pos2;
  coordinate_t c1, c2;
  vector<string> tmpV;
  const string lineDivider = " -> ";
  int i, x, y, d;

  // Initialize the min and max values
  minX = maxX = 500;
  maxY = 0;

  // Open the file and extract the rock formation data, one line at a time
  ifstream fsObj(fName);
  while (true)
  {
    // Get first pair
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Get temporary vector of coordinate strings
    pos1 = 0;
    pos2 = s.find(lineDivider, pos1);
    while (pos2 != string::npos)
    {
      tmpV.push_back(s.substr(pos1, pos2 - pos1));
      pos1 = pos2 + lineDivider.length();
      pos2 = s.find(lineDivider, pos1);
    }
    // Add the last one
    tmpV.push_back(s.substr(pos1));

    // Walk through the list of line verticies and add all points to the rock area
    for (i = 0; i < tmpV.size() - 1; i++)
    {
      // Get the vericies as coordinates
      pos2 = tmpV[i].find(',');
      c1 = {stoi(tmpV[i].substr(0, pos2)), stoi(tmpV[i].substr(pos2 + 1))};
      pos2 = tmpV[i + 1].find(',');
      c2 = {stoi(tmpV[i + 1].substr(0, pos2)), stoi(tmpV[i + 1].substr(pos2 + 1))};

      // Update the grid max/min if needed
      if (c1.x < minX)
        minX = c1.x;
      else if (c1.x > maxX)
        maxX = c1.x;
      if (c1.y > maxY)
        maxY = c1.y;
      if (c2.x < minX)
        minX = c2.x;
      else if (c2.x > maxX)
        maxX = c2.x;
      if (c2.y > maxY)
        maxY = c2.y;

      // Rock lines are either vertical or horizontal, so determine the dynamic parameter and build the line from it
      if (c1.x != c2.x)
      {
        d = signum(c2.x - c1.x);
        // This won't include the last coordinate (this is okay, it prevent's double-entering it on next loop)
        for (x = c1.x; x != c2.x; x += d)
        {
          rockArea.push_back(coordinate_t{x, c1.y});
        }
      }
      else
      {
        d = signum(c2.y - c1.y);
        // This won't include the last coordinate (this is okay, it prevent's double-entering it on next loop)
        for (y = c1.y; y != c2.y; y += d)
        {
          rockArea.push_back(coordinate_t{c1.x, y});
        }
      }
    }
    // From the above loop, the last coordinate in the chain does not get entered, so add it now
    rockArea.push_back(coordinate_t(c2));

    // Empty the temporary vector
    tmpV.clear();
  }
}

vector<gridItem_t> makeNewGridVector(int nElements, int xStart, int yStart, bool varyX)
{
  vector<gridItem_t> out;
  if (varyX)
  {
    for (int i = 0; i < nElements; i++)
      out.push_back(gridItem_t{xStart + i, yStart});
  }
  else
  {
    for (int i = 0; i < nElements; i++)
      out.push_back(gridItem_t{xStart, yStart + i});
  }
  return out;
}

void buildScanGrid(matrix2d_t &scanGrid, vector<coordinate_t> &rockArea, int minX, int maxX, int maxY)
{
  int x, y;
  gridItem_t *pGridItem;
  // Build out the air-filled scanGrid
  for (y = 0; y < (maxY + 1); y++)
  {
    scanGrid.push_back(makeNewGridVector((maxX - minX + 1), minX, y, true));
  }

  // Iterate through the rock area, setting the grid type appropriately
  for (coordinate_t c : rockArea)
  {
    pGridItem = &(scanGrid[c.y][c.x - minX]);
    pGridItem->isOccupied = true;
    pGridItem->fillType = ROCK;
  }

  // Add SOURCE type to 500,0
  scanGrid[0][500 - minX].fillType = SOURCE;
}

void addBottomToGrid(matrix2d_t &scanGrid, int yStepsToBottom)
{
  int xSpan = scanGrid[0].size();
  int ySpan = scanGrid.size();
  int minX = scanGrid[0][0].coord.x;

  for (int y = 0; y < yStepsToBottom; y++)
  {
    scanGrid.push_back(makeNewGridVector(xSpan, minX, y + ySpan, true));
  }

  ySpan = scanGrid.size();
  for (int x = 0; x < xSpan; x++)
  {
    scanGrid[ySpan - 1][x].isOccupied = true;
    scanGrid[ySpan - 1][x].fillType = ROCK;
  }
}

void printScanGrid(matrix2d_t &scanGrid, int minX, int maxX, int maxY)
{
  string s;
  string limX1, limX2, start;
  int maxDigitsX = 3;
  int maxDigitsY = to_string(maxY).length();
  string emptySpacerYIdx(maxDigitsY + 1, ' ');

  limX1 = to_string(minX);
  limX2 = to_string(maxX);
  start = "500";
  if (limX2.length() > maxDigitsX)
  {
    maxDigitsX = limX2.length();
    for (int i = 0; i < (maxDigitsX - 3); i++)
    {
      limX1 = " " + limX1;
      start = " " + start;
    }
  }
  s += '\n';
  for (int i = 0; i < maxDigitsX; i++)
  {
    s += emptySpacerYIdx;
    for (int x = 0; x <= (maxX - minX); x++)
    {
      if (x == 0)
        s += limX1[i];
      else if (x == (500 - minX))
        s += start[i];
      else if (x == (maxX - minX))
        s += limX2[i];
      else
        s += " ";
    }
    s += '\n';
  }
  for (int y = 0; y <= maxY; y++)
  {
    s += to_string(y);
    for (int i = 0; i < (maxDigitsY - to_string(y).length() + 1); i++)
      s += " ";
    for (int x = 0; x <= (maxX - minX); x++)
    {
      switch (scanGrid[y][x].fillType)
      {
      case AIR:
        s += ".";
        break;
      case SOURCE:
        s += "+";
        break;
      case ROCK:
        s += "#";
        break;
      case SAND:
        s += "o";
        break;
      default:
        s += " ";
        break;
      }
    }
    s += "\n";
  }

  cout << s << endl;
}

bool simulateSandUnit(matrix2d_t &scanGrid, coordinate_t sourceLoc, int minX, int maxX, int maxY)
{
  bool comesToRest = false;
  coordinate_t sand, nextCoord;
  gridItem_t *pSandSpace;

  // Start the grain of sand at the source location
  sand = sourceLoc;

  // Simulate sand motion until rested or falls into abyss (breaks)
  while (!comesToRest)
  {
    // Check next down space
    nextCoord = sand + coordinate_t{0, 1};
    // Check for overrun
    if (nextCoord.y > maxY)
      break;
    // Check for occupation, moving sand there if not occupied
    if (!scanGrid[nextCoord.y][nextCoord.x - minX].isOccupied)
    {
      sand = nextCoord;
      continue;
    }

    // If down space was occupied, check down left diagonal
    nextCoord = sand + coordinate_t{-1, 1};
    // Check for overrun (y overrun has already been checked, so just check x)
    if (nextCoord.x < minX)
      break;
    // Check for occupation, moving sand there if not occupied
    if (!scanGrid[nextCoord.y][nextCoord.x - minX].isOccupied)
    {
      sand = nextCoord;
      continue;
    }

    // If down left diagonal space was occupied, check down right diagonal
    nextCoord = sand + coordinate_t{1, 1};
    // Check for overrun (y overrun has already been checked, so just check x)
    if (nextCoord.x > maxX)
      break;
    // Check for occupation, moving sand there if not occupied
    if (!scanGrid[nextCoord.y][nextCoord.x - minX].isOccupied)
    {
      sand = nextCoord;
      continue;
    }

    // If this point reached, then sand does not have anywhere to move and will rest at current space. Update grid and exit
    pSandSpace = &(scanGrid[sand.y][sand.x - minX]);
    pSandSpace->isOccupied = true;
    pSandSpace->fillType = SAND;
    comesToRest = true;
  }

  return comesToRest;
}

bool simulateSandUnitPart2(matrix2d_t &scanGrid, coordinate_t sourceLoc, int minX, int maxX, int maxY, int &fullMinX, int &fullMaxX, matrix2d_t &overLeftT, matrix2d_t &overRightT)
{
  bool comesToRest = false;
  coordinate_t sand, nextCoord;
  gridItem_t *pSandSpace;

  // Start the grain of sand at the source location
  sand = sourceLoc;

  // Simulate sand motion until rested or falls into abyss (breaks)
  while (!comesToRest)
  {
    // Check next down space
    nextCoord = sand + coordinate_t{0, 1};
    // Check if nextCoord is in left overrun zone
    if (nextCoord.x < minX)
    {
      // Check for occupation, moving sand there if not occupied
      if (!overLeftT[minX - 1 - nextCoord.x][nextCoord.y].isOccupied)
      {
        sand = nextCoord;
        continue;
      }
    }
    // Check if nextCoord is in right overrun zone
    else if (nextCoord.x > maxX)
    {
      // Check for occupation, moving sand there if not occupied
      if (!overRightT[nextCoord.x - 1 - maxX][nextCoord.y].isOccupied)
      {
        sand = nextCoord;
        continue;
      }
    }
    // If not in overrun, then it's in original grid
    else
    {
      // Check for occupation, moving sand there if not occupied
      if (!scanGrid[nextCoord.y][nextCoord.x - minX].isOccupied)
      {
        sand = nextCoord;
        continue;
      }
    }

    // If down space was occupied, check down left diagonal
    nextCoord = sand + coordinate_t{-1, 1};
    // Check for overrun on x and update Transposed overrunGrid if needed
    if (nextCoord.x < fullMinX)
    {
      vector<gridItem_t> tmpV = makeNewGridVector(maxY + 1, fullMinX - 1, 0, false);
      tmpV[maxY].isOccupied = true;
      tmpV[maxY].fillType = ROCK;
      overLeftT.push_back(tmpV);
      fullMinX--;
    }
    // Check if nextCoord is in left overrun zone
    if (nextCoord.x < minX)
    {
      // Check for occupation, moving sand there if not occupied
      if (!overLeftT[minX - 1 - nextCoord.x][nextCoord.y].isOccupied)
      {
        sand = nextCoord;
        continue;
      }
    }
    // Check if nextCoord is in right overrun zone
    else if (nextCoord.x > maxX)
    {
      // Check for occupation, moving sand there if not occupied
      if (!overRightT[nextCoord.x - 1 - maxX][nextCoord.y].isOccupied)
      {
        sand = nextCoord;
        continue;
      }
    }
    // If not in overrun, then it's in original grid
    else
    {
      // Check for occupation, moving sand there if not occupied
      if (!scanGrid[nextCoord.y][nextCoord.x - minX].isOccupied)
      {
        sand = nextCoord;
        continue;
      }
    }

    // If down left diagonal space was occupied, check down right diagonal
    nextCoord = sand + coordinate_t{1, 1};
    // Check for overrun on x and update Transposed overrunGrid if needed
    if (nextCoord.x > fullMaxX)
    {
      vector<gridItem_t> tmpV = makeNewGridVector(maxY + 1, fullMaxX + 1, 0, false);
      tmpV[maxY].isOccupied = true;
      tmpV[maxY].fillType = ROCK;
      overRightT.push_back(tmpV);
      fullMaxX++;
    }
    // Check if nextCoord is in left overrun zone
    if (nextCoord.x < minX)
    {
      // Check for occupation, moving sand there if not occupied
      if (!overLeftT[minX - 1 - nextCoord.x][nextCoord.y].isOccupied)
      {
        sand = nextCoord;
        continue;
      }
    }
    // Check if nextCoord is in right overrun zone
    else if (nextCoord.x > maxX)
    {
      // Check for occupation, moving sand there if not occupied
      if (!overRightT[nextCoord.x - 1 - maxX][nextCoord.y].isOccupied)
      {
        sand = nextCoord;
        continue;
      }
    }
    // If not in overrun, then it's in original grid
    else
    {
      // Check for occupation, moving sand there if not occupied
      if (!scanGrid[nextCoord.y][nextCoord.x - minX].isOccupied)
      {
        sand = nextCoord;
        continue;
      }
    }

    // If this point reached, then sand does not have anywhere to move and will rest at current space. Update appropriate grid and exit
    // Overrun Left
    if (sand.x < minX)
      pSandSpace = &(overLeftT[minX - 1 - sand.x][sand.y]);
    // Overrrun Right
    else if (sand.x > maxX)
      pSandSpace = &(overRightT[sand.x - 1 - maxX][sand.y]);
    // Original Grid
    else
      pSandSpace = &(scanGrid[sand.y][sand.x - minX]);
    pSandSpace->isOccupied = true;
    pSandSpace->fillType = SAND;

    // Check if sand is at the source of the sand, in which case return as if it didn't come to rest
    if (sand == sourceLoc)
      break;
    else
      comesToRest = true;
  }

  return comesToRest;
}

uint64_t part1(string fName, bool verboseGridPrint = false)
{
  uint64_t nSandUnitsBeforeStable = 0;
  vector<coordinate_t> rockArea;
  int minX, maxX, maxY;
  matrix2d_t scanGrid;
  coordinate_t sourceLoc{500, 0};

  // From the file, determine which area is filled with rock
  getRockFilledArea(fName, rockArea, minX, maxX, maxY);

  // Build out the grid from the rock-filled area
  buildScanGrid(scanGrid, rockArea, minX, maxX, maxY);

  // Print the starting state of the scanGrid
  if (verboseGridPrint)
    printScanGrid(scanGrid, minX, maxX, maxY);

  // Run sand through the grid until it reaches a stable point where sand is falling into abyss instead of settling
  while (simulateSandUnit(scanGrid, sourceLoc, minX, maxX, maxY))
  {
    nSandUnitsBeforeStable++;
    if (verboseGridPrint)
    {
      cout << "\n-- Grid Status After >" << nSandUnitsBeforeStable << "< Sand Units Flowed: " << endl;
      printScanGrid(scanGrid, minX, maxX, maxY);
    }
  }

  return nSandUnitsBeforeStable;
}

uint64_t part2(string fName, bool verboseGridPrint = false)
{
  uint64_t nSandUnitsBeforeStable = 0;
  vector<coordinate_t> rockArea;
  int minX, maxX, maxY;
  matrix2d_t scanGrid;
  coordinate_t sourceLoc{500, 0};
  int fullMinX, fullMaxX;
  matrix2d_t overLeftT, overRightT;


  // From the file, determine which area is filled with rock
  getRockFilledArea(fName, rockArea, minX, maxX, maxY);

  // Build out the grid from the rock-filled area
  buildScanGrid(scanGrid, rockArea, minX, maxX, maxY);

  // Add on a bottom layer
  maxY += 2;
  addBottomToGrid(scanGrid, 2);

  // Print the starting state of the scanGrid
  if (verboseGridPrint)
    printScanGrid(scanGrid, minX, maxX, maxY);

  // Run sand through the grid until it reaches a stable point where sand is falling into abyss instead of settling
  fullMinX = minX;
  fullMaxX = maxX;
  while (simulateSandUnitPart2(scanGrid, sourceLoc, minX, maxX, maxY, fullMinX, fullMaxX, overLeftT, overRightT))
  {
    nSandUnitsBeforeStable++;
    if (verboseGridPrint)
    {
      cout << "\n-- Grid Status After >" << nSandUnitsBeforeStable << "< Sand Units Flowed: " << endl;
      printScanGrid(scanGrid, minX, maxX, maxY);
    }
  }

  return nSandUnitsBeforeStable+1;
}

int main()
{
  uint64_t result;
  string outStr1 = "Units of sand that come to rest in ";
  string outStr2 = "Units of sand that come to rest in ";
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