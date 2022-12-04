#include <iostream>
#include <fstream>
#include <map>

using namespace std;

int getMatchScore(int opp, int me)
{
  // Assuming 1,2,3 to R,P,S, difference between opponent and me,
  //  when upscaled by 3+1 and then modulo by 3, produces 0,1,2 for Loss, Draw, Win, respectively.
  // Triple those resulting values for proper score according to the guide
  return 3 * ((me - opp + 4) % 3);
}

int getMyPlayScore(int opp, int outcome)
{
  // Assuming 1,2,3 to R,P,S and outcome encoding is 0,1,2 for Loss, Draw, Win
  // Formula below will determine which play I would make (1,2,3 encoding) in order to get specified outcome
  return (outcome + opp + 1) % 3 + 1;
}

int part1(string fName)
{
  /*
  Scoring Guide:
  A = X = Rock      = 1 pts
  B = Y = Paper     = 2 pts
  C = Z = Scissors  = 3 pts
  Lose              = 0 pts
  Draw              = 3 pts
  Win               = 6 pts
  */
  char chOpp, chMe;
  int iOpp, iMe;
  int iTotalScore = 0;

  // Open and scan through the file
  ifstream fsObj(fName);

  while (true)
  {
    // Pull characters
    fsObj >> chOpp >> chMe;

    // Check if EOF has been reached
    if (fsObj.eof())
      break;

    // Map Rock,Paper,Scissor character to point value by taking advantage of ASCII values for the characters
    iOpp = (int)chOpp - 64;
    iMe = (int)chMe - 87;

    // Get Match Score
    iTotalScore += iMe + getMatchScore(iOpp, iMe);
  }
  fsObj.close();

  return iTotalScore;
}

int part2(string fName)
{
  /*
  Corrected Scoring Guide:
  A = Rock      = 1 pts
  B = Paper     = 2 pts
  C = Scissors  = 3 pts
  X = Lose      = 0 pts
  Y = Draw      = 3 pts
  Z = Win       = 6 pts
  */
  char chOpp, chOutcome;
  int iOpp, iOutcome;
  int iTotalScore = 0;

  // Open and scan through the file
  ifstream fsObj(fName);

  while (true)
  {
    // Pull characters
    fsObj >> chOpp >> chOutcome;

    // Check if EOF has been reached
    if (fsObj.eof())
      break;

    // Map RPS and Outcome characters to point value by taking advantage of ASCII values for the characters
    iOpp = (int)chOpp - 64;         // 1, 2, or 3
    iOutcome = (int)chOutcome - 88; // 0, 1, or 2

    // Get My Play, Add it to the Match Score (iOutcome*3pts), and Add both to running total
    iTotalScore += getMyPlayScore(iOpp, iOutcome) + 3 * iOutcome;
  }
  fsObj.close();

  return iTotalScore;
}

int main()
{
  int score;

  // -- PART 1
  cout << "==============PART1==============" << endl;
  // Run on example input
  score = part1("example.txt");
  cout << "Score total from example.txt is " << score << endl;

  // Run on actual input
  score = part1("input.txt");
  cout << "Score total from input.txt is " << score << endl;

  // -- PART 2
  cout << endl
       << "==============PART2==============" << endl;
  // Run on example input
  score = part2("example.txt");
  cout << "Score total from example.txt is " << score << endl;

  // Run on actual input
  score = part2("input.txt");
  cout << "Score total from input.txt is " << score << endl;

  return 0;
}