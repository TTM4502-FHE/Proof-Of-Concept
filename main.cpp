#define PROFILE

#include "palisade.h"

#include <list>
#include <fstream>
#include <iostream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <stack>
#include <vector>
#include <string>
#include "globals.h"
#include "preProcessData.h"
#include "similaritySort.h"
#include "fingerprintTemplate.h"
#include <iomanip>

#include <dirent.h>

using namespace std;
using namespace lbcrypto;

int main() {
  // FHE PALISADE stuff

  SecurityLevel securityLevel = HEStd_128_classic;

  CryptoContext<DCRTPoly> cc =
      CryptoContextFactory<DCRTPoly>::genCryptoContextCKKS(
          multDepth, scaleFactorBits, batchSize, securityLevel);

  cout << "CKKS scheme is using ring dimension " << cc->GetRingDimension()
       << endl;

  cc->Enable(ENCRYPTION);
  cc->Enable(SHE);
  auto keys = cc->KeyGen();
  cc->EvalMultKeyGen(keys.secretKey);
  cc->EvalAtIndexKeyGen(keys.secretKey, {1});

  vector<Minutia> minutiae_1;
  vector<Minutia> minutiae_2;
  DIR *dir;
  struct dirent *diread;

  if ((dir = opendir(
           "/Users/lassevad/Documents/Biometrics/crossmatchTemplates")) !=
      nullptr) {
    while ((diread = readdir(dir)) != nullptr) {
      string ds_name = diread->d_name;
      if (strcmp(diread->d_name, ".") == 0 ||
          strcmp(diread->d_name, "..") == 0 ||
          ds_name.find("DS") != std::string::npos)
        continue;
      files.push_back(diread->d_name);
    }
    closedir(dir);
  } else {
    perror("opendir");
    return EXIT_FAILURE;
  }

  int k = 2;

  create_combinations(0, k);

  // for (auto combinations : combination_complete) {
  //   cout << combinations << "\n";
  // }
  for (auto combination : combination_complete) {
    // cout << combination[0] << " ";
    // cout << combination[1] << '\n';

    // Nonmated comparison
    // if (combination[0].substr(4, 1) == "1" && combination[1].substr(4, 1) ==
    // "1") {

    // Mated comparison
    if ((combination[0].substr(0, 3) == combination[1].substr(0, 3)) &&
        (combination[0].substr(4, 1) == "1" ||
         combination[1].substr(4, 1) == "1")) {
      ostringstream oss0;

      oss0 << path_lasse << combination[0];
      string string0 = oss0.str();

      ostringstream oss1;

      oss1 << path_lasse << combination[1];
      string string1 = oss1.str();

      minutiae_1 = templatePreprocess(string0);
      minutiae_2 = templatePreprocess(string1);

      FingerprintTemplate probe(minutiae_1);
      FingerprintTemplate candidate(minutiae_2);
      // double percent = (count / 224) * 100;

      double global = localSimilaritySort(probe, candidate, cc, keys);
      cout << combination[0] << " and " << combination[1] << " = " << global
           << '\n';
    }
  }

  return 0;
};