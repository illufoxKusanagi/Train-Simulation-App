export interface TrainConstantParams {
  i_T: number;
  i_M: number;
  n_axle: number;
  n_tm: number;
  wheelDiameter: number;
  mass_P: number;
  gearRatio: number;
  carLength: number;
  loadCondition: ["AW0", "AW1", "AW2", "AW3", "AW4"];
  trainsetData: ["12", "10", "8", "6", "4", "12-Degraded", "10-Degraded"];
}

export interface TrainMassParams {
  mass_M1: number;
  mass_M2: number;
  mass_T1: number;
  mass_T2: number;
  mass_T3: number;
  mass_Tc: number;
}

export interface TrainPassangerParams {
  n_PTc: number;
  n_PM1: number;
  n_PM2: number;
  n_PT1: number; // Fixed: was n_Pt1 (lowercase t)
  n_PT2: number; // Fixed: was n_Pt2 (lowercase t)
  n_PT3: number; // Fixed: was n_Pt3 (lowercase t)
}

export interface TrainNumberParams {
  n_M1: number;
  n_M2: number;
  n_T1: number;
  n_T2: number;
  n_T3: number;
  n_Tc: number;
}

export interface TrackParams {
  n_station: number;
  x_station: number;
  radius: number;
  slope: number;
  v_limit: number;
  dwellTime: number;
}

export interface RunningParams {
  startRes: number;
  v_diffCoast: number;
  acc_start: number;
  v_p1: number;
  v_p2: number;
  v_b1: number;
  v_b2: number;
  decc_start: number;
  decc_emergency: number;
}

export interface ElectricalParams {
  stat_vol_line: number;
  stat_vol_motor: number;
  stat_pf: number;
  stat_eff_gear: number;
  stat_eff_motor: number;
  stat_eff_vvvf: number;
  p_aps: number;
}
