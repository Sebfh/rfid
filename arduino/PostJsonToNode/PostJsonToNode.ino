#include <Ethernet.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>

#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield

Adafruit_NFCShield_I2C nfc(IRQ, RESET);
 
byte mac[] = {  0x90, 0xA2, 0xDA, 0x0E, 0xBB, 0x3D };
IPAddress ip(10, 0, 1, 122);			
//IPAddress server(10,0,1,4);
IPAddress server(65,52,128,33);
    
EthernetClient client;

void setup()
  {
    nfc.begin();
    
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (! versiondata) {
      Serial.print("Didn't find PN53x board");
      while (1); // halt
    }
    
    // Set the max number of retry attempts to read from a card
    // This prevents us from waiting forever for a card, which is
    // the default behaviour of the PN532.
    nfc.setPassiveActivationRetries(0xFF);
    
    // configure board to read RFID tags
    nfc.SAMConfig();
    
    if(Ethernet.begin(mac) == 0){
      Serial.println("Failed to configure Ethernet using DHCP");
      // no point in carrying on, so do nothing forevermore:
      for(;;)
        ;
    }
    // print your local IP address:
    Serial.println(Ethernet.localIP());
    
    Serial.begin(115200);
    Serial.println(Ethernet.localIP());
    Serial.println(server);
    delay(1000);
    Serial.println("Waiting for card");

  }
void loop(void)                                           
  {        
    boolean success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;
    String key = "";
    String tpl= "{\"card\":\"##id##\"}"; 

    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);    
    
    if(success){
      Serial.println("Found a card!");
      for (uint8_t i=0; i < uidLength; i++) 
      {
        key.concat(uid[i]); 
      }
      
      tpl.replace("##id##", key);
      Serial.println(tpl);
      
      if (client.connect(server,80))
      {                                                   
        client.println("POST /scan HTTP/1.1");
        Serial.println("POST /scan HTTP/1.1");               
        client.println("Host: rfid-realtime.azurewebsites.net");
        Serial.println("Host: rfid-realtime.azurewebsites.net");
        client.println("Content-Type: application/json");
        Serial.println("Content-Type: application/json");
        client.println("Connection: close");
        Serial.println("Connection: close");
        //client.println("User-Agent: Arduino/1.0");
        //Serial.println("User-Agent: Arduino/1.0");
        client.print("Content-Length: ");
        client.println(tpl.length());
        Serial.print("Content-Length: ");
        Serial.println(tpl.length());
        client.println();
        client.print(tpl);
        Serial.print(tpl);
        client.println();
        Serial.println();

               
      }
      else
      {
        Serial.println("Cannot connect to Server");               
      }
      
//      if (client.available()) {
//        char c = client.read();
//        Serial.print(c);
//      }
      
      client.stop(); 
//      if (!client.connected()) 
//      {
//        Serial.println();
//        Serial.println("disconnecting.");
//        client.stop();
//        for(;;);
//      }   
      
//      delay(1000);
    }                             
  }
