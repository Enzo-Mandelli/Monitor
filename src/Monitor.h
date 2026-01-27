#include <Arduino.h>
#include <Web.h>
class Monitor{
    public:
        Web web;
        Monitor(String ssid, String password){
            web.changeSSID(ssid);
            web.changePassword(password);
            web.getConection();
        }
        unsigned long actualTime = 0;
        unsigned long previousTime = 0;
        void update();
        void addInt(String nome, int* ptr);
        void addFloat(String nome, float* ptr);
        void addBool(String nome, bool* ptr);
        void addString(String nome, String* ptr);
        void addChar(String nome, char* ptr);
        bool wait(); //vai esperar o delayTime
        void convertData(void* ptr, char type, String nome);
        void prepareStatement(String name, String value, String type, String pointer);
        String sliceStr(String txt, int index);
        String statement = "";
        //statemente order
        //[nameVar1, nameVar2, nameVar3], [value1, value2, value3], [tipo1, tipo2, tipo3], [pointer1, pointer2, pointer3]

};
