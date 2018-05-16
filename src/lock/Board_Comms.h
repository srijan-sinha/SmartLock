#include <stdlib.h>
#include <LGPRS.h>
#include <LGPRSClient.h>

class Board_Comms{
  
   private:
   char *server;
   char *path;
   int CONNECTED;

   int port = 80; // HTTP

   public:

   Board_Comms()
   {
   }

   String communicate(String command);

   const char *nextToken(const char* src, char* buf);

   String update(const char* str, char* lati, char* longi);

   String read_gps(char* lati, char* longi);

   void convertCoords(double latitude, double longitude, double &lat_return, double &lon_return);

   String parseGPGGA(const char* GPGGAstr, char *lati, char *longi);

   static double getIntNumber(const char *s);

   static double getDoubleNumber(const char *s);

   static unsigned char getComma(unsigned char num,const char *str);

};

