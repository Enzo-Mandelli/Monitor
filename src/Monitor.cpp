#include <Monitor.h>
#include <Arduino.h>

void Monitor :: convertData(void* ptr, char type, String nome){
    bool rightType = false;
    String typeVar;
    //types i = int; b = bool; f = float; s = String; c = char
    switch (type){
    case 'i':{
        typeVar = "int";
        int* ptrTyped = (int*)ptr;
        prepareStatement(nome, String(*ptrTyped), typeVar);
        break;
    }
    case 'b':{
        typeVar = "bool";
        bool* ptrTyped = (bool*)ptr;
        prepareStatement(nome, String(*ptrTyped), typeVar);
        break;
    }
    case 'f':{
        typeVar = "float";
        float* ptrTyped = (float*)ptr;
        prepareStatement(nome, String(*ptrTyped), typeVar);
        break;
    }
    case 's':{
        typeVar = "String";
        String* ptrTyped = (String*)ptr;
        prepareStatement(nome, String(*ptrTyped), typeVar);
        break;
    }
    case 'c':{
        typeVar = "char";
        char* ptrTyped = (char*)ptr;
        prepareStatement(nome, String(*ptrTyped), typeVar);
        break;
    }
    default:
        break;
    }

}

void Monitor :: prepareStatement(String name, String value, String type){
    //statement is formed by [names] + [values] + [types]
    String placeHolder = statement;
    name = sliceStr(statement, 0) + "," + name;
    value = sliceStr(statement, 1) + "," + value;
    type = sliceStr(statement, 2) + "," + type;
    statement = "[" + name + "],[" + value + "],[" + type + "]";
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

void Monitor :: update(){
    //data order [pointer],[data], [type];
    if(wait()){
        if(web.checkConnection()){
            previousTime = millis();
            // O '0' no final do strtoul detecta automaticamente se é decimal ou hexa (0x)

            uintptr_t pointer = strtoul(sliceStr(web.receiveData(), 0).c_str(), NULL, 0);

            String type = sliceStr(web.receiveData(), 2);// procura o tipo de dado
            switch (type.charAt(0)){// o switch precisa de um char, então seleciona o primeiro caracter que e o unico necessario
                case 'i':{
                    int* ptrTyped = (int*)pointer;
                    *ptrTyped = (sliceStr(web.receiveData(), 1)).toInt();
                    break;
                }
                case 'b':{
                    bool* ptrTyped = (bool*)pointer;
                    if(sliceStr(web.receiveData(), 1) == "t"){
                        *ptrTyped = 1;
                    }else{
                        *ptrTyped = 0;
                    }
                    break;
                }
                case 'f':{
                    float* ptrTyped = (float*)pointer;
                    *ptrTyped = (sliceStr(web.receiveData(), 1)).toFloat();
                    break;
                }
                case 's':{
                    String* ptrTyped = (String*)pointer; // Ponteiro para o OBJETO String
                    //*ptrTyped = String(sliceStr(web.receiveData(), 1));
                    break;
                }
                case 'c':{
                    char* ptrTyped = (char*)pointer;
                    *ptrTyped = (sliceStr(web.receiveData(), 1)).charAt(0);
                    break;
                }
                default:
                    break;
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
