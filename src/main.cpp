#include "LittleFS.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <string>
#include <Servo.h>
#include <TaskScheduler.h>
#include <ESPAsyncWiFiManager.h>

lfs_t lfs;

Scheduler runner;

File configFile;

const int maxServos = 4;

Servo servo[maxServos];

// Struct including the Servo-Values
struct ServoUnits
{
  unsigned short servoSpeed = 50;
  unsigned short servoAngleStart = 40;
  unsigned short servoAngleEnd = 140;

  bool servoRandomFlag = false;
  bool servoOnOff = false;
  bool servoManualControl = false;
};

ServoUnits servoUnit[maxServos];

float servo_pos[maxServos] = {90};

bool servodirectioninc[maxServos] = {true};

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
DNSServer dns;

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

double map_range(double value, double in_min, double in_max, double out_min, double out_max) {
  return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

String scriptjs_processor(const String &var)
{
  if (var == "SERVO1_SPEED")
  {
    return String(servoUnit[0].servoSpeed);
  }
  else if (var == "SERVO2_SPEED")
  {
    return String(servoUnit[1].servoSpeed);
  }
  else if (var == "SERVO3_SPEED")
  {
    return String(servoUnit[2].servoSpeed);
  }
  else if (var == "SERVO4_SPEED")
  {
    return String(servoUnit[3].servoSpeed);
  }
  return String();
}

String indexhtml_processor(const String &var)
{
  if (var == "MINSLIDERVALUE_SERVO1")
  {
    return String(servoUnit[0].servoAngleStart);
  }
  else if (var == "MAXSLIDERVALUE_SERVO1")
  {
    return String(servoUnit[0].servoAngleEnd);
  }
  else if (var == "MINSLIDERVALUE_SERVO2")
  {
    return String(servoUnit[1].servoAngleStart);
  }
  else if (var == "MAXSLIDERVALUE_SERVO2")
  {
    return String(servoUnit[1].servoAngleEnd);
  }
  else if (var == "MINSLIDERVALUE_SERVO3")
  {
    return String(servoUnit[2].servoAngleStart);
  }
  else if (var == "MAXSLIDERVALUE_SERVO3")
  {
    return String(servoUnit[2].servoAngleEnd);
  }
  else if (var == "MINSLIDERVALUE_SERVO4")
  {
    return String(servoUnit[3].servoAngleStart);
  }
  else if (var == "MAXSLIDERVALUE_SERVO4")
  {
    return String(servoUnit[3].servoAngleEnd);
  }
  else if (var == "CHECKEDRANDOM_SERVO1")
  {
    if (servoUnit[0].servoRandomFlag)
    {
      return String("checked");
    }
  }
  else if (var == "CHECKEDRANDOM_SERVO2")
  {
    if (servoUnit[1].servoRandomFlag)
    {
      return String("checked");
    }
  }
  else if (var == "CHECKEDRANDOM_SERVO3")
  {
    if (servoUnit[2].servoRandomFlag)
    {
      return String("checked");
    }
  }
  else if (var == "CHECKEDRANDOM_SERVO4")
  {
    if (servoUnit[3].servoRandomFlag)
    {
      return String("checked");
    }
  }
  else if (var == "CHECKEDONOFF_SERVO1")
  {
    if (servoUnit[0].servoOnOff)
    {
      return String("checked");
    }
  }
  else if (var == "CHECKEDONOFF_SERVO2")
  {
    if (servoUnit[1].servoOnOff)
    {
      return String("checked");
    }
  }
  else if (var == "CHECKEDONOFF_SERVO3")
  {
    if (servoUnit[2].servoOnOff)
    {
      return String("checked");
    }
  }
  else if (var == "CHECKEDONOFF_SERVO4")
  {
    if (servoUnit[3].servoOnOff)
    {
      return String("checked");
    }
  }

  else if (var == "CHECKMANUALCONTROL_SERVO1")
  {
    if (servoUnit[0].servoManualControl)
    {
      return String("checked");
    }
  }
  else if (var == "CHECKMANUALCONTROL_SERVO2")
  {
    if (servoUnit[1].servoManualControl)
    {
      return String("checked");
    }
  }
  else if (var == "CHECKMANUALCONTROL_SERVO3")
  {
    if (servoUnit[2].servoManualControl)
    {
      return String("checked");
    }
  }
  else if (var == "CHECKMANUALCONTROL_SERVO4")
  {
    if (servoUnit[3].servoManualControl)
    {
      return String("checked");
    }
  }
  return String("");
}

void write()
{
  configFile = LittleFS.open("/varstatus.txt", "w");
  for (int i = 0; i < maxServos; i++)
  {
    configFile.println(servoUnit[i].servoSpeed);
    configFile.println(servoUnit[i].servoAngleStart);
    configFile.println(servoUnit[i].servoAngleEnd);
    configFile.println(servoUnit[i].servoRandomFlag);
    configFile.println(servoUnit[i].servoOnOff);
    configFile.println(servoUnit[i].servoManualControl);
  }
  configFile.flush();
  configFile.close();
}

void read()
{
  configFile = LittleFS.open("/varstatus.txt", "r");
  for (int i = 0; i < maxServos; i++)
  {
    if (configFile.available())
    {
      servoUnit[i].servoSpeed = (short)configFile.readStringUntil('\n').toInt();
      servoUnit[i].servoAngleStart = (short)configFile.readStringUntil('\n').toInt();
      servoUnit[i].servoAngleEnd = (short)configFile.readStringUntil('\n').toInt();
      servoUnit[i].servoRandomFlag = configFile.readStringUntil('\n').toInt();
      servoUnit[i].servoOnOff = configFile.readStringUntil('\n').toInt();
      servoUnit[i].servoManualControl = configFile.readStringUntil('\n').toInt();
    }else if(!configFile.available()){
      Serial.println("Not av!!!");
    }
  }
  configFile.close();
}

bool getFormParamBool(AsyncWebServerRequest *r, const char *name)
{
  String s_name = name;
  return r->hasParam(s_name, true, false) && strcmp(r->getParam(s_name, true, false)->value().c_str(), "on") == 0;
}

unsigned short getFormParamShort(AsyncWebServerRequest *r, const char *name)
{
  String s_name = name;
  if (r->hasParam(s_name, true, false))
  {
    return (unsigned short)std::stoi(r->getParam(s_name, true, false)->value().c_str());
  }
  return 0;
}

void servoController()
{
  float rand;
  float fps = 60;
  float secondsPerAngle = 2;
  float printAllSeconds = 1;
  float last[maxServos] = {-1};
  int j = 0;
  while (true)
  {

    // j++;
    for (int i = 0; i < maxServos; i++)
    {
      /* if (j % (int)(fps * printAllSeconds / maxServos) == 0)
      {
        //Debug
        Serial.println("value of i ");
        Serial.println(0);
        Serial.println("IsOnOff? ");
        Serial.println(servoUnit[0].servoOnOff);
        Serial.println("IsInc? ");
        Serial.println(servodirectioninc[0]);
        Serial.println("CurrentPos? ");
        Serial.println(servo_pos[0]);
        Serial.println("MaxAngle? ");
        Serial.println(servoUnit[0].servoAngleEnd);
        Serial.println("MinAngle? ");
        Serial.println(servoUnit[0].servoAngleStart);
      } */ 

      if (servoUnit[i].servoOnOff)
      {
        if (!servoUnit[i].servoManualControl)
        {
          if (servoUnit[i].servoRandomFlag)
          {
            rand = random(10, 99);
          }
          else
          {
            rand = 0;
          }

          float diff = float(servoUnit[i].servoSpeed) * ((rand / 100) + 1) / (100 / secondsPerAngle) / fps;  //Calulating Servo Speed

          // If true pos+ else pos-
          servo_pos[i] += ((int)servodirectioninc[i] * 2 - 1) * diff;

          if (servo_pos[i] >= servoUnit[i].servoAngleEnd)
          {
            servodirectioninc[i] = false;
          }
          else if (servo_pos[i] <= servoUnit[i].servoAngleStart)
          {
            servodirectioninc[i] = true;
          }
          int mapVal = map_range(servo_pos[i], 0, 180, 1000, 2000);
          if (last[i] != mapVal) //Change Servo Position if last position != current position
          {
            last[i] = mapVal;
            if(i == 0){
              servo[i].writeMicroseconds(mapVal);
              servo[i].attach(D1);
              delay(8);
            }else if (i == 1){
              servo[i].writeMicroseconds(mapVal);
              servo[i].attach(D2);
              delay(8);
            }else if (i == 2){
              servo[i].writeMicroseconds(mapVal);
              servo[i].attach(D3);
              delay(8);
            }else if (i == 3){
              servo[i].writeMicroseconds(mapVal);
              servo[i].attach(D4);
              delay(8);
            }
            Serial.println("mapVal = ");
            Serial.println(mapVal);
            //Serial.println(WiFi.status() == WL_CONNECTED); //DEBUG
            servo[i].detach();
          }
        }
        else //Manual Servo Control
        {

          if(i == 0){
              servo[i].write(servoUnit[i].servoAngleStart);
              servo_pos[i] = servoUnit[i].servoAngleStart;
              servo[i].attach(D1);
              delay(100);
            }else if (i == 1){
              servo[i].write(servoUnit[i].servoAngleStart);
              servo_pos[i] = servoUnit[i].servoAngleStart;
              servo[i].attach(D2);
              delay(100);
            }else if (i == 2){
              servo[i].write(servoUnit[i].servoAngleStart);
              servo_pos[i] = servoUnit[i].servoAngleStart;
              servo[i].attach(D3);
              delay(100);
            }else if (i == 3){
              servo[i].write(servoUnit[i].servoAngleStart);
              servo_pos[i] = servoUnit[i].servoAngleStart;
              servo[i].attach(D4);
              delay(100);
            }
        }
      }

      delay(1000 / fps);
    }
  }
}

Task servoTasks(100, TASK_ONCE, &servoController); //Running the servo loop in a different task to not have them use the webservers resoucres -> more fluent ux

//SETUP

void setup()
{
  Serial.begin(115200);

  //Initialize LittleFS
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  read();

  AsyncWiFiManager wifiManager(&server, &dns);

  WiFi.hostname("flowcontrol");

  if (!wifiManager.autoConnect("FlowControl"))
  {
    delay(10000);
    ESP.reset();
    delay(5000);
  }

  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              read();
              request->send(LittleFS, "/index.html", String(), false, indexhtml_processor); });
              //Update website with current values
              

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) 
            { 
    //Dreheinheit 1
    servoUnit[0].servoRandomFlag = getFormParamBool(request,"randompatternservoname1");
    servoUnit[0].servoOnOff = getFormParamBool(request,"onoffname1");
    servoUnit[0].servoManualControl = getFormParamBool(request, "directcontrolname1");

    servoUnit[0].servoSpeed = getFormParamShort(request, "servospeed1");
    servoUnit[0].servoAngleStart = getFormParamShort(request, "minsliderservo1");
    servoUnit[0].servoAngleEnd = getFormParamShort(request, "maxsliderservo1");
    //Dreheinheit 2
    servoUnit[1].servoRandomFlag = getFormParamBool(request,"randompatternservoname2");
    servoUnit[1].servoOnOff = getFormParamBool(request,"onoffname2");
    servoUnit[1].servoManualControl = getFormParamBool(request, "directcontrolname2");

    servoUnit[1].servoSpeed = getFormParamShort(request, "servospeed2");
    servoUnit[1].servoAngleStart = getFormParamShort(request, "minsliderservo2");
    servoUnit[1].servoAngleEnd = getFormParamShort(request, "maxsliderservo2");
    //Dreheinheit 3
    servoUnit[2].servoRandomFlag = getFormParamBool(request,"randompatternservoname3");
    servoUnit[2].servoOnOff = getFormParamBool(request,"onoffname3");
    servoUnit[2].servoManualControl = getFormParamBool(request, "directcontrolname3");

    servoUnit[2].servoSpeed = getFormParamShort(request, "servospeed3");
    servoUnit[2].servoAngleStart = getFormParamShort(request, "minsliderservo3");
    servoUnit[2].servoAngleEnd = getFormParamShort(request, "maxsliderservo3");
    //Dreheinheit 4
    servoUnit[3].servoRandomFlag = getFormParamBool(request,"randompatternservoname4");
    servoUnit[3].servoOnOff = getFormParamBool(request,"onoffname4");
    servoUnit[3].servoManualControl = getFormParamBool(request, "directcontrolname4");

    servoUnit[3].servoSpeed = getFormParamShort(request, "servospeed4");
    servoUnit[3].servoAngleStart = getFormParamShort(request, "minsliderservo4");
    servoUnit[3].servoAngleEnd = getFormParamShort(request, "maxsliderservo4");
    write();

   request->redirect("/"); });

  // GET

  server.on("/pureknob.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/pureknob.js", "text/javascript"); });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/flowcontrol.ico", "image/x-icon"); });

  server.on("/scripts.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/scripts.js", "text/javascript", false); });

  server.on("/scriptscheck.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/scriptscheck.js", "text/javascript", false, scriptjs_processor); });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/style.css", "	text/css"); });

  // Start server
  server.begin();

  // add/init tasks
  runner.addTask(servoTasks);
  servoTasks.enable();
}

void loop()
{
  runner.execute();
}