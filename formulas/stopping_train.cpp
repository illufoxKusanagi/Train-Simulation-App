#include <algorithm>
#include <iostream>

using namespace std;

const float g = 9.8;
const float dt = 0.1;
const float v_b1 = 35;
const float v_b2 = 65;

float startRes;
float slope;
float radius;

double r_train;
double r_slope;
double r_radius;
double r_run;

double v = 0.0;

double acc_start;
double decc_start;
double f_res;
double f_resStart;
double f_resRunning;
double f_total;
double f_start;
double f_motor;
double f_brake;

float acc;
float decc;

float m_totalInertial;
float m_M = 10;
float m_T = 10;

float numberOfCar;
float c = 3.6;
int i = 0;

void inputData() {
  m_totalInertial = 100.0;
  cout << "Inertial mass train : " << m_totalInertial << endl;
  startRes = 4.0;
  cout << "Start resistance : " << startRes << endl;
  radius = 2000;
  cout << "Radius : " << radius << endl;
  slope = 0;
  cout << "Slope : " << slope << endl;
  numberOfCar = 12.0;
  cout << "Number of Car : " << numberOfCar << endl;
  decc_start = 1.0;
  cout << "Deceleration : " << decc_start << endl;
  v = 20.0;
  cout << "Initial speed : " << v << endl;
  cout << "Time difference : " << dt << endl;
}

double calculateResTrain(float m, float startRes) {
  return ((m * startRes) / 1000);
}

double calculateResSlope(float m, float slope) {
  return ((m * g * slope) / 1000);
}

double calculateResRadius(float m, float radius) {
  return ((m * g * (6.0 / radius)) / 1000);
}

double calculateStartRes() {
  r_train = calculateResTrain(m_totalInertial, startRes);
  r_slope = calculateResSlope(m_totalInertial, slope);
  r_radius = calculateResRadius(m_totalInertial, radius);

  return (r_train + r_slope + r_radius);
}

double calculateRunningRes(float v) {
  r_slope = calculateResSlope(m_totalInertial, slope);
  r_radius = calculateResRadius(m_totalInertial, radius);
  r_run =
      (1.0 / 1000.0) * (((1.65 + (0.0247 * v)) * (m_M * g)) +
                        ((0.78 + (0.0028 * v)) * (m_T * g)) +
                        (g * (0.028 + 0.0078 * (numberOfCar - 1)) * (v * v)));

  return r_run + r_slope + r_radius;
}

void calculateStoppingForce() {
  f_brake = m_totalInertial * (decc_start / c);
  if (v < v_b1) {
    f_motor = -f_brake;
  } else if (v > v_b1 && v <= v_b2) {
    f_motor = -((f_brake * v_b1) / v);
  } else if (v > v_b2) {
    f_motor = -((f_brake * v_b1 * v_b2) / (v * v));
  } else {
    cout << "Invalid input" << endl;
  }
}

void calculateTotalBrakeForce() { f_total = f_motor; }

void calculateValues() {
  while (v > 0) {
    f_resStart = calculateStartRes();
    f_resRunning = calculateRunningRes(v);
    calculateStoppingForce();
    calculateTotalBrakeForce();
    decc = c * f_total / m_totalInertial;
    v += decc * dt;
    i++;

    cout << "Iteration " << i << endl;
    // cout << "Resistance Train : " << r_train << " kN" << endl;
    // cout << "Resistance Slope : " << r_slope << " kN" << endl;
    // cout << "Resistance Radius : " << r_radius << " kN" << endl;
    // cout << "Resistance Running : " << r_run << " kN" << endl;
    // cout << "Resistance Force Start : " << f_resStart << " kN" << endl;
    // cout << "Resistance Force Running : " << f_resRunning << " kN" << endl;
    cout << "Braking force : " << f_brake << " kN" << endl;
    cout << "Motor force : " << f_motor << " kN" << endl;
    cout << "Total force : " << f_total << " kN" << endl;
    cout << "Decceleration : " << decc << " km/h/s" << endl;
    cout << "Speed : " << v << " km/h" << endl;
  }
}

int main() {
  inputData();
  calculateValues();
  return 0;
}
