#include "cylinder.h"
#include "usefulMethods.h"
#include <algorithm>
#include <iterator>

#include <iostream>

using std::cout;
using std::endl;

using lbcrypto::Ciphertext;
using lbcrypto::Plaintext;
using std::back_inserter;
using std::transform;

Cylinder::Cylinder(Minutia minutia1, vector<Minutia> t_,
                   vector<Minutia> conv_) {
  minutia = minutia1;
  valid = true;
  related_fingerprintTemplate = t_;
  related_convex_cylinder = conv_;
  fillWithCells();
  c_mCalc();
  c_mCalcBinary();
  valid = calcValidity();
  number_of_cells_with_contribution = countNumberOfCellsWithContribution();
}

void Cylinder::fillWithCells() {
  for (int k = 1; k < N_D + 1; k++) {
    for (int j = 1; j < N_S + 1; j++) {
      for (int i = 1; i < N_S + 1; i++) {
        Cell newCell(i, j, k, minutia, related_fingerprintTemplate,
                     related_convex_cylinder);
        cells.push_back(newCell);
      }
    }
  }
}

double Cylinder::lin(double i, double j, double k) {
  return (k - 1) * pow(N_S, 2) + (j - 1) * N_S + i;
}

int Cylinder::countNumberOfCellsWithContribution() {
  int temp = 0;
  for (Cell c : cells) {
    if (c.contribution_total > 0) {
      temp = temp + 1;
    }
  }
  return temp;
}

void Cylinder::c_mCalc() {
  for (int i = 0; i < N_S * N_S * N_D; i++) {
    c_m.push_back(0);
  }
  for (Cell c : cells) {
    if (c.valid == true) {
      c_m[lin(c.i, c.j, c.k)] = c.zigmoid;
    } else {
      c_m[lin(c.i, c.j, c.k)] = 0;
    }
  }
}

void Cylinder::c_mCalcBinary() {
  for (int i = 0; i < N_S * N_S * N_D; i++) {
    c_m_binary.push_back(0);
  }
  for (Cell c : cells) {
    if (c.valid == true) {
      c_m_binary[lin(c.i, c.j, c.k)] = c.zigmoid_binary;
    }
  }
}

bool Cylinder::calcValidity() {
  // First condition from paper
  int count_a = 0;
  for (Cell c : cells) {
    if (c.valid == true) {
      count_a += 1;
    }
  }
  if (count_a < min_vc) {
    return false;
  }

  // Second condition from paper
  int count_b = 0;
  for (Minutia a : related_fingerprintTemplate) {
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

double cylinderSimilarity(Cylinder a, Cylinder b, CryptoContext<Element> &cc,
                          LPKeyPair<Element> &keys) {
  vector<double> c_a;
  vector<double> c_b;

  c_a = c_m_matchable(a.c_m, b.c_m);
  c_b = c_m_matchable(b.c_m, a.c_m);

  // Upper vector calculated in cleartext
  vector<double> upper;
  transform(c_a.begin(), c_a.end(), c_b.begin(), back_inserter(upper),
            [&](double l, double r) { return l - r; });

  // Upper vector calculated in encrypted domain
  vector<double> upperFHE;
  // Encoding as plaintexts
  Plaintext ptxta = cc->MakeCKKSPackedPlaintext(c_a);
  Plaintext ptxtb = cc->MakeCKKSPackedPlaintext(c_b);

  // Encrypt the encoded vectors
  auto ctxta = cc->Encrypt(keys.publicKey, ptxta);
  auto ctxtb = cc->Encrypt(keys.publicKey, ptxtb);

  // Homomorphic subtraction
  auto cSub = cc->EvalSub(ctxta, ctxtb);

  Plaintext result;

  // Decrypt the result of subtraction
  cc->Decrypt(keys.secretKey, cSub, &result);
  result->SetLength(batchSize);
  upperFHE = result->GetRealPackedValue();

  double upper_distance = distanceOfVector(upper);
  double c_a_distance = distanceOfVector(c_a);
  double c_b_distance = distanceOfVector(c_b);

  // not in paper
  if (upper_distance < 1) {
    return 0;
  }
  // not in paper
  if (c_a_distance == c_b_distance && c_a_distance < 1) {
    return 0;
  }

  if (directionalDifference(a.minutia.angle, b.minutia.angle) >= (M_PI / 2.0)) {
    return 0;
  }

  int counter = 0;
  for (double i : c_a) {
    if (i > 0) {
      counter += 1;
    }
  }
  if (counter < c_a.size() * 0.1) {
    return 0;
  }

  if (c_a_distance + c_b_distance == 0) {
    return 0;
  }

  // FHE here
  return 1 - (upper_distance / (c_a_distance + c_b_distance));
}

double cylinderSimilarityBinary(Cylinder a, Cylinder b) {
  vector<double> c_a;
  vector<double> c_b;

  c_a = c_m_matchable(a.c_m, b.c_m);
  c_b = c_m_matchable(b.c_m, a.c_m);

  vector<int> upper_binary(a.c_m_binary.size());

  for (auto it = a.c_m_binary.begin(), end = a.c_m_binary.end(); it != end;
       it++) {
    auto index = distance(a.c_m_binary.begin(), it);
    if (*it != b.c_m_binary[index]) {
      upper_binary.push_back(1);
    } else {
      upper_binary.push_back(0);
    }
  }

  double upper_binary_distance = distanceOfVectorBinary(upper_binary);
  double c_a_binary_distance = distanceOfVectorBinary(a.c_m_binary);
  double c_b_binary_distance = distanceOfVectorBinary(b.c_m_binary);

  /*if (directionalDifference(a.minutia_angle, b.minutia_angle) >= (M_PI / 2.0))
  { return 0;
  }

  int counter = 0;
  for (double i : c_a) {
    if (i > 0) {
      counter += 1;
    }
  }
  if (counter < c_a.size() * 0.4) {
    return 0;
  }

  if (c_a_binary_distance + c_b_binary_distance == 0) {
    return 0;
  }*/

  return 1 -
         (upper_binary_distance / (c_a_binary_distance + c_b_binary_distance));
}

vector<double> c_m_matchable(vector<double> c_x, vector<double> c_y) {
  vector<double> temp;
  for (auto it = c_x.begin(), end = c_x.end(); it != end; it++) {
    auto index = distance(c_x.begin(), it);
    if (*it > 0 && c_y[index] > 0) {
      temp.push_back(*it);
    } else {
      temp.push_back(0);
    }
  }
  return temp;
}