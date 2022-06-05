#ifndef CYLINDER_H
#define CYLINDER_H

#include "minutia.h"
#include "cell.h"
#include "globals.h"
#include <vector>

using std::vector;

class Cylinder {
 public:
  Minutia minutia;
  vector<double> c_m;
  bool valid;
  Cylinder(Minutia minutia1, vector<Minutia> t_, vector<Minutia> conv_);
  void fillWithCells(vector<Minutia> &conv_, vector<Minutia> &t_);

  double lin(double i, double j, double k);

  bool calcValidity(int &valid_cells, vector<Minutia> &t_);
};

Cylinder createCylinderAsync(Minutia minutia1, vector<Minutia> t_,
                             vector<Minutia> conv_);

#endif