#include <algorithm>
#include <iostream>

using namespace std;

const float g = 9.8;
const float dt = 0.5;
const float v_p1 = 35;
const float v_p2 = 65;

float startRes;
float slope;
float radius;

double r_train;
double r_slope;
double r_radius;
double r_run;

float v = 0.0;

double f_res;
double f_total;
double f_start;
double f_motor;

float acc;

float m_totalInertial;
float m_M = 10;
float m_T = 10;

float numberOfCar;
float c = 3.6;
int i = 0;

void inputData() {
  m_totalInertial = 100;
  cout << "Inertial mass train : " << m_totalInertial << endl;
  startRes = 10;
  cout << "Start resistance : " << startRes << endl;
  radius = 6;
  cout << "Radius : " << radius << endl;
  slope = 10;
  cout << "Slope : " << slope << endl;
  numberOfCar = 12.0;
  cout << "Number of Car : " << numberOfCar << endl;
  acc = 1.0;
  cout << "Acceleration : " << acc << endl;
  cout << "Time difference : " << dt << endl;
}

double calculateResTrain(float m, float startRes) {
  return ((m * g * startRes) / 1000);
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

void calculatePoweringForce(float acc) {
  f_start = m_totalInertial * (acc / c) + f_res;
  if (v < 1e-6)
    v = 1e-6; // Hindari pembagian nol
  if (v <= v_p1) {
    f_motor = f_start;
  } else if (v > v_p1 && v <= v_p2) {
    f_motor = (f_start * v_p1) / v;
  } else if (v > v_p2) {
    f_motor = (f_start * v_p1 * v_p2) / (v * v);
  }
  cout << "Resistance due to force : " << f_motor << " kN" << endl;
}

void calculateTotalForce() {
  f_total = f_motor - f_res;
  cout << "Total force : " << f_total << " kN" << endl;
}

void calculateValues(float acc) {
  while (v < 10) {
    cout << "\nIteration : " << i + 1 << endl;

    f_res = (v == 0) ? calculateStartRes() : calculateRunningRes(v);
    cout << "Resistance Train : " << r_train << " kN" << endl;
    cout << "Resistance Slope : " << r_slope << " kN" << endl;
    cout << "Resistance Radius : " << r_radius << " kN" << endl;
    cout << "Resistance Running : " << r_run << " kN" << endl;
    cout << "Resistance Force : " << f_res << " kN" << endl;

    calculatePoweringForce(acc);
    calculateTotalForce();

    // Perhitungan percepatan sebelum memperbarui kecepatan
    // acc = c * f_total / m_totalInertial;
    acc = max(0.0, min(c * f_total / m_totalInertial, 5.0));
    // acc = max(0.0, c * f_total / m_totalInertial);
    cout << "Acceleration : " << acc << " km/h/s" << endl;

    // Memperbarui kecepatan
    v += acc * dt;
    cout << "Speed : " << v << " km/h" << endl;

    i++;
  }
}

int main() {
  inputData();
  calculateValues(acc);
  return 0;
}
