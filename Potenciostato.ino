// Script starts 
#include <Separador.h> 
Separador s; // Library to separate the obtained results from the smartphone 
// Circuit variables definition  
int a = 9; // PWM output pin, 254 resolution bits  
int ap = 10; // +5V supply for opamp 
int an = 7; // ICL7660 supply, turns +5V to -5V for opamp 
int a1 = 12; // ICL7660 supply, turns +5V to -5V  
int a2 = 2; // +5V for Bluetooth module 
int R2 = 330; // Used in operating potential window equation 
int R3 = 510;  
int R4 = 200; 
int val ;  
float ct = A0; // ADC analog input pin, 1024 resolution bits 
float c; // Current in bits 
float cc; 
// Redefinition of variables obtained from the smartphone  
float Estep; 
int t; 
int t_print;  
//  
void setup() { 
  TCCR1B = TCCR1B & B11111000 | B00000001; // Changes pin 9 frequency to 3.1kHz 
  Serial.begin(9600); 
  pinMode(a,OUTPUT); // Output pin 
  pinMode(a1,OUTPUT);  
  pinMode(a2,OUTPUT);  
  pinMode(ap,OUTPUT);  
  pinMode(an,OUTPUT);  
  pinMode(ct,INPUT); // Input pin, current lecture 
  delay(2500);  
  digitalWrite(an,1); // Negative supply of opamp 
  digitalWrite(ap,1); // Positive supply of opamp 
  delay(30000); 
  digitalWrite(a2,1); // Bluetooth on 
  delay(5000); // Time (ms) 
} 
// 
void loop() { 
  if (Serial.available()){ 
    String e = Serial.readString(); 
    String e1 = s.separa(e,',',0); 
    String e2 = s.separa(e,',',1); 
    String e3 = s.separa(e,',',2); 
    String e4 = s.separa(e,',',3); 
    String e5 = s.separa(e,',',4); 
    String e6 = s.separa(e,',',5); 
    String e7 = s.separa(e,',',6);
    String e8 = s.separa(e,',',7);  
    int ee1 = e1.toInt(); // E0 mV 
    int ee2 = e2.toInt(); // Ei mV 
    int ee3 = e3.toInt(); // Ef mV 
    int ee4 = e4.toInt(); // Scan speed mV/s (vb) 
    int ee5 = e5.toInt(); // Cycle number 
    int ee6 = e6.toInt(); // Estep mV and time_step ms for chronoamperometry 
    int ee7 = e7.toInt(); // Time of potential application  
    int tech = e8.toInt(); // Electrochemical technique number 
    float Eii = ee2/1000.0; // I
    float Ei = (Eii+(200.0/510.0)*4.70); // Transformation of the initial 
    int ti = Ei*(255.0/(5.5*(200.0/330.0))); 
    float Ei0 = ee1/1000.0;  
    float E0 = (Ei0+(200.0/510.0)*4.70);  
    int start = E0*(255.0/(5.5*(200.0/330.0)));  
    float Ef0 = ee3/1000.0; // Input of end potential, the greater number from -1.5V to 1.5 V 
    float Ef = (Ef0+(200.0/510.0)*4.7); // Transformation of the end potential 
    int fin = Ef*(255.0/(5.5*(200.0/330.0))); // Final potential, Arduino 
    int vb = ee4; // Scan rate, mV/s, max = 1280 mV/s 
    int const count = ee5; // Cycles number 
    float Potstep = ee5/255.0; // Fixed due to the DAC resolution 
    long t=(5000000L)/(256L*vb);  
    int time_tot = ee7*-1000; // Time (ms) 
    int tot_time = time_tot/ee6; 
    int val;  
    // Electrochemical methods 
    switch (tech){  
      // ------ Chronoamperometry------ 
      case 1:   
      digitalWrite(a1,1);  
      for(int n = 1; n < tot_time; n++){ 
        Serial.print("#c"); 
        Serial.print(1); 
        Serial.print(";"); 
        Serial.print(ti); 
        Serial.print(";"); 
        analogWrite(a,ti); 
        c = analogRead(ct);  
        cc = (0.472*(analogRead(ct))-226.1);  
        Estep = (1.843-(val*0.014379)); 
        Serial.print(c); 
        Serial.print(";"); 
        Serial.print(n*ee6); 
        Serial.print(";"); 
        Serial.println(cc); 
        int sec = ee6*1000; 
        delay(sec); 
      }  
      break; 
      // ------Linear voltammetry------ 
      case 2:  
      digitalWrite(a1,1); 
      // Anodic scan  
      if (Ei < Ef) {  
        for(int n = 1; n <= count; n++){ 
          for(int val = ti; val < fin; val++){ 
            analogWrite(a,val); 
            Serial.print("#c"); 
            Serial.print(1);
                        Serial.print(";"); 
            Serial.print(val); 
            Serial.print(";"); 
            c = analogRead(ct);  
            cc = (0.472*(analogRead(ct))-226.1);  
            Estep = (1.843-(val*0.014379)); 
            Serial.print(c); 
            Serial.print(";"); 
            Serial.print(Estep); 
            Serial.print(";"); 
            Serial.println(cc); 
            delay(t); 
          } 
        }   
      } 
// Cathodic scan  
      else if (Ei >= Ef) {  
        digitalWrite(a1,1);  
        for(int n = 1; n <= count; n++){ 
          for(int val = ti; val > fin; val--){ 
            analogWrite(a,val); 
            Serial.print("#c"); 
            Serial.print(1); 
            Serial.print(";"); 
            Serial.print(val); 
            Serial.print(";"); 
            c = analogRead(ct); 
            cc = (0.472*(analogRead(ct))-226.1);  
            Estep = (1.843-(val*0.014379)); 
            Serial.print(c); 
            Serial.print(";"); 
            Serial.print(Estep); 
            Serial.print(";"); 
            Serial.println(cc); 
            delay(t); 
          } 
        } 
      } 
      break; 
      // ------Cyclic voltammetry------ 
      case 3:  
      digitalWrite(a1,1);  
      // Anodic scan 
      if (Ei < Ef) {  
        for(int n = 1; n <= count; n++){ 
          // Start the forward scan 
          for(int val = ti; val < fin; val++){ 
            analogWrite(a,val); 
            Serial.print("#c"); 
            Serial.print(1); 
            Serial.print(";"); 
            Serial.print(val); 
            Serial.print(";"); 
            c = analogRead(ct);  
            cc = (0.472*(analogRead(ct))-226.1);  
            Estep = (1.843-(val*0.014379)); 
            Serial.print(c); 
            Serial.print(";"); 
            Serial.print(Estep); 
            Serial.print(";"); 
            Serial.println(cc); 
            delay(t); 
          } 
          // Start the reverse scan 
          for(int val = fin; val > start; val--){ 
            analogWrite(a,val); 
            Serial.print("#c"); 
            Serial.print(1); 
            Serial.print(";"); 
            Serial.print(val); 
            Serial.print(";"); 
            c = analogRead(ct); 
            cc = (0.472*(analogRead(ct))-226.1);  
            Estep = (1.843-(val*0.014379)); 
            Serial.print(c); 
            Serial.print(";"); 
            Serial.print(Estep); 
            Serial.print(";"); 
            Serial.println(cc); 
            delay(t); 
          }   
         ti = start; 
        } 
      } 
      // Cathodic scan  
      else if (Ei >= Ef) {  
        for(int n = 1; n <= count; n++){ 
          //Start the forward scan 
          for(int val = ti; val > fin; val--){ 
            analogWrite(a,val); 
            Serial.print("#c"); 
            Serial.print(1); 
            Serial.print(";"); 
            Serial.print(val); 
            Serial.print(";"); 
            c = analogRead(ct); 
            cc = (0.472*(analogRead(ct))-226.1);  
            Estep = (1.843-(val*0.014379)); 
            Serial.print(c); 
            Serial.print(";"); 
            Serial.print(Estep); 
            Serial.print(";"); 
            Serial.println(cc); 
            delay(t); 
          } 
          //Start the reverse scan 
          for(int val = fin; val < start; val++){ 
            analogWrite(a,val); 
            Serial.print("#c"); 
            Serial.print(1); 
            Serial.print(";"); 
            Serial.print(val); 
            Serial.print(";"); 
            c = analogRead(ct); 
            cc = (0.472*(analogRead(ct))-226.1);  
            Estep = (1.843-(val*0.014379)); 
            Serial.print(c); 
            Serial.print(";"); 
            Serial.print(Estep); 
            Serial.print(";"); 
            Serial.println(cc); 
            delay(t); 
          } 
         ti=fin; 
        } 
      }
      break; 
    } 
  } 
} 
//Script ends  
