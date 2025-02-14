#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

double tm_f;
double tm_t;
double tm_rpm;

double f_motor;
float v;

int n_train;
float gearRatio;
int wheel;

void calculateTractionForce() { tm_f = f_motor / n_train; }

void calculateTorque() { tm_t = (f_motor * (wheel / (2 * 100))) / gearRatio; }

void calculateRpm() {
  tm_rpm = ((v * 1000 / 60) * gearRatio) / ((M_PI * wheel) / 1000);
}

int main() {
  std::cout << "Hello World!" << std::endl;
  return 0;
}