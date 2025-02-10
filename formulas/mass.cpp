#include <iostream>

int m_me;
int m_te;
int m_m;
int m_t;
int m_totalEmpty;
int m_totalWithLoad;

int n_m1;
int m_m1;
int n_m2;
int m_m2;
int n_tc;
int m_tc;
int n_t1;
int m_t1;
int n_t2;
int m_t2;
int n_t3;
int m_t3;

int m_p;
int n_pm1;
int n_pm2;

int n_p_tc;
int n_p_t1;
int n_p_t2;
int n_p_t3;

int n;
int load;

int m_mi;
int i_cm;
int i_ct;
int m_ti;
int m_totalInertial;

void countMassEmptyCar() {
  std::cout << "===Calculate Mass of Empty Car===" << std::endl;
  std::cout << "Enter number and mass of motor car type 1: ";
  std::cin >> n_m1 >> m_m1;
  std::cout << "Enter number and mass of motor car type 2: ";
  std::cin >> n_m2 >> m_m2;
  std::cout << "Enter number and mass of trailer car: ";
  std::cin >> n_tc >> m_tc;
  std::cout << "Enter number and mass of trailer car type 1: ";
  std::cin >> n_t1 >> m_t1;
  std::cout << "Enter number and mass of trailer car type 2: ";
  std::cin >> n_t2 >> m_t2;
  std::cout << "Enter number and mass of trailer car type 3: ";
  std::cin >> n_t3 >> m_t3;

  m_me = (n_m1 * m_m1) + (n_m2 + m_m2);
  m_te = (n_tc * m_tc) + (n_t1 * m_t1) + (n_t2 * m_t2) + (n_t3 * m_t3);
  m_totalEmpty = m_me + m_te;
  std::cout << "Empty motor car mass : " << m_me << std::endl;
  std::cout << "Empty trailer car mass : " << m_te << std::endl;
  std::cout << "Total empty trainset mass : " << m_totalEmpty << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countMassWithLoad() {
  std::cout << "===Calculate Mass with Load===" << std::endl;
  std::cout << "Enter mass of passanger : ";
  std::cin >> m_p;
  std::cout << "Enter first number Mass of passenger : ";
  std::cin >> n_pm1;
  std::cout << "Enter second number Mass of passenger : ";
  std::cin >> n_pm2;
  std::cout << "Enter number of passenger for trailer car : ";
  std::cin >> n_p_tc;
  std::cout << "Enter number of passenger for first type of trailer car : ";
  std::cin >> n_p_t1;
  std::cout << "Enter number of passenger for second type of trailer car : ";
  std::cin >> n_p_t2;
  std::cout << "Enter number of passenger for third type of trailer car : ";
  std::cin >> n_p_t3;

  m_m = m_me + n_m1 * (m_p * n_pm1) + n_m2 * (m_p * n_pm2);
  m_t = m_te + n_tc * (m_p * n_p_tc) + n_t1 * (m_p * n_p_t1) +
        n_t2 * (m_p * n_p_t2) + n_t3 * (m_p * n_p_t3);
  m_totalWithLoad = m_m + m_t;
  std::cout << "Mass of loaded motor car value : " << m_m << std::endl;
  std::cout << "Mass of loaded trailer car value : " << m_t << std::endl;
  std::cout << "Total loaded trainset mass : " << m_totalWithLoad << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countMassLoadInput() {
  std::cout << "===Mass With Load Input===" << std::endl;
  std::cout << "Enter number of load : ";
  std::cin >> n;
  std::cout << "Enter load value : ";
  std::cin >> load;
  m_totalWithLoad = m_totalEmpty + n * load;
  std::cout << "Total loaded trainset mass (input) : " << m_totalWithLoad;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countIntertialMass() {
  std::cout << "===Calculate Inertial Mass Input===" << std::endl;
  std::cout << "Enter inertial motor car mass : ";
  std::cin >> i_cm;
  std::cout << "Enter inertial trailer car mass : ";
  std::cin >> i_ct;
  m_mi = (m_me * i_cm) + (n_m1 * (m_p * n_pm1) + n_m2 * (m_p * n_pm2));
  m_ti = (m_te * i_ct) + (n_tc * (m_p * n_p_tc) + n_t1 * (m_p * n_p_t1) +
                          n_t2 * (m_p * n_p_t2) + n_t3 * (m_p * n_p_t3));
  m_totalInertial = m_mi + m_ti;
  std::cout << "Total Inertial Trainset Mass: " << m_totalInertial << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countInertialMassInput() {
  std::cout << "===Inertial Mass Input===" << std::endl;
  std::cout << "Enter number of load : ";
  std::cin >> n;
  std::cout << "Enter load value : ";
  std::cin >> load;
  m_totalWithLoad = m_totalEmpty + n * load;
  m_totalInertial = (m_me * i_cm) + (m_te * i_ct) + (n * load);
  std::cout << "Total intertial trainset mass (input) : " << m_totalInertial;
}

int main() {
  countMassEmptyCar();
  countMassWithLoad();
  countMassLoadInput();
  countIntertialMass();
  countInertialMassInput();
  return 0;
}
