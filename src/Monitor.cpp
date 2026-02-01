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
            typeVar = "string";
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

void Monitor::prepareStatement(String name, String value, String type, String pointer) {
    // Captura o que já existe
    String names = sliceStr(statement, 0);
    String values = sliceStr(statement, 1);
    String types = sliceStr(statement, 2);
    String ptrs = sliceStr(statement, 3);

    // Só adiciona vírgula se oldN não estiver vazio
    String sep = (names.length() > 0) ? "," : "";

    statement = "[" + names + sep + name + "]" +
                "[" + values + sep + value + "]" +
                "[" + types + sep + type + "]" +
                "[" + ptrs + sep + pointer + "]\n";
}
String Monitor::sliceStr(String txt, int index) {
    int openBracket = -1;
    int closeBracket = -1;
    int currentIdx = 0;

    for (int i = 0; i < txt.length(); i++) {
        if (txt.charAt(i) == '[') {
            if (currentIdx == index) openBracket = i;
        } else if (txt.charAt(i) == ']') {
            if (currentIdx == index) {
                closeBracket = i;
                break;
            }
            currentIdx++;
        }
    }

    if (openBracket != -1 && closeBracket != -1) {
        return txt.substring(openBracket + 1, closeBracket);
    }
    return "";
}

bool Monitor :: checkRepeatedVar(void* ptr){
    String enderecoHex = "0x" + String((uintptr_t)ptr, HEX);
    String data = sliceStr(statement, 3); // Pega a parte dos ponteiros
    if (data.length() == 0) return false;
    String ptrAux = "";
    for(int i = 0; i < data.length(); i++){
        char c = data.charAt(i);
        if(c != ','){
            ptrAux += c;
        }
        if(c == ',' || i == data.length() - 1){
            if(ptrAux.equals(enderecoHex)){
                return true;
            }
            ptrAux = "";
        }
    }
    return false;
}

void Monitor :: beggin(){
    web.getConection();
}

void Monitor :: addInt(String nome, int* ptr){
    if(checkRepeatedVar(ptr))return;
    convertData(ptr, 'i', nome);
    quantVar++;
}

void Monitor :: addFloat(String nome, float* ptr){
    if(checkRepeatedVar(ptr))return;
    convertData(ptr, 'f', nome);
    quantVar++;
}

void Monitor :: addBool(String nome, bool* ptr){
    if(checkRepeatedVar(ptr))return;
    convertData(ptr, 'b', nome);
    quantVar++;
}

void Monitor :: addChar(String nome, char* ptr){
    if(checkRepeatedVar(ptr))return;
    convertData(ptr, 'i', nome);
    quantVar++;
}

void Monitor :: addString(String nome, String* ptr){
    if(checkRepeatedVar(ptr))return;
    convertData(ptr, 's', nome);
    quantVar++;
}

void Monitor::updateDataToSend() {
    String pointersStr = sliceStr(statement, 3);
    String typesStr = sliceStr(statement, 2);
    String newValues = "";
    
    int ptrStart = 0;
    int typeStart = 0;

    for (int i = 0; i < quantVar; i++) {
        int ptrComma = pointersStr.indexOf(',', ptrStart);
        int typeComma = typesStr.indexOf(',', typeStart);

        String currentPtrHex = (ptrComma == -1) ? pointersStr.substring(ptrStart) : pointersStr.substring(ptrStart, ptrComma);
        String currentType = (typeComma == -1) ? typesStr.substring(typeStart) : typesStr.substring(typeStart, typeComma);

        uintptr_t pointer = strtoul(currentPtrHex.c_str(), NULL, 16);
        
        // Só processa se o ponteiro for válido
        if (pointer != 0) {
            // Adiciona vírgula ANTES, exceto no primeiro item
            if (newValues.length() > 0) newValues += ",";

            switch (currentType.charAt(0)) {
                case 'i': newValues += String(*(int*)pointer); break;
                case 'f': newValues += String(*(float*)pointer); break;
                case 'b': newValues += (*(bool*)pointer ? "true" : "false"); break;
                case 'c': newValues += String(*(char*)pointer); break;
                case 's': newValues += *(String*)pointer; break;
            }
        }

        // Atualiza índices
        ptrStart = ptrComma + 1;
        typeStart = typeComma + 1;
        if (ptrComma == -1) break; 
    }

    String names = sliceStr(statement, 0);
    String types = sliceStr(statement, 2);
    String pointers = sliceStr(statement, 3);
    statement = "[" + names + "][" + newValues + "][" + types + "][" + pointers + "]\n";
}

void Monitor::update() {
    //data order [pointer],[data], [type];
    if (wait()) {
        updateDataToSend();
        web.enviaDados(statement);
        if (web.checkConnection()) {
            previousTime = millis();
            String payload = web.receiveData(); 
            if (payload.length() == 0) return; 
            if(payload.equals("__check__")){
                web.enviaDados("__online__\n");
                return;
            }
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
