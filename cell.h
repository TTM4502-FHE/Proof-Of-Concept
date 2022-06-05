#ifndef CELL_H
#define CELL_H

#include "minutia.h"
#include "globals.h"
#include <vector>

using std::vector;

class Cell {
 public:
  Minutia minutia;
  double k;
  double i;
  double j;
  double cell_angle;
  vector<double> p_ij;
  double contribution_spacial = 0.0;
  double contribution_directional = 0.0;
  double contribution_total = 0.0;
  double zigmoid = 0.0;  // zigmoid of total contribution - Cm(i, j, k)
  int zigmoid_binary = 0;
  bool valid;
  vector<Minutia> related_fingerprintTemplate_cell;
  vector<Minutia> related_convex;
  vector<Minutia> neighbourhood;

  Cell(double i_, double j_, double k_, Minutia minutia1, vector<Minutia> t_,
       vector<Minutia> conv_);

  // Equation (2) in MCC paper
  vector<double> calculatePoint();

  void calculateValid();

  // *********************** Calculating equation (1) from MCC paper
  double cellAngleAtK(double k);

  double spacialContribution(Minutia mt);

  double directionalContribution(Minutia mt);

  // Integral from equation (11)
  double gauss_integral(double alpha);

  double directionalDifference(double angle1, double angle2);

  // Calculate cell value - Cm(k, j, k)
  double totalContribution();

  vector<Minutia> calculateNeighbourhood();

  // Equation (5) from paper
  double calculateZigmoid(double v);

  int calculateZigmoidBinary(double v);
};



#endif