#ifndef STATION_DATA_H
#define STATION_DATA_H

#include <vector>

struct StationData {
  double stat_x_station;
  double stat_v_limit;
  double stat_slope;
  double stat_radius;
  double x_odo;
  int n_station = 2;
  std::vector<double> x_station;
  std::vector<double> v_limit;
  std::vector<double> x_v_limitStart;
  std::vector<double> x_v_limitEnd;
  std::vector<double> slope;
  std::vector<double> x_slopeStart;
  std::vector<double> x_slopeEnd;
  std::vector<double> radius;
  std::vector<double> x_radiusStart;
  std::vector<double> x_radiusEnd;
  std::vector<double> x_section;
};

#endif // STATION_DATA_H
