#ifndef TRAIN_DATA_H
#define TRAIN_DATA_H

struct ConstantData {
  const float g = 9.8;
  const float cV = 3.6;
  const float cF = 101.972;
  const float dt = 0.5;
};

struct TrainData {
  int n_tm;
  int n_axle;
  int n_car;
  float gearRatio;
  float wheel;
};

struct MassData {
  float m_Me;
  float m_Te;
  float m_M;
  float m_T;
  float m_M1;
  float m_M2;
  float m_TC;
  float m_T1;
  float m_T2;
  float m_T3;
  float i_M;
  float i_T;
  float m_Mi;
  float m_Ti;
  double m_totalEmpty;
  double m_totalLoad;
  double m_totalInertial;
};

struct LoadData {
  float load;
  int n_M1;
  int n_M2;
  int n_Tc;
  int n_T1;
  int n_T2;
  int n_T3;
  float m_P;
  int n_PM1;
  int n_PM2;
  int n_PTc;
  int n_PT1;
  int n_PT2;
  int n_PT3;
};

struct ResistanceData {
  float startRes;
  float slope;
  float radius;
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
  float acc;
  float decc;
  float acc_start;
  float decc_start;
  double v;
  float v_limit;
  float v_p1;
  float v_p2;
  float v_b1;
  float v_b2;
  float v_diffCoast;
};

struct TrainMotorData {
  double tm_f;
  double tm_t;
  double tm_rpm;
  double tm_adh;
  double tm_f_res;
};

struct EfficiencyData {
  float eff_gear;
  float eff_motor;
  float eff_vvvf;
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
};

#endif // TRAIN_DATA_H
