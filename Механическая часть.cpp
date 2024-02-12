#include <iostream>
#include <string>
#include <fstream> //библиотека для работы с файлами
#include <wiringPi.h>
#include "Stepper.h"
using namespace std;

const int 
STEP1 = 14, STEP2 = 8, MS = 11, DIR1 = 12, DIR2 = 7; 

int sol = 3;                 

Stepper motor1(200, Dirpin_motor1, step_motor1);
Stepper motor3(200, Dirpin_motor3, step_motor3);
int coordinate_sol = 1; 

int speed = 60;

int str = 0;
const int lenth_str = 62;
const int cal_lenth = 90;

string cal_home; 
const string name = "/home/orangepi/bot/cal.txt"; 
string total; 
string new_total;

int pogr(float steps){
	int hype_steps = 0;
	while(steps >= 1){
		hype_steps++;
        steps--;
    }
	return hype_steps;
}

void MOT(int num, float leng, float radious_f, int where_rotate, int speed){
    float steps_1of16 = 31.83*(leng/radious_f);
    if(num==1){
        digitalWrite(Dirpin_motor1, where_rotate);
        
        steps_1of16 *=16; 
        num = steps_1of16; 
        steps_1of16 -= num; 
        motor1.setSpeed(speed);
        motor1.steps(num + pogr(steps_1of16));
    }
    if(num==3){
        digitalWrite(Dirpin_motor3, where_rotate); 
        
        steps_1of16 *=16; 
        num = steps_1of16; 
        steps_1of16 -= num; 
        motor3.setSpeed(speed);
        motor3.steps(num + pogr(steps_1of16));
    }
}

void skoof(int sol_speed) {
    analogWrite(sol, sol_speed);    
    delay(30);              
    analogWrite(sol, 0);   
    delay(100);              
}

void take_braille_str(){ 
int i = 0;
    while(total[i] != 'n'){
        num_char += total[i];
        i++;
    }
    int num_char_int = stoi(num_char); 
    for(i = 0; i< total.size() ; i++){ 
        if(total[i] == '1' || total[i] == '0' || total[i] == 'm'){
            new_total += total[i];
        }
    }
}

void pechat(int i){ 
      if(new_total[i] == '1'){
          skoof(250);
      }
      if(new_total[i] == '0'){
          if(i % 2 == 0){
              MOT(3, 2.5, 12, 1, speed);
          }
          if(i % 2 == 0){
              MOT(3, 3.5, 12, 1, speed);
          }
      }
      if(new_total[i] == 'm'){ 
          MOT(3, 9.5, 12, 1, speed);
      }
}

void perenos(int i){
    if(i % 3 == 0){
            MOT(1, 3.5, 20, 1, speed);
    }
    else{
            MOT(1, 2.5, 20, 1, speed);
    }
    MOT(3, 5, 12, 1, speed);
    MOT(3, 162.5, 12, 1, speed *2);
    MOT(3, 15, 12, 1, speed);
}

int main(){
  ifstream file_reader;


  file_reader.open();

  while (!file_reader.eof()){
  getline(file_reader, cal_home);
  int a = 0;
  if(a == 0){
      getline(file_reader, etalon_points);
  }
  a++;
  total+=cal_home;
  }
  
  file_reader.close();

  take_braille_str();
  
  pinMode(DIR1, OUTPUT);
  pinMode(STEP1, OUTPUT);
  
  pinMode(DIR2, OUTPUT);
  pinMode(STEP2, OUTPUT);
  
  pinMode(sol, OUTPUT);


  
digitalWrite(MS, HIGH);
  
  int i = 0;
  
  for(; i< etalon_points; i++){
      pechat(i);
      coordinate_sol++;
      if(coordinate_sol%62 == 0){
      perenos(i);
      str++;
      }
      if(str%cal_lenth == 0){
     MOT(1, 200, 20, 1, speed);
      }
  }
  MOT(1, 200.0, 20, 1, speed * 2);
}
