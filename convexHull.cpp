#include "convexHull.h"
#include "minutia.h"
#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;
using std::swap;
Minutia p0;

Minutia secondTop(stack<Minutia> &stk) {
  Minutia tempPoint = stk.top();
  stk.pop();
  Minutia res = stk.top();  // get the second top element
  stk.push(tempPoint);      // push previous top again
  return res;
}
int squaredDist(vector<double> p1, Minutia p2) {
  return ((p1[0] - p2.x_coordinate) * (p1[0] - p2.x_coordinate) +
          (p1[1] - p2.y_coordinate) * (p1[1] - p2.y_coordinate));
}

int squaredDist(Minutia p1, Minutia p2) {
  return ((p1.x_coordinate - p2.x_coordinate) *
              (p1.x_coordinate - p2.x_coordinate) +
          (p1.y_coordinate - p2.y_coordinate) *
              (p1.y_coordinate - p2.y_coordinate));
}

int direction(Minutia a, Minutia b, Minutia c) {
  int val =
      (b.y_coordinate - a.y_coordinate) * (c.x_coordinate - b.x_coordinate) -
      (b.x_coordinate - a.x_coordinate) * (c.y_coordinate - b.y_coordinate);
  if (val == 0)
    return 0;  // colinear
  else if (val < 0)
    return 2;  // anti-clockwise direction
  return 1;    // clockwise direction
}

int direction(vector<double> a, Minutia b, Minutia c) {
  int val = (b.y_coordinate - a[1]) * (c.x_coordinate - b.x_coordinate) -
            (b.x_coordinate - a[0]) * (c.y_coordinate - b.y_coordinate);
  if (val == 0)
    return 0;  // colinear
  else if (val < 0)
    return 2;  // anti-clockwise direction
  return 1;    // clockwise direction
}

int comp(const void *point1, const void *point2) {
  Minutia *p1 = (Minutia *)point1;
  Minutia *p2 = (Minutia *)point2;
  int dir = direction(p0, *p1, *p2);
  if (dir == 0) return (squaredDist(p0, *p2) >= squaredDist(p0, *p1)) ? -1 : 1;
  return (dir == 2) ? -1 : 1;
}
vector<Minutia> findConvexHull(vector<Minutia> minutiae_) {
  vector<Minutia> convexHullPoints;
  const int n = minutiae_.size();
  int minY = minutiae_[0].y_coordinate, min = 0;
  for (int i = 1; i < n; i++) {
    int y = minutiae_[i].y_coordinate;
    // find bottom most or left most point
    if (((y < minY) || (minY == y)) &&
        (minutiae_[i].x_coordinate < minutiae_[min].x_coordinate)) {
      minY = minutiae_[i].y_coordinate;
      min = i;
    }
  }
  swap(minutiae_[0], minutiae_[min]);  // swap min point to 0th location
  p0 = minutiae_[0];
  qsort(&minutiae_[1], n - 1, sizeof(minutiae_),
        comp);      // sort points from 1 place to end
  int arrSize = 1;  // used to locate items in modified array
  for (int i = 1; i < n; i++) {
    // when the angle of ith and (i+1)th elements are same, remove points
    while (i < n - 1 && direction(p0, minutiae_[i], minutiae_[i + 1]) == 0) i++;
    minutiae_[arrSize] = minutiae_[i];
    arrSize++;
  }

  if (arrSize < 3)
    return convexHullPoints;  // there must be at least 3 points, return empty
                              // list.
  // create a stack and add first three points in the stack
  stack<Minutia> stk;
  stk.push(minutiae_[0]);
  stk.push(minutiae_[1]);
  stk.push(minutiae_[2]);
  for (int i = 3; i < arrSize; i++) {  // for remaining vertices
    while (direction(secondTop(stk), stk.top(), minutiae_[i]) != 2) {
      stk.pop();
      if (stk.size() < 2) {
        break;
      }
    }  // when top, second top and ith point are not making left
       // turn, remove point
    stk.push(minutiae_[i]);
  }
  while (!stk.empty()) {
    convexHullPoints.push_back(stk.top());  // add points from stack
    stk.pop();
  }

  return convexHullPoints;
}