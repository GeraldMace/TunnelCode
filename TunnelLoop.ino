#include<Servo.h>
//initalizing servo library
Servo esc;
//stating it is a speed controller

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

float DesSpeed = 4.5;
//tell the tunnel what you want its output to be in units of m/s

float speedangle = (DesSpeed - .78)/.0498; 
//LoBF from Excel data, solves for the required input of speed angle based on desired output in velocity. constants come from LoBF and can be more accurate with a higher degree equation and more data points for each speed.
float startspeed = 0;
//the starting speed angle for the ESC. Must start at zero and increment up to avoid torquing the bolt off of the propeller

void setup() {
Serial.begin(115200);
esc.attach(9, 1000, 2000);
esc.write(0);
delay(3000);
//esc is attached and given 3 seconds to prime before code starts. Otherwise, the esc attaching will be mistimed and never turn on
pinMode(halleffect, INPUT);
pinMode(pressure, INPUT);
//stating sensor pins are inputs


while(startspeed < speedangle){
  startspeed = startspeed +1;
  esc.write(startspeed);
  delay(200);
  //The tunnel ramps up to the desired speed in setup once the code initiates and the esc attaches. delay time is a safe margin to make sure that the tunnel is performing quickly but not torquing the bolt of the propeller
}

}

void loop() {
  // put your main code here, to run repeatedly:


 
  float count = 1;
  float starttime = micros();
  bool on_state = false;
  //these variables are required to allow the next while loop to work. The next loop will make a count each time the hall effect sensor is getting a digital reading the on_state will be changed, and when this state is changed yet the sensor gets another digital reading from the motor, the code will not take a count essentially avoiding double counts

  while(true){
  
     
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
Serial.println(startspeed);
Serial.print("Velocity = ");
Serial.println(V2);
float end_time = micros();
float time_passed = ((end_time-starttime)/1000000.0);
 //these variables are counting how much time it has passed to make the maxcount number of counts and then multiplying by 60 to get rpm 
Serial.print("Time Passed: ");
Serial.print(time_passed);
Serial.println("s");
float rpm_val = (count/time_passed)*60.0;
Serial.print(rpm_val);
Serial.println(" RPM");

//prints off all of the calculated and measured info before incrementing the speed
delay(1);  

float VelocityDiff = V2 - DesSpeed;
float AcceptedDiff = .1;
//creates a variable for the amount of error between the desired speed and the outlet velocity calculated from measured pressor. Acceptable difference can be altered here as well
if(VelocityDiff > AcceptedDiff){
  startspeed = startspeed - 1;
}

if(VelocityDiff < AcceptedDiff){
  startspeed = startspeed + 1;
}
//The previous if loops simply increment or decrement the speed by one until the desired outlet velocity is achieved.
esc.write(startspeed);
//writes the new speed to the tunnel after running through one of the previous if loops and restarts the code from the beginning. 
delay(1);

}
