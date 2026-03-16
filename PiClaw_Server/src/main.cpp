#include <pigpio.h>
#include <vector>
#include <servo.h>
#include <iostream>

//TODO: Change to chrono?
#include <time.h>

//Array storing current angles of each motor, initialised to ready position, slight bend in the elbow
int currentAngles[6] = {90, 140, 60, 180, 180, 130}; 

//Using GPIO pins 0,2,3,4,5,6
/*
    M1 - GPIO0 - Min 000 - Max 180 
    M2 - GPIO2 - Min 050 - Max 180
    M3 - GPIO3 - Min 000 - Max 140
    M4 - GPIO4 - Min 070 - Max 180
    M5 - GPIO5 - Min 000 - Max 180
    M6 - GPIO6 - Min 120 - Max 180
*/
void getLatestAngles(){
    
    /*
    Insert code to fetch latest sensor angles here
    WILL ROUNDING BE DONE HERE OR IN ETHANS SENDING DATA CODE
    
    */

}

int main(void){

    int pins[6] = {0, 2, 3, 4, 5, 6};

    //Angles when claw first gets power, limp arm to prevent sudden jerking motion from claw
    int powerupAngles[6] = {90, 180, 0, 180, 180, 130};
    
    std::pair<int,int> minMaxAngles[6] = {{0,180},{50,180},{0,140},{70,180},{0,180},{120,180}};

    std::vector<Servo> motors;

    if (gpioInitialise() < 0){
      std::cerr << "pigpio initialisation failed." << std::endl;
      return 1;
    }

    //Initializing motors to startup positions
    for (int i=0; i<6; i++){
        motors.push_back(Servo(pins[i], minMaxAngles[i], powerupAngles[i]));
        motors[i].initialize();
    }

    //Slow sweep to ready position
    //Difference between powerup and startup angles (first motor already initialized)
    //int[2] angleDifferences = {-40, 60} motor 1 and 2 need the slow sweep
    for (int i=0; i<60;i++){
        if (i<40){
            currentAngles[1]--;
            motors[1].writeAngle(currentAngles[1]);
        }
        currentAngles[2]++;
        motors[2].writeAngle(currentAngles[2]);
        
        //15 ms delay between angle changes for smooth sweep
        nanosleep((const struct timespec[]){{0, 15000000L}}, NULL);
    }

    //Infinite loop passing data from sensors to motors
    while (1){
        //updates currentAngles
        getLatestAngles();
        for (int i=0; i<6; i++){
            motors[i].writeAngle(currentAngles[i]);
        }
    }
    return 0;

}