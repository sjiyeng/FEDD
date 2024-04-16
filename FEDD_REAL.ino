#include <Servo.h>
#include <ArduinoBLE.h>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

bool bluetoothSwitch = false; // Boolean variable to be changed over Bluetooth

//define servos
Servo lockServo;
Servo doorMotor;

//constants
const int lockServoPin = A2;
const int doorMotorPin = A1;
const int upperSwitchPin = 8;
const int lowerSwitchPin = 9;
const int beamBreakPin = 10;

// booleans
bool isOpening = 0;
bool isOpen = 0;
bool isClosing = 0;
bool isClosed = 1;

//variables
int upperSwitchValue = 0;
int lowerSwitchValue = 0;
int beamBreakValue = 0;
int i = 0;



void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);


  // begin initialization
  if (!BLE.begin()) {
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("LED");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characteristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  pinMode(upperSwitchPin,INPUT_PULLUP);
  pinMode(lowerSwitchPin,INPUT_PULLUP);
  pinMode(beamBreakPin,INPUT_PULLUP);

  //attach motors and calibrate
  doorMotor.attach(doorMotorPin);
  doorMotor.writeMicroseconds(1500);
  Serial.println("Starting Calibration");
  for (int i = 5; i > 0; i--) {
    Serial.println(i);
    delay(1000);
  }
  Serial.println("Controller Calibrated");

}

void loop() {

  // Listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // If a central is connected to peripheral:
  if (central) {

    // While the central is still connected to peripheral:
    if (switchCharacteristic.written()) {
      // If characteristic has been written, update OOGA based on the received value
      bluetoothSwitch = switchCharacteristic.value();
      i = (i + 1);
      }
  }

if(bluetoothSwitch == 1 && i % 2 == 0){
  openDoor();
  bluetoothSwitch = 0;
}

else if(bluetoothSwitch == 1 && i % 2 != 0){
  closeDoor();
  bluetoothSwitch = 0;
}




}


void lock()
{
  Serial.println("Locking!");
  lockServo.attach(lockServoPin);
  lockServo.write(0);
  delay(500);
  lockServo.detach();
  Serial.println("Locked!");
}

void unlock()
{
  Serial.println("Unlocking!");
  lockServo.attach(lockServoPin);
  lockServo.write(180);
  delay(500);
  lockServo.detach();
  Serial.println("Unlocked!");
}

void openDoor()
{
  unlock();
  Serial.println("Opening!");
  upperSwitchValue = digitalRead(upperSwitchPin);
  lowerSwitchValue = digitalRead(lowerSwitchPin);
  beamBreakValue = digitalRead(beamBreakPin);
  

  while(upperSwitchValue == 1 && beamBreakValue == 1){
    upperSwitchValue = digitalRead(upperSwitchPin);
    lowerSwitchValue = digitalRead(lowerSwitchPin);
    beamBreakValue = digitalRead(beamBreakPin);
    doorMotor.writeMicroseconds(2000);
  }

  doorMotor.writeMicroseconds(1525);

  if(upperSwitchValue ==0){
    Serial.println("Opened!");
  }

  if(beamBreakValue == 0){
    Serial.println("CHECK YOUR CHICKENS!");
  
}

}

void closeDoor()
{
  unlock();
  Serial.println("Closing!");
  upperSwitchValue = digitalRead(upperSwitchPin);
  lowerSwitchValue = digitalRead(lowerSwitchPin);
  beamBreakValue = digitalRead(beamBreakPin);
  

  while(lowerSwitchValue == 1 && beamBreakValue == 1){
    upperSwitchValue = digitalRead(upperSwitchPin);
    lowerSwitchValue = digitalRead(lowerSwitchPin);
    beamBreakValue = digitalRead(beamBreakPin);
    doorMotor.writeMicroseconds(1000);
  }

  doorMotor.writeMicroseconds(1525);
  if(lowerSwitchValue == 0){
    Serial.println("Closed!");
    lock();}

  if(beamBreakValue == 0){
    Serial.println("CHECK YOUR CHICKENS!");
    openDoor();
    i = i+1;}
}