#include <iostream>

double f_start;
double f_brake;
double f_motor;
// double m_totalInertial = 100;
double acc; // in KM/h/s
double dec; // in KM/h/s
double f_res;
float v_p1;
float v_p2;
float v_b1;
float v_b2;
int notch;
float f_total;
const float g = 9.8;
float m_totalInertial;
float startRes;
float slope;
float radius;
double v = 0.0;
float m_M = 10;
float m_T = 10;
int numberOfCar = 5;
float dt = 0.5;
double r_train;
double r_slope;
double r_radius;
double r_start;
double r_run;
float c = 3.6;
int i = 0;

void collectData() {
  std::cout << "===Calculate Start Resistance===" << std::endl;
  std::cout << "Enter train m : ";
  // std::cin >> m;
  m_totalInertial = 100;
  std::cout << m_totalInertial << std::endl;
  std::cout << "Enter start resistance : ";
  // std::cin >> startRes;
  startRes = 10;
  std::cout << startRes << std::endl;
  std::cout << "Enter radius : ";
  // std::cin >> radius;
  radius = 10;
  std::cout << radius << std::endl;
  std::cout << "Enter slope : ";
  // std::cin >> slope;
  slope = 10;
  std::cout << slope << std::endl;
  std::cout << "Enter number of Car : ";
  // std::cin >> numberOfCar;
  numberOfCar = 12;
  std::cout << numberOfCar << std::endl;
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
  // collectData();
  r_train = calculateResTrain(m_totalInertial, startRes);
  r_slope = calculateResSlope(m_totalInertial, slope);
  r_radius = calculateResRadius(m_totalInertial, radius);
  // printData();
  return (r_train + r_slope + r_radius);
}

float calculateRunningRes(float v) {
  // collectData();
  r_train = calculateResTrain(m_totalInertial, startRes);
  r_slope = calculateResSlope(m_totalInertial, slope);
  r_radius = calculateResRadius(m_totalInertial, radius);
  // printData();
  return (1 / 1000 *
              (((1.65 + (0.0247 * v)) * (m_M * g)) +
               ((0.78 + (0.0028 * v) * (m_T * g)) +
                (g * (0.028 + 0.0078 * (numberOfCar - 1)) * (v * v)))) +
          r_slope + r_radius);
}

void mainResistance() {
  std::cout << "Enter v : ";
  std::cin >> v;
  f_res = v > 0 ? calculateRunningRes(v) : calculateStartRes();
  std::cout << "Resistance Force : " << f_res << " m/s^2";
}

void calculatePoweringForce() {
  // std::cout << "Enter weakening point 1 : ";
  // std::cin >> v_p1;
  // std::cout << "Enter weakening point 2 : ";
  // std::cin >> v_p2;
  if (acc == 0)
    acc = 1.0;
  f_start = m_totalInertial * acc / c + f_res;
  if (v <= v_p1) {
    f_motor = f_start;
    std::cout << "uses first condition" << std::endl;
  } else if (v > v_p1 && v <= v_p2) {
    f_motor = (f_start * v_p1) / v;
    std::cout << "uses second condition" << std::endl;
  } else if (v > v_p2) {
    f_motor = (f_start * v_p1 * v_p2) / (v * v);
    std::cout << "uses third condition" << std::endl;
  } else {
    std::cout << "Invalid input" << std::endl;
  }

  std::cout << "Starting force : " << f_start << std::endl;
  std::cout << "Powering force : " << f_motor << std::endl;
}
// Update calculatePoweringForce function
void calculatePoweringForceLL() {
  if (acc == 0)
    acc = 1.0; // Initial acceleration if not set
  f_start = m_totalInertial * acc / c + f_res;

  // Debug output
  std::cout << "Debug - acc: " << acc
            << ", m_totalInertial: " << m_totalInertial << ", f_res: " << f_res
            << std::endl;

  if (v <= v_p1) {
    f_motor = f_start;
  } else if (v > v_p1 && v <= v_p2) {
    f_motor = (f_start * v_p1) / v;
  } else if (v > v_p2) {
    f_motor = (f_start * v_p1 * v_p2) / (v * v);
  }
  std::cout << "Starting force: " << f_start << " kN" << std::endl;
  std::cout << "Motor force: " << f_motor << " kN" << std::endl;
}

void calculateBrakingForce() {
  // std::cout << "Enter braking point 1 : ";
  // std::cin >> v_b1;
  // std::cout << "Enter braking point 2 : ";
  // std::cin >> v_b2;
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

void calculateTotalForce() {
  f_total = f_motor - f_res;
  std::cout << "Total force : " << f_total << std::endl;
}
void dummy() {
  collectData();
  while (v <= 10) {
    std::cout << "\nIteration : " << i + 1 << std::endl;

    // Calculate resistance
    f_res = (v == 0) ? calculateStartRes() : calculateRunningRes(v);
    std::cout << "Resistance Force : " << f_res << " kN" << std::endl;

    // Calculate forces
    calculatePoweringForce();
    calculateTotalForce();

    // Update motion
    acc = c * f_total / m_totalInertial;
    std::cout << "Acceleration : " << acc << " m/s²" << std::endl;

    v += acc * dt;
    std::cout << "Speed : " << v << " m/s" << std::endl;

    i++;
    if (i > 100)
      break; // Safety limit
  }
}

void dummyLol() {
  std::cout << "Iteration : " << i + 1 << std::endl;
  collectData();
  if (v <= 10) {
    if (v == 0) {
      f_res = calculateStartRes();
    } else {
      f_res = calculateRunningRes(v);
    }
    std::cout << "Resistance Force : " << f_res << " m/s^2" << std::endl;
    calculatePoweringForce();
    calculateTotalForce();
    // calculateAcceleration();
    acc = c * f_total / m_totalInertial;
    std::cout << "Acceleration : " << acc << std::endl;
    v += acc * dt;
    std::cout << "Speed : " << v << std::endl;
    i++;
    if (i < 1) {
      printData();
    }
    dummy();
    // } else if (v > 10 && v <= 20) {
    //   calculateBrakingForce();
    //   calculateTotalForce();
    //   calculateAcceleration();
    // } else if (v > 20) {
    //   calculateCoastingForce();
    //   calculateTotalForce();
    //   calculateAcceleration();
  }
  // cari start resistance (saat berhenti) ketemu r_start
  // cari f_start = (m_totalInertial * acc = 0) + f_res
  // ketemu f_start = f_res
  // constant torque
  // f_motor = f_start
  // cari f_total = f_motor - f_res
  // ketemu acc = c * f_total / m_totalInertial
  // cari v -> v += acc * dt
  // balik ke f_motor buat cari percepatan
  // f_motor ketemu, cari f_total
  // acc = c * f_total / m_totalInertial
  // dapet speed baru
}

void simulateMovingTrain() {
  // Initialize simulation parameters
  v_p1 = 10.0; // Example value if not input
  v_p2 = 20.0; // Example value if not input
  acc = 1.0;   // Initial acceleration
  v = 0.0;     // Initial speed
  i = 0;       // Reset iteration counter

  std::cout << "Enter weakening point 1 (default 10.0): ";
  std::cin >> v_p1;
  std::cout << "Enter weakening point 2 (default 20.0): ";
  std::cin >> v_p2;
  dummy();
}

void simulateMovingTrainLL() {
  std::cout << "Enter weakening point 1 : ";
  std::cin >> v_p1;
  std::cout << "Enter weakening point 2 : ";
  std::cin >> v_p2;
  dummy();
}

void calculateCoastingForce() {
  std::cout << "Enter notch (-1 to 1) : ";
  std::cin >> notch;
  //   f_motor = 0;
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

void calculatev() {
  v += acc * dt; // Update kecepatan (GLBB: v = v0 + at)
}

// void calculateAcceleration() { acc = c * f_total / m_totalInertial; }

void mainTractionEffort() {
  std::cout << "Enter acceleration : ";
  std::cin >> acc;
  std::cout << "Enter deceleration : ";
  std::cin >> dec;
  std::cout << "Enter v : ";
  std::cin >> v;
  calculatePoweringForce();
  calculateBrakingForce();
  calculateCoastingForce();
}

int main() {
  simulateMovingTrain();
  // f_res = calculateStartRes();
  // calculatePoweringForce();
  return 0;
}