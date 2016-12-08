#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

void setup() {
  Serial.begin(115200);
  Serial.println();
    
  connect_to_wifi("SSID", "password");
}

void loop() {
  update_service();
  delay(4000);
}

/*
 * Connect to WiFi access point.
 */
void connect_to_wifi(const char* ssid, const char* password) {
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
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
    return -1;
  }
  
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
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
