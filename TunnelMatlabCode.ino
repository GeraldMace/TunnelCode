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
float speedd = 30;
//starting speed angle of esc



void setup() {
Serial.begin(115200);
esc.attach(9, 1000, 2000);
esc.write(0);
delay(3000);
//esc is attached and given 3 seconds to prime before code starts. Otherwise, the esc attaching will be mistimed and never turn on
pinMode(halleffect, INPUT);
pinMode(pressure, INPUT);
//stating sensor pins are inputs
}

void loop() {
 


 esc.write(speedd);
 delay(2000); //writing the current speed to the esc and waiting 2 seconds before entering the data collection loop
  for(int i = 0; i < 5; i = i + 1){ 
//this for loop allows the serial monitor to do the following code five times before incrementing up the speed. This allows the user to get five points of data to average out before moving on to the next incremented speed. 
//i < 5 can be changed to allow for more data points at each speed.  
  
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
    } 
    else{
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
      float end_time = micros();
      float time_passed = ((end_time-starttime)/1000000.0);
      float rpm_val = (count/time_passed)*60.0;
      //these variables are counting how much time it has passed to make the maxcount number of counts and then multiplying by 60 to get rpm 
      Serial.print(speedd);
      Serial.print("  ");
      Serial.print(rpm_val);
      Serial.print("  ");
      Serial.print(pressureraw);
      Serial.print("  ");
      Serial.print(actualpressure);
      Serial.print("  ");
      Serial.println(V2);
//prints off all of the calculated and measured info before incrementing the speed
      delay(1); 

   
  }

  speedd = speedd + 5;

  //increment the speed up. make this number smaller to have many more data points at the cost of a longer test period. 
     
if(speedd == 140){
  esc.write(0);
  //the idea behind this line is to stop the code once a max designated speed is reached. however, this line of code didn't function as intended and 
  //the tunnel kept increasing in speed. Use with caution. 
}

}
