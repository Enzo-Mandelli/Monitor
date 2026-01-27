#ifdef ESP32
  #include <WiFi.h>
  #include <WiFiUdp.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h> // No ESP8266, o nome é o mesmo, mas o conteúdo muda internamente
#endif
#include <Arduino.h>

class Web{
    public:
        WiFiUDP udp;
        WiFiClient tcpClient;
        int delayTime = 1000; //delay time between requisitions 
        Web(){
            WiFi.begin(ssid, password);
            while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
            }
            Serial.println("\nConectado ao WiFi!");
            udp.begin(udpPort);
        }

        void getConection();   
        bool checkConnection();
        void enviaDados(String data);
        String receiveData();
        void changeSSID(String ssid);
        void changePassword(String password);
    private:
        // Configurações de Rede
        const char* ssid = "NOME_DO_WIFI";
        const char* password = "SENHA";

        // Portas
        const int udpPort = 9192;
        const int tcpPort = 9394;

};
