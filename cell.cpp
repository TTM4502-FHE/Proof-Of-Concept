#include "cell.h"
#include "usefulMethods.h"
#include <iostream>

using std::cout;
using std::endl;

Cell::Cell(double i_, double j_, double k_, Minutia minutia1,
           vector<Minutia> t_, vector<Minutia> conv_) {
  k = k_;
  i = i_;
  j = j_;
  minutia = minutia1;

  // Eq (1)
  cell_angle = cellAngleAtK(k);

  // Eq (2)
  p_ij = calculatePoint();

  // Eq (3)
  neighbourhood = calculateNeighbourhood(t_);

  // Eq (4) + (5)
  contribution_total = totalContribution();
  zigmoid = calculateZigmoid(contribution_total, conv_);
}

// Equation (2) in MCC paper
vector<double> Cell::calculatePoint() {
  vector<double> xy_m;
  vector<double> temp;
  xy_m.push_back(minutia.x_coordinate);
  xy_m.push_back(minutia.y_coordinate);

  // Right side of + sign
  double w = (N_S + 1) / 2;
  vector<double> right_side;
  double a = cos(minutia.angle) * (i - w) + sin(minutia.angle) * (j - w);
  double b = -sin(minutia.angle) * (i - w) + cos(minutia.angle) * (j - w);
  right_side.push_back(a * Delta_S);
  right_side.push_back(b * Delta_S);

  temp.push_back(xy_m[0] + right_side[0]);
  temp.push_back(xy_m[1] + right_side[1]);
  return temp;
}

void Cell::calculateValid() {
  if (zigmoid == 0.0) {
    valid = false;
  } else {
    valid = true;
  }
}

// *********************** Calculating equation (1) from MCC paper
double Cell::cellAngleAtK(double k) {
  double d_k;
  d_k = -M_PI + ((k - 0.5) * Delta_D);
  return d_k;
}

double Cell::spacialContribution(Minutia mt) {
  double t = euclideanDistance(p_ij[0], p_ij[1], mt);

  // Gaussian spacial - equation (7) from paper
  double fraction_lower = std_s * sqrt(2 * M_PI);
  double fraction = 1 / fraction_lower;
  double power_upper = -pow(t, 2);
  double power_lower = 2 * pow(std_s, 2);

  return fraction * pow(M_E, (power_upper / power_lower));
}

double Cell::directionalContribution(Minutia mt) {
  // Equation (8) and (10) from paper
  double mmt_difference = directionalDifference(minutia.angle, mt.angle);
  double cellangle_mmt_difference =
      directionalDifference(cell_angle, mmt_difference);
  double alpha = cellangle_mmt_difference;

  // Gaussian directional - equation (11) from paper
  double fraction_lower = std_d * sqrt(2 * M_PI);
  double fraction = 1 / fraction_lower;
  double integral = gauss_integral(alpha);
  return fraction * integral;
}

// Integral from equation (11)
double Cell::gauss_integral(double alpha) {
  double upper_limit = alpha + (Delta_D / 2.0);
  double lower_limit = alpha - (Delta_D / 2.0);
  double a = 2.0 * pow(std_d, 2);
  double upper = (sqrt(M_PI) * sqrt(a) * erf(upper_limit / sqrt(a)) / 2);
  double lower = (sqrt(M_PI) * sqrt(a) * erf(lower_limit / sqrt(a)) / 2);
  return upper - lower;
}

double Cell::directionalDifference(double angle1, double angle2) {
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
       << endl;
  return 0;
}

// Calculate cell value - Cm(k, j, k)
double Cell::totalContribution() {
  double total = 0;
  double countr = 0;
  for (Minutia m : neighbourhood) {
    double spacial;
    double directional;
    spacial = spacialContribution(m);
    contribution_spacial = contribution_spacial + spacial;
    directional = directionalContribution(m);

    contribution_directional = contribution_directional + directional;

    total = total + (spacial * directional);
    countr = countr + 1;
  }
  return total;
}

vector<Minutia> Cell::calculateNeighbourhood(vector<Minutia> &t_) {
  vector<Minutia> temp;
  for (Minutia m : t_) {
    if (m.x_coordinate != minutia.x_coordinate &&
        m.y_coordinate != minutia.y_coordinate) {
      if (euclideanDistance(p_ij[0], p_ij[1], m) <= 3 * std_s) {
        temp.push_back(m);
      }
    }
  }
  return temp;
}

// Equation (5) from paper
double Cell::calculateZigmoid(double v, vector<Minutia> &conv_) {
  double temp;
  if (isInside(conv_, conv_.size(), p_ij)) {
    temp = 1 / (1 + pow(M_E, -zigmoid_t * (v - zigmoid_u)));
    valid = true;
    return temp;
  }
  valid = false;
  return -1;
}