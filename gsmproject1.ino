#include <SoftwareSerial.h>
SoftwareSerial mysim(8, 7); // connect gsm modem on this pin
String smstext;

int buz = 9;  //buzzer connected to pin 9
int led = 4;  //led connected to pin 4

const int aqsensor = A1;  //output of mq135 connected to A1 pin of Arduino
int threshold = 40;      //Threshold level for Air Quality 70

void setup() {
  Serial.begin(9600);
  mysim.begin(9600);
  Serial.println("System is Initializing..");
  mysim.println("AT");
  updateSerial();
  pinMode (buz, OUTPUT);    // buzzer is connected as Output from Arduino
  pinMode (led, OUTPUT);    // led is connected as output from Arduino
  pinMode (aqsensor, INPUT); // MQ135 is connected as INPUT to arduino
  digitalWrite(buz, LOW);
  digitalWrite(led, LOW);
  delay(5000); // wait for 5 seconds
}

void loop() {
  int ppm = analogRead(aqsensor); //read MQ135 analog outputs at A1 and store it in ppm
  Serial.println(ppm);            //print value of ppm in serial monitor

  if (ppm > threshold)            // check is ppm is greater than threshold or not
  {

    smstext =  "\n ====== Smoke Detected..! ==========";
    Serial.println(smstext);
    sendSMS(smstext);
    digitalWrite(buz, HIGH);    //Turn ON Buzzer
    digitalWrite(led, HIGH);    //Turn ON LED
    delay(500);
    mysim.println("AT"); //Once the handshake test is successful, i t will back to OK
    updateSerial();
    delay(1000);

    mysim.println("ATD+254743595043;"); //  change ZZ with country code and xxxxxxxxxxx with phone number to dial
    updateSerial();
    delay(20000); // wait for 20 seconds...
    mysim.println("ATH"); //hang up
    updateSerial();
  }
  else
  {
    digitalWrite(led, LOW);  //jump here if ppm is not greater than threshold and turn off LED
    digitalWrite(buz, LOW);  //Turn off Buzzer
    Serial.println("No smoke detected");
  }
  delay (500);
}

void sendSMS(String message)
{
  mysim.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  delay(1000);

  mysim.println("AT+CMGS=\"+254743595043\"");
  updateSerial();
  delay(1000);

  mysim.print("Smoke Detected!"); //text content
  updateSerial();
  delay(1000);

  mysim.write((char)26);   // give module time to send SMS
  updateSerial();
  delay(1000);

}

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    mysim.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (mysim.available())
  {
    Serial.write(mysim.read());//Forward what Software Serial received to Serial Port
  }
}
