///////////////////////////////////////////////////////////////////////////////////////
//Terms of use
///////////////////////////////////////////////////////////////////////////////////////
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////////////

// Hardware Notes:

#include <global.h>
#include <base64.h>
#include <WebSocketClient.h>
#include <WebSocketServer.h>
#define _WIFININA_LOGLEVEL_       1
#include <WiFiNINA_Generic.h>

#include <FastLED_NeoPixel.h>

// Console Attached
#ifndef TerminalAttached
    // true = terminal attached (send serial messages - for debugging), false = no terminal attached (no messages - production)
    #define TerminalAttached  false
#endif

// user definable to what ever you want
#define SECRET_SSID "Fireworks Controller"
#define SECRET_PASS "dhs2000001"

// global vars
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
const short webPort     = 80;
const short socketPort  = 8080;
int status = WL_IDLE_STATUS;


// fireworks vars
int selectedRow = 0;
int selectedStatus = 0;


// Background
byte sectimer = 0;
bool heatbeat = false;
uint32_t loop_timer;
int timer_counter = 0;
float timer_value = 0;


// Background
boolean Backgroundinit = false;

// WiFi
WiFiServer      WebServer(webPort);
WiFiServer      socketServer(socketPort);
WebSocketServer webSocketServer;
WiFiClient      socketClient;

// Dragon Smoker webpage, gzipped and base64 encoding to save space
char webpage_base64[] = "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";



// ** FUNCTIONS
// print wifi status
void printWifiStatus()
{
    if(TerminalAttached) {
        Serial.print("SSID: "); Serial.println(WiFi.SSID());
        Serial.print("Signal strength (RSSI): "); Serial.print(WiFi.RSSI()); Serial.println(" dBm");
        Serial.print("IP address: "); Serial.println(WiFi.localIP());
        Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
        Serial.print("Netmask: "); Serial.println(WiFi.subnetMask());
        Serial.print("Webpage is at http://"); Serial.print(WiFi.localIP()); Serial.println("/");
        Serial.print("Websocket is at http://"); Serial.print(WiFi.localIP()); Serial.println(":" + (String)socketPort + "/");
    }
}


void allOff() {
                            // Row
  digitalWrite(15, LOW);    // 1
  digitalWrite(16, LOW);    // 2
  digitalWrite(17, LOW);    // 3
  digitalWrite(18, LOW);    // 4
  digitalWrite(20, LOW);    // 5
  digitalWrite(21, LOW);    // 6
  digitalWrite(0, LOW);     // 7
  digitalWrite(1, LOW);     // 8
  digitalWrite(2, LOW);     // 9
  digitalWrite(3, LOW);     // 10
}

void plusePin(int pin) {
  // pin 99 is really digital pin 0  
  if( pin == 99) {
    pin = 0;
  }
  digitalWrite(4, HIGH);    // status led
  digitalWrite(pin, HIGH);  // selected row / pin
  delay(1000);
  digitalWrite(4, LOW);
  digitalWrite(pin, LOW);
}


// ** SETUP
void setup() {
    // initialize digital pins
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // LED ON to indicate no wifi connection yet
    pinMode(15, OUTPUT);    // set the pin mode, Row 1
    pinMode(16, OUTPUT);    // set the pin mode, Row 2
    pinMode(17, OUTPUT);    // set the pin mode, Row 3
    pinMode(18, OUTPUT);    // set the pin mode, Row 4
    pinMode(20, OUTPUT);    // set the pin mode, Row 5
    pinMode(21, OUTPUT);    // set the pin mode, Row 6
    pinMode(0, OUTPUT);     // set the pin mode, Row 7
    pinMode(1, OUTPUT);     // set the pin mode, Row 8
    pinMode(2, OUTPUT);     // set the pin mode, Row 9
    pinMode(3, OUTPUT);     // set the pin mode, Row 9
    allOff();

    pinMode(4, OUTPUT);     // Status LED
    digitalWrite(4, HIGH); // LED ON to indicate no wifi connection yet

    if(TerminalAttached) {
        Serial.begin(9600);
          delay(50);
    }
    
    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
        if(TerminalAttached) {
         Serial.println("Communication with WiFi module failed!");
        }

        // don't continue
        while (1) {
        };
    }
    
    // check and make sure we are using the lastest Firmware
    String fv = WiFi.firmwareVersion();
    if(TerminalAttached) {
       Serial.println("Version " + String(WIFININA_GENERIC_VERSION));

        // check and make sure we are using the lastest Firmware
        if (fv < WIFI_FIRMWARE_LATEST_VERSION)
        {
            Serial.print("Your current firmware NINA FW v");
            Serial.println(fv);
            Serial.print("Please upgrade the firmware to NINA FW v");
            Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
        }
    }

    // print the network name (SSID);
    if(TerminalAttached) {
       Serial.print("Creating access point named: ");
       Serial.println(ssid);
    }
    
    // Start the Access Point
    status = WiFi.beginAP(ssid, pass);
    if (status != WL_AP_LISTENING) {
        if(TerminalAttached) {
           Serial.println("Creating access point failed");
        }
    }
    
    // start the web and socket servers
    WebServer.begin();
    socketServer.begin();

    if(TerminalAttached) {
       Serial.println("Board Initialization Complete:");
    }
}


// ** MAIN LOOP
void loop() {
    // general purpose var
    unsigned int tempVar = 0;



    // Background Process 1 - process the web interface
    // compare the previous status to the current status
    if (status != WiFi.status()) {
        // it has changed update the variable
        status = WiFi.status();
    
        if (status == WL_AP_CONNECTED) {
            // a device has connected to the AP
            if(TerminalAttached) {
             Serial.println("Device connected to AP");
            }
            printWifiStatus();
        } else {
            // a device has disconnected from the AP, and we are back in listening mode
            if(TerminalAttached) {
               Serial.println("Device disconnected from AP");
            }
            // make sure the socket is also stopped, in case we want to reconnect
            socketClient.stop();
        }
    }
    
    WiFiClient client = WebServer.available();   // listen for incoming clients

    if(client) {
        if(TerminalAttached) {
           Serial.print("New client: ");
           Serial.print(client.remoteIP());
           Serial.print(" : ");
           Serial.println(client.remotePort());
        }
        String header = "";
    
        if(client.connected()) {            // loop while the client's connected
            while(client.available())
            {
                char ch = client.read();
        
                if (ch != '\r') 
                {
                header += ch;
                }
            
                if (header.substring(header.length() - 2) == "\n\n") 
                {
                if(TerminalAttached) {
                  Serial.print(header.substring(0, header.length() - 1));
                }
              
                if (header.indexOf("GET / HTTP") > -1) 
                {
                    // send the WEB Page to the browser
                    const int webpage_base64_length = sizeof(webpage_base64);
                    const int webpage_gz_length = base64_dec_len(webpage_base64, webpage_base64_length);
                    char webpage_gz[webpage_gz_length];
                    base64_decode(webpage_gz, webpage_base64, webpage_base64_length);
                    int packetsize = 1024;
                    int done = 0;
                    
                    client.println("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Encoding: gzip\n");
                
                    while (webpage_gz_length > done) 
                    {
                        if (webpage_gz_length - done < packetsize) {                    
                            packetsize = webpage_gz_length - done;
                        }
                  
                        client.write(webpage_gz + done, packetsize * sizeof(char));
                        done = done + packetsize;
                    }
                    if(TerminalAttached) {
                      Serial.println("--Interface webpage sent");
                    }
                } 
                else 
                {
                    client.println("HTTP/1.1 404 Not Found\nContent-Type: text/plain; charset=utf-8\n\n404 Not Found\n");
                    if(TerminalAttached) {
                      Serial.println("--Page not found");
                    }
                }
              
                client.stop();
                if(TerminalAttached) {
                  Serial.println("--Client disconnected");
                }
            }
          }
        }
    }


    // wait for the javascript to start the socket connection
    if(!socketClient.connected()) {
        // we are not connect to a socket so keep listening
        socketClient = socketServer.available();
    
        if (socketClient.connected() && webSocketServer.handshake(socketClient)) {
            if(TerminalAttached) {
               Serial.print("\n--Websocket connected to: ");
               Serial.print(socketClient.remoteIP());
               Serial.print(":");
               Serial.println(socketClient.remotePort());
            }
        } else {
            socketClient.stop();
        }
    } else {
        // we have a good socket so process the connection
        // Background Init - setup the background tasks, runs only once
        if(Backgroundinit == false) {
            // clear out any existing socket data
            String data = webSocketServer.getData();
            if(TerminalAttached) {
               Serial.println("Websocket Flushed");
            }

            if(TerminalAttached) {
               Serial.println("Background Init Complete");
            }
        
            // indicate that the we are ready to process commands
            Backgroundinit = true;
        }
    
        // see if we have a command/request from the user 
        String data = webSocketServer.getData();
        if (data.length() > 0) {
            // get tools to parse incoming request
            char buf[data.length()+1];
            data.toCharArray(buf, sizeof(buf));
            char *token;
            char *pter = buf;
            byte i = 0;
            String cmd;
            String usrVal; 
            while ((token = strtok_r(pter, ":", &pter))) {
                switch(i) {
                    case 0:
                        cmd = token;
                    break;
                    case 1:
                        usrVal = token;
                    break;
                }
                i++;
            }
            if(TerminalAttached) {
               Serial.println("CMD: " + String(cmd));
               Serial.println("UsrVal: " + String(usrVal));
            }
    
            // process command
            if (cmd == "rst"){
                selectedStatus = 0;
                selectedRow = 0;
                webSocketServer.sendData("C:0");
            } else if (cmd == "rw1"){
                selectedStatus = 0;
                selectedRow = 15;
                webSocketServer.sendData("R:1");
            } else if (cmd == "rw2"){
                selectedRow = 16;
                webSocketServer.sendData("R:2");
            } else if (cmd == "rw3"){
                selectedStatus = 0;
                selectedRow = 17;
                webSocketServer.sendData("R:3");
            } else if (cmd == "rw4"){
                selectedStatus = 0;
                selectedRow = 18;
                webSocketServer.sendData("R:4");
            } else if (cmd == "rw5"){
                selectedStatus = 0;
                selectedRow = 20;
                webSocketServer.sendData("R:5");
            } else if (cmd == "rw6"){
                selectedStatus = 0;
                selectedRow = 21;
                webSocketServer.sendData("R:6");
            } else if (cmd == "rw7"){
                selectedStatus = 0;
                selectedRow = 99; // need to fake this row number out
                webSocketServer.sendData("R:7");
            } else if (cmd == "rw8"){
                selectedStatus = 0;
                selectedRow = 1;
                webSocketServer.sendData("R:8");
            } else if (cmd == "rw9"){
                selectedStatus = 0;
                selectedRow = 2;
                webSocketServer.sendData("R:9");
            } else if (cmd == "rw10"){
                selectedStatus = 0;
                selectedRow = 3;
                webSocketServer.sendData("R:10");
            } else if (cmd == "arm") {
                if(selectedRow == 0) {
                    selectedStatus = 0;
                    webSocketServer.sendData("S:0"); // no row
                } else {
                    selectedStatus = 1;
                    webSocketServer.sendData("S:1"); // armed
                }
            } else if (cmd == "fire"){
                if(selectedStatus == 0) {
                    webSocketServer.sendData("S:2"); // not armed
                } else {
                    plusePin(selectedRow);
                    webSocketServer.sendData("S:3"); // fire
                }
            } else {
                webSocketServer.sendData("E:" + cmd);
            }
        }
    }


    // Background Process 4 - make a 1 second timer
    switch(sectimer) {
        case 0:
            loop_timer = micros();
            sectimer = 1;
        break;
        case 1:
            // 1 second has passed so do functions
            if(micros() - loop_timer > 1000000) {                
                // flash the heartbeat
                if(heatbeat == false) {
                    digitalWrite(LED_BUILTIN, LOW);
                    digitalWrite(4, LOW);
                    heatbeat = true;
                } else {
                   digitalWrite(LED_BUILTIN, HIGH);
                   digitalWrite(4, HIGH);
                    heatbeat = false;
                }

                // send the timer_value count every 5 seconds
                if(timer_counter++ > 5) {
                    // send the timer update
                    if(socketClient.connected()) {
                        if(timer_value) {
                            if(timer_value > 60) {
                                tempVar = timer_value / 60;
                            } else {
                                tempVar = timer_value;
                            }
                            webSocketServer.sendData("T:" + String(timer_value / 60));
                        }
                    }

                    // reset the 5 second clock
                    timer_counter = 0;
                }

                // reset the timer
                sectimer = 0;
            }
        break;
    }
}
