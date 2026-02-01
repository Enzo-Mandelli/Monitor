#include <Arduino.h>
#include <Web.h>
class Monitor{
    public:
        Web web;
        String ssid;
        String password;
        Monitor(String ssid, String password){
            this->ssid = ssid;
            this->password = password;
            web.changeSSID(ssid);
            web.changePassword(password);
        }
        bool checkRepeatedVar(void* ptr);
        void beggin();
        unsigned long actualTime = 0;
        unsigned long previousTime = 0;
        void update();
        void addInt(String nome, int* ptr);
        void addFloat(String nome, float* ptr);
        void addBool(String nome, bool* ptr);
        void addString(String nome, String* ptr);
        void addChar(String nome, char* ptr);
        bool wait();
        void convertData(void* ptr, char type, String nome);
        void prepareStatement(String name, String value, String type, String pointer);
        void updateDataToSend();
        String sliceStr(String txt, int index);
        String statement = "";
        int quantVar = 0;
        //statemente order
        //[nameVar1, nameVar2, nameVar3], [value1, value2, value3], [tipo1, tipo2, tipo3], [pointer1, pointer2, pointer3]

};
