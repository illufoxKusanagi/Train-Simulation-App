#ifndef TRAIN_DATA_H
#define TRAIN_DATA_H

struct ConstantData {
  const double g = 9.8;
  const double cV = 3.6;
  const double cF = 101.972;
  const double dt = 0.5;
};

struct TrainData {
  double n_tm;
  double n_axle;
  double n_car;
  double gearRatio;
  double wheel;
};

struct MassData {
  double mass_Me;
  double mass_Te;
  double mass_M;
  double mass_T;
  double mass_M1;
  double mass_M2;
  double mass_TC;
  double mass_T1;
  double mass_T2;
  double mass_T3;
  double i_M;
  double i_T;
  double mass_Mi;
  double mass_Ti;
  double mass_totalEmpty;
  double mass_totalLoad;
  double mass_totalInertial;
};

struct LoadData {
  double load;
  double n_M1;
  double n_M2;
  double n_Tc;
  double n_T1;
  double n_T2;
  double n_T3;
  double mass_P;
  double mass_P_final;
  double n_PM1;
  double n_PM2;
  double n_PTc;
  double n_PT1;
  double n_PT2;
  double n_PT3;
};

struct ResistanceData {
  double startRes;
  double slope;
  double radius;
  double r_train;
  double r_slope;
  double r_radius;
  double r_run;
  double f_res;
  double f_resStart;
  double f_resRunning;
  double f_total;
  double f_start;
  double f_motor;
  double f_brake;
};

struct MovingData {
  double acc;
  double decc;
  double acc_start;
  double decc_start;
  double v;
  double v_limit;
  double v_p1;
  double v_p2;
  double v_b1;
  double v_b2;
  double v_diffCoast;
  double time;
  double time_simulate;
  double x;
};

struct TrainMotorData {
  double tm_f;
  double tm_t;
  double tm_rpm;
  double tm_adh;
  double tm_f_res;
};

struct EfficiencyData {
  double eff_gear;
  double eff_motor;
  double eff_vvvf;
};

struct PowerData {
  double p_wheel;
  double p_motorOut;
  double p_motorIn;
  double p_vvvfIn;
  double p_catenary;
  double p_aps;
};

struct EnergyData {
  double e_motor;
  double e_pow;
  double e_reg;
  double e_aps;
  double vol_line;
  double curr_catenary;
  double curr_vvvf;
};

#endif // TRAIN_DATA_H
