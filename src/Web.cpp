#include <Web.h>

void Web :: getConection(){   
    // Verifica se estamos conectados ao WiFi antes de tentar discovery
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi não conectado - tentando conectar antes do discovery...");
        if (!connectWiFi()) return; // falhou ao conectar
    }

    udp.beginPacket("255.255.255.255", udpPort);
    udp.print("DISCOVER_SERVER");
    udp.endPacket();
    delay(1000);
    int packetSize = udp.parsePacket();
    if (packetSize) {
        char reply[255];
        int len = udp.read(reply, 255);
        if (len > 0) reply[len] = 0;
        Serial.printf("Resposta recebida: %s\n", reply);
        // Se a resposta for a esperada, conecta via TCP ao IP de quem respondeu
        if (String(reply) == "SERVER_ACK") {
            IPAddress serverIP = udp.remoteIP();
            Serial.print("Conectando ao TCP em: ");
            Serial.println(serverIP);

            if (tcpClient.connect(serverIP, tcpPort)) {
                tcpClient.println("Ok");
            }
        }
    }
}

bool Web :: checkConnection(){
    if (!tcpClient.connected()) {
        getConection();
    }
    return tcpClient.connected();
}

void Web :: changeSSID(String ssid){
    this->ssid = ssid;
    // Tenta (re)conectar imediatamente com a nova SSID
}

void Web :: changePassword(String password){
    this->password = password;
    // Tenta (re)conectar imediatamente com a nova senha
}

void Web :: enviaDados(String data){
    tcpClient.println(data);
}

String Web :: receiveData(){
    String data = "";
    if (tcpClient.connected()) {
        // 1. Verifica se há bytes esperando no buffer
        if (tcpClient.available()) {
            // Opção A: Ler a linha inteira (ideal para comandos de texto)
            String comando = tcpClient.readStringUntil('\n'); 
            comando.trim(); // Remove espaços ou \r extras
            Serial.print("Recebido do Java: ");
            data = comando;
        }
    }else{
        getConection();
    }
    return data;
}

bool Web::connectWiFi(unsigned long timeoutMs){
    Serial.print("Conectando ao WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid.c_str(), password.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs) {
        delay(500);
        Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConectado ao WiFi!");
        // Inicializa UDP agora que o WiFi está ativo
        udp.begin(udpPort);
        return true;
    } else {
        Serial.println("\nFalha ao conectar no WiFi (timeout)");
        return false;
    }
}


