#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

float v = 72;
double p_motorIn = 24017.9;
float p_factorMotor = 0.8;
double p_vvvfIn = 24017.9;
double vol_vvvf = 1500.0;

double vol_motor;
double curr_motor;
double curr_vvvf;
double curr_motor_rms;
double curr_vvvf_rms;

void calculateMotorVoltage() {
  if (v < 39) {
    vol_motor = 50.0;
  } else if (v >= 39) {
    vol_motor = 1135.0;
  }
}

void calculateMotorCurrent() {
  if (v <= 35) {
    curr_motor = 1000 * (p_motorIn / (sqrt(3) * vol_motor * p_factorMotor));
  } else if (v >= 39) {
    curr_motor = 1000 * (p_motorIn / (sqrt(3) * vol_motor * p_factorMotor));
  }
}

void calculateVvvfCurrent() { curr_vvvf = 1000 * (p_vvvfIn / vol_vvvf); }

void calculateRmsMotorCurrent() {
  vector<double> curr_motors = {10.0, 15.0, 20.0, 25.0, 30.0};
  double sum_curr_motor = 0.0;
  int numberOfData = curr_motors.size();

  for (double current : curr_motors) {
    sum_curr_motor += current;
  }

  curr_motor_rms = std::sqrt(sum_curr_motor / numberOfData);
}

void calculateRmsLineCurrent() {
  vector<double> curr_motors = {12.0, 14.0, 25.0, 29.5, 34.0};
  double sum_curr_vvvf = 0.0;
  int numberOfData = curr_motors.size();

  for (double current : curr_motors) {
    sum_curr_vvvf += current;
  }

  curr_vvvf_rms = std::sqrt(sum_curr_vvvf / numberOfData);
}

int main() {
  calculateMotorVoltage();
  calculateMotorCurrent();
  calculateVvvfCurrent();
  calculateRmsMotorCurrent();
  calculateRmsLineCurrent();

  cout << "Motor Voltage: " << vol_motor << endl;
  cout << "Motor Current: " << curr_motor << endl;
  cout << "VVVF Current: " << curr_vvvf << endl;
  cout << "RMS Motor Current: " << curr_motor_rms << endl;
  cout << "RMS Line Current: " << curr_vvvf_rms << endl;

  return 0;
}