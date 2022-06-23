#include "comparison.h"
#include "math.h"
#include "globals.h"
#include <future>
#include <algorithm>
#include <iostream>

using lbcrypto::Plaintext;
using std::cout;
using std::endl;

// Global score
double compareEncryptedTemplates(encrypted_template &probe,
                                 encrypted_template &candidate,
                                 CryptoContext<Element> &cc,
                                 LPKeyPair<Element> &keys) {
  vector<double> listOfSimilarities;
  int probe_CS_size = probe.encrypted_cylinders.size();
  int candidate_CS_size = candidate.encrypted_cylinders.size();
  int totalComparisons = probe_CS_size * candidate_CS_size;
  int completedComparisons = 0;
  
  for (int i = 0; i < probe_CS_size; i++) {
    auto start = std::chrono::steady_clock::now();
    vector<std::future<double>> fut_similarities;
    for (int j = 0; j < candidate_CS_size; j++) {
      fut_similarities.emplace_back(
          std::async(std::launch::async, cylinder_similarity,
                     std::ref(probe.encrypted_cylinders[i]),
                     std::ref(candidate.encrypted_cylinders[j]), std::ref(cc),
                     std::ref(keys)));
    }

    for (auto &&fut : fut_similarities) {
      listOfSimilarities.push_back(fut.get());
    }

    auto end = std::chrono::steady_clock::now();
    auto elapsedMilliSeconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    completedComparisons = completedComparisons + candidate_CS_size;

    cout << "One chunk of cylinder comparisons took "
         << elapsedMilliSeconds.count() / candidate_CS_size << " milliseconds"
         << endl;
    cout << "Completed comparisons: " << completedComparisons << " of "
         << totalComparisons << "\n";
  }

  sort(listOfSimilarities.begin(), listOfSimilarities.end());

  int size = listOfSimilarities.size();
  double min_np = 4;
  double max_np = 12;
  double tau = 0.4;
  double my = 20.0;
  double min = std::min(probe_CS_size, candidate_CS_size);
  double zigmoid = 1 / (1 + pow(M_E, -tau * (min - my)));
  int n_p = min_np + round(zigmoid * (max_np - min_np));  // 4 or 5

  double global_score = 0;
  for (int i = size - 1; i >= size - n_p; i--) {
    global_score = global_score + listOfSimilarities[i];
  }

  global_score = global_score / n_p;
  return global_score;
}

double cylinder_similarity(encrypted_cylinder &probe_cylinder,
                           encrypted_cylinder &candidate_cylinder,
                           CryptoContext<Element> &cc,
                           LPKeyPair<Element> &keys) {
  // First condition for valid cylinder similarity (cylinders are matchable)
  if (cylinder_directional_difference(probe_cylinder.cyl_angle_enc,
                                      candidate_cylinder.cyl_angle_enc, cc,
                                      keys) > (M_PI / 2.0)) {
    return 0;
  }

  // Create c_a|b and c_b|a from c_m's based on corresponding valid cells.

  probe_candidate_matchable p_c_matchable =
      c_m_matchable(probe_cylinder, candidate_cylinder, cc, keys);

  // Second condition for valid cylinder similarity (cylinders are matchable)

  auto ed_valid_cells_encrypted =
      ed_of_encrypted_vector(p_c_matchable.validity_vector, cc);
  double valid_cells = decrypt_encrypted_ed(ed_valid_cells_encrypted, keys, cc);

  if (valid_cells < 1536 * min_me) {
    return 0;
  }

  // Calculate upper part of cylinder similarity fraction
  // (vector subtraction and distance)
  auto upper_encrypted_distance = ed_of_encrypted_vector(
      p_c_matchable.probe_matchable, p_c_matchable.candidate_matchable,
      p_c_matchable.validity_vector, cc);
  double upper_distance_squared =
      decrypt_encrypted_ed(upper_encrypted_distance, keys, cc);
  auto upper_distance = sqrt(upper_distance_squared);

  // Calculate lower part(1) of cylinder similarity fraction
  // (only distance of vector)
  auto c_a_encrypted_distance = ed_of_encrypted_vector(
      p_c_matchable.probe_matchable, p_c_matchable.validity_vector, cc);
  double c_a_distance_squared =
      decrypt_encrypted_ed(c_a_encrypted_distance, keys, cc);
  auto c_a_distance = sqrt(c_a_distance_squared);

  // Calculate lower part(2) of cylinder similarity fraction
  // (only distance of vector)
  auto c_b_encrypted_distance = ed_of_encrypted_vector(
      p_c_matchable.candidate_matchable, p_c_matchable.validity_vector, cc);
  double c_b_distance_squared =
      decrypt_encrypted_ed(c_b_encrypted_distance, keys, cc);
  auto c_b_distance = sqrt(c_b_distance_squared);

  // Third condition for valid cylinder similarity (cylinders are matchable)
  if (c_a_distance + c_b_distance == 0) {
    return 0;
  }

  // Return cylinder similarity if all conditions are satisfied
  return 1 - (upper_distance / (c_a_distance + c_b_distance));
}

probe_candidate_matchable c_m_matchable(encrypted_cylinder &probe_cylinder,
                                        encrypted_cylinder &candidate_cylinder,
                                        CryptoContext<Element> &cc,
                                        LPKeyPair<Element> &keys) {
  probe_candidate_matchable p_c_matchable;
  p_c_matchable.validity_vector = cc->EvalMult(probe_cylinder.c_m_validity,
                                               candidate_cylinder.c_m_validity);
  p_c_matchable.probe_matchable = probe_cylinder.c_m_encrypted;
  p_c_matchable.candidate_matchable = candidate_cylinder.c_m_encrypted;
  return p_c_matchable;
}

Ciphertext<Element> ed_of_encrypted_vector(Ciphertext<Element> &cm_a,
                                           Ciphertext<Element> &cm_b,
                                           Ciphertext<Element> &validity_vector,
                                           CryptoContext<Element> &cc) {
  auto subVector = cc->EvalSub(cm_a, cm_b);

  return ed_of_encrypted_vector(subVector, validity_vector, cc);
}

Ciphertext<Element> ed_of_encrypted_vector(Ciphertext<Element> &encVector,
                                           Ciphertext<Element> &validity_vector,
                                           CryptoContext<Element> &cc) {
  int vectorSize = 1536;

  Ciphertext<Element> ciphertextSquaredTemplates =
      cc->EvalMult(encVector, encVector);

  ciphertextSquaredTemplates =
      cc->EvalMult(ciphertextSquaredTemplates, validity_vector);

  Ciphertext<Element> ciphertextRot =
      cc->EvalAtIndex(ciphertextSquaredTemplates, 1);

  auto encED = cc->EvalAdd(ciphertextSquaredTemplates, ciphertextRot);

  for (int i = 0; i < vectorSize - 1; i++) {
    ciphertextRot = cc->EvalAtIndex(ciphertextRot, 1);
    encED = cc->EvalAdd(encED, ciphertextRot);
  }

  return encED;
}

Ciphertext<Element> ed_of_encrypted_vector(Ciphertext<Element> &encVector,
                                           CryptoContext<Element> &cc) {
  int vectorSize = 1536;

  Ciphertext<Element> ciphertextSquaredTemplates =
      cc->EvalMult(encVector, encVector);

  Ciphertext<Element> ciphertextRot =
      cc->EvalAtIndex(ciphertextSquaredTemplates, 1);

  auto encED = cc->EvalAdd(ciphertextSquaredTemplates, ciphertextRot);

  for (int i = 0; i < vectorSize - 1; i++) {
    ciphertextRot = cc->EvalAtIndex(ciphertextRot, 1);
    encED = cc->EvalAdd(encED, ciphertextRot);
  }

  return encED;
}

double decrypt_encrypted_ed(Ciphertext<Element> &encED,
                            LPKeyPair<Element> &keys,
                            CryptoContext<Element> &cc) {
  Plaintext decED;
  cc->Decrypt(keys.secretKey, encED, &decED);
  double distance = decED->GetRealPackedValue()[0];
  return distance;
}

double cylinder_directional_difference(Ciphertext<Element> angle1,
                                       Ciphertext<Element> angle2,
                                       CryptoContext<Element> &cc,
                                       LPKeyPair<Element> &keys) {
  auto delta_difference = cc->EvalSub(angle1, angle2);
  Plaintext dec_difference;
  cc->Decrypt(keys.secretKey, delta_difference, &dec_difference);
  double delta = dec_difference->GetRealPackedValue()[0];

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