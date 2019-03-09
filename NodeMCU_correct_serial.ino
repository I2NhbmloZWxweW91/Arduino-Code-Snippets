#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


/* for pressure sensor in and out */
//int pressurePin = D2;
//int checkPin = D1;


/* --------------------------------------------------------------
   Setting up stuff to play music
*/
int speakerPin = D7;

int length = 15; // the number of notes

//twinkle twinkle little star
char notes[] = "ccggaag ffeeddc ggffeed ggffeed ccggaag ffeeddc "; // a space represents a rest
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 300;

bool playMusic = false;


void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

/* -------------------------------------------------------------- */



/* LED notifications goes here */
void blinkTwice(){
    digitalWrite(BUILTIN_LED, LOW);
    delay(100);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(100);
    digitalWrite(BUILTIN_LED, LOW);
    delay(100);
    digitalWrite(BUILTIN_LED, HIGH);
}



/* WiFi SSID and PASS */
const char* ssid = "wireless3";
const char* password = "HWHKRER0099";


/* Initialize webserver,responder and html page */
MDNSResponder mdns;
ESP8266WebServer server(80);

String sensorsHTML = "001,002,003,004";        // sample values


void setup(void) {

  /* speaker output */
  pinMode(speakerPin, OUTPUT);

  /* preasure input */
  //pinMode(pressurePin, INPUT);
  //pinMode(checkPin, OUTPUT);
  
  /* for pressure sensor */
  //digitalWrite(pressurePin, HIGH);


  /* Setting up NodeMCU to receive Microbit USB output to RX TX */
  pinMode(0, INPUT);
  pinMode(1, INPUT);

  /* Setting up serial in 9600 */
  delay(1000);
  Serial.begin(115200);
  
  /* Setting up stuff for a static ip */
  IPAddress ip(192, 168, 8, 10);
  IPAddress gateway(192, 168, 8, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress DNS(0, 0, 0, 0);

  /* make the LED pin output and initially turned off */
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(1000);

  /* setting responder */
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }


  /* -----------------------------------
     Connect to Wifi
  */

  WiFi.config(ip, gateway, subnet, DNS);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");

  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /* ------------------------------------- */


  /* start webserver */
  server.on("/", []() {
    server.send(200, "text/html", sensorsHTML); //sending sensor input to root directory
  });

  server.on("/play_music", []() {
    server.send(200, "text/html", "Music is playing now...!"); 

    /* blink twice */
    blinkTwice();

    playMusic = true;

  });

  server.on("/stop_music", []() {
    server.send(200, "text/html", "Music stoped!!");

    /* blink twice */
    blinkTwice();

    playMusic = false;

  });

  server.begin();
  Serial.println("server started!");

}




void loop(void) {


  
  String incomingByte = "";

  /* update page if only serial received */
  if (Serial.available() > 0) {

    incomingByte = Serial.readString(); //read the microbit input as a string
    Serial.print("received from MicroBit: ");  
    Serial.println(incomingByte); //print the received val
    
    /* get from preasure sensor */
    //int pressureInput = digitalRead(pressurePin);
 
    //Serial.print("received from pressure input: ");
    //Serial.println(pressureInput); //print the received val
    
    /* passing microbit + pressure input to sensor html page */
    sensorsHTML = incomingByte;   
    Serial.println(sensorsHTML);
    
    digitalWrite(BUILTIN_LED, LOW);
    delay(100);
    digitalWrite(BUILTIN_LED, HIGH);

  }
  server.handleClient();




  /* if user requested play music */
  if (playMusic) {
    for (int i = 0; i < length; i++) {
      if (notes[i] == ' ') {
        delay(beats[i] * tempo); // rest
      } else {
        playNote(notes[i], beats[i] * tempo);
      }

      // pause between notes
      delay(tempo / 2);
    }
    playMusic = false;
  }



}

