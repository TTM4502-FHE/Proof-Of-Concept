#include "globals.h"

double R = 70.0;                      // Radius of cylinder
double N_S = 16.0;                    // Number of cells in diameter
double N_D = 6.0;                     // Number of layers
double Delta_S = (2.0 * R) / N_S;     // Side of cell
double Delta_D = (2.0 * M_PI) / N_D;  // Depth of cell
double std_s = 28.0 / 3.0;            // Standard deviation spacial
double std_d = (2.0 / 9.0) * M_PI;    // Standard deviation directional
double zigmoid_u = 1.0 / 100.0;       // Zigmoid param from paper
double zigmoid_t = 400;               // Zigmoid param from paper
double min_vc = 16.0 * 16.0 * 0.75;   // Min. valid cellc required in a cylinder
double min_m = 2;  // Min. minutiae that contribute to cylinder
string path_havard =
    "/mnt/c/Users/haava/Documents/GitHub/PalisadeWindows/src/data/"
    "txt-templates/";
string path_lasse = "/Users/lassevad/Documents/Biometrics/crossmatchTemplates/";

vector<string> files;

vector<string> combination;

vector<vector<string>> combination_complete;
// FHE PALISADE variables

uint32_t multDepth = 1;
uint32_t scaleFactorBits = 50;
uint32_t batchSize = 1536;
