#include "usefulMethods.h"
#include "globals.h"
#include <algorithm>
#include <iostream>

using std::cout;
using std::max;
using std::min;

// Euclidean distance between two points
double euclideanDistance(const double x1, const double y1, const double x2,
                         const double y2) {
  double x = x2 - x1;  // calculating number to square in next step
  double y = y2 - y1;
  double dist;
  dist = pow(x, 2) + pow(y, 2);  // calculating Euclidean distance
  dist = sqrt(dist);
  return dist;
}

double euclideanDistance(const double x1, const double y1, const Minutia m) {
  double x = m.x_coordinate - x1;  // calculating number to square in next step
  double y = m.y_coordinate - y1;
  double dist;
  dist = pow(x, 2) + pow(y, 2);  // calculating Euclidean distance
  dist = sqrt(dist);
  return dist;
}

// Given three collinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(const Minutia p, const Minutia q, const vector<double> r) {
  if (q.x_coordinate <= max(p.x_coordinate, r[0]) &&
      q.x_coordinate >= min(p.x_coordinate, r[0]) &&
      q.y_coordinate <= max(p.y_coordinate, r[1]) &&
      q.y_coordinate >= min(p.y_coordinate, r[1]))
    return true;
  return false;
}

bool onSegment(vector<double> p, Minutia q, vector<double> r) {
  if (q.x_coordinate <= max(p[0], r[0]) && q.x_coordinate >= min(p[0], r[0]) &&
      q.y_coordinate <= max(p[1], r[1]) && q.y_coordinate >= min(p[1], r[1]))
    return true;
  return false;
}

bool onSegment(Minutia p, vector<double> q, Minutia r) {
  if (q[0] <= max(p.x_coordinate, r.x_coordinate) &&
      q[0] >= min(p.x_coordinate, r.x_coordinate) &&
      q[1] <= max(p.y_coordinate, r.y_coordinate) &&
      q[1] >= min(p.y_coordinate, r.y_coordinate))
    return true;
  return false;
}

bool onSegment(vector<double> p, vector<double> q, Minutia r) {
  if (q[0] <= max(p[0], r.x_coordinate) && q[0] >= min(p[0], r.x_coordinate) &&
      q[1] <= max(p[1], r.y_coordinate) && q[1] >= min(p[1], r.y_coordinate))
    return true;
  return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are collinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Minutia p, Minutia q, vector<double> r) {
  int val = (q.y_coordinate - p.y_coordinate) * (r[0] - q.x_coordinate) -
            (q.x_coordinate - p.x_coordinate) * (r[1] - q.y_coordinate);

  if (val == 0) return 0;    // collinear
  return (val > 0) ? 1 : 2;  // clock or counterclock wise
}

int orientation(Minutia p, vector<double> q, Minutia r) {
  int val = (q[1] - p.y_coordinate) * (r.x_coordinate - q[0]) -
            (q[0] - p.x_coordinate) * (r.y_coordinate - q[1]);

  if (val == 0) return 0;    // collinear
  return (val > 0) ? 1 : 2;  // clock or counterclock wise
}

int orientation(vector<double> p, vector<double> q, Minutia r) {
  int val = (q[1] - p[1]) * (r.x_coordinate - q[0]) -
            (q[0] - p[0]) * (r.y_coordinate - q[1]);

  if (val == 0) return 0;    // collinear
  return (val > 0) ? 1 : 2;  // clock or counterclock wise
}

// The function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool doIntersect(Minutia p1, Minutia q1, vector<double> p2, vector<double> q2) {
  // Find the four orientations needed for general and
  // special cases
  int o1 = orientation(p1, q1, p2);
  int o2 = orientation(p1, q1, q2);
  int o3 = orientation(p2, q2, p1);
  int o4 = orientation(p2, q2, q1);

  // General case
  if (o1 != o2 && o3 != o4) return true;

  // Special Cases
  // p1, q1 and p2 are collinear and p2 lies on segment p1q1
  if (o1 == 0 && onSegment(p1, p2, q1)) return true;

  // p1, q1 and p2 are collinear and q2 lies on segment p1q1
  if (o2 == 0 && onSegment(p1, q2, q1)) return true;

  // p2, q2 and p1 are collinear and p1 lies on segment p2q2
  if (o3 == 0 && onSegment(p2, p1, q2)) return true;

  // p2, q2 and q1 are collinear and q1 lies on segment p2q2
  if (o4 == 0 && onSegment(p2, q1, q2)) return true;

  return false;  // Doesn't fall in any of the above cases
}

// Check if point is inside convex hull
// Returns true if the point p lies inside the polygon[] with n vertices
bool isInside(vector<Minutia> polygon, int n, vector<double> p) {
  // There must be at least 3 vertices in polygon[]
  if (n < 3) return false;

  // Create a point for line segment from p to infinite
  vector<double> extreme = {INF, p[1]};

  // Count intersections of the above line with sides of polygon
  int count = 0, i = 0;
  do {
    int next = (i + 1) % n;

    // Check if the line segment from 'p' to 'extreme' intersects
    // with the line segment from 'polygon[i]' to 'polygon[next]'
    if (doIntersect(polygon[i], polygon[next], p, extreme)) {
      // If the point 'p' is collinear with line segment 'i-next',
      // then check if it lies on segment. If it lies, return true,
      // otherwise false
      if (orientation(polygon[i], p, polygon[next]) == 0)
        return onSegment(polygon[i], p, polygon[next]);

      count++;
    }
    i = next;
  } while (i != 0);
  // Return true if count is odd, false otherwise
  if (count % 2 == 1) {
    return true;
  } else {
    return false;
  }
}

double distanceOfVector(vector<double> v) {
  double temp = 0;
  for (double i : v) {
    temp = temp + pow(i, 2);
  }
  return sqrt(temp);
}

double distanceOfVectorBinary(vector<int> v) {
  double temp = 0;
  for (int i : v) {
    temp = temp + pow(i, 2);
  }
  return sqrt(temp);
}

int hammingDistance(Cylinder a, Cylinder b) {
  int dist_counter = 0;
  for (auto it = a.c_m_binary.begin(), end = a.c_m_binary.end(); it != end;
       it++) {
    auto index = distance(a.c_m_binary.begin(), it);
    if (*it != b.c_m_binary[index]) {
      dist_counter += 1;
    }
  }
  return dist_counter;
}

double hammingSimilarity(FingerprintTemplate A, FingerprintTemplate B) {
  double num_of_comparisons = A.cylinders.size() * B.cylinders.size();
  double temp = 0.0;
  for (Cylinder r : A.cylinders) {
    for (Cylinder c : B.cylinders) {
      temp += hammingDistance(r, c);
    }
  }
  return temp / num_of_comparisons;
}

double directionalDifference(double angle1, double angle2) {
  // Equation (9) from paper
  if (-M_PI <= angle1 - angle2 && angle1 - angle2 < M_PI) {
    return angle1 - angle2;
  } else if (angle1 - angle2 < -M_PI) {
    return 2 * M_PI + angle1 - angle2;
  } else if (angle1 - angle2 >= M_PI) {
    return -2 * M_PI + angle1 - angle2;
  }
  cout << "Something really bad has happened, and the angles are out "
          "of scope for directional difference"
       << '\n';
  return 0;
}