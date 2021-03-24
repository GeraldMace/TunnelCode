//**WARNING**
//**WARNING**
//**WARNING**
//**WARNING**
//**WARNING**
//do NOT use this code unless you are sure the potentiometer is at a zero value. Writing a high speed to the esc when it is off and at zero could possibly torque the bolt holding the propeller on.
//The propeller could come off during use if the bolt is loosened so ensure that the potentiometer is at zero and is dialed up slowly.
//**WARNING**
//**WARNING**
//**WARNING**
//**WARNING**
//**WARNING**



#include<Servo.h>
Servo esc;

int halleffect = 3;
//halleffect sensor pin
int pressure = 2;
//pressure sensor pin
int pot = 0;
//redundant for this code, but its the potentiometer pin
float maxcount = 50.0;
//the number of counts before the math is done to calculate rpm
float roh = 1.225;
//STD pressure of air
float k = 1.85;
//calculated loss coefficients based on matlab model




void setup() {
Serial.begin(115200);
esc.attach(9, 1000, 2000);
//we are not writing a speed to the esc until after reading the potentiometer, so no delay to prime the esc is required here
pinMode(halleffect, INPUT);
pinMode(pressure, INPUT);
pinMode(pot, INPUT);
//stating sensor pins are inputs
}

void loop() {
  
  float potvalue = analogRead(pot);
  potvalue = map(potvalue, 0,1023,0,140);
//reads the pot prior to entering the loop to ensure a value is ready and nothing is mistimed. 
  
  float count = 1;
  float starttime = micros();
  bool on_state = false;
//these variables are required to allow the next while loop to work. The next loop will make a count each time the hall effect sensor is getting a digital reading the on_state will be changed, and when this state is changed yet the sensor gets another digital reading from the motor, the code will not take a count essentially avoiding double counts


  while(true){

    float potvalue = analogRead(pot);
    potvalue = map(potvalue, 0,1023,0,140);
    esc.write(potvalue);
  //re re-reads the pot and interpolates its value to a speed angle using the map command. This speed angle is then written to the esc.
     
    if (digitalRead(halleffect)==0){
      if (on_state==false){
        on_state = true;
        count+=1.0;
         //adding a count and changing the state to avoid a double count.
      }
    } else{
      on_state = false;
    }
    
    if (count>=maxcount){
      break;
               //once the maxcount number at the beginning of the code is reached, the code breaks from the loop, prints to the serial monitor, and begins counting again from zero
    }
  }
  

  
 float pressureraw = analogRead(pressure); 
 float pressurevoltage = pressureraw/1024*5;
 float actualpressure = (pressurevoltage-.209)*(125-0)/(3.96-.209);
      //raw pressure data is taken with the sensor, converted to a voltage using the bit/voltage properties of the arduino, and then turned into an actual pressure through interpolation based on the voltage/pressure properties of the pressure sensor 

  float V2 = sqrt(2*actualpressure/(roh*(.75+.25*k)));
 //voltage is calculated at the outlet with the measured pressure
  

  Serial.print("Pressure raw = ");
  Serial.println(pressureraw);
  Serial.print("Pressure = ");
  Serial.println(actualpressure);
  Serial.print("Speed = ");
  Serial.println(potvalue);
  Serial.print("Velocity = ");
  Serial.println(V2);
  //prints off all of the calculated and measured info
  float end_time = micros();
  float time_passed = ((end_time-starttime)/1000000.0);
        //these variables are counting how much time it has passed to make the maxcount number of counts and then multiplying by 60 to get rpm 
  Serial.print("Time Passed: ");
  Serial.print(time_passed);
  Serial.println("s");
  float rpm_val = (count/time_passed)*60.0;
  Serial.print(rpm_val);
  Serial.println(" RPM");
  //some formatting for the way the data is presented. Feel free to change this. 
  delay(1);  
}
