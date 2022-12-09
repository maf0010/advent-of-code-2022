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
  uint64_t size;

public:
  std::string name;
};

class ElvenFile : public ElvenFileSystemItemBase
{
public:
  ElvenFile(std::string inName, uint64_t inSize)
  {
    name = inName;
    size = inSize;
  }
  uint64_t getSize(void)
  {
    return size;
  }
};

class ElvenDirectory : public ElvenFileSystemItemBase
{
protected:
  uint64_t sizeOfFilesOnly = 0;
  uint64_t sizeOfSubdirsOnly = 0;
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

  uint64_t getSize(void)
  {
    if (!sizeValid)
    {
      calculateSize();
    }
    return size;
  }

  void addFile(string fName, uint64_t fSize)
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
        {
          pOut = d;
          break;
        }
      }
    }
    return pOut;
  }

  uint64_t calcSizeOverThreshold(const uint64_t threshold, uint64_t runningSum = 0)
  {
    uint64_t tmp;

    tmp = getSize();
    if (tmp <= threshold)
      runningSum += tmp;

    for (ElvenDirectory *d : subdirList)
      runningSum = d->calcSizeOverThreshold(threshold, runningSum);

    return runningSum;
  }

  void printTree(string spacer = "", bool includeDirSizes = false)
  {
    if (includeDirSizes)
      cout << spacer << "- " << name << " (dir, size=" << getSize() << ")" << endl;
    else
      cout << spacer << "- " << name << " (dir)" << endl;
    for (ElvenFile *f : fileList)
    {
      cout << spacer + "  "
           << "- " << f->name << " (file, size=" << f->getSize() << ")" << endl;
    }
    for (ElvenDirectory *d : subdirList)
    {
      d->printTree(spacer + "  ", includeDirSizes);
    }
  }

  ElvenDirectory *getDirectoryToFree(const uint64_t sizeToFree, ElvenDirectory *curClosestDir = nullptr)
  {
    if (curClosestDir == nullptr)
      curClosestDir = this;
    if (getSize() >= sizeToFree) 
    {
      // If this dirs size is smaller than the current closest dir's size, then set it as the new closest
      if (getSize() < curClosestDir->getSize())
        curClosestDir = this;
      // Check subdirs as they might be closer than this parent
      for (ElvenDirectory *d : subdirList)
        curClosestDir = d->getDirectoryToFree(sizeToFree, curClosestDir);
    }
    return curClosestDir;
  }
};

// FUNCTION DEFINITIONS

uint64_t part1(string fName)
{
  ElvenDirectory rootElfDir = ElvenDirectory("/");
  ElvenDirectory *pCurDir, *pTmp;
  uint64_t sizeWithinThreshold = 0;
  const uint64_t threshold = 100000;
  string s, itemName;
  size_t pos;

  // Open file
  ifstream fsObj(fName);

  // Read through file, interpreting console input and building file tree
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Check for cd
    if (s.find("$ cd") != string::npos)
    {
      itemName = s.substr(5);
      if (itemName == "/")
        pCurDir = &rootElfDir;
      else
      {
        pTmp = pCurDir->getDir(itemName);
        if (pTmp == nullptr)
          cout << "Error, requested directory does not exist (REQ, CURRENT) " << itemName << " | " << pCurDir->name << endl;
        else
          pCurDir = pTmp;
      }
    }
    // Anything else is either ls or output of ls, but we don't need to do anything for ls command
    else if (s.find("$ ls") == string::npos)
    {
      // Check for dir in output
      if (s.find("dir") != string::npos)
      {
        pCurDir->addSubdir(s.substr(4));
      }
      // If not a dir, then it's a file
      else
      {
        pos = s.find(" ");
        pCurDir->addFile(s.substr(pos + 1), stoull(s.substr(0, pos)));
      }
    }
  }
  fsObj.close();

  // Print the file system structure
  cout << "File Structure in " << fName << endl;
  rootElfDir.printTree("", true);

  // Calculate space
  sizeWithinThreshold = rootElfDir.calcSizeOverThreshold(100000);

  return sizeWithinThreshold;
}

uint64_t part2(string fName)
{
  ElvenDirectory rootElfDir = ElvenDirectory("/");
  ElvenDirectory *pCurDir, *pTmp;
  string s, itemName;
  size_t pos;

  // Open file
  ifstream fsObj(fName);

  // Read through file, interpreting console input and building file tree
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Check for cd
    if (s.find("$ cd") != string::npos)
    {
      itemName = s.substr(5);
      if (itemName == "/")
        pCurDir = &rootElfDir;
      else
      {
        pTmp = pCurDir->getDir(itemName);
        if (pTmp == nullptr)
          cout << "Error, requested directory does not exist (REQ, CURRENT) " << itemName << " | " << pCurDir->name << endl;
        else
          pCurDir = pTmp;
      }
    }
    // Anything else is either ls or output of ls, but we don't need to do anything for ls command
    else if (s.find("$ ls") == string::npos)
    {
      // Check for dir in output
      if (s.find("dir") != string::npos)
      {
        pCurDir->addSubdir(s.substr(4));
      }
      // If not a dir, then it's a file
      else
      {
        pos = s.find(" ");
        pCurDir->addFile(s.substr(pos + 1), stoull(s.substr(0, pos)));
      }
    }
  }
  fsObj.close();

  // Calculate current unused space and how much space needs to be freed
  uint64_t currentUnusedSpace = 70000000 - rootElfDir.getSize();
  uint64_t spaceToFree = 30000000 - currentUnusedSpace;
  pTmp = &rootElfDir;
  pTmp = rootElfDir.getDirectoryToFree(spaceToFree);
  cout << "Directory to delete is "<< pTmp->name << endl;

  return pTmp->getSize();
}

int main()
{
  uint64_t result;

  // // -- PART 1
  cout << "==============PART1==============" << endl;
  // Run on example input
  result = part1("example.txt");
  cout << "Total size of directories over threshold in example.txt is " << result << endl;

  // Run on actual input
  result = part1("input.txt");
  cout << "Total size of directories over threshold in input.txt is " << result << endl;

  // -- PART 2
  cout << endl
       << "==============PART2==============" << endl;
  // Run on example input
  result = part2("example.txt");
  cout << "Total size of directory to be deleted in example.txt is " << result << endl;

  // Run on actual input
  result = part2("input.txt");
  cout << "Total size of directory to be deleted in input.txt is " << result << endl;

  return 0;
}