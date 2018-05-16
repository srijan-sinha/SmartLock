#ifndef _PARSERRR_
#define _PARSERRR_

#define UNLOCK 1
#define GPS_LOC 2
#define BAT_STAT 3
#define RESPONSE 9

class Command_parser{  
  
  // Error code: function returning 0:ERROR, function returning anything else:successful
   
  public:
  /*void S8();   // Ringing for finding a bike Command
  //void G0();   //  Query Firmware Version Command
  //void W0();   // Alarmimg Command mlo 
  //void I0();   // Obtain SIM card ICCID Code Command*/

int parser(String command)
{
  Serial.println("Inside Parser");
  Serial.println(command);
  String IMEI="",Dev_code="",Time="",CMD="",valu1="",valu2="",valu3="";
  int i=0;
  String header="";
//  Serial.println(command);
  while(command.charAt(i)!=',')
  {
    header+=command.charAt(i);
    i++;
  }
  if(header!="CMDS")
   return 1;

  i++;
  while(command.charAt(i)!=',')
  {
    Dev_code+=command.charAt(i);
    i++;
  }
  
  i++;
  while(command.charAt(i)!=',')
  {
    IMEI+=command.charAt(i);
    i++;
  }

  i++;
  while(command.charAt(i)!=',')
  {
    Time+=command.charAt(i);
    i++;
  }

  i++;
  while(command.charAt(i)!=',')
  {
    CMD+=command.charAt(i);
    i++;
  }
  
  i++;
  if(command.charAt(i)=='#')
   goto END;
  while(command.charAt(i)!=',')
  {
    valu1+=command.charAt(i);
    i++;
  }

  i++;
  if(command.charAt(i)=='#')
   goto END;
  while(command.charAt(i)!=',')
  {
    valu2+=command.charAt(i);
    i++;
  }

  i++;
  if(command.charAt(i)=='#')
   goto END;
  while(command.charAt(i)!=',')
  {
    valu3+=command.charAt(i);
    i++;
  }
  
  END:

    if ( CMD == "L0" )
     return L0_in();
    if ( CMD == "Re")
     return Re_in();
    if ( CMD == "D0")
     return D0_in();
    if ( CMD == "S5")
     return S5_in();
  
}

int L0_in()
{
  return UNLOCK;
  //if(lock.unlock());
}

int D0_in()
{
  return GPS_LOC;
}

int S5_in()
{
  return BAT_STAT;
}

int Re_in()
{
  return RESPONSE;
}

};

#endif
