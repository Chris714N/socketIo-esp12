#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <SocketIoClient.h>
#include "md5.h"
#include <TaskScheduler.h>
#include <math.h>

int timeSend = 100;

// Callback methods prototypes
void t1Callback();

//Tasks
Task t1(timeSend, TASK_FOREVER, &t1Callback);

Scheduler runner;

ESP8266WiFiMulti WiFiMulti;
MD5Builder md5;
SocketIoClient webSocket;

String macAp = "";
String macClient = "";
long int idSoc = 0;
long int idFlash = 0;
String ssid = "Pame8640";
String pass = "19781978";
int port = 3000;
String host = "192.168.1.89";
float sensorValue = 0;
boolean estado_wifi = false;
boolean status = true;

#define TRIGGER 14 //d1 d5
#define ECHO 12    //d2 d6

void t1Callback()
{
  //Serial.print("t1: ");
  //Serial.println(millis());
  if (status)
  {
    Serial.println("status: " + status);
    emitDatos();
  }
}
// server config -> Recibe datos de sensor
// status -> Recibe estado de envio.

void eventA(const char *payload, size_t length)
{
  //t1.disable();
  //runner.deleteTask(t1);
  Serial.print("Tiempo Server: ");
  Serial.println(payload);
  String tiempo = payload;
  timeSend = tiempo.toInt();
  timeSend = timeSend * 1000;
  Serial.print("Tiempo uC: ");
  Serial.println(timeSend);
  t1.setInterval(timeSend);
}

void eventB(const char *payload, size_t length)
{
  Serial.print("got message 'status' : ");
  Serial.println(payload);
  String estado = payload;
  if (estado == "ok")
  {
    Serial.println("OK");
    status = true;
  }
}

int16_t leer_distancia()
{
  long duration, longitude[20], repeat[20];
  int sensVal = 0;
  int valor = 0;
  // 20 lecturas de sensor
  for (int i = 0; i <= 19; i++)
  {
    digitalWrite(TRIGGER, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIGGER, LOW);
    duration = pulseIn(ECHO, HIGH);
    longitude[i] = (duration / 2) / 29.1;
    delay(2);
  }

  //cuantas veces se repite cada lectura
  for (int a = 0; a <= 19; a++)
  {
    repeat[a] = 0;
    for (int b = 0; b <= 19; b++)
    {
      if (longitude[a] == longitude[b])
      {
        repeat[a] = repeat[a] + 1;
      }
    }
    //La cantidad de repeticiones mas alta
    sensVal = fmax(sensVal, repeat[a]);

    //buscar el valor de acuerdo a repeticiones
    repeat[a] = 0;
    for (int b = 0; b <= 19; b++)
    {
      if (longitude[a] == longitude[b])
      {
        repeat[a] = repeat[a] + 1;
      }
      if (repeat[a] == sensVal)
      {
        valor = longitude[a];
        b = 19;
      }
    }
  }

  if (valor > 0 && valor < 400)
  {
    return valor;
  }
  else
  {
    //leer_distancia();
    Serial.println("intentando leer distancia");
    return valor = 0;
  }
}

boolean IniciarWifi(void)
{
  int progreso = 1;

  if (ssid != "" && pass != "")
  {

    Serial.println("\n\r*Connecting to " + ssid);
    //WiFi.reconnect();
    delay(300);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      if (progreso == 1)
      {
        WiFi.begin(ssid.c_str(), pass.c_str());
        Serial.println("");
        Serial.println("intento 1");
      }

      else if (progreso == 5)
      {
        WiFi.begin(ssid.c_str(), pass.c_str());
        Serial.println("");
        Serial.println("intento 2");
      }

      else if (progreso == 10)
      {
        WiFi.begin(ssid.c_str(), pass.c_str());
        Serial.println("");
        Serial.println("intento 3");
      }

      else if (progreso == 15)
      {
        WiFi.begin(ssid.c_str(), pass.c_str());
        Serial.println("");
        Serial.println("intento 4");
      }

      else if (progreso == 20)
      {
        WiFi.begin(ssid.c_str(), pass.c_str());
        Serial.println("");
        Serial.println("intento 5");
      }

      else if (progreso == 30)
      {
        WiFi.begin(ssid.c_str(), pass.c_str());
        Serial.println("");
        Serial.println("intento 6");
      }

      else if (progreso == 40)
      {
        WiFi.begin(ssid.c_str(), pass.c_str());
        Serial.println("");
        Serial.println("intento 7");
      }

      else if (progreso == 50)
      {
        WiFi.begin(ssid.c_str(), pass.c_str());
        Serial.println("");
        Serial.println("intento 8");
      }

      else if (progreso == 60)
      {
        return 0;
      }

      progreso = progreso + 1;
      Serial.print("o ");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      return 1;
    }
  }

  else
  {
    Serial.print("Necesita clava y usuario wifi");
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  runner.init();
  runner.addTask(t1);

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  macAp = WiFi.softAPmacAddress();
  macClient = WiFi.macAddress();
  estado_wifi = IniciarWifi();
  idSoc = ESP.getChipId();
  idFlash = ESP.getFlashChipId();

  //Serial.println("");
  //Serial.print("MAC AP: ");
  //Serial.println(macAp);
  //Serial.print("MAC STA: ");
  //Serial.println(macClient);
  //Serial.print("ID SOC: ");
  //Serial.println(idSoc);
  //Serial.print("ID FLASH ");
  //Serial.println(idFlash);

  webSocket.on("server config", eventA);
  webSocket.on("status", eventB);

  webSocket.begin(host.c_str(), port);
  t1.enable();
}

void emitDatos()
{
  if (status == true)
  {
    //{"data":0,"idFlash":0,"idSoc":0,"macAp":"","macClient":""}
    //"{\"data\":0,\"idFlash\":0,\"idSoc\":0,\"macAp\":\"\",\"macClient\":\"\"}"
    status = false;
    String dataC = "{\"data\":";
    String dataS = dataC + String(leer_distancia()) + ",";

    String idFlashC = "\"idFlash\":";
    String idFlashS = idFlashC + String(idFlash) + ",";

    String idSocC = "\"idSoc\":";
    String idSocS = idSocC + String(idSoc) + ",";

    String macApC = "\"macAp\":";
    String macApS = macApC + String("\"" + macAp) + "\"" + ",";

    String macClientC = "\"macClient\":";
    String macClientS = macClientC + String("\"" + macClient) + "\"" + "}";

    String objeto = dataS + idFlashS + idSocS + macApS + macClientS;
    objeto = String(objeto);
    //Serial.println(objeto);

    webSocket.emit("datos sensor", objeto.c_str());
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  webSocket.loop();
  runner.execute();
}
