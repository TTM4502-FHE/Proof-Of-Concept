#ifndef COMPARISON_H
#define COMPARISON_H

#include "palisade.h"

using lbcrypto::Ciphertext;
using lbcrypto::CryptoContext;
using lbcrypto::DCRTPoly;
using lbcrypto::LPKeyPair;
using std::vector;
using Element = DCRTPoly;

struct encrypted_cylinder {
  Ciphertext<Element> cyl_angle_enc;
  Ciphertext<Element> c_m_validity;
  Ciphertext<Element> c_m_encrypted;
};

struct encrypted_template {
  vector<encrypted_cylinder> encrypted_cylinders;
};

struct probe_candidate_matchable {
  Ciphertext<Element> probe_matchable;
  Ciphertext<Element> candidate_matchable;
  Ciphertext<Element> validity_vector;
};

double cylinder_directional_difference(Ciphertext<Element> angle1,
                                       Ciphertext<Element> angle2,
                                       CryptoContext<Element> &cc,
                                       LPKeyPair<Element> &keys);

double compareEncryptedTemplates(encrypted_template &probe,
                                 encrypted_template &candidate,
                                 CryptoContext<Element> &cc,
                                 LPKeyPair<Element> &keys);

double cylinder_similarity(encrypted_cylinder &probe_cylinder,
                           encrypted_cylinder &candidate_cylinder,
                           CryptoContext<Element> &cc,
                           LPKeyPair<Element> &keys);

probe_candidate_matchable c_m_matchable(encrypted_cylinder &probe_cylinder,
                                        encrypted_cylinder &candidate_cylinder,
                                        CryptoContext<Element> &cc,
                                        LPKeyPair<Element> &keys);

Ciphertext<Element> ed_of_encrypted_vector(Ciphertext<Element> &cm_a,
                                           Ciphertext<Element> &cm_b,
                                           Ciphertext<Element> &validity_vector,
                                           CryptoContext<Element> &cc);

Ciphertext<Element> ed_of_encrypted_vector(Ciphertext<Element> &encVector,
                                           Ciphertext<Element> &validity_vector,
                                           CryptoContext<Element> &cc);

Ciphertext<Element> ed_of_encrypted_vector(Ciphertext<Element> &encVector,
                                           CryptoContext<Element> &cc);

double decrypt_encrypted_ed(Ciphertext<Element> &encED,
                            LPKeyPair<Element> &keys,
                            CryptoContext<Element> &cc);

#endif