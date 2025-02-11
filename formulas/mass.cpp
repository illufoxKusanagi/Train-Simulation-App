#include <iostream>

float m_Me;
float m_Te;
float m_M;
float m_T;
double m_totalEmpty;
double m_totalLoad;
double m_totalInertial;

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

float N;
float load;

float i_Cm;
float i_Ct;
float m_Mi;
float m_Ti;

void countMassEmptyCar() {
  std::cout << "===Calculate Mass of Empty Car===" << std::endl;
  std::cout << "Enter number and mass of motor car type 1: ";
  std::cin >> n_M1 >> m_M1;
  std::cout << "Enter number and mass of motor car type 2: ";
  std::cin >> n_M2 >> m_M2;
  std::cout << "Enter number and mass of trailer car: ";
  std::cin >> n_Tc >> m_TC;
  std::cout << "Enter number and mass of trailer car type 1: ";
  std::cin >> n_T1 >> m_T1;
  std::cout << "Enter number and mass of trailer car type 2: ";
  std::cin >> n_T2 >> m_T2;
  std::cout << "Enter number and mass of trailer car type 3: ";
  std::cin >> n_T3 >> m_T3;

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
  std::cout << "===Calculate Mass with Load===" << std::endl;
  std::cout << "Enter mass of passanger (kg) : ";
  std::cin >> m_P;
  std::cout << "Enter first number Mass of passenger : ";
  std::cin >> n_PM1;
  std::cout << "Enter second number Mass of passenger : ";
  std::cin >> n_PM2;
  std::cout << "Enter number of passenger for trailer car : ";
  std::cin >> n_PTc;
  std::cout << "Enter number of passenger for first type of trailer car : ";
  std::cin >> n_PT1;
  std::cout << "Enter number of passenger for second type of trailer car : ";
  std::cin >> n_PT2;
  std::cout << "Enter number of passenger for third type of trailer car : ";
  std::cin >> n_PT3;
  m_P = m_P / 1000;
  std::cout << "Mass of passenger : " << m_P << std::endl;

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
  std::cin >> N;
  std::cout << "Enter load per car : ";
  std::cin >> load;
  m_totalLoad = m_totalEmpty + N * load;
  std::cout << "Total loaded trainset mass (input) : " << m_totalLoad;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countInertialMass() {
  std::cout << "===Calculate Inertial Mass===" << std::endl;
  std::cout << "Enter inertial motor car mass : ";
  std::cin >> i_Cm;
  std::cout << "Enter inertial trailer car mass : ";
  std::cin >> i_Ct;
  ;
  m_Mi = (m_Me * i_Cm) + (n_M1 * (m_P * n_PM1) + n_M2 * (m_P * n_PM2));
  m_Ti = (m_Te * i_Ct) + (n_Tc * (m_P * n_PTc) + n_T1 * (m_P * n_PT1) +
                          n_T2 * (m_P * n_PT2) + n_T3 * (m_P * n_PT3));
  m_totalInertial = m_Mi + m_Ti;
  std::cout << "Total Inertial Trainset Mass: " << m_totalInertial << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countInertialMassInput() {
  std::cout << "===Inertial Mass Input===" << std::endl;
  std::cout << "Enter number of load : ";
  std::cin >> N;
  std::cout << "Enter load value : ";
  std::cin >> load;
  m_totalLoad = m_totalEmpty + N * load;
  m_totalInertial = (m_Me * i_Cm) + (m_Te * i_Ct) + (N * load);
  std::cout << "Total floatertial trainset mass (input) : " << m_totalInertial;
}

int main() {
  countMassEmptyCar();
  countMassWithLoad();
  countMassLoadInput();
  countInertialMass();
  countInertialMassInput();
  return 0;
}
