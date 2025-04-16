using namespace std;

#ifndef TRAIN_DATA_H
#define TRAIN_DATA_H

// #include <vector>

// struct ConstantData {
//   const double g = 9.8;
//   const double cV = 3.6;
//   const double cF = 101.972;
//   const double dt = 1.0;
//   const double t_reaction = 1.5;
//   const double t_delay = 3.0;
// };

struct TrainData {
  double n_tm;
  double n_axle;
  double n_car;
  double gearRatio;
  double wheel;
  double trainsetLength;
};

// struct MassData {
//   double mass_Me;
//   double mass_Te;
//   double mass_M;
//   double mass_T;
//   double mass_M1;
//   double mass_M2;
//   double mass_TC;
//   double mass_T1;
//   double mass_T2;
//   double mass_T3;
//   double i_M;
//   double i_T;
//   double mass_Mi;
//   double mass_Ti;
//   double mass_totalEmpty;
//   double mass_totalLoad;
//   double mass_totalInertial;
// };

// struct LoadData {
//   double load;
//   double n_M1;
//   double n_M2;
//   double n_Tc;
//   double n_T1;
//   double n_T2;
//   double n_T3;
//   double mass_P;
//   double mass_P_final;
//   double n_PM1;
//   double n_PM2;
//   double n_PTc;
//   double n_PT1;
//   double n_PT2;
//   double n_PT3;
// };

// struct ResistanceData {
//   double startRes;
//   double slope;
//   double radius;
//   double r_train = 0;
//   double r_slope = 0;
//   double r_radius = 0;
//   double r_run = 0;
//   double f_res = 0;
//   double f_resStart = 0;
//   double f_resRunning = 0;
//   double f_total = 0;
//   double f_start = 0;
//   double f_motor = 0;
//   double f_brake = 0;
// };

// struct MovingData {
//   double acc = 0;
//   double acc_si = 0;
//   double decc = 0;
//   double decc_si = 0;
//   double acc_start;
//   double decc_start;
//   double decc_emergency;
//   double v = 0;
//   double v_si = 0;
//   double v_limit;
//   double v_p1;
//   double v_p2;
//   double v_b1;
//   double v_b2;
//   double v_diffCoast;
//   double time = 0;
//   double x = 0;
//   double time_total = 0;
//   double x_total = 0;
//   double x_station = 100;
// };

// struct TrainMotorData {
//   double tm_f = 0;
//   double tm_t = 0;
//   double tm_rpm = 0;
//   double tm_adh = 0;
//   double tm_f_res = 0;
// };

// struct EfficiencyData {
//   double eff_gear;
//   double eff_motor;
//   double eff_vvvf;
// };

// struct PowerData {
//   double p_wheel = 0;
//   double p_motorOut = 0;
//   double p_motorIn = 0;
//   double p_vvvfIn = 0;
//   double p_catenary = 0;
//   double p_aps;
// };

// struct EnergyData {
//   double e_motor = 0;
//   double e_pow = 0;
//   double e_reg = 0;
//   double e_aps = 0;
//   double e_catenary = 0;
//   double vol_line = 0;
//   double curr_catenary = 0;
//   double curr_vvvf = 0;
// };

// struct StationData {
//   vector<double> stationDistance;
//   vector<double> speedLimit;
//   vector<double> sectionDistance;
//   vector<double> slope;
//   vector<double> radius;
// };

#endif // TRAIN_DATA_H
