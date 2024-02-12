#include <WiringPi.h>
#include <Stepper.h>
#include <iostream>
#include <string>
#include <fstream> //библиотека для работы с файлами

using namespace std;

const uint8_t 
step_moto1 = 14, step_motor3 = 8, //инициализация пинов пошаговых двигателей
//пусть motor1 это двигатель для входной втулки
//motor2 двигатель для выходной втулки
//motor3 двигатель, вращающий ремешёк и двигающий каретку с сервомотором

Ms_all = 11, //MS-пины это пины для установке размера микрошага для каждого драйвера A4987

Dirpin_motor1 = 12, Dirpin_motor3 = 7; //указывает направление вращения для соответствующих моторов

int sol = 3;                 // Определяем номер вывода, к которому подключен силовой ключ
//это должен быть ШИМ-пин

Stepper motor1(200, Dirpin_motor1, step_motor1), motor3(200, Dirpin_motor3, step_motor3); //присвоение переменных классу Stepper
//значение 200 указывает на количество оборотов для прохождения полного круга двигателем (в характеристиках мотора указано 200)
uint8_t coordinate_sol = 1; //координата точки, над которой находится шило, первая точка имеет координату 1

int default_speed = 32;// стандартная скорость для мотора

unsigned int current_points = 0; //напечатанные потенциальные точки
const uint8_t lenth_str = 62;//длина строки
const uint8_t cal_lenth = 9;//количество строк в листе бумаги

string cal_home; //переменная для сохранения строки из файла
const string name = "/home/orangepi/bot/cal.txt"; //имя файла
string total; //переменная, которая содержит все буквы
string new_total;//переменная, которая содержит все буквы в шрифте брайля

int pogr(float steps){
//функция высчитывает погрешность перемещения
	uint16_t hype_steps = 0;
	while(steps >= 1){
		hype_steps++;
        steps--;
    }
	return hype_steps;
}

void motor_turn_on(uint8_t num, float leng, float radious_f, uint8_t where_rotate, uint16_t speed){
//функция предназначена для управления моторами
//num номер мотора к которому мы обращаемся, leng перемещение листа, radious_f радиус валика, where_rotate указывает направление вращения (1 по часовой,0 - против часовой)
    float steps_1of16 = 31.83*(leng/radious_f); //100 / 3.1415 = 31.83, steps_1of16 количество шестнадцатых шага
    if(num==1){
        digitalWrite(Dirpin_motor1, where_rotate); //указываем направление вращения мотора
        
        steps_1of16 *=16; //режим шага 1/16, поэтому 16 дробных шагов равны одному целому
        num = steps_1of16; //за большей ненадобностью переменной num мы можем использовать её для других целей,
        //в частности, для выделения целого числа шагов в режиме драйвера 1/16
        steps_1of16 -= num; //используем переменную steps_1of16 для сохранения дробного шага, который нельзя сделать,
        motor1.setSpeed(speed); //но, когда будет достигнуто такое количество маленьких шагов, что мы сможем сделать хотя бы один - мотор повернётся
        motor1.steps(num + pogr(steps_1of16));
    }
    if(num==3){
        digitalWrite(Dirpin_motor3, where_rotate); //указываем направление вращения мотора
        
        steps_1of16 *=16; //режим шага 1/16, поэтому 16 дробных шагов равны одному целому
        num = steps_1of16; //за большей ненадобностью переменной num мы можем использовать её для других целей,
        //в частности, для выделения целого числа шагов в режиме драйвера 1/16
        steps_1of16 -= num; //используем переменную steps_1of16 для сохранения дробного шага, который нельзя сделать,
        motor3.setSpeed(speed);
        motor3.steps(num + pogr(steps_1of16));
    }
}

void skoof(int sol_speed) {
    analogWrite(sol, sol_speed);     // Подаём на ножку 0V (Соленойд обесточен)
    //первый параметр - номер пина, второй - скорость соленоида
    delay(30);               // Пауза (время на выход штока)   
    analogWrite(sol, 0);   // Подаём на ножку 5V (Соленойд запитан)
    delay(100);               // Пауза (время на возвращение штока) 
}

void take_braille_str(){ //функция для получения строки сос всеми символами
int i = 0;
    while(total[i] != 'n'){
        num_char += total[i];
        i++;
    }
    int num_char_int = stoi(num_char); //количество букв (вместе с пробелами)
    for(i = 0; i< total.size() ; i++){ //(7*int_num_symb+a+1+) количество символов в файле
        if(total[i] == '1' || total[i] == '0' || total[i] == 'm'){
            new_total += total[i];
        }
    }
}

void pechat(int i){ //функция для печати
      if(new_total[i] == '1'){
          skoof(250);
      }
      if(new_total[i] == '0'){
          if(i % 2 == 0){
              motor_turn_on(3, 2.5, 12, 1, default_speed);
          }
          if(i % 2 == 0){
              motor_turn_on(3, 3.5, 12, 1, default_speed);
          }
      }
      if(new_total[i] == 'm'){ //пробел
          motor_turn_on(3, 9.5, 12, 1, default_speed);
      }
}

void perenos(int i){
    if(i % 3 == 0){
            motor_turn_on(1, 3.5, 20, 1, default_speed);
    }
    else{
            motor_turn_on(1, 2.5, 20, 1, default_speed);
    }
    motor_turn_on(3, 5, 12, 1, default_speed);
    motor_turn_on(3, 162.5, 12, 1, default_speed+ 50);
    motor_turn_on(3, 15, 12, 1, default_speed);
}

void new_paper(i){
  //  motor_turn_on(2, 200.0, радиус, 1, default_speed + 50);
    motor_turn_on(1, 200, 20, 1, default_speed);
}

int main(){
  ifstream file_reader;
// объекты класса ifstream являются воплощениями файлов, которые можно только считать

  file_reader.open();

  while (!file_reader.eof()){ // .eof() вернёт значение true, когда обнаружит конец файла
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
  
  pinMode(sol, OUTPUT);    // Назначаем вывод, к которому подключен ключ

  pinMode(pin1_motor1, OUTPUT);
  pinMode(pin2_motor1, OUTPUT);
  
  pinMode(pin1_motor3, OUTPUT);
  pinMode(pin2_motor3, OUTPUT); //установим режимы пинов на подачу напряжения 
  
digitalWrite(Ms_all, HIGH);
  
  int i = 0;
  
  for(; i< etalon_points; i++){
      pechat(i);
      coordinate_sol++;
      if(coordinate_sol%62 == 0){
      perenos(i);
      lenth_str++;
      }
      if(lenth_str%20 == 0){
      new_paper(i);
      }
  }
  motor_turn_on(1, 200.0, 20, 1, default_speed + 50);
}



