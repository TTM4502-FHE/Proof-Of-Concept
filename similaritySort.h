#ifndef SIMILARITYSORT_H
#define SIMILARITYSORT_H
#include "palisade.h"

// #include "fingerprintTemplate.h"

using lbcrypto::CryptoContext;
using lbcrypto::DCRTPoly;
using lbcrypto::LPKeyPair;

using Element = DCRTPoly;

class FingerprintTemplate;

double localSimilaritySort(FingerprintTemplate A, FingerprintTemplate B,
                           CryptoContext<Element> &cc,
                           LPKeyPair<Element> &keys);
double localSimilaritySortBinary(FingerprintTemplate A, FingerprintTemplate B);

#endif