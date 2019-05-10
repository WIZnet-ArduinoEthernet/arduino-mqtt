// This example uses an Arduino MKR together with
// an MKR Ethernet Shield to connect to test.mosquitto.org via IPv6
//
// by taylor
// 20190509
//
// Original
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <Ethernet.h>
#include <IP6Address.h>
#include <MQTT.h>

// Enter a MAC address and IP address for your controller below.
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

byte ip6_lla[] = {
0xfe, 0x80, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x02, 0x00, 0xdc, 0xff,
0xfe, 0x57, 0x57, 0x61
};

byte ip6_gua[] = {
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};

byte ip6_sn6[] = {
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};

byte ip6_gw6[] = {
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};

// https://developers.google.com/speed/public-dns/docs/using
// 2001:4860:4860::8888
// 2001:4860:4860::8844

byte ip6_dns6[] = {
0x20, 0x01, 0x48, 0x60,
0x48, 0x60, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x88, 0x88
};

IP6Address ip(192, 168, 1, 177);
IP6Address myDns(192, 168, 1, 1);
IP6Address gateway(192, 168, 1, 1);
IP6Address subnet(255, 255, 255, 0);

IP6Address lla(ip6_lla, 16);
IP6Address gua(ip6_gua, 16);
IP6Address sn6(ip6_sn6, 16);
IP6Address gw6(ip6_gw6, 16);
IP6Address dns6(ip6_dns6, 16);

EthernetClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("connecting...");
  while (!client.connect("clientId-WIZnet")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/W6100");
  // client.unsubscribe("/W6100");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
      while (!Serial) {
          ; // wait for serial port to connect. Needed for native USB port only
      }

  // start the Ethernet connection:
  Serial.println("Trying to get an IP address using DHCP");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // initialize the Ethernet device not using DHCP:
    Ethernet.begin(mac, ip, myDns, gateway, subnet, lla, gua, sn6, gw6);
  }

  Ethernet.setDnsServerIP(ip6_dns6);

  Serial.println("==================================================================");
  Serial.println("Network Information");
  Serial.println("==================================================================");
  Serial.print("IPv4 ADR: "); Serial.println(Ethernet.localIP());
  Serial.print("IPv6 LLA: "); Serial.println(Ethernet.linklocalAddress());
  Serial.print("IPv6 GUA: "); Serial.println(Ethernet.globalunicastAddress());
  Serial.print("IPv6 GAW: "); Serial.println(Ethernet.gateway6());
  Serial.print("IPv6 SUB: "); Serial.println(Ethernet.subnetmask6());
  Serial.print("IPv6 DNS: "); Serial.println(Ethernet.dnsServerIP());
  Serial.println("==================================================================");

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.

  Serial.println("To test.mosquitto.org");
  client.begin("test.mosquitto.org", net);

  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("/W6100", "Hello, W6100!");
  }
}
