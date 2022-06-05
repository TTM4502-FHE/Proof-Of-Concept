#ifndef CYLINDER_H
#define CYLINDER_H

#include "minutia.h"
#include "cell.h"
#include "globals.h"
#include <vector>
#include "palisade.h"

using lbcrypto::CryptoContext;
using lbcrypto::DCRTPoly;
using lbcrypto::LPKeyPair;

using Element = DCRTPoly;

class Cylinder {
 public:
  Minutia minutia;
  vector<Cell> cells;
  vector<Minutia> related_fingerprintTemplate;
  vector<Minutia> related_convex_cylinder;
  int number_of_cells_with_contribution;
  vector<double> c_m;
  vector<int> c_m_binary;
  bool valid;
  Cylinder(Minutia minutia1, vector<Minutia> t_, vector<Minutia> conv_);
  void fillWithCells();

  double lin(double i, double j, double k);

  int countNumberOfCellsWithContribution();

  void c_mCalc();

  void c_mCalcBinary();

  bool calcValidity();
};

double cylinderSimilarity(Cylinder a, Cylinder b, CryptoContext<Element> &cc,
                          LPKeyPair<Element> &keys);

double cylinderSimilarityBinary(Cylinder a, Cylinder b);

vector<double> c_m_matchable(vector<double> c_x, vector<double> c_y);

#endif