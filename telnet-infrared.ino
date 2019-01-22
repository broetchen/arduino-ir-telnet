#include <SPI.h>
#include <Ethernet.h>
#include <IRremote.h>
#include <string.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 178, 118);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);

IRsend irsend;

char linebuffer[80];
byte lp;
short int eolfound;
char clientchar;
uint32_t ircode;
char buf[100];
char xbuf[100];
char firstrun;
char *q;
 
// telnet defaults to port 23
EthernetServer server(23);
boolean alreadyConnected = false; // whether or not the client was connected previously


void setup() {
  // initialize the ethernet device
  Ethernet.begin(mac, ip, gateway, subnet);
  // start listening for clients
  server.begin();
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.print("infrared server address:");
  Serial.println(Ethernet.localIP());
  memset(linebuffer, '\0', 80);
  lp = 0;
  eolfound=0;
  clientchar=0;
  firstrun=1;
}

void loop() {
  // wait for a new client:
  EthernetClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    if (!alreadyConnected) {
      // clear out the input buffer
      client.flush();
      Serial.println("We have a new client");
      client.println("Hello, client!");
      alreadyConnected = true;
    }
    if (client.available() > 0) {
      if (firstrun){
        firstrun=0;
        while (client.read() != -1){;;}
      }
      eolfound=0;
      lp=0;
      
      while (! eolfound){
        clientchar = -1;
        while ((clientchar == -1) || (clientchar == '\n')){
          clientchar=client.read();
        }
        
        if ((clientchar == '\r') || (lp >= 79)) {
          clientchar = '\0';
          eolfound=1;
        }
        linebuffer[lp]=clientchar;
        lp++;
      } // while eolfound

      if (eolfound){
        if (strncmp(linebuffer, "send", 4) == 0) {
          // send
          memcpy(xbuf,&linebuffer[5],4);
          if (strncmp(xbuf, "nec", 3) == 0) {
            // nec
            memcpy(xbuf,&linebuffer[9],8);
            ircode = strtoul(xbuf, &q, 16);
            irsend.sendNEC(ircode, 32);
            Serial.println("sending NEC command");
          }
        }
        eolfound=0;
     }
    }
  }
}


/*
          sprintf(buf, "Dies ist mein String: %s ", xbuf);
          Serial.println(buf);

 */
