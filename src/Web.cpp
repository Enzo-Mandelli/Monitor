#include <Web.h>

void Web :: getConection(){   
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
                tcpClient.println("Ola Servidor! Conexao TCP estabelecida.");
            }
        }
    }
}

bool Web :: checkConnection(){
    bool connected = false;
    if(!tcpClient.connected()){
        getConection();
        connected = true;
    }
    return connected;
}

void Web :: changeSSID(String ssid){
    this.ssid = ssid;
}

void Web :: changePassword(String password){
    this.password = password;
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


