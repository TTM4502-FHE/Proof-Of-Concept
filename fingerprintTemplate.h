#ifndef FINGERPRINTTEMPLATE_H
#define FINGERPRINTTEMPLATE_H

#include "cylinder.h"

class FingerprintTemplate {
 public:
  vector<Cylinder> cylinders;
  vector<Minutia> convexHull;
  vector<Cylinder> CS;
  vector<Minutia> minutiae;
  //  Dividing template into vector of x's, y's and angles

  FingerprintTemplate(vector<Minutia> minutiae1);

  Cylinder getCylinderAtIndex(int index);
  void fillWithCylinders();

  vector<Cylinder> createCylinderSet();
};



#endif