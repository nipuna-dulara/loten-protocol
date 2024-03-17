
#include <esp_now.h>
#include <WiFi.h>
#define PACKET_PAYLOAD_SIZE 200
#define MAX_PACKETS 100
// Structure example to receive data
// Must match the sender structure
struct PacketHeader {
    uint8_t packetNumber;
    uint8_t totalPackets;
      char mac[20];
      bool handshake;
      char key[10];
    // Add any other necessary fields for security, etc.
};
uint8_t Addr[6];
bool handShaking = false; 
struct Packet {
    PacketHeader header;
    uint8_t payload[PACKET_PAYLOAD_SIZE];
};
typedef struct hi{
  int temp[1000];
} hi;
struct ReceivedData {
    int totalPackets;
    int receivedPackets;
    hi data;
};

ReceivedData receivedData;
// Create a struct_message called myData
Packet myData;
void processReceivedData(const hi &data) {
    // Process the received data here
    // For example, print it
    Serial.println("Received data:");
    // Assuming you have a print function for hi struct
    // for(int i = 0; i < 1000 ; i++ ){
    //   Serial.println(data.temp[i]);
    // }
}
void receivePacket(const Packet &packet) {
    // Assuming the received packet is stored in a structure named Packet
    int packetNumber = packet.header.packetNumber;
    int totalPackets = packet.header.totalPackets;

    // Ensure packet number is within bounds
    if (packetNumber >= MAX_PACKETS) {
        Serial.println("Packet number out of bounds");
        return;
    }

    // Copy payload into received data buffer
    int payloadSize = min(PACKET_PAYLOAD_SIZE,(int) sizeof(hi) - packetNumber * PACKET_PAYLOAD_SIZE);
    memcpy(((byte *)&receivedData.data) + packetNumber * PACKET_PAYLOAD_SIZE, packet.payload, payloadSize);

    // Update received packets count
    receivedData.receivedPackets++;

    // Check if all packets have been received
    if (receivedData.receivedPackets == totalPackets) {
        // All packets received, process the data
        processReceivedData(receivedData.data);

        // Reset receivedData for next transmission
        receivedData.totalPackets = 0;
        receivedData.receivedPackets = 0;
        memset(&receivedData.data, 0, sizeof(hi));
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
// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  convertMacAddress(myData.header.mac ,Addr);
  if(myData.header.handshake == true){
    handShaking == true;
    Serial.println("handshaking");
    Serial.println(myData.header.key);
  }
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("packet number: ");
  Serial.print(myData.header.packetNumber);
  receivePacket(myData);
  
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

}
