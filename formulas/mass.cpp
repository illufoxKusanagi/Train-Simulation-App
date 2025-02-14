#include <iostream>

int m_motorEmpty;
int m_trailerEmpty;
int m_motorLoaded;
int m_trailerLoaded;
int m_totalEmpty;
int m_totalWithLoad;

int n_motor1;
int n_motor2;
int n_trailerCabin;
int n_trailer1;
int n_trailer2;
int n_trailer3;
int m_motor1;
int m_motor2;
int m_trailerCabin;
int m_trailer1;
int m_trailer2;
int m_trailer3;

int m_passenger;
int n_passengerMotorCar1;
int n_passengerMotorCar2;

int n_passengerTrailerCabin;
int n_passengerTrailerCar1;
int n_passengerTrailerCar2;
int n_passengerTrailerCar3;

int numberOfCar;
int loadPerCar;

int i_coeficientMotor;
int i_coeficientTrailer;
int m_inertialMotor;
int m_inertialTrailer;
int m_totalInertial;

void countMassEmptyCar() {
  std::cout << "===Calculate Mass of Empty Car===" << std::endl;
  std::cout << "Enter number and mass of motor car type 1: ";
  std::cin >> n_motor1 >> m_motor1;
  std::cout << "Enter number and mass of motor car type 2: ";
  std::cin >> n_motor2 >> m_motor2;
  std::cout << "Enter number and mass of trailer car: ";
  std::cin >> n_trailerCabin >> m_trailerCabin;
  std::cout << "Enter number and mass of trailer car type 1: ";
  std::cin >> n_trailer1 >> m_trailer1;
  std::cout << "Enter number and mass of trailer car type 2: ";
  std::cin >> n_trailer2 >> m_trailer2;
  std::cout << "Enter number and mass of trailer car type 3: ";
  std::cin >> n_trailer3 >> m_trailer3;

  m_motorEmpty = (n_motor1 * m_motor1) + (n_motor2 + m_motor2);
  m_trailerEmpty = (n_trailerCabin * m_trailerCabin) +
                   (n_trailer1 * m_trailer1) + (n_trailer2 * m_trailer2) +
                   (n_trailer3 * m_trailer3);
  m_totalEmpty = m_motorEmpty + m_trailerEmpty;
  std::cout << "Empty motor car mass : " << m_motorEmpty << std::endl;
  std::cout << "Empty trailer car mass : " << m_trailerEmpty << std::endl;
  std::cout << "Total empty trainset mass : " << m_totalEmpty << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countMassWithLoad() {
  std::cout << "===Calculate Mass with Load===" << std::endl;
  std::cout << "Enter mass of passanger : ";
  std::cin >> m_passenger;
  std::cout << "Enter first number Mass of passenger : ";
  std::cin >> n_passengerMotorCar1;
  std::cout << "Enter second number Mass of passenger : ";
  std::cin >> n_passengerMotorCar2;
  std::cout << "Enter number of passenger for trailer car : ";
  std::cin >> n_passengerTrailerCabin;
  std::cout << "Enter number of passenger for first type of trailer car : ";
  std::cin >> n_passengerTrailerCar1;
  std::cout << "Enter number of passenger for second type of trailer car : ";
  std::cin >> n_passengerTrailerCar2;
  std::cout << "Enter number of passenger for third type of trailer car : ";
  std::cin >> n_passengerTrailerCar3;

  m_motorLoaded = m_motorEmpty +
                  n_motor1 * (m_passenger * n_passengerMotorCar1) +
                  n_motor2 * (m_passenger * n_passengerMotorCar2);
  m_trailerLoaded = m_trailerEmpty +
                    n_trailerCabin * (m_passenger * n_passengerTrailerCabin) +
                    n_trailer1 * (m_passenger * n_passengerTrailerCar1) +
                    n_trailer2 * (m_passenger * n_passengerTrailerCar2) +
                    n_trailer3 * (m_passenger * n_passengerTrailerCar3);
  m_totalWithLoad = m_motorLoaded + m_trailerLoaded;
  std::cout << "Mass of loaded motor car value : " << m_motorLoaded
            << std::endl;
  std::cout << "Mass of loaded trailer car value : " << m_trailerLoaded
            << std::endl;
  std::cout << "Total loaded trainset mass : " << m_totalWithLoad << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countMassLoadInput() {
  std::cout << "===Mass With Load Input===" << std::endl;
  std::cout << "Enter number of car : ";
  std::cin >> numberOfCar;
  std::cout << "Enter load per car : ";
  std::cin >> loadPerCar;
  m_totalWithLoad = m_totalEmpty + numberOfCar * loadPerCar;
  std::cout << "Total loaded trainset mass (input) : " << m_totalWithLoad;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countIntertialMass() {
  std::cout << "===Calculate Inertial Mass===" << std::endl;
  std::cout << "Enter inertial motor car mass : ";
  std::cin >> i_coeficientMotor;
  std::cout << "Enter inertial trailer car mass : ";
  std::cin >> i_coeficientTrailer;
  ;
  m_inertialMotor = (m_motorEmpty * i_coeficientMotor) +
                    (n_motor1 * (m_passenger * n_passengerMotorCar1) +
                     n_motor2 * (m_passenger * n_passengerMotorCar2));
  m_inertialTrailer =
      (m_trailerEmpty * i_coeficientTrailer) +
      (n_trailerCabin * (m_passenger * n_passengerTrailerCabin) +
       n_trailer1 * (m_passenger * n_passengerTrailerCar1) +
       n_trailer2 * (m_passenger * n_passengerTrailerCar2) +
       n_trailer3 * (m_passenger * n_passengerTrailerCar3));
  m_totalInertial = m_inertialMotor + m_inertialTrailer;
  std::cout << "Total Inertial Trainset Mass: " << m_totalInertial << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
}

void countInertialMassInput() {
  std::cout << "===Inertial Mass Input===" << std::endl;
  std::cout << "Enter number of load : ";
  std::cin >> numberOfCar;
  std::cout << "Enter load value : ";
  std::cin >> loadPerCar;
  m_totalWithLoad = m_totalEmpty + numberOfCar * loadPerCar;
  m_totalInertial = (m_motorEmpty * i_coeficientMotor) +
                    (m_trailerEmpty * i_coeficientTrailer) +
                    (numberOfCar * loadPerCar);
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
