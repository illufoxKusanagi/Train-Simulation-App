#include <iostream>

using namespace std;

string answer;

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

float n;
float load;

float i_M;
float i_T;
float m_Mi;
float m_Ti;

void storeMass() {
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
  n_PM1 = 20;
  n_PM2 = 20;
  n_PTc = 10;
  n_PT1 = 10;
  n_PT2 = 10;
  n_PT3 = 10;
  i_M = 1.2;
  i_T = 1.2;
  m_P = 70;
}

void inputDatas() {
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
  // storeMass();
  m_Me = (n_M1 * m_M1) + (n_M2 * m_M2);
  m_Te = (n_Tc * m_TC) + (n_T1 * m_T1) + (n_T2 * m_T2) + (n_T3 * m_T3);
  m_totalEmpty = m_Me + m_Te;
  std::cout << "Empty motor car mass : " << m_Me << std::endl;
  std::cout << "Empty trailer car mass : " << m_Te << std::endl;
  std::cout << "Total empty trainset mass : " << m_totalEmpty << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
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
  std::cout << std::endl;
  std::cout << std::endl;
}

void countMassLoadInput() {
  std::cout << "===Mass With Load Input===" << std::endl;
  std::cout << "Enter number of car : ";
  std::cin >> n;
  std::cout << "Enter load per car : ";
  std::cin >> load;
  m_totalLoad = m_totalEmpty + n * load;
  std::cout << "Total loaded trainset mass (input) : " << m_totalLoad;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countInertialMass() {

  m_Mi = (m_Me * i_M) + (n_M1 * (m_P * n_PM1) + n_M2 * (m_P * n_PM2));
  m_Ti = (m_Te * i_T) + (n_Tc * (m_P * n_PTc) + n_T1 * (m_P * n_PT1) +
                         n_T2 * (m_P * n_PT2) + n_T3 * (m_P * n_PT3));
  m_totalInertial = m_Mi + m_Ti;
  std::cout << "Total Inertial Trainset Mass: " << m_totalInertial << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countInertialMassInput() {
  std::cout << "===Inertial Mass Input===" << std::endl;
  std::cout << "Enter number of car : ";
  std::cin >> n;
  std::cout << "Enter load value : ";
  std::cin >> load;
  m_totalLoad = m_totalEmpty + n * load;
  m_totalInertial = (m_Me * i_M) + (m_Te * i_T) + (n * load);
  std::cout << "Total inertial trainset mass (input) : " << m_totalInertial;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countTrainMass() {
  cout << "Manual data input? (y/n)" << endl;
  cin >> answer;
  if (answer == "y") {
    inputDatas();
  } else {
    storeMass();
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
}

int main() {
  countTrainMass();
  return 0;
}
