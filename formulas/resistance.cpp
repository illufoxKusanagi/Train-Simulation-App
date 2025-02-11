#include <iostream>

const float g = 9.8;
float mass;
float startRes;
float slope;
float radius;
float speed;
float m_m = 130;
float m_t = 120;
int numberOfCar = 5;

double r_train;
double r_slope;
double r_radius;
double r_start;
double r_run;
double f_res;

void collectData() {
  std::cout << "===Calculate Start Resistance===" << std::endl;
  std::cout << "Enter train mass : ";
  std::cin >> mass;
  std::cout << "Enter start resistance : ";
  std::cin >> startRes;
  std::cout << "Enter radius : ";
  std::cin >> radius;
  std::cout << "Enter slope : ";
  std::cin >> slope;
  std::cout << "Enter number of Car : ";
  std::cin >> numberOfCar;
}

void printData() {
  std::cout << "Resistance due to train itself at start  value : " << r_train
            << " kN" << std::endl;
  std::cout << "Resistance due to slope  value : " << r_slope << " kN"
            << std::endl;
  std::cout << "Resistance due to radius  value : " << r_radius << " kN"
            << std::endl;
}

double calculateResTrain(float mass, float startRes) {
  return (mass * g * startRes * (1.0 / 1000));
}

double calculateResSlope(float mass, float slope) {
  return (mass * g * slope * (1.0 / 1000));
}

double calculateResRadius(float mass, float radius) {
  return (mass * g * (6.0 / radius) * (1.0 / 1000));
}

double countStartRes() {
  collectData();
  r_train = calculateResTrain(mass, startRes);
  r_slope = calculateResSlope(mass, slope);
  r_radius = calculateResRadius(mass, radius);
  printData();
  return (r_train + r_slope + r_radius);
}

float countRunningRes(float speed) {
  collectData();
  r_train = calculateResTrain(mass, startRes);
  r_slope = calculateResSlope(mass, slope);
  r_radius = calculateResRadius(mass, radius);
  printData();
  return (1 / 1000 *
              (((1.65 + (0.0247 * speed)) * (m_m * g)) +
               ((0.78 + (0.0028 * speed) * (m_t * g)) +
                (g * (0.028 + 0.0078 * (numberOfCar - 1)) * (speed * speed)))) +
          r_slope + r_radius);
}

int main() {
  std::cout << "Enter speed : ";
  std::cin >> speed;
  f_res = speed > 0 ? countRunningRes(speed) : countStartRes();
  std::cout << "Resistance Force : " << f_res << " m/s^2";
  return 0;
}