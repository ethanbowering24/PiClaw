#include <pigpio.h>
#include <vector>
#include <servo.h>

//TODO: Change to chrono?
#include <time.h>

//Using GPIO pins 0,2,3,4,5,6
/*
    M1 - GPIO0 - Min 000 - Max 180 
    M2 - GPIO2 - Min 050 - Max 180
    M3 - GPIO3 - Min 000 - Max 140
    M4 - GPIO4 - Min 070 - Max 180
    M5 - GPIO5 - Min 000 - Max 180
    M6 - GPIO6 - Min 120 - Max 180
*/
int main(void){

    int[6] pins = {0, 2, 3, 4, 5, 6};

    //Angles when claw first gets power, limp arm to prevent sudden jerking motion from claw
    int[6] powerupAngles = {90, 180, 0, 180, 180, 130};

    
    //Array storing current angles of each motor, initialised to ready position, slight bend in the elbow
    int[6] currentAngles = {90, 140, 60, 180, 180, 130}; 
    std::pair<int,int>[6] minMaxAngles= {{0,180},{50,180},{0,140},{70,180},{0,180},{120,180}};

    std::vector<Servo> motors;

    if (gpioInitialise() < 0){
      cerr << "pigpio initialisation failed." << endl;
      return 1;
    }

    //Initializing motors to startup positions
    for (int i=0;i<6;i++){
        motors.push_back(Servo(pins[i], minMaxAngles[i], powerupAngles[i]))
        motors[i].initialize();
    }

    //Slow sweep to ready position
    //Difference between powerup and startup angles (first motor already initialized)
    //int[2] angleDifferences = {-40, 60} motor 1 and 2 need the slow sweep
    for (int i=0; i<60;i++){
        if (i<40){
            currentAngles[1]--;
            motors[1].write(currentAngles[1])
        }
        currentAngles[2]++;
        motors[2].write(currentAngles[2]);
        
        //15 ms delay between angle changes for smooth sweep
        nanosleep((const struct timespec[]){{0, 15000000L}}, NULL);
    }

}