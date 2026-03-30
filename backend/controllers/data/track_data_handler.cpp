#include "track_data_handler.h"

TrackDataHandler::TrackDataHandler(AppContext *context, QObject *parent)
    : QObject(parent), m_movingData(context->movingData.data()),
      m_resistanceData(context->resistanceData.data()),
      m_stationData(context->stationData.data()) {}

void TrackDataHandler::setMaxSpeedValue(double maxSpeed) {
  if (m_stationData) {
    m_stationData->stat_v_limit = maxSpeed;
  }
}

void TrackDataHandler::setStationDistanceValue(double stationDistance) {
  if (m_stationData) {
    m_stationData->stat_x_station = stationDistance;
  }
}

void TrackDataHandler::setSlopeValue(double slope) {
  if (m_stationData) {
    m_stationData->stat_slope = slope;
  }
}

void TrackDataHandler::setRadiusValue(double radius) {
  if (m_stationData) {
    m_stationData->stat_radius = radius;
  }
}

void TrackDataHandler::setStationDistanceList(
    const vector<double> &stationDistanceList,
    const vector<double> &stationDistanceTotalList) {
  if (m_stationData) {
    m_stationData->x_station = stationDistanceList;
    m_stationData->tot_x_station = stationDistanceTotalList;
  }
}

void TrackDataHandler::setSlopeList(
    const vector<double> &slopeList,
    const vector<double> &slopeStartCoordinateList,
    const vector<double> &slopeEndCoordinateList) {
  if (m_stationData) {
    m_stationData->slope = slopeList;
    m_stationData->x_slopeStart = slopeStartCoordinateList;
    m_stationData->x_slopeEnd = slopeEndCoordinateList;
  }
}

void TrackDataHandler::setRadiusList(
    const vector<double> &radiusList,
    const vector<double> &radiusStartCoordinateList,
    const vector<double> &radiusEndCoordinateList) {
  if (m_stationData) {
    m_stationData->radius = radiusList;
    m_stationData->x_radiusStart = radiusStartCoordinateList;
    m_stationData->x_radiusEnd = radiusEndCoordinateList;
  }
}

void TrackDataHandler::setMaxSpeedList(
    const vector<double> &maxSpeedList,
    const vector<double> &maxSpeedStartCoordinateList,
    const vector<double> &maxSpeedEndCoordinateList) {
  if (m_stationData) {
    m_stationData->v_limit = maxSpeedList;
    m_stationData->x_v_limitStart = maxSpeedStartCoordinateList;
    m_stationData->x_v_limitEnd = maxSpeedEndCoordinateList;
  }
}

std::vector<double>
TrackDataHandler::toStdVector(const QList<double> &list) const {
  std::vector<double> vec;
  vec.reserve(list.size());
  for (double val : list) {
    vec.push_back(val);
  }
  return vec;
}