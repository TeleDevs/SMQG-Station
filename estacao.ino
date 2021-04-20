#include "DHT.h"
#include <EEPROM.h> // Biblioteca para acesso e manipulação da EEPROM
#include <SoftwareSerial.h>
#include <TimeLib.h>

#define DHTPIN A5 // Pino DATA do Sensor ligado na porta Analogica A5
#define DHTTYPE DHT11 // DHT 11
#define time_cont 1800000 // 3600000 1 em 1 hora -- 21600000 de 6 em 6 hrs

//const unsigned long DEFAULT_TIME = 1618800634; // 19 Abril 2021

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial serial1(10,11); // RX, TX
unsigned long ContSensor = millis(); //variavel para contar o tempo das leituras do sensor     
                             
byte array_temp[4];
byte array_umid[4];
byte array_temp_deci[4];
byte array_dia[4];
byte array_mes[4];
byte array_ano[4];
byte array_hora[4];
byte array_minuto[4];
byte array_segundo[4];

int cont_read = 0;
String data_day = "";

int end_temp = 0;
int end_temp_deci = 1;
int end_umid = 2;
int end_ano = 3;
int end_mes = 4;
int end_dia = 5;
int end_hora = 6;
int end_minuto = 7;
int end_segundo = 8;


int lim_memory;
String command = "";
char b1;

bool iniciaColeta = false;

void setup() 
{
  serial1.begin(9600);
  // Aguarda 1 seg antes de acessar as informações do sensor
//  setTime(1618835400); // Date and time (GMT): Monday, 19 April 2021 12:30:00
  delay(1000);
  dht.begin();
}
 
void loop() 
{
  if(iniciaColeta){
    read_sensor();
  }
  read_serial();
  
} 

void read_sensor()
{
 if (millis() - ContSensor >= time_cont){
      int h = dht.readHumidity();
      float t = dht.readTemperature();

      array_temp[cont_read] = int(t);
      array_temp_deci[cont_read] = int((t-int(t))*100);
      array_umid[cont_read] = h;
      array_ano[cont_read] = year()%2000; 
      array_mes[cont_read] = month(); 
      array_dia[cont_read] = day(); 
      array_hora[cont_read] = hour(); 
      array_minuto[cont_read] = minute(); 
      array_segundo[cont_read] = second();      

      int aux_cont = cont_read + 1;
      aux_cont = aux_cont%4;
     
      if (aux_cont == 0){
        //salvar dados de 1 dia
        serial1.println("Um dia foi salvo...");
        save_data();
        cont_read = 0;
      }
      else{
         cont_read++;
      }

      ContSensor = millis();
   }
}


void read_serial()
{
    if (serial1.available()>0) { // Se existem dados para leitura
   
      b1 = serial1.read(); //Variável para armazenar o dado atual
      command += b1; //Variável para armazenar todos os dados
       
      if (b1 == '\n'){ //Se o dado atual for um pulador de linha (\n)
        if(command[7] == '1'){
          int h = dht.readHumidity();
          float t = dht.readTemperature();
          
          // Mostra os valores lidos, na serial
          serial1.print("{\"tmp\":");
          serial1.print(t);
          serial1.print(",\"umi\":");
          serial1.print(h);
          serial1.print(",\"dia\":");
          serial1.print("\"20");
          serial1.print(year()%2000);
          printDays(month());
          printDays(day());
          serial1.print(" ");
          printDigits(hour(), false);
          printDigits(minute(), true);
          printDigits(second(), true);
          serial1.println("\"}");
        }
        else if(command[7] == '2'){
          serial1.println("Descarregando coletas armazenadas...");
          read_data();
        }
        else if(command[7] == '0'){
          serial1.println("Sincronizando relógio...");
          unsigned long horaSync = 0;

          // 16 - 25
          int arraySync[10];
          int k = 0;
          for(int j = 16; j<= 25; j++){
            k = j - 16;
            arraySync[k] = (int)command[j] - '0';
          }
          for (int i = 0; i <= 9; i++) {
              horaSync *= 10;
              horaSync += arraySync[i];
          }
          horaSync = horaSync - (3600*3); // Ajusta para o GTM -3
          setTime(horaSync);
          iniciaColeta = true;
        }
        command = ""; //Limpa o comando para futuras leituras
      } 
      
    }
}

String ReadStringSerial(){
  String data = "";
  char character;
  
  // Enquanto receber algo pela serial
  while(serial1.available() > 0) {
    // Lê byte da serial
    character = serial1.read();
    // Ignora caractere de quebra de linha
    if (character != '\n'){
      // Concatena valores
      data.concat(character);
    }
    // Aguarda buffer serial ler próximo caractere
    delay(10);
  } 
  return data;
}

void save_data()
{
    //------->>falta salvar a data!!!!
    
    lim_memory = EEPROM.length();
    if (end_temp >= lim_memory-5 || end_umid >= lim_memory-5){
      end_temp = 0;
      end_temp_deci = 1;
      end_umid = 2;
      end_ano = 3;
      end_mes = 4;
      end_dia = 5;
      end_hora = 6;
      end_minuto = 7;
      end_segundo = 8;

      serial1.println("resetando");
      }
    for (int i =0 ; i <= 3; i++){
      EEPROM.write(end_temp,array_temp[i]);
      EEPROM.write(end_temp_deci,array_temp_deci[i]);
      EEPROM.write(end_umid,array_umid[i]);
      EEPROM.write(end_ano,array_ano[i]);
      EEPROM.write(end_mes,array_mes[i]);
      EEPROM.write(end_dia,array_dia[i]);
      EEPROM.write(end_hora,array_hora[i]);
      EEPROM.write(end_minuto,array_minuto[i]);
      EEPROM.write(end_segundo,array_segundo[i]);
      
      end_temp = end_temp + 9;
      end_temp_deci = end_temp_deci + 9;
      end_umid = end_umid + 9;
      end_ano = end_ano + 9;
      end_mes = end_mes + 9;
      end_dia = end_dia + 9;
      end_hora = end_hora + 9;
      end_minuto = end_minuto + 9;
      end_segundo = end_segundo + 9;
    }
    
  
}

void read_data()
{
  if (end_temp == 0){
    
    serial1.println("Não há dados armazenados em memória.");
  }
  else{

    // ajusta a quantidade de leituras que devem ser realizadas
    int leituras = end_temp - 9; // Remove a próxima leitura

    if(leituras > 27 && leituras%4 != 3){
      leituras = leituras - (leituras%4 + 1)*9;
    }
    
      for(int y = 0; y <= leituras; y = y + 9){
        delay(500);
        //byte aux_temp;
        //byte aux_umid;
        //aux_temp = EEPROM.read(y);
        //aux_umid = EEPROM.read(y+1);
  
        // Mostra os valores lidos, na serial
        serial1.print("{\"tmp\":");
        serial1.print(float(EEPROM.read(y))+float(EEPROM.read(y+1))/100);
        serial1.print(",\"umi\":");
        serial1.print(EEPROM.read(y+2));
        serial1.print(",\"dia\":");
        serial1.print("\"20");
        serial1.print(EEPROM.read(y+3));
        printDays(EEPROM.read(y+4));
        printDays(EEPROM.read(y+5));
        serial1.print(" ");
        printDigits(EEPROM.read(y+6), false);
        printDigits(EEPROM.read(y+7), true);
        printDigits(EEPROM.read(y+8), true);
        serial1.println("\"}");
      }
    }
}

void printDigits(int digits, bool pontos){
  if(pontos)
    serial1.print(":");
  if(digits < 10)
    serial1.print('0');
  serial1.print(digits);
}

void printDays(int digits){
  serial1.print("-");
  if(digits < 10)
    serial1.print('0');
  serial1.print(digits);
}
