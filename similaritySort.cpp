#include "similaritySort.h"
#include "fingerprintTemplate.h"
#include <algorithm>
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::min;

// Global score
double localSimilaritySort(FingerprintTemplate A, FingerprintTemplate B,
                           CryptoContext<Element> &cc,
                           LPKeyPair<Element> &keys) {
  vector<double> listOfSimilarities;
  vector<vector<double>> matrix(A.CS.size() + 1,
                                vector<double>(B.CS.size() + 1));

  for (auto itA = A.CS.begin(), endA = A.CS.end(); itA != endA; itA++) {
    for (auto itB = B.CS.begin(), endB = B.CS.end(); itB != endB; itB++) {
      auto rowIndex = distance(A.CS.begin(), itA);
      auto columnIndex = distance(B.CS.begin(), itB);
      matrix[rowIndex][columnIndex] = cylinderSimilarity(*itA, *itB, cc, keys);
      listOfSimilarities.push_back(matrix[rowIndex][columnIndex]);
      // cout << matrix[rowIndex][columnIndex] << " ";
    }
    // cout << endl;
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
  double n_a = A.CS.size();
  double n_b = B.CS.size();
  double min_np = 4;
  double max_np = 12;
  double tau = 0.4;
  double my = 20.0;
  double min = std::min(n_a, n_b);
  double zigmoid = 1 / (1 + pow(M_E, -tau * (min - my)));

  int n_p = min_np + round(zigmoid * (max_np - min_np));  // 4 or 5

  // Considering the top 10% cylinder comparisons
  // int n_p = A.CS.size() * B.CS.size() * 0.10;

  // int n_p = count * 0.1;

  for (int i = size - 1; i >= size - n_p; i--) {
    P.push_back(listOfSimilarities[i]);
    // cout << listOfSimilarities[i] << endl;
  }
  double global_score = 0;
  for (double i : P) {
    global_score = global_score + i;
  }
  global_score = global_score / n_p;
  return global_score;
}

double localSimilaritySortBinary(FingerprintTemplate A, FingerprintTemplate B) {
  vector<double> listOfSimilarities;
  vector<vector<double>> matrix(A.CS.size(), vector<double>(B.CS.size()));

  for (auto itA = A.CS.begin(), endA = A.CS.end(); itA != endA; itA++) {
    for (auto itB = B.CS.begin(), endB = B.CS.end(); itB != endB; itB++) {
      auto rowIndex = distance(A.CS.begin(), itA);
      auto columnIndex = distance(B.CS.begin(), itB);
      matrix[rowIndex][columnIndex] = cylinderSimilarityBinary(*itA, *itB);
      listOfSimilarities.push_back(matrix[rowIndex][columnIndex]);
    }
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

  // Considering the top 10% cylinder comparisons
  // int n_p = A.CS.size() * B.CS.size() * 0.10;

  int n_p = count * 0.20;

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