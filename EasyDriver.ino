/*
 * To move motors send the following character string over the serial port:
 * 
 * M A520 B720 C960
 * 
 * That will move motor A 520 degrees, B 720 degrees and C 960 Degrees
 * Those degrees are positions, to move motor A another 520 degrees, but leave the other motors in the same position
 * you would send the following command
 * M A1040 B720 C960
 * 
 * You can look up AccelStepper arduino library to learn mmore about how 
 * the motors are moved. 
 * 
 * 
 * Motors don't know where they are at when they start
 * 
 */
#include <AccelStepper.h>

#define STEP_PER_REVOLUTION 200 
#define MICROSTEPS 1
#define GEAR_RATIO 1
#define MAX_SPEED 1000
#define ACCEL 500
#define EN 6

//Message & Command Variables
char incoming_message[32];
int incoming_index = 0;

//Stepper Variables


// Define two steppers and the pins they will use
AccelStepper stepper1(1, 9,8);
AccelStepper stepper2(1, 11, 10);
AccelStepper stepper3(1, 13, 12);


void setup()
{  
  Serial.begin(9600);
  Serial.println("Arduino Reset");
  
  stepper1.setMaxSpeed(MAX_SPEED);
  stepper1.setAcceleration(ACCEL);
  stepper2.setMaxSpeed(MAX_SPEED);
  stepper2.setAcceleration(ACCEL);
  stepper3.setMaxSpeed(MAX_SPEED);
  stepper3.setAcceleration(ACCEL);
  stepper1.moveTo(50);
  stepper2.moveTo(50);
  stepper3.moveTo(50);

  pinMode(EN, OUTPUT);

}

void loop()
{
  
  read_command();
  stepper1.run();
  stepper2.run();
  stepper3.run();
}

int degrees2steps(float deg){
  /*
   * Converts a number from degrees to steps, assuming 0 degree is 0 step posiiton
   */
  int steps;
  deg = (deg/360.)*STEP_PER_REVOLUTION *MICROSTEPS * GEAR_RATIO;
  steps = (int)deg;
  Serial.print("DEG/360, STEPS: ");
  Serial.print(deg);
  Serial.println(steps);
  return steps;
}


void read_command(){
  /*
   * Checks if any data is in the Serial buffer and reads that in 
   * Adds to the incoming_message character buffer
   * Calls interpret_message when a '\n' is detected
   */  
 while (Serial.available()){
  incoming_message[incoming_index]=Serial.read();
  // If we are at the end of the message, interpret and reset the buffer
  
  if (incoming_message[incoming_index] == '\n'){
    interpret_message();
    incoming_index = 0;
    Serial.print("Message Recieved: ");
    Serial.println(incoming_message);
    memset(incoming_message, 0, sizeof(incoming_message));
    }
  
  // If this is not the last value add more to the buffer
  else {
    incoming_index = incoming_index + 1;
 }}}

void interpret_message()
{
  /*
   * Reads the incoming message and calls the corresponding function
   */
  char utility_char = incoming_message[0];
  stepper1.moveTo(100);
  stepper2.moveTo(100);
  if (utility_char == 'M'){
    move_motors();
   }
}

void move_motors(){
  /*Retrieve position information (in degrees) from the 3 motors.
   * 
   */
  int new_pos1 = 0;
  int new_pos2 = 0;
  int new_pos3 = 0;
  int update1 = 0;
  float temp_deg;
  char incoming_char;

  for (int i = 1; i<= incoming_index; i++){
    incoming_char = incoming_message[i];
    if (incoming_char == 'A'){
      update1=i;
    }
    else if(incoming_char == 'B'){
      temp_deg = get_float_from_message(update1, i);
      new_pos1 = degrees2steps(temp_deg);
      update1=i;
      Serial.print("Retrieved A...");
      Serial.println(new_pos1);
    }
    else if(incoming_char == 'C'){

      temp_deg = get_float_from_message(update1, i);
      new_pos2 = degrees2steps(temp_deg);
      Serial.print("Retrieved B...");
      Serial.println(new_pos2);
      update1=i;
    }
    else if(incoming_char == '\n'){
      temp_deg = get_float_from_message(update1, i);
      new_pos3 = degrees2steps(temp_deg);
      update1=i-1;
      Serial.print("Retrieved C...");
      Serial.println(new_pos3);
    }
  }
  
  stepper1.moveTo(new_pos1);
  stepper2.moveTo(new_pos2);
  stepper3.moveTo(new_pos3);
}

float get_float_from_message(int start_idx, int stop_idx){
  /* Return a float from the incoming message using start and stop indexes
   */
   float number;
   int array_len = stop_idx-1 - start_idx;
   char new_array[array_len];
   for (int i=0; i<=array_len-1; i++){
    new_array[i]=incoming_message[start_idx+i+1];
    Serial.println(incoming_message[start_idx+i+1]);
   }
   number = atof(new_array);
   return number;
}
