#ifndef _LOCKKK_
#define _LOCKKK_

#include "Arduino.h"
#include "Command_parser.h"
#include "Board_Comms.h"
#include <math.h>

#define MOTOR 4
#define RED 7
#define LED_pos 3
#define LED_neg 2
#define BUZZER 8
#define LIM_SWITCH1 6
#define BUTTON 7
//CMDS,OM,863158022988725,20180511070223,L0,0,204.74.120.137.119,1497689816,
//CMDS,OM,863158022988725,20180511070351,Re,L0,
class Lock{
    public:
        int BUT_STATUS;
    private:
  
        int STATUS,RIDE_STATUS;
        String USER,IMEI,DEV_CODE,TIME;
        char *LAT,*LONG;

        uint32_t unlocking,locking;        //time variables
        const uint32_t period = 300000 ;   //5 mins for gps  NOTE: millis() overflow period is 50 days
        uint32_t next_D0;

        Board_Comms comm1;

    public:

    Lock()
    {
        ///
        /// Empty Constructor
        ///

    }

    void INIT()
    {
        ///
        /// Set initial status variables.
        ///

        STATUS=1;                      // status=1-> Locked   status=0->unlocked   
        RIDE_STATUS=0;                 //0-> Ride ended   1-> Ride Ongoing    2-> Ride Halted
        BUT_STATUS=0;                  //0-> Button not pressed 1-> Button pressed
        IMEI = "863158022988725";
        DEV_CODE = "OM";
        TIME = "1497689816";  //Default
        USER = "0.0.0.0.0";   //Default
        LAT="0.000000";
        LONG="0.000000";

        locking =0;     //Assume lock is locked at time=0
        unlocking=-1;   //-1 denotes lock is locked
        next_D0=period;
            
        pinMode(MOTOR,OUTPUT);
        pinMode(RED,OUTPUT);
        pinMode(LED_pos,OUTPUT);
        pinMode(LED_neg,OUTPUT);
        pinMode(BUZZER,OUTPUT);
        pinMode(LIM_SWITCH1,INPUT);
        pinMode(2,OUTPUT);
        pinMode(BUTTON,INPUT);
    
        digitalWrite(LED_neg,LOW);

        STATUS = get_lock_status();
    
        if(STATUS==0)
        {
            locking=-1;
            unlocking=0;
        }
    }
  
    int unlock();

    int on_lock();

    int lock();

    int halt();

    void LED();

    void buzzer(int n);

    void button();

    String send_server(String command);

    int get_lock_status();

    void RFID_setup();

    int RFID_read();

    void com_par(String command);

    String package_creator();

    String Local_time();

    int bat_stat();

    String get_gps();

    int GPS_send(String loc);

    int GPS_periodic();

    void err_buzzer(int j);  
};

#endif
