/*
При публичном размещении кода ссылка на первоисточник обязательна.
*/

#define btn_long_push 1000   // Длительность долинного нажатия кнопки
volatile uint8_t lastcomb=7, enc_state, btn_push=0;
volatile int enc_rotation=0, btn_enc_rotate=0;
volatile boolean btn_press=0;
volatile uint32_t timer;
#define btn_new 500


//********************************
void setup() 
{
  pinMode(A1,INPUT_PULLUP); // ENC-A
  pinMode(A2,INPUT_PULLUP); // ENC-B
  pinMode(A3,INPUT_PULLUP); // BUTTON

  PCICR =  0b00000010; // PCICR |= (1<<PCIE1); Включить прерывание PCINT1
  PCMSK1 = 0b10001101; // Разрешить прерывание для  A1, A2, A3
  
  Serial.begin(115200);
}

//****************************************
void loop() 
{
  switch (enc_state)
    {
  case 1:   {
            Serial.print("Вращение без нажатия ");
            Serial.println(enc_rotation);
            }
   break;
   
  case 2:  {
            Serial.print("Вращение с нажатием ");
            Serial.println(btn_enc_rotate);  
           }
    break;
    
  case 3: Serial.println("Нажатие кнопки ");
  break;

  case 4: Serial.println("Длинное нажатие кнопки ");
  break;
    }
  enc_state=0; //обнуляем статус энкодера
}

//****************************************
ISR (PCINT1_vect) //Обработчик прерывания от пинов A1, A2, A3
{
  uint8_t comb = bitRead(PINC, 3) << 2 | bitRead( PINC, 2)<<1 | bitRead(PINC, 1); //считываем состояние пинов энкодера и кнопки

 if (comb == 3 && lastcomb == 7) btn_press=1; //Если было нажатие кнопки, то меняем статус
 
 if (comb == 4)                         //Если было промежуточное положение энкодера, то проверяем его предыдущее состояние 
 {
    if (lastcomb == 5) --enc_rotation; //вращение по часовой стрелке
    if (lastcomb == 6) ++enc_rotation; //вращение против часовой
    enc_state=1;                       // был поворот энкодера    
    btn_enc_rotate=0;                  //обнулить показания вращения с нажатием
  }
  
   if (comb == 0)                      //Если было промежуточное положение энкодера и нажатие, то проверяем его предыдущее состояние 
   {
    if (lastcomb == 1) --btn_enc_rotate; //вращение по часовой стрелке
    if (lastcomb == 2) ++btn_enc_rotate; //вращение против частовой
    enc_state=2;                        // был поворот энкодера с нажатием  
    enc_rotation=0;                     //обнулить показания вращения без нажатия
    btn_press=0;                         //обнулить показания кнопки
   }

   if (comb == 7 && lastcomb == 3 && btn_press) //Если было отпускание кнопки, то проверяем ее предыдущее состояние 
   {
    if (millis() - timer > btn_long_push)         // проверяем сколько прошло миллисекунд
    {
      enc_state=4;                              // было длинное нажатие 
    } else {
             enc_state=3;                    // было нажатие 
            }
      btn_press=0;                           //обнулить статус кнопки
    }
   
  timer = millis();                       //сброс таймера
  lastcomb = comb;                        //сохраняем текущее состояние энкодера
}
