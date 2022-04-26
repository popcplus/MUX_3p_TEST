// clock channel identify 0x68
// The I2C address of the DS3231 is 0x68. 0x57 is the I2C address of the AT24C32 EEPROM on your ZS-042 RTC module
// recreate 2 channel i2c

// Following is for an I2C Multiplexor system.
// Arduino to LCA9548A
// 10k PULL UP RESISTORS

// Using 3 Peripheral I2C channels: 1,2,3
// NOTE: SD writing - working, BMP serial monitor displaying - working, magnetic compass function serial - working 
// INDIVIUDALLY TESTED AND WORKING PERIPHERALS
//

// MAGNETIC COMPASS
//
// magnetic compass x,y,z recordings serial print out + write to SD card
// SD_BMP180_Clock_QMC5583L


#include <SFE_BMP180.h>
#include <Wire.h>
SFE_BMP180 pressure;
#define ALTITUDE 0 // Altitude of Robojax Headquarter (Ajax, Ontario, Canada)

////////////////////

#include <Wire.h>
#include <RTC.h>

DS3231 RTC;
#include <SD.h>
#include <SPI.h>
File myFile;

int pinCS = 10; // Pin 10 on Arduino Uno
String liveDirectory;
String fileNameAllocation = "dive0.txt";

#include <QMC5883LCompass.h>
QMC5883LCompass compass;
#define TotalSamples 100;

/////////////////////////////////////////////////////////////////////  FUNCTION CALLS

void BMP180_check();
void compassCheck();
void writeSD(int x,int y,int z);
void logProfileNameAllocation(); // can be created at the initiation of dive arming
void tcaSelect(uint8_t bus);

////////////////////////////////////////////////////////////////////
       
//  TCA9548 module I2C address:  0x70 >> MULTIPLEXOR

//  I2C device found at address 0x0D  ! QMC5883L >> COMPASS
//  I2C device found at address 0x68  ! DS3231 RTC >> CLOCK LINE 
//  I2C device found at address 0x77  ! BMP180 >> PRESSURE BAROMETER

#define addr1   0x68       // CLOCK device 3 (I2C bus 2)
#define addr2   0x0D       // QMC device 1 (I2C bus 1)
#define addr3   0x77       // BMP device 4 (I2C bus 2)

#define TCA_Address 0x70       // address of I2C switch module

#define bus1    1          // CLOCK
#define bus2    2          // COMPASS
#define bus3    3          // BMP

void setup() {

////////////////////////////////////////////////////////////////////////// CLOCK ACTIVATE
  
  Serial.begin(9600); // serial boot
  Wire.begin(); // initialize i2c comms

  tcaSelect(bus1);               // enable I2C channel 1
  Wire.beginTransmission(addr1);
  RTC.begin();
  Wire.endTransmission();
  
//////////////////////////////////////////////////////////////////////////// COMPASS ACTIVATE

  tcaSelect(bus2);               // enable I2C channel 2
  Wire.beginTransmission(addr2);
  compass.init();
  compass.setCalibration(-428, 656, -2357, 0, 0, 980);
  Wire.endTransmission();

//////////////////////////////////////////////////////////////////////////// BMP180 ACTIVATE - TEMP/PRESSURE

  tcaSelect(bus3);               // enable I2C channel 3
  Wire.beginTransmission(addr3);
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
  Wire.endTransmission();
  
//////////////////////////////////////////////////////////////////////////// SPI COMMS, SD INITIALIZE, CS

  pinMode (pinCS,OUTPUT); // cs pin needs to be in a low setting for SPI communication to work correctly

  // pinMode(pinCS, OUTPUT);
  
  // SD Card Initialization
  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

logProfileNameAllocation(); // call to scan card if file exists otherwise break and write file as the default setting dive0

} // END OF SETUP ***************************************************************************************************************************************************************

void loop() {
 
  compassCheck();

  BMP180_check();

}


void logProfileNameAllocation(){ // using SPI comms

  int diveNumber = 0;

  // TODO: be creative with naming systems to help identify with dive types i.e time/dive type category etc

                          // see if the directory exists, create it if not.

                        liveDirectory = "LOG/PRO";

// TODO: make a folder structure creation with mkdir
                          
////                        if(!SD.exists(liveDirectory)) // check if folder structure/ directory exists
////                        {

//                          if(SD.mkdir(liveDirectory + fileNameAllocation)) 
//                          {
//                            Serial.print("File directory created!");
//                          }                           
//                        
//                        else {
//                                ("file Exists, directory not created");
//                          }
                          
                                                while(SD.exists(fileNameAllocation)){
                                              
                                                          // need to extract numbers suffix out of current writen file name ending
                                                          
                                                          diveNumber ++;        
                                                          String diveNumberString = String(diveNumber);
                                                          fileNameAllocation = "dive" + diveNumberString + ".txt"; // concatanate an iteration one up from the existing global scoped variable/existing file
                                                          
//                                                          Serial.println(fileNameAllocation);                                                         
                                                          }
                                                          Serial.println(fileNameAllocation);                                                  
}


// must we switch bus and then address begin transmission following to utilize channel??

// **********************************************************************************************************************************************************************

// enable requested I2C channel (0..7) // 
// The TCA9548A() function can be called to select the bus that you want to communicate with. It sends a byte to the multiplexer with the port number.
// You must call this function whenever you want to select the I2C port.

void tcaSelect(uint8_t bus) {
  
  if (bus > 7) return;
  Wire.beginTransmission(TCA_Address);
  Wire.write(1 << bus);
  Wire.endTransmission();
  Serial.print("bus live: ");
  Serial.print(bus);
  Serial.println();
  
}


void compassCheck(){


            tcaSelect(bus2);               // enable I2C channel 2
            Wire.beginTransmission(addr2);
            
            int compass_x, compass_y, compass_z, a, b;
            char myArray[3];
          
            compass.read();
            Serial.println();
            compass_x = compass.getX();
            compass_y = compass.getY();
            compass_z = compass.getZ();
            Serial.print("X: ");
            Serial.print(",");
            Serial.print(compass_x);
            Serial.print(",");
            Serial.print(" Y: ");
            Serial.print(compass_y);
            Serial.print(",");
            Serial.print(" Z: ");
            Serial.print(compass_z);
            
            a = compass.getAzimuth();
            b = compass.getBearing(a);
            compass.getDirection(myArray, a);
            
                                                Serial.println();
                                                Serial.print(" Azimuth: ");
                                                Serial.print(a);
                                                Serial.println();
                                                Serial.print(" Bearing: ");
                                                Serial.print(b);
                                                Serial.println();
                                                Serial.print(" Direction: ");
                                              
                                                                                   Serial.print(myArray[0]);
                                                                                   Serial.print(myArray[1]);
                                                                                   Serial.print(myArray[2]);                                              
                                                                                   Serial.println();

                                                                                   Wire.endTransmission();
                                               


                                                /////////////////////////////////////////

                                                tcaSelect(bus1);               // enable I2C channel 1, busline switch to clockline function call
                                                Wire.beginTransmission(addr1);
                                                
                                                String absoluteTime = ""; 

                                                
                                                
                                                absoluteTime = absoluteTime + RTC.getHours() + ":" +  RTC.getMinutes()+ ":" + RTC.getSeconds();
                                                Serial.print(" Time: ");
                                                Serial.println(absoluteTime);


                                                
                                                
                                                writeSD(compass_x,compass_y,compass_z);
                                                Wire.endTransmission();
                                                

}
  
  
  void writeSD(int x,int y,int z){
 
  myFile = SD.open(fileNameAllocation, FILE_WRITE);
  if (myFile) {  
    
                  myFile.println();
                  myFile.print("X: ");
                  myFile.print(",");
                  myFile.print(x);
                  myFile.print(",");
                  myFile.print(" Y: ");
                  myFile.print(",");
                  myFile.print(y);
                  myFile.print(" Z: ");
                  myFile.print(",");
                  myFile.print(z);
                  Serial.println();
                  
                  Serial.println("Success >> File write to card");
                  
  myFile.close(); // close the file
                  }
                                                                            // if the file didn't open, print an error:
                                                                            else {
                                                                              Serial.println("error opening test.txt");
                                                                            }


}


void BMP180_check(){

  tcaSelect(bus3);               // enable I2C channel 3
  Wire.beginTransmission(addr3);

  char status;
  double T,P,p0,J;

  // Loop here getting pressure readings every 10 seconds.

  // If you want sea-level-compensated pressure, as used in weather reports,
  // you will need to know the altitude at which your measurements are taken.
  // We're using a constant called ALTITUDE in this sketch:
  
  Serial.println();
  Serial.print("provided altitude: ");
  Serial.print(ALTITUDE,0);
  Serial.print(" meters, ");
  Serial.print(ALTITUDE*3.28084,0);
  Serial.println(" feet");
  
  // If you want to measure altitude, and not pressure, you will instead need
  // to provide a known baseline pressure. This is shown at the end of the sketch.

  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.print(" deg C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" deg F");
      
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");

          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = pressure.sealevel(P,ALTITUDE); // we're at 90 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");

          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          J = pressure.altitude(P,p0);
          Serial.print("computed altitude: ");
          Serial.print(J,0);
          Serial.print(" meters, ");
          Serial.print(J*3.28084,0);
          Serial.println(" feet");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");


Wire.endTransmission();
  
}
