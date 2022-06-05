#include "minutia.h"

Minutia::Minutia() {
  x_coordinate = 0;
  y_coordinate = 0;
  angle = 0;
}

Minutia::Minutia(double x_coordinate1, double y_coordinate1, double angle1) {
  x_coordinate = x_coordinate1;
  y_coordinate = y_coordinate1;
  angle = angle1;
}