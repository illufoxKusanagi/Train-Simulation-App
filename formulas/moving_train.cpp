#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;

const float g = 9.8;
const float dt = 0.1;
const float c = 3.6;
const float v_p1 = 35;
const float v_p2 = 65;
const float v_b1 = 35;
const float v_b2 = 65;
const float v_limit = 70.0;
const float v_diffCoast = 5.0;

float startRes;
float slope;
float radius;
// float time;

double r_train;
double r_slope;
double r_radius;
double r_run;

double v;
float acc_start;
float decc_start;

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
int i = 0;

void inputData() {
  m_totalInertial = 100.0;
  startRes = 4.0;
  radius = 2000;
  slope = 0;
  numberOfCar = 12.0;
  acc_start = 1.0;
  decc_start = 1.0;
  cout << "Inertial mass train : " << m_totalInertial << endl;
  cout << "Start resistance : " << startRes << endl;
  cout << "Radius : " << radius << endl;
  cout << "Slope : " << slope << endl;
  cout << "Number of Car : " << numberOfCar << endl;
  cout << "Acceleration : " << acc_start << endl;
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

void calculatePoweringForce(float acc) {
  if (v == 0) {
    f_start = m_totalInertial * (acc / c) + f_resStart;
  }
  if (v <= v_p1) {
    f_motor = f_start;
  } else if (v > v_p1 && v <= v_p2) {
    f_motor = (f_start * v_p1) / v;
  } else if (v > v_p2) {
    f_motor = (f_start * v_p1 * v_p2) / (v * v);
  }
  cout << "Resistance due to force : " << f_motor << " kN" << endl;
}

void calculateStoppingForce(float decc) {
  f_brake = m_totalInertial * (decc_start / c);
  // f_brake = m_totalInertial * (abs(decc) / c);
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

void calculateTotalForce() {
  f_total = f_motor - (v < 1 ? f_resStart : f_resRunning);
}
void calculateTotalBrakeForce() { f_total = f_motor; }

void calculateBrakingValue() {
  i = 0;
  while (v > 0) {
    f_resStart = calculateStartRes();
    f_resRunning = calculateRunningRes(v);
    // calculateStoppingForce();
    calculateTotalBrakeForce();
    decc = c * f_total / m_totalInertial;
    v += decc * dt;
    i++;

    cout << "Iteration " << i << endl;
    cout << "Braking force : " << f_brake << " kN" << endl;
    cout << "Motor force : " << f_motor << " kN" << endl;
    cout << "Total force : " << f_total << " kN" << endl;
    cout << "Decceleration : " << decc << " km/h/s" << endl;
    cout << "Speed : " << v << " km/h" << endl;
  }
}

void calculateValues(float acc) {
  while (v < v_limit) {
    cout << endl << "\nIteration : " << i + 1 << endl;
    f_resStart = calculateStartRes();

    if (v > 0) {
      f_resRunning = calculateRunningRes(v);
    }
    calculatePoweringForce(acc);
    calculateTotalForce();
    acc = c * f_total / m_totalInertial;
    v += acc * dt;

    // TODO : need to add condition of minimum total force, at acceleration = 0
    if (f_total <= 0.02)
      break;
    i++;

    cout << "Resistance Train : " << r_train << " kN" << endl;
    cout << "Resistance Slope : " << r_slope << " kN" << endl;
    cout << "Resistance Radius : " << r_radius << " kN" << endl;
    cout << "Resistance Running : " << r_run << " kN" << endl;
    cout << "Resistance Force Start : " << f_resStart << " kN" << endl;
    cout << "Resistance Force Running : " << f_resRunning << " kN" << endl;
    cout << "Acceleration : " << acc << " km/h/s" << endl;
    cout << "Speed : " << v << " km/h" << endl;
  }
}

void simulateTrainMovement(float acc, float decc) {
  i = 0;
  bool isAccelerating = true;
  bool isCoasting = false; // Flag untuk tracking fase coasting
  float time = 0;          // Waktu mulai
  string phase;
  int coastingCount = 0;

  std::ofstream outFile("train_simulation.csv", std::ios::app);
  if (i == 0) { // Tambahkan header hanya sekali
    outFile
        << "Phase,Iteration,Time,Speed,Acceleration,F_motor,F_res,F_total\n";
  }

  while (v >= 0) {
    cout << "\nIteration : " << i + 1 << endl;

    f_resStart = calculateStartRes();
    if (v > 0) {
      f_resRunning = calculateRunningRes(v);
    }
    if (isAccelerating) {
      if (v >= v_limit) {
        isAccelerating = false;
        isCoasting = true;
        phase = "Coasting";
        continue;
      }
      phase = "Accelerating";
      calculatePoweringForce(acc);
      calculateTotalForce();
      acc = c * f_total / m_totalInertial;
      v += acc * dt;
    } else if (isCoasting) {
      if (v <= (v_limit - v_diffCoast)) {
        isCoasting = false;
        isAccelerating = true;
        coastingCount++;
        if (coastingCount >= 3) {
          // Start braking after 3 coasting cycles
          isAccelerating = false;
          isCoasting = false;
        }
        continue;
      }
      phase = "Coasting";
      f_motor = 0;
      f_total = -f_resRunning;
      acc = c * f_total / m_totalInertial;
      v += acc * dt;
    } else {
      phase = "Braking";
      calculateStoppingForce(decc);
      calculateTotalBrakeForce();
      decc = c * f_total / m_totalInertial;
      v += decc * dt;
      if (v <= 0)
        break;
    }

    time += dt; // Update waktu
    i++;

    // Simpan data ke file CSV
    outFile << phase << "," << i + 1 << "," << time << "," << v << ","
            << (isAccelerating || isCoasting ? acc : decc) << "," << f_motor
            << "," << (v < 1 ? f_resStart : f_resRunning) << "," << f_total
            << "\n";
    cout << "Phase: " << phase << endl;
    cout << "Speed : " << v << " km/h" << endl;
    cout << "Force motor : " << f_motor << " kN" << endl;
    cout << "Force total : " << f_total << " kN" << endl;
    if (isAccelerating) {
      cout << "Acceleration : " << acc << " km/h/s" << endl;
    } else {
      cout << "Deceleration : " << decc << " km/h/s" << endl;
    }
  }
  outFile.close();
}

void simulateTrainMovementLLL() {
  i = 0;
  bool isAccelerating = true;

  while (v >= 0) {
    cout << "\nIteration : " << i + 1 << endl;

    f_resStart = calculateStartRes();
    if (v > 0) {
      f_resRunning = calculateRunningRes(v);
    }
    if (isAccelerating) {
      if (v >= v_limit) {
        isAccelerating = false;
        continue;
      }
      calculatePoweringForce(acc_start);
      calculateTotalForce();
      acc = c * f_total / m_totalInertial;
      v += acc * dt;
    } else {
      // calculateStoppingForce();
      calculateTotalBrakeForce();
      decc = c * f_total / m_totalInertial;
      v += decc * dt;

      if (v <= 0)
        break; // Stop when velocity reaches 0
    }

    i++;

    cout << "Phase: " << (isAccelerating ? "Accelerating" : "Braking") << endl;
    cout << "Speed : " << v << " km/h" << endl;
    cout << "Force motor : " << f_motor << " kN" << endl;
    cout << "Force total : " << f_total << " kN" << endl;
    if (isAccelerating) {
      cout << "Acceleration : " << acc << " km/h/s" << endl;
    } else {
      cout << "Deceleration : " << decc << " km/h/s" << endl;
    }
  }
}

int main() {
  inputData();
  simulateTrainMovement(acc_start, decc_start);
  // calculateValues(acc_start);
  // calculateBrakingValue();
  return 0;
}
