#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <list>

using namespace std;

// TYPEDEFS & STRUCTS
typedef struct myObject
{
  bool isList = false;
  uint32_t myValue;
  vector<myObject> myList;
  bool const operator==(const myObject &c) const { return ((isList == c.isList) && (myValue == c.myValue) && (myList == c.myList)); }
} myObject_t;

typedef enum comparisonResults
{
  LEFT_IS_SMALLER = -1,
  EQUAL = 0,
  RIGHT_IS_SMALLER = 1,
} comparisonResults_t;


// PROTOTYPES
myObject_t parseOjbectListFromString(string s, int *pIdx);
void printList(myObject_t obj);
comparisonResults_t compareTwoPackets(myObject_t *pLeft, myObject_t *pRight);
uint32_t addSortedPacket(list<myObject_t> *pMasterList, myObject_t *pObj);


// CLASS DEFINITIONS


// FUNCTION DEFINITIONS
myObject_t parseOjbectListFromString(string s, int *pIdx)
{
  myObject_t retObj;
  retObj.isList = true;
  char c;
  size_t commaLoc, rbracketLoc;
  bool isScanning = true;
  uint32_t tmpVal;

  while (isScanning)
  {
    // Get leading character at current index
    c = s[*pIdx];

    // Check if it is a number or a list
    if ((c >= '0') && (c <= '9'))
    {
      // Find the next comma or right bracket to find end of the number
      commaLoc = s.find(',', *pIdx);
      rbracketLoc = s.find(']', *pIdx);

      // There should always be an rbracketLoc; if not, then there's a problem
      if (rbracketLoc == string::npos)
      {
        cout << "Error, no ']' found in string : " << s.substr(*pIdx) << endl;
        isScanning = false;
        break;
      }

      // If commaLoc is npos or greater than rbracketLoc, then use rbracketLoc and mark reaching end of the list
      if ((commaLoc == string::npos) || (commaLoc > rbracketLoc))
      {
        tmpVal = stoul(s.substr(*pIdx, rbracketLoc));
        *pIdx = rbracketLoc + 1;
        isScanning = false;
      }
      // If commaLoc is less than rbracketetLoc, then use the commaLoc
      else
      {
        tmpVal = stoul(s.substr(*pIdx, commaLoc));
        *pIdx = commaLoc + 1;
      }

      // Create new object and add this value to it
      myObject_t tmpObj;
      tmpObj.isList = false;
      tmpObj.myValue = tmpVal;

      // Add the new object to the parent list
      retObj.myList.push_back(tmpObj);
    }
    // So it's not a value, look for an lbracket to see if a new list is starting
    else if (c == '[')
    {
      // Recursively call this parser (should also update the index within it)
      (*pIdx)++;
      retObj.myList.push_back(parseOjbectListFromString(s, pIdx));
    }
    // Check for an rbracket in case this was an empty list
    else if (c == ']')
    {
      isScanning = false;
      (*pIdx)++;
    }
    // If comma, then it's likely remenant from scanning a nested list; just increment index and move on
    else if (c == ',')
    {
      (*pIdx)++;
    }
    else
    {
      cout << "Uncaught character in parser for character, index" << s[*pIdx] << ", " << *pIdx << " in string " << s << endl;
      isScanning = false;
    }
  }

  return retObj;
}

void printList(myObject_t obj)
{
  int i;

  if (obj.isList)
  {
    cout << '[';
    for (i = 0; i < obj.myList.size(); ++i)
    {
      printList(obj.myList[i]);
      if (i != obj.myList.size() - 1)
      {
        cout << ',';
      }
    }
    cout << ']';
  }
  else
  {
    cout << obj.myValue;
  }
}

comparisonResults_t compareTwoPackets(myObject_t *pLeft, myObject_t *pRight)
{
  comparisonResults_t retEnum = EQUAL;
  // bool isLeftOutOfItems = false;
  // bool isRightOutOfItems = false;
  int i;

  // If both objects are not lists
  if ((!pLeft->isList) && (!pRight->isList))
  {
    if (pLeft->myValue < pRight->myValue)
      retEnum = LEFT_IS_SMALLER;
    else if (pLeft->myValue > pRight->myValue)
      retEnum = RIGHT_IS_SMALLER;
    else
      retEnum = EQUAL;
  }

  // If left object is list, but right object is not
  else if (pLeft->isList && !pRight->isList)
  {
    myObject_t tmpObj;
    tmpObj.isList = true;
    tmpObj.myList.push_back(*pRight);
    retEnum = compareTwoPackets(pLeft, &tmpObj);
  }

  // If right object is list, but left object is not
  else if (pRight->isList && !pLeft->isList)
  {
    myObject_t tmpObj;
    tmpObj.isList = true;
    tmpObj.myList.push_back(*pLeft);
    retEnum = compareTwoPackets(&tmpObj, pRight);
  }

  // If both objects are lists
  else
  {
    // Loop through the lists and recursively call this until we get a value that isn't "EQUAL"
    for (i = 0; i < pLeft->myList.size(); i++)
    {
      // Check to ensure that current index is within size of pRight
      if (i >= pRight->myList.size())
      {
        retEnum = RIGHT_IS_SMALLER;
        break;
      }
      retEnum = compareTwoPackets(&(pLeft->myList[i]), &(pRight->myList[i]));
      if (retEnum != EQUAL)
        break;
    }

    // If we have reached the end w/ retEnum still being EQUAL, then check to see if left is shorter than right, otherwise continue to return EQUAL
    if ((retEnum == EQUAL) && (pLeft->myList.size() < pRight->myList.size()))
      retEnum = LEFT_IS_SMALLER;
  }

  return retEnum;
}

uint32_t addSortedPacket(list<myObject_t> *pMasterList, myObject_t *pObj)
{
  uint32_t idx = 1;
  list<myObject_t>::iterator it;
  bool isInserted = false;

  // Scan through the master list, inserting the element at the first occurance of "Left not being smaller"
  for (it = pMasterList->begin(); it != pMasterList->end(); ++it)
  {
    if (compareTwoPackets(&(*it), pObj) == RIGHT_IS_SMALLER)
    {
      pMasterList->insert(it, *pObj);
      isInserted = true;
      break;
    }
    idx++;
  }
  // If made it to end of the list without insertion, add it to the back
  if (!isInserted)
  {
    pMasterList->push_back(*pObj);
  }

  return idx;
}

uint64_t part1(string fName)
{
  uint64_t outSum = 0;
  string s;
  myObject_t p1, p2;
  int idx;
  int packetPairCount = 1;
  comparisonResults_t retEnum;

  // Open file, read through it, grabbing two lines and skipping a blank line
  ifstream fsObj(fName);
  while (true)
  {
    // Get first pair
    getline(fsObj, s);
    if (fsObj.eof())
      break;
    idx = 1;
    p1 = parseOjbectListFromString(s, &idx);

    // Get second pair
    getline(fsObj, s);
    if (fsObj.eof())
      break;
    idx = 1;
    p2 = parseOjbectListFromString(s, &idx);

    // Extract the next line (blank)
    getline(fsObj, s);

    // Prints
    printList(p1);
    cout << endl;
    printList(p2);
    cout << endl;

    // Compare the packets
    retEnum = compareTwoPackets(&p1, &p2);
    switch (retEnum)
    {
    case LEFT_IS_SMALLER:
      outSum += packetPairCount;
      cout << "Packets In Right Order" << endl;
      break;
    case RIGHT_IS_SMALLER:
      cout << "Packets NOT In Right Order" << endl;
      break;
    default:
      cout << "Packets Equal" << endl;
      break;
    }
    packetPairCount++;
    cout << endl;
  }

  return outSum;
}

uint64_t part2(string fName)
{
  uint64_t decoderKey;
  string s;
  list<myObject_t> masterList;
  myObject_t pObj;
  uint32_t divPos1 = 0, divPos2 = 0;
  comparisonResults_t retEnum;
  int idx;

  // Open file, read through it, grabbing each packet and adding it to the sorted list
  ifstream fsObj(fName);
  while (true)
  {
    // Get packet
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // If is is a blank line, continue
    if (s == "")
      continue;

    // Add the object to the master list
    idx = 1;
    pObj = parseOjbectListFromString(s, &idx);
    addSortedPacket(&masterList, &pObj);
  }

  // Now add the dividers at the appropriate locations, getting the decoder key value in the process
  s = "[[2]]";
  idx = 1;
  pObj = parseOjbectListFromString(s, &idx);
  decoderKey = addSortedPacket(&masterList, &pObj);

  s = "[[6]]";
  idx = 1;
  pObj = parseOjbectListFromString(s, &idx);
  decoderKey *= addSortedPacket(&masterList, &pObj);

  return decoderKey;
}

int main()
{
  uint64_t result;
  string outStr1 = "Sum of index of sorted pairs in ";
  string outStr2 = "Decoder Key in ";
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