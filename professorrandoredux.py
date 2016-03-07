#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2016 evanlimanto <evanlimanto@gmail.com>
#
# Distributed under terms of the MIT license.

# Solver program to the puzzle at
# https://www.janestreet.com/puzzles/professor-rando-redux/

def mul(x, y):
  return x * y

def abs(x, y):
  if x > y:
    return x - y
  return y - x

def sum(x, y):
  return x + y

N = 1
while N > 0:
  print N
  funcs = [abs, max, min, sum, mul]
  scores = [0] * 5
  arr = []
  for i in range(1, N + 1):
    for j in range(i, N + 1):
      arr.append([(i, j, func(i, j)) for func in funcs])
  pos = 0
  count = 0
  while len(arr) > 0:
    s = {}
    for item in arr:
      value = item[pos][2]
      if value in s:
        s[value] += 1
      else:
        s[value] = 1
    count = len(s)
    for key, value in s.items():
      if value == 1:
        for (i, item) in enumerate(arr):
          if item[pos][2] == key:
            # print "pos : {}, pair : ({}, {})".format(pos, item[pos][0], item[pos][1])
            arr = arr[:i] + arr[i+1:]
            score = 1 if item[pos][0] == item[pos][1] else 2
            scores[pos] += score
            break
    pos = (pos + 1) % 5
  print scores
  N += 1
