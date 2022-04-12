const int loopTime = 10; // time interval between loops in milliseconds

const int pinVacuum = 7; // vacuum pump pin
const int pinInflation = 6; // inflation pump pin
const int pinButton = 9; //programmable button

// each Port  (1,2,3) has two valves connected to it, depending on your pneumatic connection they will work as vacuum or inflation
// this code follows the configuration of the box as it was shipped to you. see documentation for details.
// These are the five values associated with each port
typedef struct {
  int pinPressureSensor;
  int pinValveInflation;
  int pinValveVacuum;
  int state; // -1 in vacuuming, 0 standby, +1 inflating
  float pressure;
} Port;

// ports on the Box are indexed from 1, C++ indexes arrays from 0. So ports are mapped as:
// physical box  |  code
// Port 1        |  ports[0]
// Port 2        |  ports[1]
// Port 3        |  ports[2]

Port ports[3] = {
  {A7, 1, 0, 0, 0.0}, // Port 1 | Port[0]
  {A6, 3, 2, 0, 0.0}, // Port 2 | Port[1]
  {A3, 5, 4, 0, 0.0}, // Port 3 | Port[2]
};

float val = 0;  // variable to store the value read

void setup() {
  Serial.begin(9600);  // start serial communication at 9600 baud
   pinMode(LED_BUILTIN, OUTPUT); // LED for communication testing
  
  // Valve 1 is a TX pin meaning that it can be involved in Serieal Communication. Our Custom PCB uses it as a  some of the pins in the custom PCB are connected to the serial1 pins. As serial1 is turned on by default this Arduino Nano 33 BLE
  // Specifically Valve 1 (the TX pin) is turned on and should be turned off
  digitalWrite(ports[0].pinValveVacuum, LOW);

  // now we can just setup our ports
  pinMode(pinInflation, OUTPUT);
  pinMode(pinVacuum, OUTPUT);

  for (int p = 0; p < 3; p++) {
    pinMode(ports[p].pinValveInflation, OUTPUT);
    pinMode(ports[p].pinValveVacuum, OUTPUT);
  }

  Serial.begin(9600);           //  setup serial
}

//Read pressure sensor in absolute PSI (remember, atmosphere pressure = 14.7 psi)
void updatePressures() {
  int temp;
  for (int p = 0; p < 3; p++) {
    temp = analogRead(ports[p].pinPressureSensor);
    ports[p].pressure = ((((temp / 1023.0) * 3.3) - 0.33) / 0.0264); // as in the datasheet: LINK TO DATASHEET
    //Serial.print("Time: ");
    //Serial.println(millis());
    //Serial.print("Port "); //define pin
    //Serial.print(p + 1);
    //Serial.print(": ");
    //Serial.println(ports[p].pressure, 3);
  }
}

//Inflation Function
void inflatePort(Port p) {
  digitalWrite(p.pinValveVacuum, LOW);
  digitalWrite(p.pinValveInflation, HIGH);
}

//Vacuuming Function
void vacuumPort(Port p) {
  digitalWrite(p.pinValveInflation, LOW);
  digitalWrite(p.pinValveVacuum, HIGH);
}

//Hold Pressure in Port Function
void holdPort(Port p) {
  digitalWrite(p.pinValveInflation, LOW);
  digitalWrite(p.pinValveVacuum, LOW);
}

void loop() {
  //variables to track changes in time
  unsigned long timeNow = millis();
  static unsigned long lastUpdate = 0;


  static int demoState = 0; //toggles betwwen 4 different states - waiting for button, inflation, vacuum and holdport
  static unsigned long demoTimeStateChange = 0;

   if (Serial.available()) { // check for incoming serial data
    String command = Serial.readString();  // read command from serial port
    if (command.startsWith("write")) {
      String return_info;
      return_info += "write\n"; //return string to send back to controller
      int i = 6;
      String tempString = ""; //A temporary string buffer to store input ints
      int portNumber = 0; // Port number, 0 by default
      float inputPressure = 15.00; // Pressure, 15 PSI by default
      while(command[i] != ',') { // read port number until seperating char ','
        tempString += command[i];
        i++;
      }
      i++; // skip over ','
      tempString.trim(); // Strip whitespace
      portNumber = tempString.toInt(); // Convert to integer
      tempString = ""; // clear temporary buf
      while(command[i] != ')') { // read pressure until ending char ')'
        tempString += command[i];
        i++;
      }
      tempString.trim(); // Strip whitespace
      inputPressure = tempString.toFloat();
      return_info += "Port number: ";
      return_info += String(portNumber);
      return_info += "\nPressure: ";
      return_info += String(inputPressure);
      return_info += "\n";
      Serial.print(return_info);
      int state = 0; // Inflation not finished
      while (!state){
        unsigned long timeNow = millis();
        if ((timeNow - lastUpdate) >= loopTime) {
          lastUpdate = timeNow;
          updatePressures();

        if (ports[portNumber].pressure < inputPressure + .3 && ports[portNumber].pressure > inputPressure - .3) { // if withing .3 of the input pressure, hold.
          Serial.println("holding");
          digitalWrite(pinVacuum, LOW);
          digitalWrite(pinInflation, LOW);
          holdPort(ports[portNumber]);
          state = 1;
        }
        
        if (ports[portNumber].pressure < inputPressure - 1 && !state) { // If pressure is lower than input and state is not reached, inflate
          Serial.println("inflate");
          digitalWrite(pinVacuum, LOW);
          digitalWrite(pinInflation, HIGH);
          inflatePort(ports[portNumber]);
          delay(2000);
        }

        if (ports[portNumber].pressure > inputPressure + 1 && !state) { // If pressure is higher than input and state is not reached, deflate
          digitalWrite(pinVacuum, HIGH);
          digitalWrite(pinInflation, LOW);
          Serial.println("pressure > " + String(inputPressure));
          delay(500);
          demoTimeStateChange = timeNow;
      }
      }
    }
   }

   if (command.startsWith("read")) {
      String return_info;
      return_info += "read\n"; //return string to send back to controller
      int i = 5;
      String tempString = ""; //A temporary string buffer to store input ints
      int portNumber = 0; // Port number, 0 by default
      float readPressure; // Pressure to be read
      while(command[i] != ')') { // read port number until seperating char ','
        tempString += command[i];
        i++;
      }
      tempString.trim(); // Strip whitespace
      portNumber = tempString.toInt(); // Convert to integer

      int temp;
      temp = analogRead(ports[portNumber].pinPressureSensor);
      readPressure = ((((temp / 1023.0) * 3.3) - 0.33) / 0.0264); // as in the datasheet: LINK TO DATASHEET

      return_info += "Port number: ";
      return_info += String(portNumber);
      return_info += "\nPressure: ";
      return_info += String(readPressure);
      return_info += "\n";
      Serial.print(return_info);

   }




    
/*
    if (demoState == 0) { // standby, waiting for button push
      if (digitalRead(pinButton))
      {
        demoState = 1;
        demoTimeStateChange = timeNow;
      }
    }

    else if (demoState == 1) { // inflating Port 1
      Serial.println("inflate");
      digitalWrite(pinVacuum, LOW);
      digitalWrite(pinInflation, HIGH);
      inflatePort(ports[0]);
      //inflatePort(ports[1]); //these commented out lines inflate Port 2 and 3
      //inflatePort(ports[2]);
      delay(5000);

     if (ports[0].pressure > 25) { // Inflates to 19 PSI as state 1
        digitalWrite(pinVacuum, LOW);
        digitalWrite(pinInflation, LOW);
        Serial.println("pressure > 20");
        //delay(2000);
        demoState = 2;
        demoTimeStateChange = timeNow;
      }
    }

    
   else if (demoState == 2) { // vacuuming Port 1
      digitalWrite(pinVacuum, HIGH);
      digitalWrite(pinInflation, LOW);
      vacuumPort(ports[0]);
      //vacuumPort(ports[1]); //these commented out lines vacuum Port 2 and 3
      //vacuumPort(ports[2]);
       Serial.println("vacuum");

      if (ports[0].pressure < 14.5) { // Vacuums until 14.5 PSI in state 2
        demoState = 3;
        demoTimeStateChange = timeNow;
        Serial.println("pressure < 14.5");
      }
    }

    else if (demoState == 3) { // holding Port 1
      Serial.println("hold");
      digitalWrite(pinVacuum, LOW);
      digitalWrite(pinInflation, LOW);
      holdPort(ports[0]);
      //holdPort(ports[1]); //these commented out lines hold Port 2 and 3
      //holdPort(ports[2]);

      demoState = 0;
      demoTimeStateChange = timeNow;
    }
    */
  }
}
