#include <iostream>

using namespace std;

const float c = 3.6;
float v;

double p_wheel;
double p_motorOut;
double p_motorIn;
double p_vvvfIn;
double p_catenary;
double p_aps;

double e_motor;
double f_pow;
double e_reg;
double e_aps;

float eff_gear;
float eff_motor;
float eff_vvvf;

double f_motor;
float t = 10;
float dt = 0.5;

void inputData() {
  v = 72;
  eff_gear = 0.34;
  eff_motor = 0.14;
  eff_vvvf = 0.4;
  f_motor = 115.881;
  p_aps = 30;
  cout << "Speed : " << v << " km/h" << endl;
  cout << "Efficiency of Gear : " << eff_gear << endl;
  cout << "Efficiency of Motor : " << eff_motor << endl;
  cout << "Efficiency of VVVF : " << eff_vvvf << endl;
  cout << "Force of Motor : " << f_motor << " kN" << endl;
  cout << "Power of APS : " << p_aps << " kW" << endl;
  cout << endl;
}

void calculatePowerWheel() { p_wheel = f_motor * (v / c); }

void calculateOutputTractionMotor() { p_motorOut = p_wheel / eff_gear; }

void calculateInputTractionMotor() { p_motorIn = p_motorOut / eff_motor; }

void calculatePowerInputOfVvvf() { p_vvvfIn = p_motorIn / eff_vvvf; }

void calculatePowerOfCatenary() { p_catenary = p_vvvfIn + p_aps; }

void calculateEnergyMotorConsumption() {
  for (int i = 0; i < t; i++)
    e_motor += (p_motorOut / 3600) * dt;
  cout << "Calculate Energy of Motor Consumption for 0 to 10 second" << endl;
}

void calculateEnergyOfPowering() {
  for (int i = 0; i < t; i++)
    f_pow += (p_vvvfIn / 3600) * dt;
  cout << "Calculate Energy of Powering for 0 to 10 second" << endl;
}

void calculateEnergyRegeneration() {
  for (int i = 0; i < t; i++)
    e_reg += (p_vvvfIn / 3600) * dt;
  cout << "Calculate Energy of Regenerative for 0 to 10 second" << endl;
}

void calculateEnergyOfAps() {
  for (int i = 0; i < t; i++)
    e_aps += (p_aps / 3600) * dt;
  cout << "Calculate Energy of APS for 0 to 10 second" << endl;
}

int main() {
  inputData();
  calculatePowerWheel();
  calculateOutputTractionMotor();
  calculateInputTractionMotor();
  calculatePowerInputOfVvvf();
  calculatePowerOfCatenary();
  cout << "Power of Wheel : " << p_wheel << " kW" << endl;
  cout << "Output Power of Traction Motor : " << p_motorOut << " kW" << endl;
  cout << "Input Power of Traction Motor : " << p_motorIn << " kW" << endl;
  cout << "Power of VVVF : " << p_vvvfIn << " kW" << endl;
  cout << "Power of Catenary : " << p_catenary << " kW" << endl;
  cout << endl;
  calculateEnergyMotorConsumption();
  cout << "Energy of Motor Consumption : " << e_motor << " kWh" << endl;
  calculateEnergyOfPowering();
  cout << "Energy of Powering : " << f_pow << " kWh" << endl;
  calculateEnergyRegeneration();
  cout << "Energy of Regeneration : " << e_reg << " kWh" << endl;
  calculateEnergyOfAps();
  cout << "Energy of APS : " << e_aps << " kWh" << endl;
  return 0;
}