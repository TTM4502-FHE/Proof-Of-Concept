#include "comparison.h"
#include "math.h"
#include "globals.h"
#include <future>
#include <algorithm>
#include <iostream>
#include <algorithm>

using std::back_inserter;
using std::transform;

using std::cout;
using std::endl;

// Global score
double compareTemplates(template_struct &probe, template_struct &candidate) {
  vector<double> listOfSimilarities;

  // int totalComparisons = probe.cylinders.size() * candidate.cylinders.size();
  // int completedComparisons = 0;
  int probeSize = probe.cylinders.size();
  for (int i = 0; i < probeSize; i++) {
    // auto start = std::chrono::steady_clock::now();
    vector<std::future<double>> fut_similarities;
    int candidateSize = candidate.cylinders.size();
    for (int j = 0; j < candidateSize; j++) {
      fut_similarities.emplace_back(std::async(
          std::launch::async, cylinder_similarity, std::ref(probe.cylinders[i]),
          std::ref(candidate.cylinders[j])));
    }

    for (auto &&fut : fut_similarities) {
      listOfSimilarities.push_back(fut.get());
    }
    // for (auto number : listOfSimilarities) {
    //   cout << number << "\n";
    // }
    // auto end = std::chrono::steady_clock::now();
    // auto elapsedSeconds =
    //     std::chrono::duration_cast<std::chrono::seconds>(end - start);
    // completedComparisons = completedComparisons + candidate.cylinders.size();

    // cout << "One chunk of cylinder comparisons took "
    //      << elapsedSeconds.count() / candidate.cylinders.size() << " seconds"
    //      << endl;
    // cout << "Completed comparisons: " << completedComparisons << " of "
    //      << totalComparisons << "\n";
  }

  sort(listOfSimilarities.begin(), listOfSimilarities.end());
  vector<double> P;
  int size = listOfSimilarities.size();
  int count = 0;

  for (double i : listOfSimilarities) {
    if (i > 0) {
      count = count + 1;
    }
  }

  double min_np = 4;
  double max_np = 12;
  double tau = 0.4;
  double my = 20.0;
  double min = std::min(probe.cylinders.size(), candidate.cylinders.size());
  double zigmoid = 1 / (1 + pow(M_E, -tau * (min - my)));

  int n_p = min_np + round(zigmoid * (max_np - min_np));  // 4 or 5

  // Considering the top 10% cylinder comparisons

  for (int i = size - 1; i >= size - n_p; i--) {
    P.push_back(listOfSimilarities[i]);
  }
  double global_score = 0;
  for (double i : P) {
    global_score = global_score + i;
  }
  global_score = global_score / n_p;
  return global_score;
}

double cylinder_similarity(cylinder_struct &probe_cylinder,
                           cylinder_struct &candidate_cylinder) {
  // First condition for valid cylinder similarity (cylinders are matchable)

  if (cylinder_directional_difference(probe_cylinder.cyl_angle,
                                      candidate_cylinder.cyl_angle) >
      (M_PI / 2.0)) {
    return 0;
  }
  // Create c_a|b and c_b|a from c_m's based on corresponding valid cells.

  probe_candidate_matchable p_c_matchable =
      c_m_matchable(probe_cylinder, candidate_cylinder);

  // Second condition for valid cylinder similarity (cylinders are matchable)

  if (p_c_matchable.valid_cells < 1536 * min_me) {
    return 0;
  }

  // Calculate upper part of cylinder similarity fraction
  // (vector subtraction and distance)

  vector<double> matchable_substracted;
  for (int i = 0; i < 1536; i++) {
    matchable_substracted.push_back(0);
  }

  // std::set_difference(
  //     p_c_matchable.probe_matchable.begin(),
  //     p_c_matchable.probe_matchable.end(),
  //     p_c_matchable.candidate_matchable.begin(),
  //     p_c_matchable.candidate_matchable.end(),
  //     std::inserter(matchable_substracted, matchable_substracted.begin()));

  for (int i = 0; i < 1536; i++) {
    matchable_substracted[i] =
        p_c_matchable.candidate_matchable[i] - p_c_matchable.probe_matchable[i];
  }

  double upper_distance = distance_of_vector(matchable_substracted);

  // Calculate lower part(1) of cylinder similarity fraction
  // (only distance of vector)
  double c_a_distance = distance_of_vector(p_c_matchable.probe_matchable);

  // Calculate lower part(2) of cylinder similarity fraction
  // (only distance of vector)
  double c_b_distance = distance_of_vector(p_c_matchable.candidate_matchable);

  // Third condition for valid cylinder similarity (cylinders are matchable)
  if (c_a_distance + c_b_distance == 0) {
    return 0;
  }

  // Return cylinder similarity if all conditions are satisfied
  return 1 - (upper_distance / (c_a_distance + c_b_distance));
}

probe_candidate_matchable c_m_matchable(cylinder_struct &probe_cylinder,
                                        cylinder_struct &candidate_cylinder) {
  probe_candidate_matchable p_c_matchable;
  p_c_matchable.valid_cells = 0;
  int c_m_Size = probe_cylinder.c_m.size();
  for (int i = 0; i < c_m_Size; i++) {
    if (probe_cylinder.c_m[i] != 0.0 && candidate_cylinder.c_m[i] != 0.0) {
      p_c_matchable.valid_cells += 1;
      p_c_matchable.probe_matchable.push_back(probe_cylinder.c_m[i]);
      p_c_matchable.candidate_matchable.push_back(candidate_cylinder.c_m[i]);
    } else {
      p_c_matchable.probe_matchable.push_back(0.0);
      p_c_matchable.candidate_matchable.push_back(0.0);
    }
  }
  return p_c_matchable;
}

double distance_of_vector(vector<double> &matchable_vector) {
  double temp = 0;
  for (double i : matchable_vector) {
    temp = temp + pow(i, 2);
  }
  return sqrt(temp);
}

double cylinder_directional_difference(double angle1, double angle2) {
  double delta = angle2 - angle1;
  // Equation (9) from paper
  if (-M_PI <= delta && delta < M_PI) {
    return delta;
  } else if (delta < -M_PI) {
    return 2 * M_PI + delta;
  } else if (delta >= M_PI) {
    return -2 * M_PI + delta;
  }
  cout << "Something really bad has happened, and the angles are out "
          "of scope for directional difference"
       << endl;
  return 0;
}