#ifndef USEFULMETHODS_H
#define USEFULMETHODS_H
// #include "minutia.h"
// #include "cylinder.h"
#include "fingerprintTemplate.h"
#include <vector>

using std::vector;
class Minutia;
class Cylinder;


// Euclidean distance between two points
double euclideanDistance(const double x1, const double y1, const double x2,
                         const double y2);

double euclideanDistance(const double x1, const double y1, const Minutia m);

// Given three collinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(const Minutia p, const Minutia q, const vector<double> r);

bool onSegment(vector<double> p, Minutia q, vector<double> r);

bool onSegment(Minutia p, vector<double> q, Minutia r);

bool onSegment(vector<double> p, vector<double> q, Minutia r);

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are collinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Minutia p, Minutia q, vector<double> r);

int orientation(Minutia p, vector<double> q, Minutia r);

int orientation(vector<double> p, vector<double> q, Minutia r);

// The function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool doIntersect(Minutia p1, Minutia q1, vector<double> p2, vector<double> q2);

// Check if point is inside convex hull
// Returns true if the point p lies inside the polygon[] with n vertices
bool isInside(vector<Minutia> polygon, int n, vector<double> p);


double distanceOfVector(vector<double> v);

double distanceOfVectorBinary(vector<int> v);

int hammingDistance(Cylinder a, Cylinder b);

double hammingSimilarity(FingerprintTemplate A, FingerprintTemplate B);

double directionalDifference(double angle1, double angle2);

#endif