
void setup() 
{
      pinMode(11, OUTPUT);
      Serial.begin(9600);
}
int handShaking = 0;

void loop() 
{
      int recvLen = Serial.available();
      if( recvLen && !handShaking)
      {
            String recvMes = serialRecv(recvLen);
            Serial.println(recvMes);
            handShaking = 1;
            digitalWrite(11, HIGH);
   //         delay(1000);
      }
      delay(100);        // delay in between reads for stability
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
