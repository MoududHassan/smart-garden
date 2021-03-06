/*
 Created by Rui Santos
 Visit: https://randomnerdtutorials.com for more arduino projects

 Arduino with Ethernet Shield
 */

#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>
int led = 4;
Servo microservo;
int pos = 0;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 192, 168, 0, 190 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 0, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(80);                             //server port    
String readString;
String level;

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  pinMode(led, OUTPUT);
  microservo.attach(7);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {  
      if (client.available()) {
        char c = client.read();
     
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          //Serial.print(c);
         }

         String password = "XXXX";

         //if HTTP request has ended
         if (c == '\n') {          
           Serial.println(readString); //print to serial monitor for debuging

          if( readString.indexOf(password) > 0){
            
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println();    
           client.println("<HTML>");
           client.println("<HEAD>");
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
           client.println("<link rel='stylesheet' type='text/css' href='https://randomnerdtutorials.com/ethernetcss.css' />");
           client.println("<TITLE>Random Nerd Tutorials Project</TITLE>");
           client.println("</HEAD>");
           client.println("<BODY>");
           client.println("<H1>Random Nerd Tutorials Project</H1>");
           client.println("<hr />");
           client.println("<br />");  
           client.println("<H2>Arduino with Ethernet Shield</H2>");
           client.println("<br />");  
           client.println("<a href=\"/?button1on&" + password + "\"\">Pump On</a>");
           client.println("<a href=\"/?button1off&" + password + "\"\">Pump Off</a><br />");  
           client.println("<br />");    
           client.println("<br />");
           
           
           client.println("<p>Created by Rui Santos. Visit https://randomnerdtutorials.com for more projects!</p>");  
           client.println("<br />");
           client.println("</BODY>");
           client.println("</HTML>");

           
             //controls the Arduino if you press the buttons
             if (readString.indexOf("?button1on") >0){
                 if(digitalRead(led) == LOW){
                  Serial.println("from low to high"); 
                  digitalWrite(led, HIGH);
                 }
             }
             if (readString.indexOf("?button1off") >0){
                 if(digitalRead(led) == HIGH){
                  Serial.println("from high to low"); 
                  digitalWrite(led, LOW); 
                 }
             }
  
             if(digitalRead(led) == HIGH){
              client.println("ON"); 
             } else  {
              client.println("OFF"); 
             }
           } else {
            client.println("You are not authorized"); 
           }
           delay(1);
           //stopping client
           client.stop();

            //clearing string for next read
            readString="";  
           
         }
       }
    }
}
}
