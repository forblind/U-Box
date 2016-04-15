#include <EEPROMex.h>
#include <EEPROMVar.h>


#define MOTOR 2    // Вибро-мотор analogRead  4
#define TRIG 3     // Триггер датчика расстояния  1

#define ECHO 1     // Ответный сигнал датчика расстояния  3
#define BUTTON 0   // Кнопка переключения режимов 2
#define BATT 4    // Вход АЦП для отслеживания заряда аккумуллятора  - 0

#define pulse_ms 100  /* Длительность импульса мотора (мс) */

#define frame_rate 3000     // Время, в течении которого пользователь должен ввести команду (мс)
#define bounce_rate 10      // Интервал ожидания окончания дребезга контактов (мс)
#define scan_rate 50  // Интервал опроса кнопки (мс)

int L1 = 50;
int L2 = 100;
int L3 = 150;

int X1 = L1;
int X2 = L2;
int X3 = L3;

bool flag = 0;
bool flag1 = 0;
bool flag11 = 0;
bool flag2 = 0;
bool flag22 = 0;
bool flag3 = 0;
bool flag41 = 1;
bool flag42 = 1;
bool flag411 = 0;
bool flag422 = 0;
char flag5 = 0;
bool flag6 = 0;
bool flag7 = 0;
bool flag8 = 0;


#define delta  3

int dist = 0;

int L1a;
int L2a;
int L3a;

int L1b;
int L2b;
int L3b;

int low = 0;
int t1st = 0;
int dist1 = 0;
int pulse1_stp = 0;
int Level1 = 0;

int count  = 0; // Счетчик для ответных импульсов по нажатию кнопки
int count1 = 0; // Счетчик для ответных импульсов по нажатию кнопки
int count2 = 0; // Счетчик для батареи
int count3 = 0; // Счетчик для работы разрядки батарейки
int count4 = 0; // Счетчик для опрашивания батарейки и соника
int count5 = 0; // Счетчик для передачи импульсов с соника
int count6 = 0;
int count7 = 0;
int count8 = 0;
int count9 = 0;
int count10 = 0;
bool count_fading = 0; // Счетчик для возобновления оповещения низкого заряда батарейки


int level;

unsigned int button_scan = 0;  // Основной счётчик для кнопки
unsigned int frame = 0;        // Счётчик временного окна ввода команды

int fading = 200; // Скважность импульса
bool time_for_button = 0;  // Условие для приостановки других функций, если кнопка была нажата
bool time_for_batt = 0;

char battl = 0;  // Уровень заряда батарейки

#define reminder 60000 // Напоминание о разряженной батарейки (Период в мс) 10000

char order = 0;        // Команда
bool pressed = false;  // Флаг нажатия на кнопку
bool released = true;  // Флаг отпускания кнопки
bool input = false;    // Флаг начала ввода

#define addres 125

char readed = 0;
char order1 = 0;

int dynamic = 205;
bool opros = 0;
bool opros1 = 0;
char value;

void setup() 
{

  DDRB = 0x00; //INICIALIZACIA PORTA
  PORTB = 0x01; //PULLUP P2
  
  pinMode(BUTTON, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BATT, OUTPUT);
  
  Timer0_Init(64, 1);
  pulse1_stp = 1;  

}



void Timer0_Init(int prescaler1, int prescaler2)
{
  switch (prescaler1)
  {

    case (64):     TCCR0B = 0<<CS02 | 1<<CS01 | 1<<CS00; break;   // CLK

  }
  TIMSK = 1<<TOIE0;  // Overflov interrupt enabled
  t1st = 255 - prescaler2;
  TCNT0 = t1st;       // Start count value
  sei();
}


ISR(TIMER0_OVF_vect)
{
  TCNT0 = t1st;
  
  button_scan++;
  frame++;
  count2++;
  count3++;
  count4++;
  count5++;

    if ((count_fading == 1) && (count == 0))
    {
      count6++;
    }
  
   if ((order == 0) && count > 0)
   {
    count1 = count1 + 1;  
   }

   if (flag7 == 1) count10++;
   if (count10 >= 2000) {flag7 = 0; count10 = 0; opros1 = 1;}
  
   if ((battl == 2) && (count3 >= reminder)) 
    {
      if (flag8 == 0){ value = dynamic; flag8 = 1;}
      
      dynamic = 0;
        
        if (count2 == 70)  
        {

           if ((fading <=200) && fading >80)   fading = fading - 5;
           if (fading <=80)                    fading = fading - 2;
           count2 = 0;
        }
    }

    if ((count3 >= reminder) && (fading <= 22)) {fading = 200;   count3 = 0; dynamic = value; flag8 = 0; time_for_batt = 0;}
    if (count3 == (reminder + 4000))                            count3 = 0;
    if (count2 == 73)                                           count2 = 0; 
    if (count4 == 101)                                          count4 = 0;
    if (count6 == 1001)                                         count6 = 0;
    if (count7 == 10000)                                        count7 = 3001;


    if (flag3 == 1) count7++;

    
    if ((time_for_button == 0) && (opros1 == 1)){
        switch(pulse1_stp)
        {
          case (0):  analogWrite(BATT, 0);  break;
          case (1):  analogWrite(BATT, dynamic); count5 = 0; pulse1_stp++; break;
          case (2):  (count5 >= pulse_ms) ? (digitalWrite(BATT, 0), count5 = 0, pulse1_stp++) : (0); break;
          case (3):  (count5 >= low) ? (pulse1_stp = 1) : (0); break;    
        }
    }
    

     if (flag3 == 1) count7++;
    
    if ((count7 >= 3001) && (flag41 == 0))  // Условиие для уменьшения интенсивности вибрации
    {
        if (count2 == 70)  
        {
         dynamic = dynamic - 2;
         count2 = 0;
        }
        if (dynamic <= 45) {flag411 = 1;  dynamic = 0; flag41 = 1; count7 = 3001;}
    }

    
    if (flag411 == 1)    {count8 = count8 + 1;}
    if (count8 >= 500) {flag42 = 1; dynamic = 45; flag411 = 0; count8 = 0;}
    

    if ((count7 >= 3001) && (flag42 == 1))  // Условиие для увеличения интенсивности вибрации
    {
        if (count2 == 70)  
        {
         dynamic = dynamic + 2;
         count2 = 0;
        }
        if (dynamic >= 254) {flag422 = 1; dynamic = 0; flag42 = 0; count7 = 3001;}
    }
    
    if (flag422 == 1)    {count9 = count9 + 1; }
    if (count9 >= 500) {flag41 = 0; dynamic = 255;  flag422 = 0; count9 = 0;}
    
}



int GetDist (int trig, int echo)
{
  int dist;
  if (time_for_button == 0)
  {
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW); 
    dist = pulseIn(echo, HIGH)/58;     
    return(dist);
  }
}

void GetLevel (int &dist, int &level)
{
  if (time_for_button == 0)
  {

          L1a = L1 + delta;
          L2a = L2 + delta;
          L3a = L3 + delta;

          L1b = L1 - delta;
          L2b = L2 - delta;
          L3b = L3 - delta;


        if ((dist <= L1a) && (flag == 0))                 {X1 = L1a;}
        if ((dist <= L1b) && (flag == 1))                 {X1 = L1a;           flag = 0;}
        if ((dist > L1a) && (dist < L2a) && (flag != 1))  {X1 = L1b; X2 = L2a; flag = 1;}

        if ((dist <= L2a) && (flag1 == 0))                {X2 = L2a;}
        if ((dist <= L2b) && (flag1 == 1))                {X2 = L2a;           flag1 = 0; flag11 = 0;}
        if ((dist > L2a) && (dist < L3a) && (flag1 != 1)) {X2 = L2b; X3 = L3a; flag1 = 1; flag11 = 1;}

        if ((dist <= L3a) && (flag2 == 0))                {X3 = L3a;}
        if ((dist <= L3b) && (flag2 == 1))                {X3 = L3a;           flag2 = 0; flag22 = 0;}
        if ((dist > L3a) && (flag2 != 1))                 {X3 = L3b;           flag2 = 1; flag22 = 1;}        



    (dist >= 0  && dist < X1) ? (level = 1):
    (dist >= X1 && dist < X2) ? (level = 2):
    (dist >= X2 && dist < X3) ? (level = 3):  
    (dist >= X3) ? (level = 0) : (level = 0);
  }
}



void GetInterval (int level, int &low, int &stp)
{
  switch(level)
  {
    case (1):  low = pulse_ms*1; break;
    case (2):  low = pulse_ms*4; break;
    case (3):  low = pulse_ms*7; break;    
  }

  if (level != 0 && stp == 0)    stp = 1;
  else if (level == 0)           stp = 0;     
}

void ButtonScan()
{  
  if(button_scan >= scan_rate)  // Если пришло время проверить кнопку
  {
    if(!pressed)                // Если кнопка находилась в ненажатом состоянии
    {
      if(!digitalRead(BUTTON))   // Если сейчас кнопка нажата
        pressed = true;         // то взводим флаг нажатия
      else
      {
        button_scan = 0;        // Если кнопка не нажата - сбрасываем счётчик кнопки
      }
    }
    else if(button_scan >= (scan_rate + bounce_rate))    // Если кнопка находилась в нажатом состоянии и задержка от дребезга прошла
    {
      input = true;
      
      time_for_button = 1; count3 = 0;
      if (flag6 == 0)  analogWrite (BATT, 0); 
      level = 0;
      flag3 = 1; flag5 = 2;


       if (button_scan >= frame_rate)
       {
          flag6 = 1;
          button_scan = frame_rate;
          analogWrite (BATT, dynamic);
       }
   
      if(!digitalRead(BUTTON) && (released))  // Если кнопка нажата и флаг отпускания кнопки установлен (кнопка счиаталась отпущенной) и спустя задержку кнопка всё ещё нажата
      {        
        released = false;      // сбрасываем флаг "отпущенности" кнопки
      }     
      else if(digitalRead(BUTTON))  // Если кнопка не нажата
      {
        if(!released)  // Если флаг "отпущенности" сброшен, т.е. кнопку нажали, выдержали дребезг и отпустили
        {
          flag3 = 0; count7 = 0; flag41 = 1; flag42 = 1;
          
          if (button_scan >= frame_rate)
          {
             if (flag5 == 2) {flag5 = 0; EEPROM.write(150, dynamic);}
             fading = 200;
             time_for_button = 0;
             flag6 = 0;
          }
          else
          {
            if(!input)       // Если находимся не в режиме ввода команды  
            {
              frame = 0;     // то чистим счётчик окна ввода команды
              input = true;  // выставляем флаг, говорящий о том, что выполнен вход в режим ввода команды        
            }        
            order++;         // засчитываем нажатие и плюсуем счётчик команд 
  
            if (order > 3)
            {
              order = 3;          
            }
          }
        }
        button_scan = 0;       // Сбрасываем счётчик
        pressed = false;       // Кнопка нажата - нет
        released = true;       // Кнопка отпущена - да
      }
      
    }

    if(!input){    // Если не в режиме ввода
      frame = 0;  // то сбрасываем счётчик окна ввода команды
    }
    else if(frame >= frame_rate)    // Если был произведён вход в режим ввода 
    {
      for(; order > 0; order--)
      {
        count++;  order1++;
      }  
      order = 0;       // сбрасываем её
      input = false;   // выходим мз режима ввода команды
    }
  }

  if (flag5 == 0)
        {
          dynamic = EEPROM.read(150); flag5 = 1;
        }

  while (count != 0)
  {
    count_fading = 1;
    if (count1 == 150){ analogWrite (BATT, 255);}
    if (count1 == 300){ analogWrite (BATT, 0); count1 = 0; count = count - 1;}
  }  

    if ((count_fading == 1) && (count == 0))
    {
        EEPROM.write(addres, order1); 
        readed = EEPROM.read(addres); 

      if (count6 == 1000)
      {

          switch (order1)
        {
        
          case (1):  L1 = 25; L2 = 50; L3 = 75; X1 = L1; X2 = L2; X3 = L3; break;
          case (2):  L1 = 50; L2 = 100; L3 = 150; X1 = L1; X2 = L2; X3 = L3; break;
          case (3):  L1 = 100; L2 = 200; L3 = 300; X1 = L1; X2 = L2; X3 = L3; break;    
        }

        fading = 200;
        count_fading = 0;
        time_for_button = 0;
        count6 = 0; order1 = 0;
      }
    }
}


float getBatt()
{
  float batt = analogRead(1)*5.0/1024;
  return(batt);
}


void playBatt()
{
  float battery = getBatt();

  if (battery <= 3.0)       battl = 2;
  else                      battl = 1;
   
    if ((battl == 2) && (count3 >= reminder))    {time_for_batt = 1; analogWrite (BATT,fading);}
   // else if ((count1 == 0) && (pulse1_stp == 0))  analogWrite (BATT, 0);
 }


void ScanBatt()
{
  float battery = getBatt();

        if (battery >= 3.6)       count = 5;
        else if (battery >= 3.3)  count = 4;
        else if (battery >= 3.1)  count = 3; 
        else if (battery >= 2.8)  count = 2; 
        else                      count = 1;
      
       
          while (count != 0)
          {
            if (count1 == 300){ analogWrite (BATT, dynamic);}
            if (count1 == 600){ analogWrite (BATT, 0); count1 = 0; count = count - 1;}
          } 
          
          if (count == 0)   {opros = 1; flag7 = 1;}
}

void loop() 
{
  
    if (opros == 0) 
    {
        dynamic = EEPROM.read(150);
        ScanBatt();
    }
    if (opros1 == 1)
    {
    
      ButtonScan(); 
      if (time_for_button == 0){ 
        playBatt();
         if (time_for_batt == 0)
         {
            if (count4 == 100){
              dist1 = GetDist(TRIG, ECHO);
              GetLevel(dist1, Level1);
              GetInterval(Level1, low, pulse1_stp);
              count4 = 0;
            }
         }
      }
    }
}

