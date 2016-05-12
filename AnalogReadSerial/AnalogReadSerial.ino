
void setup() 
{
      Serial.begin(9600);
}
int handShaking = 0;

void loop() 
{     
      if(handShaking)
      {
            int sensorVal0 = analogRead(0);
            int sensorVal1 = analogRead(1);
            
            String mes = "(" + String(sensorVal0) + ", " + String(0) + ")";
            Serial.println(mes);
      }
      delay(100);        // delay in between reads for stability
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
                  Serial.println("OK");
                  handShaking = 0;
            }
            else
                  Serial.println("Error");
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
