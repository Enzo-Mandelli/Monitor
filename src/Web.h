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
          // Não tenta conectar no construtor porque as credenciais
          // podem ser atualizadas por Monitor antes da conexão.
        }

        void getConection();   
        bool checkConnection();
        void enviaDados(String data);
        String receiveData();
        void changeSSID(String ssid);
        void changePassword(String password);
    bool connectWiFi(unsigned long timeoutMs = 10000);
    private:
        // Configurações de Rede
        String ssid = "NOME_DO_WIFI";
        String password = "SENHA";

        // Portas
        const int udpPort = 9192;
        const int tcpPort = 9394;

};
