#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <Servo.h>
#include <AFMotor.h>

// Моторы (Motor Shield)
AF_DCMotor leftMotor(1);   // Порт 1
AF_DCMotor rightMotor(2);  // Порт 2

// Датчик цвета
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X);


// Ультразвуковой дальномер
const int trigPin = 7;
const int echoPin = 8;

// Сервоприводы манипулятора
Servo gripServo;    // Захват (открытие/закрытие)


// Пороги для цветов 
const int RED_THRESH = 100;
const int GREEN_THRESH = 100;
const int BLUE_THRESH = 100;

const int WHITE_THRESH = 500;


void setup() {
  Serial.begin(9600);

  // Инициализация моторов
  leftMotor.setSpeed(200);
  rightMotor.setSpeed(200);


  // Датчик цвета
  if (!tcs.begin()) {
    Serial.println("Ошибка: датчик цвета не найден!");
    while (1);
  }

  // Сервоприводы
  gripServo.attach(9);
  liftServo.attach(10);
  rotateServo.attach(11);


  // Начальные позиции сервоприводов
  gripServo.write(90);  // Открыто

  // Пины дальномера
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // 1. Поиск объекта
  float distance = getDistance();
  if (distance < 20) { // Объект близко (20 см)
    stopMotors();

    // 2. Определение цвета
    uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);

    String color = detectColor(r, g, b);
    Serial.print("Цвет: ");
    Serial.println(color);

    delay(500);


    // 3. Захват объекта
    if (color != "unknown") {
      gripObject();
      delay(1000);


      // 4. Движение к люку нужного цвета
      moveToColorHatch(color);


      // 5. Сброс объекта
      releaseObject();
      delay(1000);
    }
  } else {
    // Продолжаем поиск (движение вперёд/разворот)
    driveForward();
  }
}

// Получение расстояния от HC‑SR04
float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2; // см
}

// Остановка моторов
void stopMotors() {
  leftMotor.run(RELEASE);
  rightMotor.run(RELEASE);
}

// Движение вперёд
void driveForward() {
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
}

// Определение цвета по данным TCS34725
String detectColor(uint16_t r, uint16_t g, uint16_t b) {
  if (r > RED_THRESH && g < GREEN_THRESH && b < BLUE_THRESH) return "red";
  if (g > GREEN_THRESH && r < RED_THRESH && b < BLUE_THRESH) return "green";
  if (b > BLUE_THRESH && r < RED_THRESH && g < GREEN_THRESH) return "blue";
  if (r > WHITE_THRESH && g > WHITE_THRESH && b > WHITE_THRESH) return "white";
  return "unknown";
}

// Захват объекта 
void gripObject() {
  gripServo.write(0);    // Закрыть захват
}

// Сброс объекта (открыть захват)
void releaseObject() {
  gripServo.write(90);    // Открыть захват
}

// Движение к люку заданного цвета (упрощённо)
void moveToColorHatch(String color) {
  // Здесь должна быть логика навигации:
  // - поворот к нужному люку;
  // - движение по линии/меткам;
  // - распознавание периметра люка.
  // Для прототипа: просто пауза и имитация движения.
  Serial.print("Едем к люку цвета: ");
  Serial.println(color);
  delay(2000);
}
