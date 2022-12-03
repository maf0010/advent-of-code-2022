#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int part1(string fName)
{
  int calorieSum = 0, maxCalorieSum = 0;
  string s;

  // Open file, read through contents, summing up each elf's (separated by newlines) packed calories, remembereing the latest maximum calorie elf
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);

    // New elf reached; update latest calorieMax, reset sum
    if (s.empty())
    {
      if (calorieSum > maxCalorieSum)
        maxCalorieSum = calorieSum;
      calorieSum = 0;
    }
    // Otherwise, add to the latest caloriesum
    else
      calorieSum += stoi(s);

    // Check if EOF has been reached
    if (fsObj.eof())
      break;
  }
  fsObj.close();

  return maxCalorieSum;
}

int part2(string fName)
{
  int calorieSum = 0;
  int maxCalSum1 = 0, maxCalSum2 = 0, maxCalSum3 = 0;
  string s;

  // Open file, read through contents, summing up each elf's (separated by newlines) packed calories, remembereing the latest 3 maximum calorie elf
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);

    // New elf reached; update latest calorieMax, reset sum
    if (s.empty())
    {
      if (calorieSum > maxCalSum1)
      {
        maxCalSum3 = maxCalSum2;
        maxCalSum2 = maxCalSum1;
        maxCalSum1 = calorieSum;
      }
      else if (calorieSum > maxCalSum2)
      {
        maxCalSum3 = maxCalSum2;
        maxCalSum2 = calorieSum;
      }
      else if (calorieSum > maxCalSum3)
      {
        maxCalSum3 = calorieSum;
      }

      calorieSum = 0;
    }
    // Otherwise, add to the latest caloriesum
    else
      calorieSum += stoi(s);

    // Check if EOF has been reached
    if (fsObj.eof())
      break;
  }
  fsObj.close();

  // Sum top 3 calorie totals and return that sum
  return maxCalSum1 + maxCalSum2 + maxCalSum3;
}

int main()
{
  int calorieSum;

  // -- PART 1
  // Run on example input
  calorieSum = part1("example.txt");
  cout << "Highest calorie total in example.txt is " << calorieSum << endl;

  // Run on actual input
  calorieSum = part1("input.txt");
  cout << "Highest calorie total in input.txt is " << calorieSum << endl;
  
  // -- PART 2
  // Run on example input
  calorieSum = part2("example.txt");
  cout << "Highest 3 calorie total in example.txt is " << calorieSum << endl;

  // Run on actual input
  calorieSum = part2("input.txt");
  cout << "Highest 3 calorie total in input.txt is " << calorieSum << endl;
  
  return 0;
}