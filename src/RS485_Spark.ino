/*
Spark Wiring Diagram                Particle Wiring
3-5V                        ->          3.3V
RX-I                        ->          TX
TX-O                        ->          RX
RTS                         ->          D2              (any digital output, in this example D2)
GND                         ->          GND
                                    UltraSonic
A                           ->        Brown (RS485 B+)
B                           ->        Green (RS485 A-)
GND                         ->        GND

*/
int rts = D2;

uint32_t currTime;
uint32_t prevTime;
byte data[12];
byte sendRS485Request[6]={0xAA,0x01,0x03,0x00,0x00,0xAE}; //Send Request for STATUS Command
size_t bytes;
float resultTemp = 0;
float resultUltra = 0;
//The setup function is called once at startup of the sketch
void setup()
{
 Serial1.begin(19200);
 pinMode(rts, OUTPUT);
 digitalWrite(rts, LOW);
 

 prevTime = millis();
}

// The loop function is called in an endless loop
void loop()
{
 Serial1.flush();
 currTime = millis();

 // Send data once per second
 if (currTime - prevTime >= 10000) {
 prevTime = currTime;

 // Send command
 Send(sendRS485Request, data);
 Serial.println();
 }
}


void Send(byte * cmd, byte* ret) {
 // use default send function
 sendCommand(cmd);

 // receive answer
  if (Serial1.available()){  //Read return data package (NOTE: Demo is just for your reference, the data package haven't be calibrated yet)
    for(int j=0; j < 12; j++){
      ret[j++]=(Serial1.read());
      
    }
    Serial.println("Data Begin");
    Serial.println(ret[0],HEX); //byte 1    //Sensor ID Tag
    Serial.println(ret[2],HEX); //byte 2    //Response Code
    Serial.println(ret[4],HEX); //byte 3    //Range LSB
    Serial.println(ret[6],HEX); //byte 4    //Range MSB
    Serial.println(ret[8],HEX); //byte 5    //Temperature Data
    Serial.println(ret[10],HEX); //byte 6   //Checksum mod 256
    Serial.println("Data End");

    Serial1.flush();
    Serial.println("Received data Done");

    resultTemp = (float(ret[8])*0.48876)-50;
    Serial.print("Temperature: ");
    Serial.print(resultTemp);
    Serial.println(" C");

    resultUltra = (float(ret[6]*256)+float(ret[4]))/128;
    Serial.print("Level: ");
    Serial.print(resultUltra);
    Serial.println(" In");

  }
  else{
    Serial.println("Error reading RS485");
  }


}

/*sendCommand(...)******************************************************************************
 * General function that sends command to RS485 peripheral device
 * <CR> is added to all commands
 * For RS485 communication, RTS pin has to be HIGH to allow writing to peripheral
 **********************************************************************************************/
void sendCommand(byte *cmd) {
/*
//check if trasmit the correct data
 Serial.println("Send Command");
 for(int i=0; i < 6; i++){
    Serial.print(cmd[i]);
    Serial.println("");
 }
 */

 digitalWrite(rts, HIGH); // set RTS to HIGH to allow writing to MAX485
 delay(100);

 for(int i=0; i < 6; i++){
   Serial1.write(cmd[i]); 
  // Serial.print(cmd[i]);
 }
 // send command
 Serial1.flush(); // Make sure message is fully transferred
 digitalWrite(RTS, LOW); // set RTS to LOW to receive answer
 delay(10);
}
