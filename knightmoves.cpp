/*
 * knightmoves.cpp
 * Copyright (C) 2016 evanlimanto <evanlimanto@gmail.com>
 *
 * Distributed under terms of the MIT license.
 *
 * Solver program to the puzzle at
 * https://www.janestreet.com/puzzles/knight-moves/
 */

#include <iomanip>
#include <fstream>
#include <vector>
#include <cstring>
#include <cassert>
#include <cstdio>
#include <utility>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <unordered_set>

using namespace std;

const int knightDx[] = {-2, -1, 1, 2, 2, 1, -1, -2};
const int knightDy[] = {-1, -2, -2, -1, 1, 2, 2, 1};
const int kEndStepNumber = 28;

int rowsums[] = {10, 34, 108, 67, 63, 84, 24, 16};
int colsums[] = {7, 14, 72, 66, 102, 90, 42, 13};
unordered_set<int> initStep, initPos;
vector<int> rowScores, colScores;
pair<int, int> steps[64]; // (key, value) -> (stepNumber, position)
int visited[64]; // (key, value) -> (position, stepNumber)

inline int calcPos(int x, int y) {
  int pos = x * 8 + y;
  assert(pos >= 0 && pos < 64);
  return pos;
}

inline bool isVisited(int x, int y) {
  return visited[calcPos(x, y)] > 0;
}

inline void markVisited(int x, int y, int stepNumber) {
  int pos = calcPos(x, y);
  steps[stepNumber] = make_pair(x, y);
  visited[pos] = stepNumber;
}

inline void markUnvisited(int x, int y) {
  int pos = calcPos(x, y);
  assert(visited[pos] > 0);
  steps[visited[pos]] = make_pair(0, 0);
  visited[pos] = 0;
}

inline bool validateBoard() {
  for (int i = 0; i < 8; i++)
    if (rowScores[i] != rowsums[i] || colScores[i] != colsums[i])
      return false;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int curX, curY, nextX, nextY;
      {
        curX = i; curY = j;
        nextX = j; nextY = 7 - i;
        if ((visited[calcPos(curX, curY)] > 0) != (visited[calcPos(nextX, nextY)] > 0))
          return false;
      }
      {
        curX = j; curY = 7 - i; 
        nextX = 7 - i; nextY = 7 - j;
        if ((visited[calcPos(curX, curY)] > 0) != (visited[calcPos(nextX, nextY)] > 0))
          return false;
      }
      {
        curX = 7 - i; curY = 7 - j;
        nextX = 7 - j; nextY = i;
        if ((visited[calcPos(curX, curY)] > 0) != (visited[calcPos(nextX, nextY)] > 0))
          return false;
      }
      {
        curX = 7 - j; curY = i;
        nextX = i; nextY = j;
        if ((visited[calcPos(curX, curY)] > 0) != (visited[calcPos(nextX, nextY)] > 0))
          return false;
      }
    }
  }
  return true;
}

inline bool validPos(int x, int y) {
  return (x >= 0 && x < 8 && y >= 0 && y < 8);
}

inline void printBoard(bool outputToFile = true) {
  int board[8][8];
  memset(board, 0, sizeof(board));

  for (int i = 1; i <= 28; i++) {
    pair<int, int> &pos = steps[i];
    board[pos.first][pos.second] = i;
  }

  printf("\n");
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      printf("%2d%c", board[i][j], " \n"[j == 7]);
    }
  }
  printf("\n");

  if (outputToFile) {
    ofstream fout("answerBoard");
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        fout << board[i][j];
        if (j < 8)
          fout << " ";
        else
          fout << "\n";
      }
    }
    fout.close();
  }
}

bool recurse(int x, int y, int stepNumber) {
  if (stepNumber == kEndStepNumber) {
    if (validateBoard()) {
      printBoard();
      return true;
    }
    return false;
  }

  for (int i = 0; i < 8; i++) {
    int nextX = x + knightDx[i];
    int nextY = y + knightDy[i];

    if (validPos(nextX, nextY)) {
      if (initStep.find(stepNumber + 1) != initPos.end()) {
        if (initPos.find(calcPos(nextX, nextY)) != initPos.end() &&
            recurse(nextX, nextY, stepNumber + 1))
          return true;
      } else if (!isVisited(nextX, nextY)) {
        bool rowScoresValid = (rowScores[nextX] + stepNumber + 1 <= rowsums[nextX]);
        bool colScoresValid = (colScores[nextY] + stepNumber + 1 <= colsums[nextY]);
        
        if (!rowScoresValid || !colScoresValid)
          continue;

        rowScores[nextX] += (stepNumber + 1);
        colScores[nextY] += (stepNumber + 1);
        markVisited(nextX, nextY, stepNumber + 1);
        if (recurse(nextX, nextY, stepNumber + 1))
          return true;
        markUnvisited(nextX, nextY);
        rowScores[nextX] -= (stepNumber + 1);
        colScores[nextY] -= (stepNumber + 1);
      }
    }
  }
  return false;
}

void solve() {
  int startX = 0, startY = 0;
  if (ifstream("lastPosition")) {
    ifstream fin("lastPosition"); 
    fin >> startX >> startY;
    fin.close();
  }

  for (int i = startX; i < 8; i++) {
    for (int j = (i == startX) ? startY : 0; j < 8; j++) {
      printf("Trying to start at position (%d, %d)...\n", i, j);
      if (!isVisited(i, j)) {
        ofstream fout("lastPosition", ios_base::out | ios_base::trunc);
        fout << i << " " << j;
        fout.close();

        rowScores[i]++;
        colScores[j]++;
        markVisited(i, j, 1);
        if (recurse(i, j, 1))
          return;
        markUnvisited(i, j);
        rowScores[i]--;
        colScores[j]--;
      }
      printf("Failed!\n");
    }
  }
}

int main() {
  steps[8] = make_pair(4, 2);
  steps[11] = make_pair(2, 3);
  steps[14] = make_pair(3, 5);
  steps[15] = make_pair(5, 4);
  visited[calcPos(4, 2)] = 8;
  visited[calcPos(2, 3)] = 11;
  visited[calcPos(3, 5)] = 14;
  visited[calcPos(5, 4)] = 15;
  initStep.insert(8);
  initStep.insert(11);
  initStep.insert(14);
  initStep.insert(15);
  initPos.insert(calcPos(4, 2));
  initPos.insert(calcPos(2, 3));
  initPos.insert(calcPos(3, 5));
  initPos.insert(calcPos(5, 4));

  rowScores.resize(8, 0);
  colScores.resize(8, 0);
  rowScores[3] = colScores[5] = 14;
  rowScores[5] = colScores[4] = 15;
  rowScores[2] = colScores[3] = 11;
  rowScores[4] = colScores[2] = 8;

  solve();
  return 0;
}
