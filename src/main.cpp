#include <Arduino.h>
#include <HardwareSerial.h>
/*!
 * @file HelloWorld.ino
 * @brief Show helloworld.
 * @copyright	Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @maintainer [yangfeng](feng.yang@dfrobot.com)
 * @version  V1.0
 * @date  2021-09-24
 * @url https://github.com/DFRobot/DFRobot_RGBLCD1602
 */
#include "DFRobot_RGBLCD1602.h"

#define RADAR_TX_PIN 4
#define RADAR_RX_PIN 2

HardwareSerial SerialPort(1);

const int colorR = 0;
const int colorG = 0;
const int colorB = 255;

#define DISPLAY_DELAY 400
#define READ_BUFFER_LEN 256

float speed=0;
bool flag=false;
unsigned long time_now=0;

DFRobot_RGBLCD1602 lcd(/*lcdCols*/16,/*lcdRows*/2);  //16 characters and 2 lines of show
void readSpeed();
void display();
void comportTransmit(byte* pMsg, int nMsgLen);
int comportReceive(byte* pMsg, int nLen);
void transmitFromSerial();

void readSpeed()
{
    //check if it is something to receive
    if(SerialPort.available()>0)
        {
            speed=0;
            char speed_buffer[READ_BUFFER_LEN];
            int nread_bytes=0;
            // while(SerialPort.available()){
            //     speed_buffer[index]=(uint8_t)SerialPort.read();
            //     index++;
            // }
            nread_bytes=comportReceive((byte*)speed_buffer, READ_BUFFER_LEN);
            
            speed_buffer[READ_BUFFER_LEN-1]='\0';
            Serial.println(speed_buffer);

            char* pFound_char=NULL;
            if(nread_bytes>=READ_BUFFER_LEN){
                speed_buffer[READ_BUFFER_LEN-1]='\0';
                pFound_char=strchr((const char*)speed_buffer, '\r');
                if(pFound_char!=NULL)
                    *pFound_char='\0';
            }
            else
                {speed_buffer[nread_bytes]='\0';}

            //Serial.println((const char*)speed_buffer);
            speed=atof((const char*)speed_buffer);
            if(speed!=0.00)flag=true;
        }
}

void display()
{
    

    //display only if it is something to display and we are past the delay
        if(flag)
        {
         //check if it is possible to display(if we are in delay range)   
            if(millis()-time_now>DISPLAY_DELAY){
                //it is possible to display
                
                //reset time
                time_now=millis();

                //reset flag to get real time data next time
                flag=false;

                lcd.setCursor(0,1);
                lcd.print(speed);
                lcd.print("                     ");


            }
            else{
                //when we cannot display but the reading is valid, discard the reading
                flag=false;
            }
         
        }
        else{
            if(millis()-time_now>DISPLAY_DELAY){
                 //reset time
                time_now=millis();

                //clear speed data
                lcd.setCursor(0,1);
                lcd.print("                ");
            }
        } 
}



void setup() {
    Serial.begin(115200);
    while(!Serial){
        ;
    }

    SerialPort.begin(19200,SERIAL_8N1,RADAR_RX_PIN,RADAR_TX_PIN);
    
    
    lcd.init();

    lcd.setRGB(colorR, colorG, colorB);
    
    // Print a message to the LCD.
    //lcd.print("hello, world!");

    
    lcd.setCursor(0, 0);
    lcd.print("Viteza este:");
    delay(100);

    // //set speed unit to km/h
    // SerialPort.print("UK");
    // delay(1);

    //     while(!SerialPort.available()){
    //     delay(1);
    // }

    // while(SerialPort.available()){
    //     Serial.print(SerialPort.read());
    // }

    

    //   delay(100);

    // //set minimum speed to 10km/h
    // SerialPort.print("R>10\r");

    //     while(!SerialPort.available()){
    //     delay(1);
    // }

    //   while(SerialPort.available()){
    //     Serial.print(SerialPort.read());
    // }

}

void loop() {
    readSpeed();
    display();

    transmitFromSerial();

    delay(1);

}


//nLen is the maximum length to be received
int comportReceive(byte *pMsg, int nLen){
    int nBytes = 0;

    while (SerialPort.available()>0)
    {
        pMsg[nBytes++]=(byte)SerialPort.read();

        if(nBytes>=nLen){
            break;
        }
    }//end while

    return nBytes;
    
}

void comportTransmit(byte* pMsg, int nMsgLen){

    SerialPort.write(pMsg, nMsgLen);

}//end comport transmit

void transmitFromSerial(){
    byte buffer[READ_BUFFER_LEN];
    int index=0;
    while(Serial.available()>0){
        buffer[index++]=(byte)Serial.read();
    }

    comportTransmit(buffer, index);
}