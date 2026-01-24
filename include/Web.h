#include <WiFi.h>
#include <WiFiUdp.h>

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

    private:
        // Configurações de Rede
        const char* ssid = "NOME_DO_WIFI";
        const char* password = "SENHA";

        // Portas
        const int udpPort = 9478;
        const int tcpPort = 8080;

};