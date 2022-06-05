#ifndef GLOBALS_H
#define GLOBALS_H
// Define Infinite (Using INT_MAX caused overflow problems)
#define INF 10000
#define _USE_MATH_DEFINES
#include "math.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

extern double R;          // Radius of cylinder
extern double N_S;        // Number of cells in diameter
extern double N_D;        // Number of layers
extern double Delta_S;    // Side of cell
extern double Delta_D;    // Depth of cell
extern double std_s;      // Standard deviation spacial
extern double std_d;      // Standard deviation directional
extern double zigmoid_u;  // Zigmoid param from paper
extern double zigmoid_t;  // Zigmoid param from paper
extern double min_vc;     // Min. valid cellc required in a cylinder
extern double min_m;      // Min. minutiae that contribute to cylinder
extern double min_me;
extern double omega;  // Padding outside of convex hull

extern string path_havard;
extern string path_lasse;
extern string path_ntnuserver;
extern const char* templates_path;
extern vector<string> files;

extern vector<string> combination;

extern vector<vector<string>> combination_complete;

extern bool mated;

// FHE PALISADE
extern uint32_t multDepth;
extern uint32_t scaleFactorBits;
extern uint32_t batchSize;

#endif