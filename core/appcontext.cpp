#include "appcontext.h"

AppContext::AppContext() {
  trainData = QSharedPointer<TrainData>::create();
  massData = QSharedPointer<MassData>::create();
  loadData = QSharedPointer<LoadData>::create();
  resistanceData = QSharedPointer<ResistanceData>::create();
  movingData = QSharedPointer<MovingData>::create();
  trainMotorData = QSharedPointer<TrainMotorData>::create();
  efficiencyData = QSharedPointer<EfficiencyData>::create();
  powerData = QSharedPointer<PowerData>::create();
  energyData = QSharedPointer<EnergyData>::create();
  stationData = QSharedPointer<StationData>::create();
}
