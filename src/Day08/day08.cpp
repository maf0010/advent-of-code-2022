#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

// TYPEDEFS
typedef struct
{
  bool isValid = false;
  uint32_t colOrRowIdx = 0;
  uint8_t value = 0;
} localMax_t;

typedef vector<vector<uint8_t>> matrix2d_t;

// PROTOTYPES
void setColOrRowMax(localMax_t *pMax, uint32_t idx, uint8_t value);
void clearColOrRowMax(localMax_t *pMax);

// FUNCTION DEFINITIONS
void setColOrRowMax(localMax_t *pMax, uint32_t idx, uint8_t value)
{
  pMax->colOrRowIdx = idx;
  pMax->value = value;
  pMax->isValid = true;
}

void clearColOrRowMax(localMax_t *pMax)
{
  pMax->colOrRowIdx = 0;
  pMax->value = 0;
  pMax->isValid = false;
}

uint32_t part1(string fName)
{
  uint32_t nTreesVisible = 0;
  matrix2d_t treeGrid;
  string s;
  bool isVisible;
  uint8_t treeUnderTest;
  uint32_t nRows, nCols;

  // Open file, parse the tree grid from it
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;
    vector<uint8_t> v;
    for (char c : s)
    {
      v.push_back((uint8_t)(c - '0'));
    }
    treeGrid.push_back(v);
  }
  fsObj.close();

  // Look through the grid and find the trees that can be seen from outside
  nRows = treeGrid.size();
  nCols = treeGrid[0].size();

  // Perimeter items can be seen
  nTreesVisible += 2 * (nRows + (nCols - 2));

  // Setup local maxima
  localMax_t colMaxFromTop[nCols], colMaxFromBot[nCols];
  localMax_t rowMaxFromLeft, rowMaxFromRight;
  localMax_t *pColMax;
  for (int col = 0; col < nCols; col++)
  {
    setColOrRowMax(&colMaxFromTop[col], 0, treeGrid[0][col]);
  }

  // Moving top-down, left-right
  for (int row = 1; row < nRows - 1; row++)
  {
    setColOrRowMax(&rowMaxFromLeft, 0, treeGrid[row][0]);
    clearColOrRowMax(&rowMaxFromRight);
    for (int col = 1; col < nCols - 1; col++)
    {
      isVisible = false;
      treeUnderTest = treeGrid[row][col];

      // Check if can be seen from left (we know rowMaxFromLeft is valid, so skip check)
      if (treeUnderTest > rowMaxFromLeft.value)
      {
        isVisible = true;
        setColOrRowMax(&rowMaxFromLeft, col, treeUnderTest);
      }

      // Check if can be seen from top regardless of if already visible to update column max either way
      //  (we also know colMaxFromTop is valid since top-down is the order of progression)
      pColMax = &colMaxFromTop[col];
      if (treeUnderTest > pColMax->value)
      {
        isVisible = true;
        setColOrRowMax(pColMax, row, treeUnderTest);
      }

      // If we already know tree is visible, don't waste time scanning to the right
      if (!isVisible)
      {
        // Check if tree can be seen from right
        // If local max claims validity, but we've progressed past it's corresponding index, invalidate it
        if (rowMaxFromRight.isValid && (rowMaxFromRight.colOrRowIdx <= col))
          clearColOrRowMax(&rowMaxFromRight);

        // If local max is valid AND TUT is taller than latest max OR if local max is not valid,
        //   scan rightward to establish visibility.
        if ((rowMaxFromRight.isValid && (treeUnderTest > rowMaxFromRight.value)) || !rowMaxFromRight.isValid)
        {
          // For the upcoming loop, we should assume the TUT is visible and prove it isnt inside the loop
          isVisible = true;
          for (int col2 = col + 1; col2 < nCols; col2++)
          {
            // If tree isn't visible, record the invisibility and record the new rightward local max
            if (treeUnderTest <= treeGrid[row][col2])
            {
              isVisible = false;
              setColOrRowMax(&rowMaxFromRight, col2, treeGrid[row][col2]);
              break;
            }
          }
          // If the tree was never found to be invisible in the above loop, then great, its already been set
        }
        // Alternate case is the local max is valid and it's taller than out TUT, so there's no point in
        //  scanning further. The local max provided a shortcut to having to scan rightward.
      }

      // If we already know tree is visible, don't waste time scanning downward
      if (!isVisible)
      {
        pColMax = &colMaxFromBot[col];
        // Check if tree can be seen from the bottom
        // If local max claims validity, but we've progressed past it's corresponding index, invalidate it
        if (pColMax->isValid && (pColMax->colOrRowIdx <= row))
          clearColOrRowMax(pColMax);

        // If local max is valid AND TUT is taller than latest max OR if local max is not valid,
        //   scan downward to establish visibility.
        if ((pColMax->isValid && (treeUnderTest > pColMax->value)) || !pColMax->isValid)
        {
          // For the upcoming loop, we should assume the TUT is visible and prove it isnt inside the loop
          isVisible = true;
          for (int row2 = row + 1; row2 < nRows; row2++)
          {
            // If tree isn't visible, record the invisibility and record the new downward local max
            if (treeUnderTest <= treeGrid[row2][col])
            {
              isVisible = false;
              setColOrRowMax(pColMax, row2, treeGrid[row2][col]);
              break;
            }
          }
          // If the tree was never found to be invisible in the above loop, then great, its already been set
        }
        // Alternate case is the local max is valid and it's taller than out TUT, so there's no point in
        //  scanning further. The local max provided a shortcut to having to scan downward.
      }

      // If tree determined to be visible, increment the visibile tree counter
      if (isVisible)
        nTreesVisible++;
    }
  }

  return nTreesVisible;
}

uint32_t part2(string fName)
{
  uint32_t nTreesVisible = 0;
  uint32_t scenicScore, maxScenicScore = 0;
  matrix2d_t treeGrid;
  string s;
  uint8_t treeUnderTest;
  uint32_t nRows, nCols;
  uint32_t nLeft, nRight, nUp, nDown;

  // Open file, parse the tree grid from it
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;
    vector<uint8_t> v;
    for (char c : s)
    {
      v.push_back((uint8_t)(c - '0'));
    }
    treeGrid.push_back(v);
  }
  fsObj.close();

  // Alright, not doing anything as fancy as in part 1, just going to work through the tree grid, calculate each tree's scenic score, find the max
  nRows = treeGrid.size();
  nCols = treeGrid[0].size();

  // Perimiter trees have scenic scores of 0. skip them
  for (int row = 1; row < nRows - 1; row++)
  {
    for (int col = 1; col < nCols - 1; col++)
    {
      treeUnderTest = treeGrid[row][col];

      // Left trees
      nLeft=0;
      for (int col2=col-1; col2>=0; col2--)
      {
        nLeft++;
        if (treeUnderTest <= treeGrid[row][col2])
          break;
      }

      // Right trees
      nRight=0;
      for (int col2=col+1; col2<nCols; col2++)
      {
        nRight++;
        if (treeUnderTest <= treeGrid[row][col2])
          break;
      }

      // Up trees
      nUp=0;
      for (int row2=row-1; row2>=0; row2--)
      {
        nUp++;
        if (treeUnderTest <= treeGrid[row2][col])
          break;
      }

      // Down trees
      nDown=0;
      for (int row2=row+1; row2<nRows; row2++)
      {
        nDown++;
        if (treeUnderTest <= treeGrid[row2][col])
          break;
      }
      
      // Scenic score for tree
      scenicScore = nLeft * nRight * nUp * nDown;
      if (scenicScore > maxScenicScore)
        maxScenicScore = scenicScore;
    }
  }

  return maxScenicScore;
}

int main()
{
  uint32_t result;
  string outStr1 = "Number of trees visible from outside of the grid in ";
  string outStr2 = "Maximum scenic score in ";
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