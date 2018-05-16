#include "Board_Comms.h"
#include <stdlib.h>
#include <LGPRS.h>
#include <LGPRSClient.h>
#include <LGPS.h>

LGPRSClient client; // GPRS client
gpsSentenceInfoStruct info; // GPS Data
char buff[256];
char gmaps_buff[50];
int port = 80; // HTTP

String Board_Comms::communicate(String command)
{
    ///
    /// Setup the server connection, send the command and receive the response.
    ///

    // Serial.println(command);

    server = "linkitonetest.000webhostapp.com";
    uint32_t curr = millis();
    while (!LGPRS.attachGPRS("wholesale", NULL, NULL))
    {
        delay(500);
        if((millis() - curr)>10000)
            break;
    }

    // If you get a connection, report back via serial:
    Serial.print("Connect to ");
    Serial.println(server);

    if (client.connect(server, port))
    {
        // Make a HTTP request:
        Serial.println("connected");
        client.print("GET ");
        String str = "GET /server.php?";
        str+="command=";
        str+=command;

        // Send command to server
        client.print(str);
        client.print(path);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(server);
        client.println("Connection: close");
        client.println();

        Serial.println(str);
    }
    else
    {
        // If you didn't get a connection to the server:
        Serial.println("connection failed");
        delay(500);
        return communicate(command);
    }

    String com = "";
    command = "";
    int flag = 0;

    // Get response from the server:
    while(1)
    {
        // If server is connected get response:
        if (client.available())
        {
            char c = client.read();           
            if( c == 'C')
             flag = 1;
            else if( c == 'M' && flag == 1)
             flag = 2;
            else if( c == 'D' && flag == 2)
             flag = 3;
            else if( c == 'S' && flag == 3)
             flag = 4;
            else if( c == '#')
            {
              flag = 0;
              com += c;
              command = com;
            }
            else if( flag != 4 )
             flag = 0;
            if(flag > 0)
             com += c;
            else
            com = "";
        }

        // If the server's disconnected, stop the client:
        if (!client.available() && !client.connected())
        {
            Serial.println();
            Serial.println("disconnecting.");
            client.stop();
            break;
        }
    }

    Serial.println(command);
    return command;
}

const char *Board_Comms::nextToken(const char* src, char* buf)
{
    ///
    /// Read next token.
    ///

    int i = 0;
    while(src[i] != 0 && src[i] != ',')
        i++;
    if(buf)
    {
        strncpy(buf, src, i);
        buf[i] = 0;
    }
    if(src[i])
        i++;
    return src + i;
}

String Board_Comms::update (const char* str, char* lati, char* longi)
{
    ///
    /// Update the coordinates of lock by the data received from GPS.
    ///

    char latitude[20];
    char longitude[20];
    char buf[20];
    const char* p = str;
    p = nextToken(p, 0); // GGA
    p = nextToken(p, 0); // Time
    p = nextToken(p, latitude); // Latitude
    p = nextToken(p, 0); // N
    p = nextToken(p, longitude); // Longitude
    String loc="";
    loc+=latitude;
    loc+=",";
    loc+=longitude;

    if(buf[0] == '1')
    {
        Serial.print("GPS is fixed:");
        Serial.print(atoi(buf));
        Serial.println(" satellite(s) found!");
        Serial.print("Latitude:");
        Serial.println(latitude);
        *lati=*latitude;
        Serial.print("Longitude:");
        *longi=*longitude;
        Serial.println(longitude);
        return loc;
    }
    else
    {
        Serial.println("GPS is not fixed yet.");
        String oc="";
        return oc;
    }
}

unsigned char Board_Comms::getComma(unsigned char num,const char *str)
{
    ///
    /// Parse a char array to get comma.
    ///

    unsigned char i,j = 0;
    int len=strlen(str);
    for(i = 0;i < len;i ++)
    {
    if(str[i] == ',')
        j++;
    if(j == num)
        return i + 1; 
    }
    return 0; 
}

double Board_Comms::getDoubleNumber(const char *s)
{
    ///
    /// Parse char array to get double.
    ///

    char buf[10];
    unsigned char i;
    double rev;
  
    i=getComma(1, s);
    i = i - 1;
    strncpy(buf, s, i);
    buf[i] = 0;
    rev=atof(buf);
    return rev; 
}

double Board_Comms::getIntNumber(const char *s)
{
    ///
    /// Parse char array to get int.
    ///

    char buf[10];
    unsigned char i;
    double rev;

    i=getComma(1, s);
    i = i - 1;
    strncpy(buf, s, i);
    buf[i] = 0;
    rev=atoi(buf);
    return rev; 
}

String Board_Comms::parseGPGGA(const char* GPGGAstr, char *lati, char *longi)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */

    double latitude, latitude_g;
    double longitude, longitude_g;
    char latitude_dir, longitude_dir;

      int tmp, hour, minute, second, num ;
    if(GPGGAstr[0] == '$')
    {
        tmp = getComma(1, GPGGAstr);
        hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
        minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
        second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');

        sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
        Serial.println(buff);
        sprintf(buff, "%2d%2d%2d", hour, minute, second);
        String loc="";
        loc+=buff;loc+=",A,";
        tmp = getComma(2, GPGGAstr);
        latitude = getDoubleNumber(&GPGGAstr[tmp]);

        tmp = getComma(4, GPGGAstr);
        longitude = getDoubleNumber(&GPGGAstr[tmp]);

        //get lat/lon direction
        tmp = getComma(3, GPGGAstr);
        latitude_dir = (GPGGAstr[tmp]);
        tmp = getComma(5, GPGGAstr);       
        longitude_dir = (GPGGAstr[tmp]);
        convertCoords(latitude, longitude, latitude_g, longitude_g);
        sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
        sprintf(gmaps_buff,"http://maps.google.com/?q=%.6f%c,%.6f%c",latitude_g,latitude_dir,longitude_g,longitude_dir);
        Serial.println(buff); 
        Serial.println(gmaps_buff);

        tmp = getComma(7, GPGGAstr);
        num = getIntNumber(&GPGGAstr[tmp]);    
        sprintf(buff, "satellites number = %d", num);
        Serial.println(buff); 

        loc+=(double)latitude;
        loc+=",N,";
        loc+=(double)longitude;loc+=",E,0.066,,22041998,,,A";
        return loc;
    }
    else
    {
        String oc="";
        Serial.println("Not get data"); 
        return oc;
    }
}

//----------------------------------------------------------------------
//!\brief  Convert GPGGA coordinates (degrees-mins-secs) to true decimal-degrees
//!\return  -
//----------------------------------------------------------------------
void Board_Comms::convertCoords(double latitude, double longitude, double &lat_return, double &lon_return){
  int lat_deg_int = int(latitude/100);    //extract the first 2 chars to get the latitudinal degrees
  int lon_deg_int = int(longitude/100);   //extract first 3 chars to get the longitudinal degrees
    // must now take remainder/60
    //this is to convert from degrees-mins-secs to decimal degrees
    // so the coordinates are "google mappable"
    float latitude_float = latitude - lat_deg_int * 100;    //remove the degrees part of the coordinates - so we are left with only minutes-seconds part of the coordinates
    float longitude_float = longitude - lon_deg_int * 100;     
    lat_return = lat_deg_int + latitude_float / 60 ;      //add back on the degrees part, so it is decimal degrees
    lon_return = lon_deg_int + longitude_float / 60 ;
}

String Board_Comms::read_gps (char* lati, char* longi)
{
    ///
    /// Read GPS data and store in GPS object info.
    ///

    LGPS.powerOn();
    delay(2000);
    LGPS.getData(&info);
    delay(1000);
    for(int i=0;i<4;i++)
    {
        Board_Comms::parseGPGGA((const char*)info.GPGGA, lati, longi);
        delay(500);
    }
    
    return Board_Comms::parseGPGGA((const char*)info.GPGGA, lati, longi);
}

