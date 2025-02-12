#include <iostream>

double f_start;
double f_brake;
double f_motor;
double m_totalInertial = 130;
double acc;
double dec;
double f_res = 14.5236;
float v;
float v_p1;
float v_p2;
float v_b1;
float v_b2;
int notch;
float f_total;
const float g = 9.8;
float m;
float startRes;
float slope;
float radius;
float speed;
float m_M = 130;
float m_T = 120;
int numberOfCar = 5;

double r_train;
double r_slope;
double r_radius;
double r_start;
double r_run;
double f_res;

void collectData() {
  std::cout << "===Calculate Start Resistance===" << std::endl;
  std::cout << "Enter train m : ";
  std::cin >> m;
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

double calculateResTrain(float m, float startRes) {
  return (m * g * startRes * (1.0 / 1000));
}

double calculateResSlope(float m, float slope) {
  return (m * g * slope * (1.0 / 1000));
}

double calculateResRadius(float m, float radius) {
  return (m * g * (6.0 / radius) * (1.0 / 1000));
}

double calculateStartRes() {
  collectData();
  r_train = calculateResTrain(m, startRes);
  r_slope = calculateResSlope(m, slope);
  r_radius = calculateResRadius(m, radius);
  printData();
  return (r_train + r_slope + r_radius);
}

float calculateRunningRes(float speed) {
  collectData();
  r_train = calculateResTrain(m, startRes);
  r_slope = calculateResSlope(m, slope);
  r_radius = calculateResRadius(m, radius);
  printData();
  return (1 / 1000 *
              (((1.65 + (0.0247 * speed)) * (m_M * g)) +
               ((0.78 + (0.0028 * speed) * (m_T * g)) +
                (g * (0.028 + 0.0078 * (numberOfCar - 1)) * (speed * speed)))) +
          r_slope + r_radius);
}

int mainResistance() {
  std::cout << "Enter speed : ";
  std::cin >> speed;
  f_res = speed > 0 ? calculateRunningRes(speed) : calculateStartRes();
  std::cout << "Resistance Force : " << f_res << " m/s^2";
  return 0;
}

void calculatePoweringForce() {
  std::cout << "Enter weakening point 1 : ";
  std::cin >> v_p1;
  std::cout << "Enter weakening point 2 : ";
  std::cin >> v_p2;
  f_start = m_totalInertial * acc + f_res;
  if (v <= v_p1) {
    f_motor = f_start;
  } else if (v > v_p1 && v <= v_p2) {
    f_motor = (f_start * v_p1) / v;
  } else if (v > v_p2) {
    f_motor = (f_start * v_p1 * v_p2) / (v * v);
  } else {
    std::cout << "Invalid input" << std::endl;
  }
  std::cout << "Powering force : " << f_motor << std::endl;
}

void calculateBrakingForce() {
  std::cout << "Enter braking point 1 : ";
  std::cin >> v_b1;
  std::cout << "Enter braking point 2 : ";
  std::cin >> v_b2;
  f_brake = m_totalInertial * dec;
  if (v <= v_b1) {
    f_motor = -f_brake;
  } else if (v > v_b1 && v <= v_b2) {
    f_motor = -((f_brake * v_b1) / v);
  } else if (v > v_b2) {
    f_motor = -((f_brake * v_b1 * v_b2) / (v * v));
  } else {
    std::cout << "Invalid input" << std::endl;
  }
  std::cout << "Braking force : " << f_motor << std::endl;
}

void calculateCoastingForce() {
  std::cout << "Enter notch (-1 to 1) : ";
  std::cin >> notch;
  f_motor = 0;
  if (notch == 1) {
    f_total = f_motor - f_res;
    std::cout << "Powering total force : " << f_total << std::endl;
  } else if (notch == 0) {
    f_total = -f_res;
    std::cout << "Coasting total force : " << f_total << std::endl;
  } else if (notch == -1) {
    f_total = -f_brake;
    std::cout << "Braking total force : " << f_total << std::endl;
  } else {
    std::cout << "Invalid input" << std::endl;
  }
}

int main() {
  std::cout << "Enter acceleration : ";
  std::cin >> acc;
  std::cout << "Enter deceleration : ";
  std::cin >> dec;
  std::cout << "Enter speed : ";
  std::cin >> v;
  calculatePoweringForce();
  calculateBrakingForce();
  calculateCoastingForce();
  return 0;
}