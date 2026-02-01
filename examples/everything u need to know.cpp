#include <Arduino.h>
#include <Monitor.h>
int a = 1;
int b = 0;
float c = 0;
char z = 'l';
Monitor monitor("SSID(wifi name)", "PASSWORD"); // the constructor only needs the SSID and the password to prepare the connection

//YOUR COMPUTER AND YOUR ESP32 MUST BE IN THE SAME NETWORK
//this software should work with esp12 also but i didn't test it

void setup() {
  Serial.begin(9600);
  //if u want to see if the conection is working properly it will print o Serial what it is doing until the connection happens
  //so it is recomended that u use Serial.begin()
  monitor.beggin();//if u do, remember to use before this line
  //this is the line that start the connection and the process needed
  // currently u can only add this 3 types
  monitor.addFloat("c" , &c); 
  monitor.addInt("aabc", &a); 
  monitor.addInt("dudv", &b); 
  monitor.addChar("l", &z);
  //the parameter will always be the name you want to show and a pointer to the variable
  //so just put an & in the back of the variable name
  //so it will always be monitor.addSomething("name to show", &pointerToTheVar)


  monitor.web.delayTime = 0; //IT IS JUST AN EXAMPLE DON'T DO IT
  //it has an delay time so it doesn't update all the time, but u change it if u want
  //don't recommend making it more than 10sec or instant like i just did here, but do what u want
}

void loop() {
  //you can put the add on the main loop but please don't, it slow down a lot your code
  a++;
  b--;
  c = 100000/a;
  delay(1000);//it isn't needed 

  //======= very important =======
  monitor.update(); //update the data on the interface and the values here so u need to put it on the main loop
  // ============================

  //just to test if u can monitor char and yes u do but u can't change the char sorry about that :/
  if(z == 'l'){
    z = 'h';
  }else{
    z = 'l';
  }
  //u just need to import it put some lines of code and run the software
  //https://github.com/Enzo-Mandelli/Monitor_Interface/blob/master/Monitor.jar
  //it is a java app so it should work fine on your device

}



