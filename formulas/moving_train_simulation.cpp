// TODO : Separate functions into different classes

#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

const float g = 9.8;
const float cV = 3.6;
const double cF = 101.97162129779;
const float dt = 0.5;
const float v_diffCoast = 5.0;

float v_p1;
float v_p2;
float v_b1;
float v_b2;
float v_limit;

float m_Me;
float m_Te;
float m_M;
float m_T;
double m_totalEmpty;
double m_totalLoad;
double m_totalInertial;

float n_axle;
float n_M1;
float n_M2;
float n_Tc;
float n_T1;
float n_T2;
float n_T3;
float m_M1;
float m_M2;
float m_TC;
float m_T1;
float m_T2;
float m_T3;

float m_P;
float n_PM1;
float n_PM2;
float n_PTc;
float n_PT1;
float n_PT2;
float n_PT3;

float i_M;
float i_T;
float m_Mi;
float m_Ti;

float startRes;
float slope;
float radius;

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

double tm_f;
double tm_t;
double tm_rpm;
double tm_adh;
float tm_f_res;

float acc;
float decc;

// float n;
float load;
float numberOfCar;
int i = 0;
int tm_n = 24;
int n_ax = 4;
float gearRatio;
float wheel;

void initTrainMassData() {
  n_M1 = 3;
  n_M2 = 3;
  m_M1 = 20;
  m_M2 = 20;
  n_Tc = 2;
  m_TC = 10;
  n_T1 = 2;
  m_T1 = 10;
  n_T2 = 1;
  m_T2 = 10;
  n_T3 = 1;
  m_T3 = 10;
  n_PM1 = 200;
  n_PM2 = 200;
  n_PTc = 100;
  n_PT1 = 200;
  n_PT2 = 200;
  n_PT3 = 200;
  i_M = 1.2;
  i_T = 1.2;
  m_P = 70;
  numberOfCar = 12.0;
}

void initManualData() {
  std::cout << "===Calculate Mass of Empty Car===" << std::endl;
  std::cout << "Enter n_m1 and m_M1 : ";
  std::cin >> n_M1 >> m_M1;
  std::cout << "Enter n_m2 and m_M2 : ";
  std::cin >> n_M2 >> m_M2;
  std::cout << "Enter n_Tc and m_TC : ";
  std::cin >> n_Tc >> m_TC;
  std::cout << "Enter n_T1 and m_T1 : ";
  std::cin >> n_T1 >> m_T1;
  std::cout << "Enter n_T2 and m_T2 : ";
  std::cin >> n_T2 >> m_T2;
  std::cout << "Enter n_T3 and m_T3 : ";
  std::cin >> n_T3 >> m_T3;
  std::cout << "===Calculate Mass with Load===" << std::endl;
  std::cout << "Enter mass of passanger (kg) : ";
  std::cin >> m_P;
  std::cout << "Enter n_PM1 : ";
  std::cin >> n_PM1;
  std::cout << "Enter n_PM2 : ";
  std::cin >> n_PM2;
  std::cout << "Enter n_PTc : ";
  std::cin >> n_PTc;
  std::cout << "Enter n_PT1 : ";
  std::cin >> n_PT1;
  std::cout << "Enter n_PT2 : ";
  std::cin >> n_PT2;
  std::cout << "Enter n_PT3 : ";
  std::cin >> n_PT3;
  std::cout << "===Calculate Inertial Mass===" << std::endl;
  std::cout << "Enter inertial coeficient of motor car : ";
  std::cin >> i_M;
  std::cout << "Enter inertial coeficient of trailer car : ";
  std::cin >> i_T;
}

void countMassEmptyCar() {
  m_Me = (n_M1 * m_M1) + (n_M2 * m_M2);
  m_Te = (n_Tc * m_TC) + (n_T1 * m_T1) + (n_T2 * m_T2) + (n_T3 * m_T3);
  m_totalEmpty = m_Me + m_Te;
  std::cout << "Empty motor car mass : " << m_Me << std::endl;
  std::cout << "Empty trailer car mass : " << m_Te << std::endl;
  std::cout << "Total empty trainset mass : " << m_totalEmpty << std::endl;
}

void countMassWithLoad() {
  m_P = m_P / 1000;
  m_M = m_Me + n_M1 * (m_P * n_PM1) + n_M2 * (m_P * n_PM2);
  m_T = m_Te + n_Tc * (m_P * n_PTc) + n_T1 * (m_P * n_PT1) +
        n_T2 * (m_P * n_PT2) + n_T3 * (m_P * n_PT3);
  m_totalLoad = m_M + m_T;
  std::cout << "Mass of loaded motor car value : " << m_M << std::endl;
  std::cout << "Mass of loaded trailer car value : " << m_T << std::endl;
  std::cout << "Total loaded trainset mass : " << m_totalLoad << std::endl;
}

void countMassLoadInput() {
  std::cout << "===Mass With Load Input===" << std::endl;
  std::cout << "Enter number of car : ";
  std::cin >> numberOfCar;
  std::cout << "Enter load per car : ";
  std::cin >> load;
  m_totalLoad = m_totalEmpty + numberOfCar * load;
  std::cout << "Total loaded trainset mass (input) : " << m_totalLoad;
}

void countInertialMass() {
  m_Mi = (m_Me * i_M) + (n_M1 * (m_P * n_PM1) + n_M2 * (m_P * n_PM2));
  m_Ti = (m_Te * i_T) + (n_Tc * (m_P * n_PTc) + n_T1 * (m_P * n_PT1) +
                         n_T2 * (m_P * n_PT2) + n_T3 * (m_P * n_PT3));
  m_totalInertial = m_Mi + m_Ti;
  std::cout << "Total Inertial Trainset Mass: " << m_totalInertial << std::endl;
}

void countInertialMassInput() {
  std::cout << "===Inertial Mass Input===" << std::endl;
  std::cout << "Enter number of car : ";
  std::cin >> numberOfCar;
  std::cout << "Enter load value : ";
  std::cin >> load;
  m_totalLoad = m_totalEmpty + numberOfCar * load;
  m_totalInertial = (m_Me * i_M) + (m_Te * i_T) + (numberOfCar * load);
  std::cout << "Total inertial trainset mass (input) : " << m_totalInertial;
}

void initTrainMassDataCsvDatas() {
  std::ofstream outFile("mass_train_simulation.csv", std::ios::app);
  outFile << "nTc, nM1, nM2, nT1, nT2, nT3\n";
  outFile << n_Tc << "," << n_M1 << "," << n_M2 << "," << n_T1 << "," << n_T2
          << "," << n_T3 << "\n";
  outFile << "mTC, mT1, mT2, mT3, mMe, mTe\n";
  outFile << m_TC << "," << m_T1 << "," << m_T2 << "," << m_T3 << "," << m_Me
          << "," << m_Te << "\n";
  outFile << "nPM1, nPM2, nPTc, nPT1, nPT2, nPT3\n";
  outFile << n_PM1 << "," << n_PM2 << "," << n_PTc << "," << n_PT1 << ","
          << n_PT2 << "," << n_PT3 << "\n";
  outFile << "mP, mTe, mMe, MT, MM, mMi, mTi\n";
  outFile << m_P << "," << m_Te << "," << m_Me << "," << m_T << "," << m_M
          << "," << m_Mi << "," << m_Ti << "\n";
  outFile << "Total empty mass, Total load mass, Total inertial mass\n";
  outFile << m_totalEmpty << "," << m_totalLoad << "," << m_totalInertial
          << "\n";
  outFile.close();
}

void saveAdhesionCsv() {
  std::ofstream outFile("adhesion_train_simulation.csv", std::ios::app);
  outFile << "adhesion\n";
  outFile << tm_adh << "\n";
  outFile.close();
}

void countTrainMass() {
  string answer;
  cout << "Manual data input? (y/n)" << endl;
  cin >> answer;
  if (answer == "y") {
    initManualData();
  } else {
    initTrainMassData();
  }
  countMassEmptyCar();
  countMassWithLoad();
  countInertialMass();
  cout << "Manual load inputs? (y/n)" << endl;
  cin >> answer;
  if (answer == "y") {
    countMassLoadInput();
    countInertialMassInput();
  }
  // initTrainMassDataCsvDatas();
}

void initData() {
  initTrainMassData();
  countTrainMass();
  startRes = 39.2;
  radius = 2000;
  slope = 0;
  acc_start = 1.0;
  decc_start = 1.0;
  v_limit = 70.0;
  v = 0.0;
  v_p1 = 35;
  v_p2 = 65;
  v_b1 = 55;
  v_b2 = 70;
  gearRatio = 3.0;
  wheel = 500.0;

  cout << "Start resistance : " << startRes << endl;
  cout << "Radius : " << radius << endl;
  cout << "Slope : " << slope << endl;
  cout << "Number of Car : " << numberOfCar << endl;
  cout << "Acceleration : " << acc_start << endl;
  cout << "Initial speed : " << v << endl;
  cout << "Time difference : " << dt << endl;
  cout << endl;
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
  r_train = calculateResTrain(m_totalLoad, startRes);
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

void calculatePoweringForce(float acc, float v) {
  if (v <= 0) {
    f_start = m_totalInertial * (acc / cV) + f_resStart;
  }
  if (v <= v_p1) {
    f_motor = f_start;
  } else if (v > v_p1 && v <= v_p2) {
    f_motor = (f_start * v_p1) / v;
  } else if (v > v_p2) {
    f_motor = (f_start * v_p1 * v_p2) / (v * v);
  }
}

void calculateStoppingForce(float decc) {
  f_brake = m_totalInertial * (decc_start / cV);
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

void calculateTotalForce(float v) {
  f_total = f_motor - (v <= 0 ? f_resStart : f_resRunning);
}

void calculateTotalBrakeForce() { f_total = f_motor; }

void calculateBrakingValue() {
  i = 0;
  while (v > 0) {
    f_resStart = calculateStartRes();
    f_resRunning = calculateRunningRes(v);
    // calculateStoppingForce();
    calculateTotalBrakeForce();
    decc = cV * f_total / m_totalInertial;
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

double calculateTractionForce(double f_motor) { return (f_motor / tm_n); }

double calculateTorque(double f_motor) {
  return ((f_motor * (wheel / (2 * 1000))) / gearRatio);
}

double calculateRpm(double v) {
  return ((v * 1000 / 60) * gearRatio) / ((M_PI * wheel) / 1000);
}

double calculateAdhesion() {
  return ((tm_f * cF) / (((m_M1 + m_M2) * 1000) / n_ax));
}

double calculateResForcePerMotorCar(double f_res) {
  return (f_res / (n_M1 + n_M2));
}

void simulateDynamicTrainMovement(float acc, float decc, ofstream &outFile) {
  i = 0;
  bool isAccelerating = true;
  bool isCoasting = false;
  float time = 0;
  string phase;
  int coastingCount = 0;
  while (v >= 0) {
    f_resStart = calculateStartRes();
    // if (v > 0) {
    f_resRunning = calculateRunningRes(v);
    // }
    tm_f_res = calculateResForcePerMotorCar(v > 0 ? f_resRunning : f_resStart);
    if (isAccelerating) {
      if (v >= v_limit) {
        isAccelerating = false;
        isCoasting = true;
        phase = "Coasting";
        continue;
      }
      phase = "Accelerating";
      calculatePoweringForce(acc, v);
      calculateTotalForce(v);
      acc = cV * f_total / m_totalInertial;
      v += acc * dt;
    } else if (isCoasting) {
      if (v <= (v_limit - v_diffCoast)) {
        isCoasting = false;
        isAccelerating = true;
        coastingCount++;
        if (coastingCount >= 3) {
          isAccelerating = false;
          isCoasting = false;
        }
        continue;
      }
      phase = "Coasting";
      f_motor = 0;
      f_total = -f_resRunning;
      acc = cV * f_total / m_totalInertial;
      v += acc * dt;
    } else {
      phase = "Braking";
      calculateStoppingForce(decc);
      calculateTotalBrakeForce();
      decc = cV * f_total / m_totalInertial;
      v += decc * dt;
      if (v <= 0)
        break;
    }
    if (i == 0) {
      outFile << "Starting" << "," << i << "," << time << "," << 0 << "," << acc
              << "," << f_motor << "," << f_resStart << "," << f_total << "\n";
    }

    time += dt;

    // Simpan data ke file CSV
    outFile << phase << "," << i + 1 << "," << time << "," << v << ","
            << (isAccelerating || isCoasting ? acc : decc) << "," << f_motor
            << "," << (v > 0 ? f_resRunning : f_resStart) << "," << f_total
            << "\n";

    // cout << "\nIteration : " << i + 1 << endl;
    // cout << "Phase: " << phase << endl;
    // cout << "Speed : " << v << " km/h" << endl;
    // cout << "Force motor : " << f_motor << " kN" << endl;
    // cout << "Force total : " << f_total << " kN" << endl;
    // if (isAccelerating) {
    //   cout << "Acceleration : " << acc << " km/h/s" << endl;
    // } else {
    //   cout << "Deceleration : " << decc << " km/h/s" << endl;
    // }
    i++;
  }
  outFile << "Stopped" << "," << i + 1 << "," << time << "," << 0 << "," << 0
          << "," << f_motor << "," << (v > 0 ? f_resRunning : f_resStart) << ","
          << f_total << "\n";
}

void simulateStaticTrainMovement(float acc, float decc, ofstream &outFile) {
  v_limit = 130;
  i = 0;
  bool isAccelerating = true;
  bool isCoasting = false;
  float time = 0;
  string phase;
  int coastingCount = 0;
  phase = "Accelerating";
  while (v <= v_limit) {
    f_resStart = calculateStartRes();
    f_resRunning = calculateRunningRes(v);
    calculatePoweringForce(acc, v);
    calculateTotalForce(v);
    tm_f_res = calculateResForcePerMotorCar(v > 0 ? f_resRunning : f_resStart);
    tm_f = calculateTractionForce(f_motor);
    tm_t = calculateTorque(f_motor);
    acc = cV * f_total / m_totalInertial;
    v++;
    tm_rpm = calculateRpm(v);
    if (i == 0) {
      tm_adh = calculateAdhesion();
      outFile << "Starting" << "," << i << "," << time << "," << 0 << "," << acc
              << "," << f_motor << "," << f_resStart << "," << f_total << ","
              << tm_f_res << "," << tm_f << "," << tm_t << "," << 0 << ","
              << "\n";
    }
    time += dt;
    outFile << phase << "," << i + 1 << "," << time << "," << v << "," << acc
            << "," << f_motor << "," << (v > 0 ? f_resRunning : f_resStart)
            << "," << f_total << "," << tm_f_res << "," << tm_f << "," << tm_t
            << "," << tm_rpm << "," << "\n";
    i++;
  }
  saveAdhesionCsv();
}

void simulate(float acc, float decc) {
  string option;
  string filename;
  cout << "Which simulation? static or dynamic (s/d)" << endl;
  cin >> option;
  option == "d" ? filename = "dynamic_train_simulation.csv"
                : filename = "static_train_simulation.csv";
  std::ofstream outFile(filename, std::ios::app);
  if (i == 0) {
    outFile << "Phase,Iteration,Time,Speed,Acceleration,F motor,F res,F "
               "total,F motor/TM,F res/TM,Torque,RPM \n";
  }
  option == "d" ? simulateDynamicTrainMovement(acc, decc, outFile)
                : simulateStaticTrainMovement(acc, decc, outFile);

  outFile.close();
}

int main() {
  initData();
  simulate(acc_start, decc_start);
  cout << "Simulation complete!";
  return 0;
}
