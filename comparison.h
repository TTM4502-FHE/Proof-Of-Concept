#ifndef COMPARISON_H
#define COMPARISON_H

#include <vector>

using std::vector;

struct cylinder_struct {
  double cyl_angle;
  vector<double> c_m;
};

struct template_struct {
  vector<cylinder_struct> cylinders;
};

struct probe_candidate_matchable {
  double valid_cells;
  vector<double> probe_matchable;
  vector<double> candidate_matchable;
};

double cylinder_directional_difference(double angle1, double angle2);

double compareTemplates(template_struct &probe, template_struct &candidate);

double cylinder_similarity(cylinder_struct &probe_cylinder,
                           cylinder_struct &candidate_cylinder);

probe_candidate_matchable c_m_matchable(cylinder_struct &probe_cylinder,
                                        cylinder_struct &candidate_cylinder);

double distance_of_vector(vector<double> &matchable_vector);

#endif
