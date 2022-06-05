#include "fingerprintTemplate.h"
#include "convexHull.h"

FingerprintTemplate::FingerprintTemplate(vector<Minutia> minutiae1) {
  minutiae = minutiae1;
  convexHull = findConvexHull(minutiae);
  fillWithCylinders();
  CS = createCylinderSet();
}

Cylinder FingerprintTemplate::getCylinderAtIndex(int index) {
  auto cylinder_front = cylinders.begin();
  advance(cylinder_front, index);
  return *cylinder_front;
}
void FingerprintTemplate::fillWithCylinders() {
  for (auto it = minutiae.begin(), end = minutiae.end(); it != end; it++) {
    Cylinder newCylinder(*it, minutiae, convexHull);
    cylinders.push_back(newCylinder);
  }
}

vector<Cylinder> FingerprintTemplate::createCylinderSet() {
  vector<Cylinder> CS_temp;
  for (Cylinder c : cylinders) {
    if (c.valid == true) {
      CS_temp.push_back(c);
    }
  }
  return CS_temp;
}