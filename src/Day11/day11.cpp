#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <map>

using namespace std;

// TYPEDEFS
typedef uint64_t (*opPtr)(uint64_t, uint64_t);

// PROTOTYPES
uint64_t monkeyAdd(uint64_t val1, uint64_t val2);
uint64_t monkeyMultiply(uint64_t val1, uint64_t val2);
vector<uint64_t> getItemListFromString(string itemStr);

// CLASS DEFINITIONS
class Monkey
{
private:
  queue<uint64_t> itemQueue;
  opPtr op;
  uint64_t opArg2;
  bool isSecondOpArgTheOldItem;
  uint64_t testDivisor;
  Monkey *passMonkey, *failMonkey;
  uint64_t inspectionCount;
  uint64_t worryDivisor;

public:
  uint8_t id;
  bool isWorryReducedByDivision;
  Monkey(uint8_t idArg, uint64_t testDivisorArg, opPtr pOp, bool useOldForArg2, uint64_t opArg2Arg);
  void act(void);
  uint64_t inspectItem(uint64_t itemWl);
  void testItem(uint64_t itemWl);
  void addItemToInventory(uint64_t itemWl);
  void setTestDestMonkeys(Monkey *pm, Monkey *fm);
  uint64_t getInspectionCount(void);
  void reportItems(void);
  void setWorryDivisor(uint64_t newDivisor);
};

// Class Functions
Monkey::Monkey(uint8_t idArg, uint64_t testDivisorArg = 1, opPtr pOp = &monkeyAdd, bool useOldForArg2 = true, uint64_t opArg2Arg = 0)
{
  id = idArg;
  op = pOp;
  opArg2 = opArg2Arg;
  isSecondOpArgTheOldItem = useOldForArg2;
  testDivisor = testDivisorArg;
  inspectionCount = 0;
  passMonkey = this;
  failMonkey = this;
  worryDivisor = 3;
  isWorryReducedByDivision = true;
}

void Monkey::act(void)
{
  // Monkey goes through all items in itemQueue, inspecting them, decreasing the post-inspection worry-level,
  //  testing the item, then tossing it off to a different monkey
  uint64_t curItemWl;

  while (!itemQueue.empty())
  {
    curItemWl = itemQueue.front();
    curItemWl = inspectItem(curItemWl);
    if (isWorryReducedByDivision)
      curItemWl = curItemWl / worryDivisor;
    else
      curItemWl = curItemWl % worryDivisor;
    testItem(curItemWl);
    itemQueue.pop();
  }
}

uint64_t Monkey::inspectItem(uint64_t itemWl)
{
  uint64_t retVal;

  // Perform inspection operation
  if (isSecondOpArgTheOldItem)
    retVal = op(itemWl, itemWl);
  else
    retVal = op(itemWl, opArg2);

  // Increment inspection count and return the new item's worry level
  inspectionCount++;
  return retVal;
}

void Monkey::testItem(uint64_t itemWl)
{
  // Not divisible by divisor -> Toss to "FailMonkey"
  if (itemWl % testDivisor)
    failMonkey->addItemToInventory(itemWl);
  // Divisible -> Toss to PassMonkey
  else
    passMonkey->addItemToInventory(itemWl);
}

void Monkey::addItemToInventory(uint64_t itemWl)
{
  itemQueue.push(itemWl);
}

void Monkey::setTestDestMonkeys(Monkey *pm, Monkey *fm)
{
  passMonkey = pm;
  failMonkey = fm;
}

uint64_t Monkey::getInspectionCount(void)
{
  return inspectionCount;
}

void Monkey::reportItems(void)
{
  queue<uint64_t> tmpQ = itemQueue;
  cout << "Monkey " << (uint64_t)id << ": ";
  while (!tmpQ.empty())
  {
    cout << tmpQ.front() << ", ";
    tmpQ.pop();
  }
  cout << endl;
}

void Monkey::setWorryDivisor(uint64_t newDivisor)
{
  worryDivisor = newDivisor;
}

// FUNCTION DEFINITIONS
uint64_t monkeyAdd(uint64_t val1, uint64_t val2)
{
  return val1 + val2;
}

uint64_t monkeyMultiply(uint64_t val1, uint64_t val2)
{
  return val1 * val2;
}

vector<uint64_t> getItemListFromString(string itemStr)
{
  vector<uint64_t> retVec;
  size_t pos1, pos2;
  // "  Starting items: " takes up 18 characters, so startin pos is 18
  // Look for commas as the delimiter and add each new value to the vector
  pos1 = 18;
  if (itemStr.length() != pos1)
  {
    while (true)
    {
      pos2 = itemStr.find(",", pos1);
      if (pos2 == string::npos)
      {
        retVec.push_back(stoul(itemStr.substr(pos1)));
        break;
      }
      else
      {
        retVec.push_back(stoull(itemStr.substr(pos1, pos2 - pos1)));
        pos1 = pos2 + 2; // Pass the ", "
      }
    }
  }
  return retVec;
}

uint64_t part1(string fName)
{
  string s, itemStr, opStr, testDivStr;
  vector<string> testDestStr1, testDestStr2;
  vector<Monkey *> troop;
  vector<uint64_t> startingItems;
  uint8_t tmpId;
  char opSymb;
  bool useOldForArg2;
  uint64_t opArg2, divisor;
  size_t pos;
  map<char, opPtr>::iterator it;
  opPtr tmpOp;
  uint32_t i, j1, j2;
  uint64_t iCount, iCountM1, iCountM2;
  uint64_t monkeyBusiness = 0;

  map<char, opPtr> symb2Op = {{'+', &monkeyAdd}, {'*', &monkeyMultiply}};

  // Open and scan through monkey group
  ifstream fsObj(fName);
  while (true)
  {
    // Get instruction
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Find the Monkey definition
    if (s.find("Monkey") == 0)
    {
      // Get the id
      pos = s.find(":");
      tmpId = stoul(s.substr(7, pos - 7));

      // Get the subsequent strings and parse their data when appropropriate
      getline(fsObj, itemStr);
      getline(fsObj, opStr);
      getline(fsObj, testDivStr);
      getline(fsObj, s);
      testDestStr1.push_back(s);
      getline(fsObj, s);
      testDestStr2.push_back(s);

      // Operation symbol and argument
      opSymb = opStr[23];
      it = symb2Op.find(opSymb);
      if (it != symb2Op.end())
        tmpOp = it->second;
      else
      {
        cout << "ERROR: Could not find mapped operation for " << opSymb << endl;
        break;
      }
      if (opStr.find("old", 24) != string::npos)
      {
        useOldForArg2 = true;
        opArg2 = 0;
      }
      else
      {
        useOldForArg2 = false;
        opArg2 = stoull(opStr.substr(25));
      }

      // Test Divisor
      divisor = stoull(testDivStr.substr(21));

      // Get list of starting items
      startingItems = getItemListFromString(itemStr);

      // Create the monkey
      Monkey *m = new Monkey(tmpId, divisor, tmpOp, useOldForArg2, opArg2);
      for (uint64_t item : startingItems)
      {
        m->addItemToInventory(item);
      }
      troop.push_back(m);
    }
  }
  fsObj.close();

  // Go back through the test desinations and add the approprate monkey pointers
  for (i = 0; i < troop.size(); i++)
  {
    // If true statement is 29 characters until the monkey id
    // If false statement is 30 characters until the monkey id
    j1 = stoul(testDestStr1[i].substr(29));
    j2 = stoul(testDestStr2[i].substr(30));
    troop[i]->setTestDestMonkeys(troop[j1], troop[j2]);
  }

  // Iterate monkey passing for 20 rounds
  for (i = 0; i < 20; i++)
  {
    for (Monkey *m : troop)
    {
      m->act();
    }
    cout << "After round " << i + 1 << ", monkeys look like this:" << endl;
    for (Monkey *m : troop)
    {
      m->reportItems();
    }
    cout << endl;
  }

  // Report monkey inspection count and calculate monkey business
  iCountM1 = 0;
  iCountM2 = 0;
  cout << endl;
  cout << "At CONCLUSION:" << endl;
  for (Monkey *m : troop)
  {
    iCount = m->getInspectionCount();
    if (iCount >= iCountM1)
    {
      iCountM2 = iCountM1;
      iCountM1 = iCount;
    }
    else if (iCount > iCountM2)
    {
      iCountM2 = iCount;
    }
    cout << "Monkey " << (uint32_t)m->id << " inspected items " << iCount << " times." << endl;
  }

  monkeyBusiness = iCountM1 * iCountM2;

  return monkeyBusiness;
}

uint64_t part2(string fName)
{
  string s, itemStr, opStr, testDivStr;
  vector<string> testDestStr1, testDestStr2;
  vector<Monkey *> troop;
  vector<uint64_t> startingItems;
  uint8_t tmpId;
  char opSymb;
  bool useOldForArg2;
  uint64_t opArg2, divisor;
  size_t pos;
  map<char, opPtr>::iterator it;
  opPtr tmpOp;
  uint32_t i, j1, j2;
  uint64_t iCount, iCountM1, iCountM2;
  uint64_t runningDivisor = 1;
  uint64_t monkeyBusiness = 0;

  map<char, opPtr> symb2Op = {{'+', &monkeyAdd}, {'*', &monkeyMultiply}};

  // Open and scan through monkey group
  ifstream fsObj(fName);
  while (true)
  {
    // Get instruction
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Find the Monkey definition
    if (s.find("Monkey") == 0)
    {
      // Get the id
      pos = s.find(":");
      tmpId = stoul(s.substr(7, pos - 7));

      // Get the subsequent strings and parse their data when appropropriate
      getline(fsObj, itemStr);
      getline(fsObj, opStr);
      getline(fsObj, testDivStr);
      getline(fsObj, s);
      testDestStr1.push_back(s);
      getline(fsObj, s);
      testDestStr2.push_back(s);

      // Operation symbol and argument
      opSymb = opStr[23];
      it = symb2Op.find(opSymb);
      if (it != symb2Op.end())
        tmpOp = it->second;
      else
      {
        cout << "ERROR: Could not find mapped operation for " << opSymb << endl;
        break;
      }
      if (opStr.find("old", 24) != string::npos)
      {
        useOldForArg2 = true;
        opArg2 = 0;
      }
      else
      {
        useOldForArg2 = false;
        opArg2 = stoull(opStr.substr(25));
      }

      // Test Divisor
      divisor = stoull(testDivStr.substr(21));
      runningDivisor *= divisor;

      // Get list of starting items
      startingItems = getItemListFromString(itemStr);

      // Create the monkey
      Monkey *m = new Monkey(tmpId, divisor, tmpOp, useOldForArg2, opArg2);
      m->isWorryReducedByDivision=false;  // New change for part 2
      for (uint64_t item : startingItems)
      {
        m->addItemToInventory(item);
      }
      troop.push_back(m);
    }
  }
  fsObj.close();

  // Go back through the test desinations and add the approprate monkey pointers, Set the new worry divisor for part 2 on each monkey
  for (i = 0; i < troop.size(); i++)
  {
    // If true statement is 29 characters until the monkey id
    // If false statement is 30 characters until the monkey id
    j1 = stoul(testDestStr1[i].substr(29));
    j2 = stoul(testDestStr2[i].substr(30));
    troop[i]->setTestDestMonkeys(troop[j1], troop[j2]);
    troop[i]->setWorryDivisor(runningDivisor);
  }

  // Iterate monkey passing for 10000 rounds
  for (i = 0; i < 10000; i++)
  {
    for (Monkey *m : troop)
    {
      m->act();
    }
    // cout << "After round " << i + 1 << ", monkeys look like this:" << endl;
    // for (Monkey *m : troop)
    // {
    //   m->reportItems();
    // }
    // cout << endl;
  }

  // Report monkey inspection count and calculate monkey business
  iCountM1 = 0;
  iCountM2 = 0;
  cout << endl;
  cout << "At CONCLUSION:" << endl;
  for (Monkey *m : troop)
  {
    iCount = m->getInspectionCount();
    if (iCount >= iCountM1)
    {
      iCountM2 = iCountM1;
      iCountM1 = iCount;
    }
    else if (iCount > iCountM2)
    {
      iCountM2 = iCount;
    }
    cout << "Monkey " << (uint32_t)m->id << " inspected items " << iCount << " times." << endl;
  }

  monkeyBusiness = iCountM1 * iCountM2;

  return monkeyBusiness;
}

int main()
{
  uint64_t result;
  string outStr1 = "Level of Monkey Business in ";
  string outStr2 = "Level of Monkey Business in ";
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