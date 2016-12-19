#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// TCP socket
WiFiServer server(8000); 

// Connection to target network established
bool connected = false;

void setup() {
  
  Serial.begin(115200);
  Serial.println("hello");

  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP8266", "12345678");

  server.begin();
}

void loop() {
  String msg, ssid, password;

  if(receive_message(msg, '<', '>')) { // <SSID;pw>
    parse_credentials(msg, ';', ssid, password);

    Serial.println("Received new credentials:");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);

    connected = connect_to_wifi(ssid.c_str(), password.c_str());
  }

  if(connected) update_service();
  delay(4000);
}

/*
 * Listens for incoming TCP connections and reads the stream. 
 * Once a full message is received (delimited with a special start and end char)
 * it is returned.
 * 
 * returns
 *    true    full message received and written to msg string
 *    false   no full message received yet
 */
bool receive_message(String &msg, char startChar, char stopChar) {
  static WiFiClient client;
  static String readString = "";
  
  if (!client.connected()) {
    client = server.available();
    return false;
  } 
  
  while (client.available()) {
    readString += char(client.read());
  }

  // remove before last startChar (incl.)
  int startIdx = readString.lastIndexOf(startChar);
  if(startIdx + 1 < readString.length()) {
    readString = readString.substring(startIdx + 1, readString.length());
  } else {
    readString = "";
  }

  // get string until first stopChar and remove from readString
  int endIdx = readString.indexOf(stopChar);
  if(endIdx == -1) return false;

  msg = readString.substring(0, endIdx);
  readString = readString.substring(endIdx, readString.length());
  
  return true;  
}

/*
 * Parse message string containing credentials separated with a special char.
 */
void parse_credentials(const String &msg, char separator, String &ssid, String &password) {
  int idx = msg.indexOf(separator);
  if(idx > -1) {
    ssid = msg.substring(0, idx);
    password = msg.substring(idx + 1, msg.length());
  } else {
    ssid = msg;
    password = "";
  }
}

/*
 * Connect to WiFi access point.
 * 
 * return
 *    true   if successful connected
 *    false  if connection couldn't be established (10s timeout) 
 */
bool connect_to_wifi(const char* ssid, const char* password) {
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(++count > 20) {
      Serial.println("Couldn't connect to WiFi");
      return false;
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

/*
 * Contacts web service with MAC address as ID. Should be
 * called at least every 5 seconds to be displayed as online.
 * 
 * returns
 *    0   if successful
 *   -1   if there was no wifi connection
 *   -2   if the connection to the host failed
 *   -3   if the service returned a failure
 */
int update_service() {
  const char* host = "tiipwilab.eng.au.dk";
  const char* url = "/exercises/group5/app.php";
  const int httpsPort = 443;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("not connected to wifi");
    connected = false;
    return -1;
  }
  
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    connected = false;
    return -2;
  }

  Serial.print("requesting URL: ");
  Serial.println(url);

  String post_data = "id=" + WiFi.macAddress();

  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n" + 
               "Content-Type: application/x-www-form-urlencoded;\r\n" +
               "Content-Length: " + post_data.length() + "\r\n" +
               "\r\n" +
               post_data + "\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("updated")) {
    Serial.println("service update successfull!");
    return 0;
  } else {
    Serial.println("service update has failed");
    Serial.println("reply was:");
    Serial.println("==========");
    Serial.println(line);
    Serial.println("==========");
    return -3;
  }
  Serial.println("closing connection");
}
