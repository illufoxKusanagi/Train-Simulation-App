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

void calculatePoweringForce() {
  std::cout << "Enter speed : ";
  std::cin >> v;
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
  std::cout << "Enter speed : ";
  std::cin >> v;
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
  std::cout << "Enter speed : ";
  std::cin >> v;
  std::cout << "Enter notch : ";
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
  calculatePoweringForce();
  calculateBrakingForce();
  calculateCoastingForce();
  return 0;
}