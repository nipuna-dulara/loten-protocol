
#include <esp_now.h>
#include <WiFi.h>
#define PACKET_PAYLOAD_SIZE 200
// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
String Address = WiFi.macAddress();

struct PacketHeader {
    uint8_t packetNumber;
    uint8_t totalPackets;
      char mac[20];
      bool handshake;
      char key[10];
    // Add any other necessary fields for security, etc.
};
String key = "keyy";
struct Packet {
    PacketHeader header;
    uint8_t payload[PACKET_PAYLOAD_SIZE];
};
typedef struct hi{
  int temp[1000];
} hi;

// Create a struct_message called myData
hi data;


esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
void sendLargeStruct(const void *data, size_t dataSize) {
    // Serialize LargeStruct into byte array
    byte serializedData[dataSize];
     memcpy(serializedData, data, dataSize);

    // Calculate total number of packets needed
    int totalPackets = dataSize / PACKET_PAYLOAD_SIZE + 1;

    // Send packets
    for (int i = 0; i < totalPackets; ++i) {
        Packet packet;
        packet.header.packetNumber = i;
        packet.header.totalPackets = totalPackets;
        packet.header.handshake = true;
        key.toCharArray(packet.header.key, 10);
        Address.toCharArray(packet.header.mac, 20);
        // Copy payload into packet
        int payloadSize = min(PACKET_PAYLOAD_SIZE,(int)  dataSize - i * PACKET_PAYLOAD_SIZE);
        memcpy(packet.payload,serializedData + i * PACKET_PAYLOAD_SIZE, payloadSize);
         Serial.println("this is packet size");
          Serial.println(sizeof(packet));
         Serial.println("this is packet number");
         Serial.println(i);
        // Send packet over datalink protocol
        // sendDataPacket(packet);
          esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &packet, sizeof(Packet));

  if (result == ESP_OK) {
    Serial.println("Sent with success");

  }
  else {
    Serial.println("Error sending the data");
  }
    }
}

 String convertMacAddressToString(uint8_t* macAddress) {
  String macAddressString;
  // Convert each byte of the MAC address array to its hexadecimal representation and concatenate them with ":"
  for (int i = 0; i < 6; i++) {
    macAddressString += String(macAddress[i], HEX);
    if (i < 6 - 1) {
      macAddressString += ":";
    }
  }
  return macAddressString;
}

void convertMacAddress(String macAddressString, uint8_t * address) {
  // Split the string by ":" delimiter
  for (int i = 0; i < 6; i++) {
    // Extract two characters representing a byte
    String byteString = macAddressString.substring(i * 3, i * 3 + 2);

        while (byteString.length() < 2) {
      byteString = "0" + byteString;
    }
    // Convert the hexadecimal string to an integer
    address[i] = (uint8_t)strtol(byteString.c_str(), NULL, 16);
  }
}
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  // Set values to send
 
      for (int i = 0; i < 1000; i++) {
        data.temp[i] = i + 1; // Example values from 1 to 100
    }
sendLargeStruct(&data, sizeof(data));

  delay(2000);
}