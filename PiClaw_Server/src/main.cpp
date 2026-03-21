#include "udpReceiver.h"

#include <pigpio.h>
#include <vector>
#include <servo.h>
#include <iostream>
#include <algorithm>

//TODO: Change to chrono?
#include <time.h>

//Socket includes (make sure they are all actually needed)
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

//Change as needed
//proposed data format: [0,1,2,3,4,5,6,7,8,9]
/*
0- Hand pitch
1- Hand roll
2- Hand yaw
3- Wrist pitch
4- Wrist roll
5- Wrist Yaw
6- Bicep Pitch
7- Bicep Roll
8- Bicep Yaw
9- Claw opening
*/
const int PORT = 8080;
const int BUFFER_SIZE = 1024;
char buffer[BUFFER_SIZE]; //Defining buffer globally so that we can constantly access it from another thread

//Array storing current angles of each motor, initialised to ready position, slight bend in the elbow
int currentAngles[6] = {90, 140, 60, 180, 180, 130};

int sensorData[10];

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
    int maxYaw = std::max({sensorData[2], sensorData[5], sensorData[8]});
    currentAngles[0] = maxYaw;

    //Motor 2 angle
    currentAngles[1] = sensorData[6];

    //Motor 3 angle
    currentAngles[2] = sensorData[3] - sensorData[6];

    //Motor 4 angle
    currentAngles[3] = sensorData[0] - sensorData[3] - sensorData[6];

    //Motor 5 angle
    currentAngles[4] = sensorData[1];

    //Motor 6 angle
    //This assumes that we recieve the correct angle directly from the glove side Pi
    currentAngles[5] = sensorData[10];
}

void udp_reader_thread(int sockfd){
    
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while(true){
        ssize_t bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&client_addr, &client_len);

        if (bytes_received < 0){
            std::cerr << "Error in recvfrom. Exiting thread." << std::endl;
            break;
        }

        buffer[bytes_received]='\\0'; //null terminate received data
    }
}

int init_socket(){
    int sockfd;
    struct sockaddr_in server_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    //Bind the socket
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        std::cerr << "bind failed" << std::endl;
        close(sockfd);
        return -1;
    }
    std::cout << "UDP Server listening on port" << PORT << std::endl;

    //Reader thread creation
    std::thread reader_thread(udp_reader_thread, sockfd);


    //HANDLING FOR REOPENING THE THREAD IF IT CLOSES


    return 0;
}

//TODO, THIS FUNCTION WILL RUN IN THE MAIN INFINITE LOOP AND FIX THE SOCKET THREAD IF IT GOES DOWN
int check_socket_open(){
    return 0;
}
/*
int main(void){

    //Socket setup
    if (init_socket() < 0){
        std::cerr << "socket creation failed" << std::endl;
    }



    int pins[6] = {0, 2, 3, 4, 5, 6};

    //Angles when claw first gets power, limp arm to prevent sudden jerking motion from claw
    int powerupAngles[6] = {90, 180, 0, 180, 180, 130};
    
    std::pair<int,int> minMaxAngles[6] = {{0,180},{50,180},{0,140},{70,180},{0,180},{120,180}};

    std::vector<Servo> motors;

    if (gpioInitialise() < 0){
      std::cerr << "pigpio initialisation failed." << std::endl;
      return 1;
      //Handling to restart connection so we don't lose connection to the arm???
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
        //TODO: Look into linear interpolation for reducing potential jerking motion. See if any even occurs first
        check_socket_open(); //If statement handling here or will the function handle the reopening?
        getLatestAngles();
        for (int i=0; i<6; i++){
            motors[i].writeAngle(currentAngles[i]);
        }
    }
    return 0;

}*/

int main()
{
    UdpReceiver sockRecv;
    sockRecv.init(8080);
    while (true)
    {
        Packet packet;
        if (sockRecv.receive(packet))
        {
            std::cout << packet.id << std::endl;
        }
        else
        {
            std::cerr << "recv failed" << std::endl;
        }
    }
    
}