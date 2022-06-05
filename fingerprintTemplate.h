#ifndef FINGERPRINTTEMPLATE_H
#define FINGERPRINTTEMPLATE_H

#include "cylinder.h"

struct template_struct;

class FingerprintTemplate {
 public:
  vector<Cylinder> cylinders;
  vector<Minutia> convexHull;
  //  Dividing template into vector of x's, y's and angles

  FingerprintTemplate(vector<Minutia> &minutiae1, template_struct &template_struct);

  void fillWithCylinders(template_struct &template_struct, vector<Minutia> &minutiae1);
};

#endif