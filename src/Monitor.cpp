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
    statement = "[" + name + "][" + value + "][" + type + "]" + "[" + pointer + "]\n";
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
    String data = sliceStr(statement, 3);
    String ptrAux = "";
    for(int i = 0; i < data.length(); i++){
        if(data.charAt(i) != ','){
            ptrAux = ptrAux + data.charAt(i);
        }else{
            if(ptrAux.equals(enderecoHex)){
                return true;
            }
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
    convertData(&ptr, 's', nome);
    quantVar++;
}

void Monitor::updateDataToSend() {
    String pointersStr = sliceStr(statement, 3);
    String typesStr = sliceStr(statement, 2);
    String newValues = "";
    
    int ptrStart = 0;
    int typeStart = 0;

    for (int i = 0; i < quantVar; i++) {
        //get the colma index
        int ptrComma = pointersStr.indexOf(',', ptrStart);
        int typeComma = typesStr.indexOf(',', typeStart);

        // get the adress and the type
        String currentPtrHex = (ptrComma == -1) ? pointersStr.substring(ptrStart) : pointersStr.substring(ptrStart, ptrComma);
        String currentType = (typeComma == -1) ? typesStr.substring(typeStart) : typesStr.substring(typeStart, typeComma);

        // Convert str to ptr
        uintptr_t pointer = strtoul(currentPtrHex.c_str(), NULL, 16); // Base 16 para Hex

        if (pointer != 0) {
            switch (currentType.charAt(0)) {
                case 'i': {
                    int* p = (int*)pointer;
                    newValues += String(*p);
                    break;
                }
                case 'f': {
                    float* p = (float*)pointer;
                    newValues += String(*p);
                    break;
                }
                case 'b': {
                    bool* p = (bool*)pointer;
                    newValues += (*p ? "true" : "false");
                    break;
                }
                case 'c': {
                    char* p = (char*)pointer;
                    newValues += String(*p);
                    break;
                }
                case 's': {
                    String* p = (String*)pointer;
                    newValues += (*p);
                    break;
                }
            }
        }

        //add colmas
        if (i < quantVar - 1) {
            newValues += ",";
        }

        // update the index
        ptrStart = ptrComma + 1;
        typeStart = typeComma + 1;
        
        // error check
        if (ptrComma == -1 && i < quantVar - 1) break;
    }

    // update statement
    String names = sliceStr(statement, 0);
    String types = sliceStr(statement, 2);
    String pointers = sliceStr(statement, 3);
    
    statement = "[" + names + "][" + newValues + "][" + types + "][" + pointers + "]\n";
}

void Monitor::update() {
    //data order [pointer],[data], [type];
    if (wait()) {
        web.enviaDados(statement);
        updateDataToSend();
        if (web.checkConnection()) {
            previousTime = millis();
            String payload = web.receiveData(); 
            if (payload.length() == 0) return; // Segurança contra pacotes vazios
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
