#ifndef PREPROCESSDATA_H
#define PREPROCESSDATA_H
#include <vector>
#include <string>
using std::vector;
using std::string;

class Minutia;

vector<Minutia> templatePreprocess(string txtFileLocation);

vector<double> extractDoubleWords(string str);

void create_combinations(int offset, int k);



#endif