#include <LBattery.h>
#include "Lock.h"
#include "RFID.h"

    int Lock::unlock()
    {
        ///
        /// Run upon receiving unlock command from the Server
        ///

        digitalWrite(MOTOR,HIGH);
        delay(1200);
        digitalWrite(MOTOR,LOW);
        Serial.println("Unlocked");

        if(RIDE_STATUS != 2)
        {
            unlocking = millis();
            locking = -1;
        }

        STATUS=0;   //Unlocked
        RIDE_STATUS=1;   //Ride Started

        String command = package_creator();             // Lock-> Server  UNLOCK STATUS COMMAND

        command += "L0,";
        command += STATUS;command += ",";
        command += USER;command += ",";
        command += TIME;
        digitalWrite(LED_pos,HIGH);
        String rec= send_server(command);
        digitalWrite(LED_pos,LOW);
        buzzer(1);
        com_par(rec);
        return 1;
    }

    int Lock::on_lock()
    {
        ///
        /// Run whenever the lock is locked manually
        ///
          
        if(digitalRead(LIM_SWITCH1) == LOW && STATUS == 0) 
        {
            STATUS = 1;  // Locked
            int flag=0;
            Serial.println("Inside On Lock");
            Serial.print("BUT_STATUS is :");
            Serial.println(BUT_STATUS);
            if(BUT_STATUS == 1)
            {
                Serial.println("CHECK FOR LOCK");
                //D0 command 
                //    CMDR,OM,863158022988725,000000000000,D0,0,124458.00,2237.75314,11408.62621#<LF> 
                //    String loci = "124458.00,2200.75314,5155.62621"; //for non geofencing
                String loci = "124458.00,2832.71,7711.6"; //for geofencing

                String command = package_creator();

                command +="D0,";
                command +="0,";
                command +=loci;
                digitalWrite(LED_pos,HIGH);
                String rec= send_server(command);
                digitalWrite(LED_pos,LOW);
                buzzer(1);
                Serial.println(rec);
                Serial.println("Going into parser");
                com_par(rec);
                lock();
                Serial.println(rec);
                Serial.println("Going into parser");
                com_par(rec);
                BUT_STATUS = 0;
            }
            else
            {
                Serial.println("FOR HALT");
                halt();
            }
            return 1;  
        }
        else
        {
            return 0;
        }
    }

    int Lock::lock()
    {
        ///
        /// Run if the cycle is in geofenced area and ride is needed to end
        ///

        USER = "0.0.0.0.0";
        RIDE_STATUS = 0;


        //Lock command
        String command = package_creator();             // Lock-> Server  UNLOCK STATUS COMMAND
        Serial.println("Locked");
        command += "L1,";
        command += USER;command += ",";
        command += TIME;command += ",";
        uint32_t ride=millis() - unlocking;
        command += ride;

        unlocking = -1;
        locking=0;
        locking=millis();
        digitalWrite(LED_pos,HIGH);
        String rec= send_server(command);
        digitalWrite(LED_pos,LOW);
        buzzer(1);
        com_par(rec);

        return 1;   
    }

    int Lock::halt()
    {
        ///
        /// Run if the ride is to be halted.
        ///

        RIDE_STATUS = 2;
        Serial.println("Halted");
        String command = package_creator();

        command += "L2,";
        command += USER;command += ",";
        command += TIME;
        digitalWrite(LED_pos,HIGH);
        String rec= send_server(command);
        digitalWrite(LED_pos,LOW);
        buzzer(1);
        com_par(rec);
        return 1;    
    
    }

  
    int Lock::get_lock_status()
    {
        ///
        /// Returns lock status. 
        ///

        int switch1 = digitalRead(LIM_SWITCH1);
        if(switch1 == LOW)
            return 1;    //Locked
        else
            return 0;   //Unlocked
    }

    void Lock::LED()
    {
        ///
        /// Redundant.
        ///

//   if(RIDE_STATUS==0)                    //Ride Ended
//   {
//     digitalWrite(RED,HIGH);
//     digitalWrite(GREEN,LOW);
//     digitalWrite(ORANGE,LOW);
//   }
//   else if(RIDE_STATUS==1)              // Ongoing Ride
//   {
//     digitalWrite(RED,LOW);
//     digitalWrite(GREEN,HIGH);
//     digitalWrite(ORANGE,LOW);
//   }
//   else                                 //Ride Halted
//   {
//     digitalWrite(RED,LOW);
//     digitalWrite(GREEN,LOW);
//     digitalWrite(ORANGE,HIGH);
//   }
    }

    void Lock::buzzer(int j)
    {
        ///
        /// Ring the buzzer.
        ///

        for(int i = 0; i < j; i++)
        {
            digitalWrite(BUZZER,HIGH);
            delay(50);
            digitalWrite(BUZZER,LOW);
            delay(50);
        }
    }

    void Lock::button()
    {
        ///
        /// Change the button status on button press.
        ///

        int cpress = 0;
        cpress = digitalRead(BUTTON);
        if(cpress == 0)
        {
            Serial.println("BUT_STATUS changed to 1");
            BUT_STATUS = 1;
        }
    }  

    void Lock::err_buzzer(int j)
    {
        ///
        /// Use for error.
        ///

        for(int i = 0; i < j; i++)
        {
            digitalWrite(BUZZER,HIGH);
            delay(400);
            digitalWrite(BUZZER,LOW);
            delay(400);
        }
    }

    String Lock::send_server(String command)
    {
        ///
        /// Send command to server.
        ///
        return comm1.communicate(command);
    }

    void Lock::RFID_setup()
    {
        ///
        /// Setup RFID.
        ///

        Serial.println("RFID setup");
        RFID_setup1();
    }

    int Lock::RFID_read()
    {
        ///
        /// Read RFID Card and send to server for authentication.
        ///

        //Serial.println("RFID read");
        String s = loop1();
        if( s != "" && s!=USER && RIDE_STATUS == 0)
        {
            USER = s;
            buzzer(2);
            Serial.println(USER);
            delay(1000);

            String command = package_creator();             // Lock-> Server  RFID DETECTED COMMAND

            command += "R0,";
            command += "0,";
            command += USER;command += ",";
            command += TIME;
            /* command += "L1" ;
            command += USER;command += ",";
            command += TIME;command += ",";
            command += "20";*/
            digitalWrite(LED_pos,HIGH);
            String rec= send_server(command);
            digitalWrite(LED_pos,LOW);
            buzzer(1);
            Serial.println(rec);
            Serial.println("Going into parser");
            com_par(rec);
            Serial.println("End of RFID");
            return 1;
        }
        else if( s != "" && s == USER && RIDE_STATUS==2)
        {
            buzzer(2);
            unlock();
            return 1;
        }
        else if( s != "" && s == USER && STATUS == 1 && RIDE_STATUS==1)
        {
            buzzer(2);
            return 1;
        }
        else
            return 0;
    }

    void Lock::com_par(String command)
    {
        ///
        /// Parse the command and call respective function according to command received.
        ///

        Command_parser pars;
        Serial.println(command);
        Serial.println("Parsing command");
        if(command == "")
        {
            err_buzzer(1);
        }
        else
        {
            int check=pars.parser(command);
            switch(check)
            {
                case UNLOCK:
                    Serial.println("Going into Unlock");
                    unlock();
                    break;
                case GPS_LOC:
                    GPS_send(get_gps());
                    break;
                case BAT_STAT:
                    Serial.println("Going into batstat");
                    bat_stat();
                    break;
                case RESPONSE:
                    Serial.println("Server Response has been recorded");
                    break;
            }
        }
    }

    String Lock::package_creator()
    {
        ///
        /// Generate a basic command to which command code, value etc. can be added.
        ///

        String str="C";
        str+="MDR,";
        str+=DEV_CODE;str+=",";
        str+=IMEI;str+=",";
        str+=TIME;  //write code for returning time here
        str+=",";

        return str;
    }

    String Lock::Local_time()
    {

    }

    int Lock::bat_stat()
    {
        ///
        /// Send the battery status command to server.
        ///

        int stat = LBattery.level();

        String command = package_creator();             // Lock-> Server  UNLOCK STATUS COMMAND

        command += "S5,";
        command += stat;command += ",";
        command += "31";command += ",";
        command += "0,";
        command += STATUS;command+=",";
        command += "0";
        digitalWrite(LED_pos,HIGH);
        String rec= send_server(command);
        digitalWrite(LED_pos,LOW);
        buzzer(1);
        com_par(rec);
        return 1; 
    }

    String Lock::get_gps()
    {
        ///
        /// Update the lock coordinates by receiving values from GPS.
        ///

        return comm1.read_gps(LAT,LONG);
    }

    int Lock::GPS_send(String loc)
    {
        ///
        /// Send location command to the server.
        ///

        String command = package_creator();

        command +="D0,";
        command +="0,";
        command +=loc;
        digitalWrite(LED_pos,HIGH);
        String rec= send_server(command);
        digitalWrite(LED_pos,LOW);
        buzzer(1);
        com_par(rec);
        return 1;
    }

    int Lock::GPS_periodic()
    {     
        ///
        /// Use to send Location command periodically.
        ///

        if(millis()>next_D0)
        {
            next_D0 = millis() +period; 
            GPS_send(get_gps());   //Send Current Location
        }
    }

