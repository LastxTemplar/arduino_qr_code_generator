int powerTimer;

/**********************
| SCREEN              |
**********************/
    #include <TFT.h>  // Arduino LCD library
    #include <SPI.h>
    #define cs   10
    #define dc   9
    #define rst  8
    TFT TFTscreen = TFT(cs, dc, rst);

/**********************
| DS3231              |
**********************/
    #include <DS3231.h>
    DS3231  rtc(SDA, SCL); // Pins A4 and A5
    String strDate;
    String strTime;
    char dateCharArray[11]; // dd.mm.yyyy
    char timeCharArray[7]; // xx:xx:xx

/**********************
| QR Code             |
**********************/
    #include "qrcode.h"
    QRCode qrcode;




void setup() {
    Serial.begin(115200);
    powerTimer = 10;

    /**********************
    | 5V SOURCE           |
    **********************/
        pinMode(4, OUTPUT); // 5V
        digitalWrite(4, HIGH);

    /**********************
    | RELAY / LED         |
    **********************/
        pinMode(7, OUTPUT); // Relay
        pinMode(2, OUTPUT); // LED
        digitalWrite(7, LOW);
        digitalWrite(2, HIGH);

    /**********************
    | SCREEN              |
    **********************/
        TFTscreen.begin();
        // Rotate the screen
        TFTscreen.setRotation(2);
        // clear the screen with a white background
        TFTscreen.background(255,255,255);
        TFTscreen.setTextSize(2);
        TFTscreen.stroke(0,0,0);
        TFTscreen.fill(0,0,0);
    

    /**********************
    | DS3231              |
    **********************/
        rtc.begin();
        // The following lines can be uncommented to set the date and time
        // You need to do this just once, the battery on the DS3231 will help remember the setting.
        // -----
        // rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
        // rtc.setTime(23, 14, 0);     // Set the time to 12:00:00 (24hr format)
        // rtc.setDate(18, 04, 2018);   // Set the date to 18 April 2018
        // Set Initial Time
        strDate = String(rtc.getDateStr());
        strTime = String(rtc.getTimeStr());
        strDate.toCharArray(dateCharArray, 11);
        strTime.toCharArray(timeCharArray, 6);
        TFTscreen.text(dateCharArray,0,0);
        TFTscreen.text(timeCharArray,0,20);

    /**********************
    | QR Code             |
    **********************/
        // Print First Time
        uint8_t qrcodeData[qrcode_getBufferSize(3)];
        qrcode_initText(&qrcode, qrcodeData, 3, 0, dateCharArray); // your text in last parameter, e.g. "Hello World"
        for (uint8_t y = 0; y < qrcode.size; y++) {
            // Each horizontal module
            for (uint8_t x = 0; x < qrcode.size; x++) {
                if(qrcode_getModule(&qrcode, x, y)) {
                    TFTscreen.rect(5+(x*4), 40+(y*4), 4, 4);
                }
            }
        }
}

void loop() {
    strDate = String(rtc.getDateStr());
    strTime = String(rtc.getTimeStr());
    
    if(powerTimer==0) {
        digitalWrite(2, LOW);  // TURN OFF LED
        digitalWrite(7, HIGH); // POWER OFF ARDUINO
    }

    // Overwriting the date and time on screen but dont touch the qr code
    if(strTime.substring(6)=="00" && powerTimer>0) {
        // Remove previous value
        TFTscreen.stroke(255,255,255);
        TFTscreen.text(dateCharArray,0,0);
        TFTscreen.text(timeCharArray,0,20);
        
        strDate.toCharArray(dateCharArray, 11);
        strTime.toCharArray(timeCharArray, 6);
        
        // Set new value
        TFTscreen.stroke(0,0,0);
        TFTscreen.text(dateCharArray,0,0);
        TFTscreen.text(timeCharArray,0,20);
    }

    powerTimer = powerTimer - 1;
    delay (1000);
    
}
