#include <Arduino.h>
#include <Web.h>
class Monitor{
    public:
        Web web;
        Monitor(){
            web.getConection();
        }
        int contVar = 0;
        void update();
        void addInt(String nome, int* ptr);
        void addFloat(String nome, float* ptr);
        void addBool(String nome, bool* ptr);
        void addString(String nome, String ptr);
        void addChar(String nome, char* ptr);
        String convertData(void* ptr, char type, String nome);
        String sliceStr(String txt, int index);
        String prepareStatement(String name, String value, String type);
        String statement = "";
        //statemente order
        //[nameVar1, nameVar2, nameVar3], [value1, value2, value3], [tipo1, tipo2, tipo3], [pointer1, pointer2, pointer3]

};
