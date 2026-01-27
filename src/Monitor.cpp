#include <Monitor.h>
#include <Arduino.h>

void Monitor::convertData(void* ptr, char type, String nome) {
    String typeVar;
    
    // Convertemos o endereço (ptr) para uma String Hexadecimal para o Java
    String enderecoHex = "0x" + String((uintptr_t)ptr, HEX);

    switch (type) {
        case 'i': {
            typeVar = "int";
            int* ptrTyped = (int*)ptr;
            // Usamos *ptrTyped para o valor e enderecoHex para o local na memória
            prepareStatement(nome, String(*ptrTyped), typeVar, enderecoHex);
            break;
        }
        case 'b': {
            typeVar = "bool";
            bool* ptrTyped = (bool*)ptr;
            prepareStatement(nome, (*ptrTyped ? "true" : "false"), typeVar, enderecoHex);
            break;
        }
        case 'f': {
            typeVar = "float";
            float* ptrTyped = (float*)ptr;
            prepareStatement(nome, String(*ptrTyped), typeVar, enderecoHex);
            break;
        }
        case 's': {
            typeVar = "String";
            String* ptrTyped = (String*)ptr;
            prepareStatement(nome, *ptrTyped, typeVar, enderecoHex);
            break;
        }
        case 'c': {
            typeVar = "char";
            char* ptrTyped = (char*)ptr;
            prepareStatement(nome, String(*ptrTyped), typeVar, enderecoHex);
            break;
        }
        default:
            break;
    }
}

void Monitor :: prepareStatement(String name, String value, String type, String pointer){
    //statement is formed by [names] + [values] + [types] + [pointer]
    String placeHolder = statement;
    name = sliceStr(statement, 0) + "," + name;
    value = sliceStr(statement, 1) + "," + value;
    type = sliceStr(statement, 2) + "," + type;
    pointer = sliceStr(statement, 3) + "," + pointer;
    statement = "[" + name + "],[" + value + "],[" + type + "]" + "[" + pointer + "]";
}

String Monitor :: sliceStr(String txt, int index){
    String lista = "";
    int cont = index + 1;
    for(int i = 1; i < txt.length(); i++){
        if(txt.charAt(i) == ']') cont++;
        if(!txt.charAt(i) == '[' && cont == index && !txt.charAt(i) == ']'){
           lista = lista + txt.charAt(i);
        }
    }
    return lista;
}

void Monitor :: beggin(){
    web.getConection();
}

void Monitor :: addInt(String nome, int* ptr){
    convertData(ptr, 'i', nome);
}

void Monitor :: addFloat(String nome, float* ptr){
    convertData(ptr, 'f', nome);
}

void Monitor :: addBool(String nome, bool* ptr){
    convertData(ptr, 'b', nome);
}

void Monitor :: addChar(String nome, char* ptr){
    convertData(ptr, 'i', nome);
}

void Monitor :: addString(String nome, String* ptr){
    convertData(&ptr, 's', nome);
}

void Monitor::update() {
    //data order [pointer],[data], [type];
    if (wait()) {
        if (web.checkConnection()) {
            previousTime = millis();
            
            String payload = web.receiveData(); 
            if (payload.length() == 0) return; // Segurança contra pacotes vazios

            uintptr_t pointer = strtoul(sliceStr(payload, 0).c_str(), NULL, 0);
            String valueStr = sliceStr(payload, 1); // Valor recebido
            String typeStr  = sliceStr(payload, 2); // Tipo do dado

            // Segurança: Se o ponteiro for nulo, não tenta gravar na memória (evita crash)
            if (pointer == 0) return;

            switch (typeStr.charAt(0)) {
                case 'i': {
                    int* ptrTyped = (int*)pointer;
                    *ptrTyped = valueStr.toInt();
                    break;
                }
                case 'b': {
                    bool* ptrTyped = (bool*)pointer;
                    // Aceita 't' (true), '1' ou 'L' (Ligado)
                    *ptrTyped = (valueStr == "t" || valueStr == "1");
                    break;
                }
                case 'f': {
                    float* ptrTyped = (float*)pointer;
                    *ptrTyped = valueStr.toFloat();
                    break;
                }
                case 's': {
                    String* ptrTyped = (String*)pointer;
                    *ptrTyped = valueStr; // Atribuição direta funciona para objetos String
                    break;
                }
                case 'c': {
                    char* ptrTyped = (char*)pointer;
                    *ptrTyped = valueStr.charAt(0);
                    break;
                }
            }
        }
    }
}

bool Monitor :: wait(){
    actualTime = millis(); // Pega o tempo agora
    // Verifica se o tempo decorrido é maior ou igual ao intervalo
    if (millis() - previousTime >= web.delayTime) return true;
    return false; // Ainda não deu o tempo
}
