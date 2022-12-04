#include <iostream>
#include <string>
#include <fstream>
#include <map>

using namespace std;

// Find the item (char) common in both the front and back halves of a string
char findCommonItem(string s)
{
  char c = 0;
  size_t pos;
  int i, j;

  i = s.length();
  for (j = 0; j < i / 2; j++)
  {
    c = s[j];
    pos = s.find(c, i / 2);
    if (pos != string::npos)
      break;
  }
  return c;
}

char findCommonItemIn3(string s1, string s2, string s3)
{
  string *sa, *sb, *sc;

  // Order the strings by length
  sa = &s1;
  if (s2.length() < sa->length())
  {
    sb = sa;
    sa = &s2;
  }
  else
  {
    sb = &s2;
  }
  if (s3.length() < sa->length())
  {
    sc = sb;
    sb = sa;
    sa = &s3;
  }
  else if (s3.length() < sb->length())
  {
    sc = sb;
    sb = &s3;
  }
  else
  {
    sc = &s3;
  }

  // Starting with the smallest string, check it's characters (items) see if they exist in both the next two larger strings (in order)
  string:: iterator it;
  for (it=sa->begin(); it != sa->end(); it++)
  {
    if (sb->find(*it)!= string::npos)
    {
      if (sc->find(*it)!= string::npos)
        break;
    }
  }

  return *it;
}

// Determine item 'priority' value
int getItemPriority(char c)
{
  /*
  Item priority:
  a-z : 1-26
  A-Z : 27-52
  */
  int ret;

  // Using ASCII to our advantage
  // If lowercase
  if ((int)c > 96)
    ret = (int)c - 96;
  // If uppdercase
  else
    ret = (int)c - 64 + 26;
  return ret;
}

int part1(string fName)
{
  string s;
  char c;
  int prioritySum = 0;

  // Open and scan through the file
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Find common item in each side of the sack (string), calculate it's priority, and add it to the prioritySum
    c = findCommonItem(s);
    prioritySum += getItemPriority(c);
  }
  fsObj.close();

  return prioritySum;
}

int part2(string fName)
{
  string s1, s2, s3;
  char c;
  int prioritySum = 0;

  // Open and scan through the file
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s1);
    if (fsObj.eof())
      break;
    getline(fsObj, s2);
    getline(fsObj, s3);

    // Find common item in all 3 strings, calculate it's priority, and add it to the prioritySum
    c = findCommonItemIn3(s1, s2, s3);
    prioritySum += getItemPriority(c);
  }
  fsObj.close();

  return prioritySum;
}

int main()
{
  int result;

  // -- PART 1
  cout << "==============PART1==============" << endl;
  // Run on example input
  result = part1("example.txt");
  cout << "Priority sum from example.txt is " << result << endl;

  // Run on actual input
  result = part1("input.txt");
  cout << "Priority sum from input.txt is " << result << endl;

  // -- PART 2
  cout << endl
       << "==============PART2==============" << endl;
  // Run on example input
  result = part2("example.txt");
  cout << "Priority sum from example.txt is " << result << endl;

  // Run on actual input
  result = part2("input.txt");
  cout << "Priority sum from input.txt is " << result << endl;

  return 0;
}