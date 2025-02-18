#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

using namespace std;

const double coversion = 101.97162129779;

double tm_f;
double tm_t;
double tm_rpm;
double tm_adhesive;
float mM1 = 20.0;

double f_motor = 72.4678;
float v = 10.0;

int n_train = 12;
int n_ax;
float gearRatio;
float wheel;

void calculateTractionForce() { tm_f = f_motor / n_train; }

void calculateTorque() { tm_t = (f_motor * (wheel / (2 * 1000))) / gearRatio; }

void calculateRpm() {
  tm_rpm = ((v * 1000 / 60) * gearRatio) / ((M_PI * wheel) / 1000);
}

void calculateAdhesive() {
  tm_adhesive = (tm_f * coversion) / ((mM1 * 1000) / n_ax);
}

int main() {
  cout << "Enter gear ratio: ";
  cin >> gearRatio;
  cout << "Enter wheel diameter: ";
  cin >> wheel;
  calculateTractionForce();
  calculateTorque();
  calculateRpm();
  cout << "Traction force: " << tm_f << " N" << endl;
  cout << "Torque: " << tm_t << " Nm" << endl;
  cout << "RPM: " << tm_rpm << " rpm" << endl;
  return 0;
}