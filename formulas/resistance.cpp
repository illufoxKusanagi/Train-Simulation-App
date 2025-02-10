#include <iostream>

const float g = 9.8;
float m;
float startRes;
float slope;
float radius;
float v;
float m_m = 130;
float m_t = 120;
int n = 5;

double r_train;
double r_slope;
double r_radius;
double r_start;
double r_run;
double f_res;

void collectData() {
  std::cout << "===Calculate Start Resistance===" << std::endl;
  std::cout << "Enter train mass : ";
  std::cin >> m;
  std::cout << "Enter start resistance : ";
  std::cin >> startRes;
  std::cout << "Enter radius : ";
  std::cin >> radius;
  std::cout << "Enter slope : ";
  std::cin >> slope;
}

double calculateResTrain(float m, float startRes) {
  return (m * g * startRes * (1.0 / 1000));
}

double calculateResSlope(float m, float slope) {
  return (m * g * slope * (1.0 / 1000));
}

double calculateResRadius(float m, float radius) {
  return (m * g * (6.0 / radius) * (1.0 / 1000));
}

double countStartRes() {
  collectData();
  r_train = calculateResTrain(m, startRes);
  r_slope = calculateResSlope(m, slope);
  r_radius = calculateResRadius(m, radius);
  std::cout << "r_train value : " << r_train << "kN" << std::endl;
  std::cout << "r_slope value : " << r_slope << "kN" << std::endl;
  std::cout << "r_radius value : " << r_radius << "kN" << std::endl;

  return (r_train + r_slope + r_radius);
}

float countRunningRes(float v) {
  collectData();
  r_train = calculateResTrain(m, startRes);
  r_slope = calculateResSlope(m, slope);
  r_radius = calculateResRadius(m, radius);
  std::cout << "r_train value : " << r_train << "kN" << std::endl;
  std::cout << "r_slope value : " << r_slope << "kN" << std::endl;
  std::cout << "r_radius value : " << r_radius << "kN" << std::endl;
  return (1 / 1000 *
              (((1.65 + (0.0247 * v)) * (m_m * g)) +
               ((0.78 + (0.0028 * v) * (m_t * g)) +
                (g * (0.028 + 0.0078 * (n - 1)) * (v * v)))) +
          r_slope + r_radius);
}

int main() {
  std::cout << "Enter speed : ";
  std::cin >> v;
  f_res = v > 0 ? countRunningRes(v) : countStartRes();
  std::cout << "Resistance Force : " << f_res << "m/s^2";
  return 0;
}