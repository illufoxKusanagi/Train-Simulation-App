#ifndef STATION_DATA_H
#define STATION_DATA_H

#include <vector>

struct StationData {
  std::vector<double> stationDistance;
  std::vector<double> speedLimit;
  std::vector<double> sectionDistance;
  std::vector<double> slope;
  std::vector<double> radius;
};

#endif // STATION_DATA_H
