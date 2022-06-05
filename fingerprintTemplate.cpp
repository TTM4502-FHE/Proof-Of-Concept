#include "fingerprintTemplate.h"
#include "convexHull.h"
#include <chrono>
#include <future>
#include "comparison.h"

#include <iostream>

using std::cout;

FingerprintTemplate::FingerprintTemplate(vector<Minutia> &minutiae1,
                                         template_struct &template_struct) {
  convexHull = findConvexHull(minutiae1);
  fillWithCylinders(template_struct, minutiae1);
}

void FingerprintTemplate::fillWithCylinders(template_struct &template_struct,
                                            vector<Minutia> &minutiae1) {
  vector<std::future<Cylinder>> fut_cylinders;
  for (auto it = minutiae1.begin(), end = minutiae1.end(); it != end; it++) {
    fut_cylinders.emplace_back(std::async(
        std::launch::async, createCylinderAsync, *it, minutiae1, convexHull));
  }
  for (auto &&fut : fut_cylinders) {
    auto cyl_temp = fut.get();
    if (cyl_temp.valid) {
      cylinders.push_back(cyl_temp);
      cylinder_struct cylinder_struct;
      cylinder_struct.cyl_angle = cyl_temp.minutia.angle;
      cylinder_struct.c_m = cyl_temp.c_m;
      template_struct.cylinders.push_back(cylinder_struct);
    }
  }
}