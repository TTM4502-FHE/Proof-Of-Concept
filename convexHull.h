#ifndef CONVEXHULL_H
#define CONVEXHULL_H

#include <stack>
#include <vector>

using std::stack;
using std::vector;
class Minutia;

Minutia secondTop(stack<Minutia> &stk);

int squaredDist(vector<double> p1, Minutia p2);

int squaredDist(Minutia p1, Minutia p2);

int direction(Minutia a, Minutia b, Minutia c);

int direction(vector<double> a, Minutia b, Minutia c);

int comp(const void *point1, const void *point2);

vector<Minutia> findConvexHull(vector<Minutia> minutiae_);

#endif