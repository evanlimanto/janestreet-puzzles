#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2016 evanlimanto <evanlimanto@gmail.com>
#
# Distributed under terms of the MIT license.

# Solver program to the puzzle at
# https://www.janestreet.com/puzzles/travel-agent/

from random import shuffle
import argparse
import multiprocessing as mp
import time
import sys
import threading

board = [
  [1, 5, 4, 4, 6, 1, 1, 4, 1, 3, 7, 5],
  [3] + [-1] * 10 + [1],
  [4, -1, 6, 4, 1, 8, 1, 4, 2, 1, -1, 3],
  [7, -1, 1] + [-1] * 6 + [1, -1, 2],
  [1, -1, 1, -1, 6, 1, 6, 2, -1, 2, -1, 1],
  [8, -1, 4, -1, 1, -1, -1, 8, -1, 3, -1, 5],
  [4, -1, 2, -1, 5, -1, -1, 3, -1, 5, -1, 2],
  [8, -1, 5, -1, 1, 1, 2, 3, -1, 4, -1, 6],
  [6, -1, 1] + [-1] * 6 + [3, -1, 6],
  [3, -1, 6, 3, 6, 5, 4, 3, 4, 5, -1, 1],
  [6] + [-1] * 10 + [3],
  [2, 1, 6, 6, 4, 5, 2, 1, 1, 1, 7, 1]
]

def getBestScore():
  score_lock.acquire()
  with open("bestScore", "r") as f:
    val = int(f.readline())
    f.close()
    score_lock.release()
    return val

def writeBestData(score, s):
  score_lock.acquire()
  with open("bestScore", "w+") as f:
    f.write(str(score))
    f.close()
  with open("bestData", "w+") as f:
    f.write(s)
    f.close()
  score_lock.release()

def printBoard(board):
  s = "Board Contents:\n"
  for row in board:
    fmt = ' '.join(['%2d'] * 12)
    s += (fmt % tuple(row))
    s += '\n'
  return s

start = (0, 0)
end = (11, 11)
deltaR = (1, 0, -1, 0)
deltaC = (0, -1, 0, 1)

def printVisited(position, visited):
  board = [[0] * 12 for _ in range(12)]
  board[position[0]][position[1]] = 1
  while position != start:
    prev_position = visited[position[0]][position[1]]
    board[prev_position[0]][prev_position[1]] = board[position[0]][position[1]] + 1
    position = prev_position
  return printBoard(board)

def prev_index(index):
  return 3 if index == 0 else index - 1
def next_index(index):
  return 0 if index == 3 else index + 1
def get_direction(current_position, next_position):
  for i in range(4):
    if current_position[0] + deltaR[i] == next_position[0] and \
       current_position[1] + deltaC[i] == next_position[1]:
      return i
def get_opp_direction(direction):
  return (direction + 2) % 4

class Dice:
  BOTTOM = 0
  LEFT = 1
  RIGHT = 2
  TOP = 3
  FRONT = 4
  BACK = 5
  DUMMY_VALUE = -1

  def __init__(self, position, values = [(-1, 0) for _ in range(6)]):
    self.values = list(values)
    self.position = position

  @classmethod
  def fromDice(cls, other):
    return cls(other.position, other.values)

  def is_valid(self):
    return self.position[0] >= 0 and self.position[0] < 12 and \
           self.position[1] >= 0 and self.position[1] < 12

  def inc_top(self):
    self.values[Dice.TOP] = (self.values[Dice.TOP][0], self.values[Dice.TOP][1] + 1)

  def dec_top(self):
    self.values[Dice.TOP] = (self.values[Dice.TOP][0], self.values[Dice.TOP][1] - 1)

  def get_score(self):
    score = 1
    for pair in self.values:
      score = score * (pair[0] ** pair[1])
    return score

  def get_top(self):
    return self.values[Dice.TOP]

  def set_top(self, new_value):
    self.values[Dice.TOP] = new_value

  def mutate_position(self, position_index):
    self.position = (self.position[0] + deltaR[position_index], self.position[1] + deltaC[position_index])

  def mutate_values(self, mutated_positions):
    new_values = list(self.values)
    for i in range(4):
      current_position = mutated_positions[i]
      next_position = mutated_positions[next_index(i)]
      new_values[current_position] = self.values[next_position]
    self.values = list(new_values)

  def rotate(self, rotation_index):
    rotation_methods = [self.rotateSouth, self.rotateWest, self.rotateNorth, self.rotateEast]
    rotation_methods[rotation_index]()

  def rotateEast(self):
    mutated_positions = (Dice.BOTTOM, Dice.RIGHT, Dice.TOP, Dice.LEFT)
    self.mutate_values(mutated_positions)
    self.mutate_position(3)

  def rotateWest(self):
    mutated_positions = (Dice.BOTTOM, Dice.LEFT, Dice.TOP, Dice.RIGHT)
    self.mutate_values(mutated_positions)
    self.mutate_position(1)

  def rotateNorth(self):
    mutated_positions = (Dice.BOTTOM, Dice.FRONT, Dice.TOP, Dice.BACK)
    self.mutate_values(mutated_positions)
    self.mutate_position(2)

  def rotateSouth(self):
    mutated_positions = (Dice.BOTTOM, Dice.BACK, Dice.TOP, Dice.FRONT)
    self.mutate_values(mutated_positions)
    self.mutate_position(0)

  def fillMax(self):
    for i in range(len(self.values)):
      if self.values[i][0] == Dice.DUMMY_VALUE:
        self.values[i] = (9, self.values[i][1])

  def __str__(self):
    return 'Values: {}'.format(self.values)

def backtrack():
  nonlocal dice, visited, best_score, directions
  if dice.position == end:
    temp_dice = Dice.fromDice(dice)
    temp_dice.fillMax()
    cumulative_score = temp_dice.get_score()

    # Only compute path if score is better than previous best
    if cumulative_score > best_score:
      sys.stdout.flush()
      best_score = cumulative_score
      print(best_score)
    if cumulative_score > getBestScore():
      print(cumulative_score)
      print(temp_dice)
      sys.stdout.flush()

      scores = [[0] * 12 for _ in range(12)]
      steps = [[0] * 12 for _ in range(12)]
      scores[start[0]][start[1]] = 1

      current_position = end
      steps[end[0]][end[1]] = 1

      while current_position != start:
        # Sanity checking
        # if board[current_position[0]][current_position[1]] != Dice.DUMMY_VALUE:
        #  assert temp_dice.get_top()[0] == board[current_position[0]][current_position[1]]
        scores[current_position[0]][current_position[1]] = temp_dice.get_top()[0]
        prev_position = visited[current_position[0]][current_position[1]]
        steps[prev_position[0]][prev_position[1]] = steps[current_position[0]][current_position[1]] + 1

        direction = get_direction(prev_position, current_position)
        temp_dice.rotate(get_opp_direction(direction))

        current_position = prev_position

      s = str(dice) + "\n"
      s += printBoard(scores) + "\n"
      s += printBoard(steps)

      writeBestData(cumulative_score, s)
    return

  currentRow, currentCol = dice.position
  # Try all four possible directions, randomized to increase entropy
  for direction in directions:
    dice.rotate(direction)
    opposite_direction = get_opp_direction(direction)
    row, col = dice.position
    if dice.is_valid() and visited[row][col] == (-1, -1):
      visited[row][col] = (currentRow, currentCol)
      if board[row][col] == Dice.DUMMY_VALUE:
        dice.inc_top()
        backtrack()
        dice.dec_top()
      elif dice.get_top()[0] == Dice.DUMMY_VALUE:
        prev_value = dice.get_top()
        dice.set_top((board[row][col], prev_value[1] + 1))
        backtrack()
        dice.set_top(prev_value)
      elif dice.get_top()[0] == board[row][col]:
        dice.inc_top()
        backtrack()
        dice.dec_top()
      visited[row][col] = (-1, -1)
    dice.rotate(opposite_direction)

best_score = 0
visited = [[(-1, -1)] * 12 for _ in range(12)]

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument('--reverse', help="Reverse start and end positions", action="store_true")
  args = parser.parse_args()

  if args.reverse:
    start, end = end, start

  dice = Dice(start)
  dice.set_top((board[start[0]][start[1]], 1))
  visited[start[0]][start[1]] = start

  directions = list(range(4))
  shuffle(directions)
  backtrack()