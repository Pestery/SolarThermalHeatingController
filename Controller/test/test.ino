#include <Ethernet.h>
#include <MySQL_Connection.h>
#include <Dns.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

char hostname[] = "localhost:8306"; // change to your server's hostname/URL
char user[] = "dbuser";               // MySQL user login username
char password[] = "userpass";         // MySQL user login password

IPAddress server_ip;
EthernetClient client;
MySQL_Connection conn((Client *)&client);
DNSClient dns_client;   // DNS instance

void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect
  Ethernet.begin(mac_addr);
//   Begin DNS lookup
  dns_client.begin(Ethernet.dnsServerIP());
  dns_client.getHostByName(hostname, server_ip);
  Serial.println(server_ip);
   //End DNS lookup
  Serial.println("Connecting...");
  if (conn.connect(server_ip, 8306, user, password)) {
    delay(1000);
    Serial.println("Success.");
    // You would add your code here to run a query once on startup.
  }
  else
    Serial.println("Connection failed.");
  conn.close();
}

void loop() {
}
