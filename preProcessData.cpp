#include "preProcessData.h"
#include "minutia.h"
#include "globals.h"
#include <sstream>
#include <fstream>

using std::ifstream;
using std::stringstream;

vector<double> extractDoubleWords(string str) {
  stringstream ss;
  vector<double> features;

  /* Storing the whole string into string stream */
  ss << str;

  /* Running loop till the end of the stream */
  string temp;
  double found;
  while (!ss.eof()) {
    /* extracting word by word from stream */
    ss >> temp;

    /* Checking the given word is integer or not */
    if (stringstream(temp) >> found) features.push_back(found);
  }

  /* To save from space at the end of string */
  temp = "";

  return features;
}

// Text file of template to template on correct format - [[a1, b1, ø1], [a2,
// b2, ø2], ...]
vector<Minutia> templatePreprocess(string txtFileLocation) {
  // Declare the template, list of vectors - each vector includes x, y and
  // angle of a minutia from the template
  vector<vector<double>> formattedTemplate;
  // Extract string from text file to vector of double values (x, y and angle)
  vector<double> feature_numbers;
  string line;
  ifstream inFile;
  inFile.open(txtFileLocation);
  if (inFile.is_open()) {
    while (getline(inFile, line)) {
      feature_numbers = extractDoubleWords(line);
    }
    inFile.close();
  }

  double x = 0, y = 0, angle = 0;

  vector<Minutia> minutiae;
  for (auto it = feature_numbers.begin(), end = feature_numbers.end();
       it != end; it++) {
    if (x == 0) {
      x = *it;
      continue;
    }
    if (y == 0) {
      y = *it;
      continue;
    }

    if (angle == 0) {
      angle = *it;
    }

    minutiae.push_back(Minutia(x, y, angle));
    x = 0, y = 0, angle = 0;
  }
  return minutiae;
}

void create_combinations(int offset, int k) {
  if (k == 0) {
    combination_complete.push_back(combination);
    return;
  }
  int filessize = files.size();
  for (int i = offset; i <= filessize - k; ++i) {
    combination.push_back(files[i]);
    create_combinations(i + 1, k - 1);
    combination.pop_back();
  }
}