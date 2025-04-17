#ifndef ENERGY_DATA_H
#define ENERGY_DATA_H

struct EnergyData {
  double e_motor = 0;
  double e_pow = 0;
  double e_reg = 0;
  double e_aps = 0;
  double e_catenary = 0;
  double vol_line = 0;
  double curr_catenary = 0;
  double curr_vvvf = 0;
};

#endif // ENERGY_DATA_H
