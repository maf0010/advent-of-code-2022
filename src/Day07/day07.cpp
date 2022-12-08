#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

// PROTOTYPES

// CLASSES
class ElvenFileSystemItemBase
{
protected:
  uint32_t size;

public:
  std::string name;
};

class ElvenFile : public ElvenFileSystemItemBase
{
public:
  ElvenFile(std::string inName, uint32_t inSize)
  {
    name = inName;
    size = inSize;
  }
  uint32_t getSize(void)
  {
    return size;
  }
};

class ElvenDirectory : public ElvenFileSystemItemBase
{
protected:
  uint32_t sizeOfFilesOnly = 0;
  uint32_t sizeOfSubdirsOnly = 0;
  bool sizeValid = false;
  vector<ElvenFile *> fileList;
  vector<ElvenDirectory *> subdirList;
  ElvenDirectory *myParentDir;

public:
  ElvenDirectory(std::string inName, ElvenDirectory *pParentDir = nullptr)
  {
    name = inName;
    size = 0;
    myParentDir = pParentDir;
  }

  void calculateSize(void)
  {
    sizeOfSubdirsOnly = 0;
    sizeOfFilesOnly = 0;
    for (ElvenFile *f : fileList)
      sizeOfFilesOnly += f->getSize();
    for (ElvenDirectory *d : subdirList)
      sizeOfSubdirsOnly += d->getSize();
    size = sizeOfFilesOnly + sizeOfSubdirsOnly;
    sizeValid = true;
  }

  uint32_t getSize(void)
  {
    if (!sizeValid)
    {
      calculateSize();
    }
    return size;
  }

  void addFile(string fName, uint32_t fSize)
  {
    fileList.push_back(new ElvenFile(fName, fSize));
  }

  void addSubdir(string dName)
  {
    subdirList.push_back(new ElvenDirectory(dName, this));
  }

  ElvenDirectory *getDir(string dName)
  {
    ElvenDirectory *pOut = nullptr;
    if (dName == "..")
    {
      if (myParentDir != nullptr)
        pOut = myParentDir;
      else
        pOut = this;
    }
    else
    {
      for (ElvenDirectory *d : subdirList)
      {
        if (d->name == dName)
          pOut = d;
      }
    }
    return pOut;
  }

  void printTree(string spacer = "")
  {
    cout << spacer << "- " << name << " (dir)" << endl;
    for (ElvenFile *f : fileList)
    {
      cout << spacer + "  "
           << "- " << f->name << " (file, size=" << f->getSize() << ")" << endl;
    }
    for (ElvenDirectory *d : subdirList)
    {
      d->printTree(spacer + "  ");
    }
  }
};

// FUNCTION DEFINITIONS

uint32_t part1(string fName)
{
  ElvenDirectory rootElfDir = ElvenDirectory("/");
  ElvenDirectory *pCurDir, *pTmp;
  uint32_t sizeWithinThreshold = 0;
  const uint32_t threshold = 100000;
  string s, itemName;

  // Open file
  ifstream fsObj(fName);

  // Read through file, interpreting console input and building file tree
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;
    
    // Check for command
    if (s.find("$ cd") != string::npos)
    {
      itemName = s.substr(5);
      if (itemName == "/")
        pCurDir = &rootElfDir;
      else
      {
        pTmp = pCurDir->getDir(itemName);
        if (pTmp==nullptr)
          cout << "Error, requested directory does not exist (REQ, CURRENT) " << itemName << " | " << pCurDir->name << endl;
        else
          pCurDir = pTmp;
      }
    }
    // TODO: PARSE "ls" command
    // TOOD: PARSE "dir <x>" output
    // TODO: PARSE "<bytes> <fname> output"
  }
  fsObj.close();

  return sizeWithinThreshold;
}

// string part2(string fName)
// {

// }

int main()
{
  // string result;

  // // // -- PART 1
  // cout << "==============PART1==============" << endl;
  // // Run on example input
  // result = part1("example.txt");
  // cout << "Items at top of stack in example.txt are " << result << endl;

  // // Run on actual input
  // result = part1("input.txt");
  // cout << "Items at top of stack in input.txt are " << result << endl;

  // // -- PART 2
  // cout << endl
  //      << "==============PART2==============" << endl;
  // // Run on example input
  // result = part2("example.txt");
  // cout << "Items at top of stack in example.txt are " << result << endl;

  // // Run on actual input
  // result = part2("input.txt");
  // cout << "Items at top of stack in input.txt are " << result << endl;

  return 0;
}