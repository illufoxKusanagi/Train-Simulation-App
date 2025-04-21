#ifndef STATION_DATA_H
#define STATION_DATA_H

#include <vector>

struct StationData {
  std::vector<double> x_station;
  std::vector<double> v_limit;
  std::vector<double> x_section;
  std::vector<double> slope;
  std::vector<double> x_slopeStart;
  std::vector<double> x_slopeEnd;
  std::vector<double> radius;
  std::vector<double> x_radiusStart;
  std::vector<double> x_radiusEnd;
};

#endif // STATION_DATA_H
