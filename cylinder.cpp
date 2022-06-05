#include "cylinder.h"
#include "usefulMethods.h"
#include <algorithm>
#include <iterator>

#include <iostream>

using std::back_inserter;
using std::transform;
using std::cout;

Cylinder::Cylinder(Minutia minutia1, vector<Minutia> t_,
                   vector<Minutia> conv_) {
  minutia = minutia1;
  valid = true;
  fillWithCells(conv_, t_);
}

void Cylinder::fillWithCells(vector<Minutia> &conv_, vector<Minutia> &t_) {
  int valid_cells = 0;
  for (int i = 0; i < 1536; i++) {
    c_m.push_back(0);
  }

  for (int k = 1; k < N_D + 1; k++) {
    for (int j = 1; j < N_S + 1; j++) {
      for (int i = 1; i < N_S + 1; i++) {
        Cell temp_cell(i, j, k, minutia, t_, conv_);
        if (temp_cell.valid) {
          c_m[lin(temp_cell.i, temp_cell.j, temp_cell.k)] = temp_cell.zigmoid;
          valid_cells += 1;
        } else {
          c_m[lin(temp_cell.i, temp_cell.j, temp_cell.k)] = 0;
        }
      }
    }
  }
  valid = calcValidity(valid_cells, t_);
}

double Cylinder::lin(double i, double j, double k) {
  return (k - 1) * pow(N_S, 2) + (j - 1) * N_S + i - 1;
}

bool Cylinder::calcValidity(int &valid_cells, vector<Minutia> &t_) {
  // First condition from paper
  if (valid_cells < min_vc) {
    return false;
  }

  // Second condition from paper
  int count_b = 0;
  for (Minutia a : t_) {
    if (minutia.x_coordinate != a.x_coordinate &&
        minutia.y_coordinate != a.y_coordinate) {
      if (euclideanDistance(minutia.x_coordinate, minutia.y_coordinate,
                            a.x_coordinate,
                            a.y_coordinate) <= R + (3 * std_s)) {
        count_b += 1;
      }
    }
  }
  if (count_b < min_m) {
    return false;
  }
  return true;
}

Cylinder createCylinderAsync(Minutia minutia1, vector<Minutia> t_,
                             vector<Minutia> conv_) {
  Cylinder temp(minutia1, t_, conv_);
  return temp;
}