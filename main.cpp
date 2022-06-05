#define PROFILE

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
#include "fingerprintTemplate.h"
#include <iomanip>
#include <chrono>
#include "comparison.h"

#include <dirent.h>
#include <cstring>

using std::cout;
using std::endl;
using std::ostringstream;

int main() {
  // Read template files in directory to a list
  DIR *dir;
  struct dirent *diread;
  if ((dir = opendir(templates_path)) != nullptr) {
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
  
  // Create combinations of templates
  int k = 2;
  create_combinations(0, k);


  // Summary of what is being compared for the txt file
  if (mated) {
    cout << "Mated ";
  } else {
    cout << "Non-mated ";
  }
  cout << "comparison of the templates from" << templates_path << ":"
       << "\n";

  // for (auto combination : combination_complete) {
  //   if (mated) {
  //     if ((combination[0].substr(0, 3) == combination[1].substr(0, 3)) &&
  //         (combination[0].substr(4, 1) == "1" ||
  //          combination[1].substr(4, 1) == "1")) {
  //       cout << combination[0] << " == " << combination[1] << "\n";
  //     }
  //   } else {
  //     if (combination[0].substr(4, 1) == "1" &&
  //         combination[1].substr(4, 1) == "1") {
  //       cout << combination[0] << " == " << combination[1] << "\n";
  //     }
  //   }
  // }

  // Iterate over combinations and compare
  for (auto combination : combination_complete) {
    if (mated) {
      if (!((combination[0].substr(0, 3) == combination[1].substr(0, 3)) &&
            (combination[0].substr(4, 1) == "1" ||
             combination[1].substr(4, 1) == "1"))) {
        continue;
      }
    } else {
      if (!(combination[0].substr(4, 1) == "1" &&
            combination[1].substr(4, 1) == "1")) {
        continue;
      }
    }

    // Read files and generate lists of minutiae
    ostringstream oss0;
    oss0 << templates_path << combination[0];
    string string0 = oss0.str();

    ostringstream oss1;
    oss1 << templates_path << combination[1];
    string string1 = oss1.str();

    vector<Minutia> minutiae_1;
    vector<Minutia> minutiae_2;

    auto start = std::chrono::steady_clock::now();
    minutiae_1 = templatePreprocess(string0);
    minutiae_2 = templatePreprocess(string1);

    auto end = std::chrono::steady_clock::now();
    auto elapsednanoseconds =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    // cout << "\n" << "Generating two lists of minutiae took "
    //      << elapsednanoseconds.count() << " nanoseconds" << endl;

    // Generate probe and reference templates
    template_struct probe_template;
    template_struct candidate_template;
    start = std::chrono::steady_clock::now();

    FingerprintTemplate probe(minutiae_1, probe_template);
    end = std::chrono::steady_clock::now();
    elapsednanoseconds =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    // cout << "\n" << "Generating probe template took " << elapsednanoseconds.count()
    //      << " nanoseconds" << endl;


    start = std::chrono::steady_clock::now();
    FingerprintTemplate candidate(minutiae_2, candidate_template);
    end = std::chrono::steady_clock::now();
    elapsednanoseconds =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    // cout << "\n" << "Generating candidate template took " << elapsednanoseconds.count()
    //      << " nanoseconds" << endl;

    start = std::chrono::steady_clock::now();
    double global = compareTemplates(probe_template, candidate_template);

    end = std::chrono::steady_clock::now();
    auto elapsedmilliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout << "\n" << "Comparison took " << elapsedmilliseconds.count()
         << " milliseconds" << endl;
    cout << "\n" << combination[0] << " and " << combination[1] << " = " << global
         << '\n';
  }
  return 0;
};
