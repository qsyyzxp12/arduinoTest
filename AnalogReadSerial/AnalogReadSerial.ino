
void setup() 
{
      Serial.begin(9600);
}
int handShaking = 0;

void loop() 
{     
      if(handShaking)
      {
            String mes = "";
            for(int i=0; i<5; i++)
            {
                  int sensorVal = analogRead(i);
                  mes += String(sensorVal) + ",";
            }
            Serial.println(mes);
      }
      delay(50);        // delay in between reads for stability
}

void serialEvent()
{
      int recvLen = Serial.available();
      if( recvLen)
      {
            String recvMes = serialRecv(recvLen);
            if(recvMes == "Arduino?")
            {
                  Serial.println("YES");
                  handShaking = 1;
            }
            else if(recvMes == "End")
            {
                  handShaking = 0;
            }
            else
            {
                  String errMes = "Error, Arduino receive unexpected message: " + recvMes;
                  Serial.println(errMes);
            }
      }
}

String serialRecv(int length)
{            
      String recvMes = ""; 
      for(int i=0; i<length ;i++)
      {
            char incomingByte = Serial.read();
            recvMes += incomingByte;
      }
      return recvMes;
}
