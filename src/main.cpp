/*******************************************************************************************************
 * CIRCUITO DE CONTROL DE ACCESO USANDO TARJETAS RFID
 * AUTOR: MARIO SAUL DE LA FUENTE BARRUETA 
 * GITHUB:https://github.com/LuasDB
 * Librerias:
 *          bblanchon/ArduinoJson@^7.0.3
 *          miguelbalboa/MFRC522@^1.4.11
 *          arduino-libraries/SD@^1.2.4
 * El circuito utiliza el microcontrolador ESP32 que ademas cuenta con conexión vía Bluetooth y Wifi
 * Se utiliza para la lectura de las tarjetas RFID el modulo RFID-RC522.
 * Para guardar la información de las tarjetas se utiliza un modulo lector de memoria SD , ademas de 
 * guardar un listado de los registros de entrada y salida en formato JSON para poder abrirlo en una 
 * aplicación WEB 
 * 
 * 
***************************************************************************************************************/
#include <Arduino.h>
#include <esp_system.h>
/*******************************************************************************************************
 * Para configurar SD Module
***************************************************************************************************************/
#include <SPI.h>
#include <SD.h>
#define CS_SD 4 
/*******************************************************************************************************
 * Libreria para manejar archivos JSON
***************************************************************************************************************/
#include <ArduinoJson.h>
/*******************************************************************************************************
 * Libreria para manejar modulo RFID-RC522
***************************************************************************************************************/
#include <MFRC522.h>
#define CS_RFID 5
#define RST_RFID 2
//Creamos una intancia de la clase MFRC522
MFRC522 rfid(CS_RFID,RST_RFID);
/*******************************************************************************************************
 * Librerias para manejar Pantalla OLED
***************************************************************************************************************/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
/******************************************************************************************
LOGO SIRSA
*******************************************************************************************/
// Logo Sirsa configurado
static const uint8_t image_data_Logosirsa_OLED[1024] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff,
    0xff, 0xbf, 0xef, 0xff, 0xff, 0xff, 0xfd, 0xff, 0x85, 0xe1, 0xff, 0xb7, 0xff, 0xbd, 0xff, 0xff,
    0xff, 0xbf, 0xef, 0xff, 0xff, 0x7f, 0xff, 0xff, 0x7d, 0xee, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xb8, 0x61, 0x8e, 0x02, 0x31, 0xc5, 0x8f, 0x3d, 0xee, 0x0c, 0x34, 0x31, 0xb1, 0xc3, 0xff,
    0xff, 0xbf, 0x6e, 0x76, 0xfb, 0x6e, 0xdd, 0x77, 0x8d, 0xe1, 0xeb, 0xb7, 0xaf, 0xae, 0xdd, 0xff,
    0xff, 0xb8, 0x6e, 0x76, 0xc3, 0x6e, 0xdd, 0x77, 0xf5, 0xed, 0x0b, 0xb4, 0x2f, 0xae, 0xdd, 0xff,
    0xff, 0xbb, 0x6e, 0x76, 0xdb, 0x6e, 0xdd, 0x77, 0x75, 0xec, 0x6b, 0xb5, 0xad, 0xae, 0xdd, 0xff,
    0xff, 0x80, 0x61, 0x8e, 0xc3, 0x31, 0xdd, 0x8f, 0x8d, 0xee, 0x0c, 0x34, 0x31, 0xb1, 0xdd, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xf7, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0xe7, 0x9c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0xe7, 0x9c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0xe3, 0x0c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0xc3, 0x0c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xf8, 0x00, 0x44, 0x8c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xf8, 0x00, 0x44, 0x8c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xf0, 0x00, 0x7d, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xe0, 0x00, 0x7c, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xc0, 0x00, 0xf8, 0x7c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xc0, 0xff, 0xf8, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xc1, 0xff, 0xf8, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xc1, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xc1, 0xff, 0xfe, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xc1, 0xff, 0xfc, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xc0, 0xff, 0xf8, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xc0, 0x00, 0xf8, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xe0, 0x00, 0x38, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xf0, 0x00, 0x18, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xf0, 0x00, 0x08, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xfc, 0x00, 0x08, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xfe, 0x00, 0x08, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xfe, 0x00, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0x00, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0x00, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0x00, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0x00, 0x3c, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x08, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x0c, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x1e, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x3e, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
void initDisplay(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(500);
  Serial.println("Pantalla lista");
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.invertDisplay(true);
  display.drawBitmap(0, 0, image_data_Logosirsa_OLED, 128, 68, 1);
  display.display();
}
void messageUser(JsonDocument& doc,int index,String msg){

  display.clearDisplay();
  delay(500);
  display.setTextColor(WHITE);
  display.invertDisplay(false);
  display.setCursor(5,5);
  display.print(msg);
  display.setCursor(5,20);
  display.print(doc["personal"][index]["nombre"].as<String>());
  display.display();
  delay(2000);
  display.clearDisplay();
  delay(100);
  display.invertDisplay(true);
  display.drawBitmap(0, 0, image_data_Logosirsa_OLED, 128, 68, 1);
  display.display();


} 
void messageError(){
   display.clearDisplay();
  delay(500);
  display.setTextColor(WHITE);
  display.invertDisplay(false);
  display.setCursor(5,5);
  display.print("UID NO ENCONTRADO");
  display.setCursor(5,20);
  display.print("No se reconoce");
  display.display();
  delay(2000);
  display.clearDisplay();
  delay(100);
  display.invertDisplay(true);
  display.drawBitmap(0, 0, image_data_Logosirsa_OLED, 128, 68, 1);
  display.display();
}

/*******************************************************************************************************
 * PARA CONTROLAR RTC-DS3231
***************************************************************************************************************/
#include "RTClib.h"
#include <WiFi.h>
#include "time.h"
const char *ntpServer = "time.google.com";
const long gmtOffset_sec = 5;
const int daylightOffset_sec = 3600;
unsigned long epochTime;
RTC_DS3231 rtc;
RTC_DATA_ATTR DateTime ultimaConexion = DateTime(2023, 1, 1, 0, 0, 0); // Fecha y hora de la última conexión, conservada en la memoria RTC
void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}


/*******************************************************************************************************
 * CONTROL RGB
***************************************************************************************************************/
// Variables para led RGB
// Definimos los pines que utilizaremos para las salidas de los leds, es necesario revisar el PIOUT del modulo
// DEFINIMOS LOS PINES
#define RED 27
#define GREEN 12
#define BLUE 14
// Configuramos los parametros de trabajo del PWM
const int freq = 5000; // Frecuencia de trabajo del PWM
const int azul = 0;    // Canal que se utilizara se tienen 16 canales disponibles numerados del 0 al 15
const int verde = 1;
const int rojo = 2;
const int res = 8; // Resolucion de bits para el ciclo útil de trtabajo, con 8 bits podemos utilizar un ciclo desde 0 a 255

void ledrgb(int r, int g, int b)
{

  // Setear cada pin PWM con el valor de entrada de la función
  ledcWrite(rojo, 255 - r);
  ledcWrite(verde, 255 - g);
  ledcWrite(azul, 255 - b);
}


/*******************************************************************************************************
 * Variables generales
***************************************************************************************************************/
#define PUERTA 25
String uid;
unsigned long sendDataPrevMillis=0;

void abrir(){
  ledrgb(0, 100, 100);
  Serial.println("ABRIENDO_____");
  digitalWrite(PUERTA,HIGH);
  delay(1500);
  digitalWrite(PUERTA,LOW);
  ledrgb(0, 20, 204);
}

void printFile(const char* filename) {
  // Open file for reading
  File file = SD.open(filename);
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file
  file.close();
}

bool searchUser(String uid){
  digitalWrite(CS_SD,LOW);
  JsonDocument doc;
  File file = SD.open("/personal.txt");
  if(file){
   deserializeJson(doc,file);
  }
  file.close();
  
  digitalWrite(CS_SD,HIGH);

  // Obtenemos el array de objetos del documento JSON
  JsonArray personal = doc["personal"];
  // serializeJsonPretty(personal,Serial);
  //Iteramos sobre los elementos del array
  // Serial.println("INICIANDO");
  int index=0;
  for(JsonObject obj : personal){
    String uidPersona = obj["uid"];  
    //Cuando se encuentre que el usuario coincide 
    if(uidPersona == uid){
      Serial.print("UID:");
      Serial.println(uid);
      abrir();
      DateTime now2 = rtc.now();
      String fecha2 = String(now2.year(), DEC) + "-" + String(now2.month(), DEC) + "-" + String(now2.day(), DEC);
      String hora2 = String(now2.hour() - 6, DEC) + ":" + String(now2.minute(), DEC) + ":" + String(now2.second(), DEC);
      if(obj["status"]==false){
        messageUser(doc,index,"Bienvenido");
        doc["personal"][index]["status"]=true;
        doc["personal"][index]["entrada"]["fecha"]=fecha2;
        doc["personal"][index]["entrada"]["hora"]=hora2;
        // serializeJsonPretty(doc,Serial);
        digitalWrite(CS_SD,LOW);
        SD.remove("/personal.txt");
        File fileUpdate = SD.open("/personal.txt",FILE_WRITE);
        if(fileUpdate){
          serializeJsonPretty(doc,fileUpdate);
        }
        fileUpdate.close();
        digitalWrite(CS_SD,HIGH);
      }else if(obj["status"]==true){
        messageUser(doc,index,"Hasta pronto!");
        doc["personal"][index]["status"]=false;
        doc["personal"][index]["salida"]["fecha"]=fecha2;
        doc["personal"][index]["salida"]["hora"]=hora2;
        // Serial.println("Nuevo doc Salida y push a registro:");
        // serializeJsonPretty(doc,Serial);
        digitalWrite(CS_SD,LOW);
        SD.remove("/personal.txt");
        File fileUpdate = SD.open("/personal.txt",FILE_WRITE);
        if(fileUpdate){
          serializeJsonPretty(doc,fileUpdate);
        }
        fileUpdate.close();
        File fileRegister = SD.open("/registro.txt",FILE_APPEND);
        if(fileRegister){
          String fecha = doc["personal"][index]["entrada"]["fecha"].as<String>();
          String nombre = doc["personal"][index]["nombre"].as<String>();
          String empresa = doc["personal"][index]["empresa"].as<String>();
          String asunto = doc["personal"][index]["asunto"].as<String>();
          String hora_entrada = doc["personal"][index]["entrada"]["hora"].as<String>();
          String hora_salida = doc["personal"][index]["salida"]["hora"].as<String>();
          String registro = fecha + "," + nombre + "," + empresa + "," + asunto + "," + hora_entrada + "," + hora_salida;
          fileRegister.println(registro);
        }
        fileRegister.close();
        digitalWrite(CS_SD,HIGH);
      }
      return true;
    }
    index++;
  }
  Serial.print("NO ENCONTRADO:");
  Serial.println(uid);
  messageError();
  return false;
  
}
/*******************************************************************************************************
 * Funciones para el manejo de RFID-RC522
***************************************************************************************************************/
String readRFID(){
  rfid.PICC_ReadCardSerial();
  String uid;
  for (size_t i = 0; i <rfid.uid.size; i++)
  {
    uid += String(rfid.uid.uidByte[i] < 0x10 ? " 0" : "");
    uid += String(rfid.uid.uidByte[i], HEX); 
  }
  return uid;
}


/*******************************************************************************************************
 * CONFIGURACIÓN WIFI
***************************************************************************************************************/
const char* ssid = "DEPTO. TECNICO";
const char* password = "S1r2492020#";
// const char* ssid = "INFINITUM3BAF";
// const char* password = "FzAetH7huj";

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("CONECTANDO WiFi ..");
  delay(2000);

 if(WiFi.status() != WL_CONNECTED) {
    Serial.println("No se encontro conexion");
  }else{
  Serial.println(WiFi.localIP());
  ledrgb(0, 20, 204);

  }


}
void printNow(){
  DateTime now2 = rtc.now();
  String fecha2 = String(now2.day(), DEC)+ "-" + String(now2.month(), DEC) + "-" + String(now2.year(), DEC);
  String hora2 = String(now2.hour() - 6, DEC) + ":" + String(now2.minute(), DEC) + ":" + String(now2.second(), DEC);
      
  display.clearDisplay();
  delay(100);
  display.invertDisplay(true);
  display.drawBitmap(0, 0, image_data_Logosirsa_OLED, 128, 68, 1);
  display.display();
  display.clearDisplay();
  delay(3000);
  display.setTextColor(WHITE);
  display.invertDisplay(false);
  display.setCursor(10,2);
  display.print("Laboratorio");
  display.setCursor(5,20);
  display.print("Fecha:");
  display.setCursor(45,20);
  display.print(fecha2);
   display.setCursor(5,50);
  display.print("Hora:");
  display.setCursor(45,50);
  display.print(hora2);
  display.display(); 
}

void setup() {                                          
  //Inicializamos el puerto serial
  Serial.begin(9600);
  Serial.println("Iniciando---");
  //Configuración para lel LED RGB
  //Inicializamos la salida PWM con la funcion ledcSetup(Canal, frecuencia, resolucion )
  ledcSetup(azul, freq, res);
  ledcSetup(verde, freq, res);
  ledcSetup(rojo, freq, res);
  // Configuración de los pines PWM (Asignamos a cada pin un canal)
  ledcAttachPin(RED, rojo);
  ledcAttachPin(GREEN, verde);
  ledcAttachPin(BLUE, azul);

  ledrgb(255, 20, 204);
  //INICIALIZACIÓN DE WIFI
  initWiFi();
  //CONFIGURACION DE RELOJ
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  rtc.begin();

  // Configuración mediante NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    if (rtc.lostPower())
    {
      Serial.println("RTC lost power, let's set the time!");
      // When time needs to be set on a new device, or after a power loss, the
      // following line sets the RTC to the date & time this sketch was compiled
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // This line sets the RTC with an explicit date & time, for example to set
      // January 21, 2014 at 3am you would call:
    }
  }
  else
  {
    rtc.adjust(DateTime(timeinfo.tm_year - 100, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
  }
  DateTime now2 = rtc.now();
  String fecha2 = String(now2.year(), DEC) + "-" + String(now2.month(), DEC) + "-" + String(now2.day(), DEC);
  String hora2 = String(now2.hour() - 6, DEC) + ":" + String(now2.minute(), DEC) + ":" + String(now2.second(), DEC);
  Serial.printf("Fecha: %s hora: %s", fecha2, hora2);
  Serial.println();

  

  delay(1000);
  //Inicialización de pines
  pinMode(PUERTA,OUTPUT);
  digitalWrite(PUERTA,LOW);
  //Inicializamos el display
  initDisplay();
  //Inicializamos el puerto SPI
  SPI.begin(); 
  //Inicializamos el modulo RFID-RC522
  rfid.PCD_Init();
  //Inicializamos la SD
  if(!SD.begin(CS_SD)){
    Serial.println("Card Mount Failed");
    return;
  }
  Serial.println("Tarjeta SD inicializada correctamente.");
  digitalWrite(CS_SD,HIGH);
  //Inicializamos nuestra variable para contar los milisegundos
  sendDataPrevMillis = millis();
   ledrgb(0, 20, 204);
   printNow();


  

}


void loop() {  
  if(millis() - sendDataPrevMillis > 60000 || sendDataPrevMillis == 0){
    sendDataPrevMillis=millis();
    printNow();
  }
  
  if(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()){
   ledrgb(10, 100, 100);

    uid="";
    uid=readRFID();
    digitalWrite(CS_RFID,HIGH);
    delay(100);
    if(searchUser(uid)){
      Serial.println("Se abrio");
    }
    digitalWrite(CS_SD,LOW);
    delay(1000);
   ledrgb(0, 20, 204);

    }   
    
}
