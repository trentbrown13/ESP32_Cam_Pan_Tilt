//#define ESP32_RTOS  // Uncomment this line if you want to use the code with freertos only on the ESP32
                    // Has to be done before including "OTA.h"
#include "OTA.h"
#include "esp_camera.h"
#include <TelnetStream.h>
#include <TelnetPrint.h>
//#include <esp_camera.h>
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h" // disable brownout problems
#include "soc/rtc_cntl_reg.h" // disable brownout problems
#include "esp_http_server.h"
#include <ESP32Servo.h>
// Replace with your network credentials
//const char* ssid = "QUACKY--2G";
//const char* password = "5martRG!";
const char* ssid = "BlueZebra";
const char* password = "jY7bzy2XJv";
#define PART_BOUNDARY "123456789000000000000987654321"
#define CAMERA_MODEL_AI_THINKER
#if defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#else
#error "Camera model not selected"
#endif
#define SERVO_1 14
#define SERVO_2 13
//#define SERVO_1 16
//#define SERVO_2 0
#define servo_speed 25  // delay between servo moves
#define servo_min_angle 3     // Minimum angle servo can move to
#define servo_max_angle 177   // Max angle the servo can move to

//#define Telnet


void home_servos(void);
void sweep_left(void);
void sweep_right(void);
void sweep_up(void);
void sweep_down(void);
void rotateLeft(void);
void rotateRight(void);

void init_servos(void);

uint32_t entry;  // for TelnetStream
#define SERVO_STEP 1
//#define SERVO_STEP 5
Servo servoN1;
Servo servoN2;
Servo servo1;
Servo servo2;
int servo1Pos = 0;
int servo2Pos = 0;
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary="
PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length:%u\r\n\r\n";
httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<html>
<head>
<p> <p> <p> <title>ESP32-CAM Robot</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body {
font-family: Arial;
text-align: center;
margin:0px auto;
padding-top: 30px;
}
table { margin-left: auto; margin-right: auto; }
td { padding: 8 px; }
.button {
background-color: #2f4468;
border: none;
color: white;
padding: 10px 20px;
text-align: center;
text-decoration: none;
display: inline-block;
font-size: 18px;
margin: 6px 3px;
cursor: pointer;
-webkit-touch-callout: none;
-webkit-user-select: none;
-khtml-user-select: none;
-moz-user-select: none;
-ms-user-select: none;
user-select: none;
-webkit-tap-highlight-color: rgba(0,0,0,0);
}
img { width: auto ;
max-width: 100% ;
height: auto ;
}
</style>
</head>
<body>
<p> <h1>ESP32-CAM PAN and TILT VSBD</h1></p>
<!-- Hacky way to add space to accomodate image rotation - I need to look at css  FIXME -->
<br> </br>
<br> </br>
<img src="" id="photo" >
<br </br>
<br> </br>
<br> </br>
<p> <h4>STEP</h4>
<table>
<tr><td colspan="3" align="center"><button class="button"
onmousedown="toggleCheckbox('up');"
ontouchstart="toggleCheckbox('up');">Up</button></td></tr>
<tr><td align="center"><button class="button"
onmousedown="toggleCheckbox('left');"
ontouchstart="toggleCheckbox('left');">Left</button></td><td
align="center"></td><td align="center"><button class="button"
onmousedown="toggleCheckbox('right');"
ontouchstart="toggleCheckbox('right');">Right</button></td></tr>
<tr><td colspan="3" align="center"><button class="button"
onmousedown="toggleCheckbox('down');"
onmouseup="toggleCheckbox('stop');"
ontouchstart="toggleCheckbox('down');">Down</button></td></tr>
</table></p>

<p>
<table>
<tr><td colspan="1" align="center"><button class="button"
onmousedown="toggleCheckbox('home');"
ontouchstart="toggle_Checkbox('home');">Home</button></td></tr>
</table></p>

<p> <h4>SWEEP</h4>
<table>
<tr><td colspan="3" align="center"><button class="button"
onmousedown="toggleCheckbox('sweep_up');"
ontouchstart="toggle_Checkbox('sweep_up');">Up</button></td></tr>
<tr><td align="center"><button class="button"
onmousedown="toggleCheckbox('sweep_left');"
ontouchstart="toggleCheckbox('sweep_left');">Left</button></td><td
align="center"></td><td align="center"><button class="button"
onmousedown="toggleCheckbox('sweep_right');"
ontouchstart="toggleCheckbox('sweep_right');">Right</button></td></tr>
<tr><td colspan="3" align="center"><button class="button" onmousedown="toggleCheckbox('sweep_down');" ontouchstart="toggleCheckbox('sweep_down');">Down</button></td></tr>
</table></p>

<p> 
<h4>Rotate Image</h4> 
<table>
<tr><td colspan="2" align="center"><button class="button"
<button onClick="rotateLeft()">LEFT</button>
<tr><td align="center"><button class="button"
<button onClick="rotateRight()">RIGHT</button>
</table>
</p>

<script>
let rotation = 0;

function rotateRight()
{
  rotation += 90; 
  if (rotation === 360)
  {
    // rotate back to 0
    rotation = 0;
  }
   document.querySelector("#photo").style.transform = `rotate(${rotation}deg)`;
}

function rotateLeft()
{
  rotation -= 90; 
  if (rotation === 0)
  {
    rotation = 360;
  }
   document.querySelector("#photo").style.transform = `rotate(${rotation}deg)`;
}
</script>

<script>
function toggleCheckbox(x) {
var xhr = new XMLHttpRequest();
xhr.open("GET", "/action?go=" + x, true);
xhr.send();
}

window.onload = document.getElementById("photo").src =
window.location.href.slice(0, -1) + ":81/stream";
</script>


</body>
<!-- </html> -->
</html>
)rawliteral";
static esp_err_t index_handler(httpd_req_t *req){
httpd_resp_set_type(req, "text/html");
return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}
static esp_err_t stream_handler(httpd_req_t *req){
camera_fb_t * fb = NULL;
esp_err_t res = ESP_OK;
size_t _jpg_buf_len = 0;
uint8_t * _jpg_buf = NULL;
char * part_buf[64];
res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
if(res != ESP_OK){
return res;
}
while(true){
fb = esp_camera_fb_get();
if (!fb) {
Serial.println("Camera capture failed");
res = ESP_FAIL;
} else {
if(fb->width > 400){
if(fb->format != PIXFORMAT_JPEG){
 bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
esp_camera_fb_return(fb);
fb = NULL;
if(!jpeg_converted){
Serial.println("JPEG compression failed");
res = ESP_FAIL;
}
} else {
_jpg_buf_len = fb->len;
_jpg_buf = fb->buf;
}
}
}
if(res == ESP_OK){
size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
}
if(res == ESP_OK){
res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
}
if(res == ESP_OK){
res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
}
if(fb){
esp_camera_fb_return(fb);
fb = NULL;
_jpg_buf = NULL;
} else if(_jpg_buf){
free(_jpg_buf);
_jpg_buf = NULL;
}
if(res != ESP_OK){
break;
}
//Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
}
return res;
}

static esp_err_t cmd_handler(httpd_req_t *req){
char* buf;
size_t buf_len;
char variable[32] = {0,};
buf_len = httpd_req_get_url_query_len(req) + 1;
if (buf_len > 1) {
  buf = (char*)malloc(buf_len);
  if(!buf){
    httpd_resp_send_500(req);
    return ESP_FAIL;
    }
  if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
    if (httpd_query_key_value(buf, "go", variable, sizeof(variable)) ==
      ESP_OK) {
      } else {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
        }
    } else {
      free(buf);
      httpd_resp_send_404(req);
      return ESP_FAIL;
       }
    free(buf);
    } else {
        httpd_resp_send_404(req);
        return ESP_FAIL;
        }
    sensor_t * s = esp_camera_sensor_get();
    int res = 0;
    //char stopServos[]  = "false";
    //strcpy(stopServos, "false");

    //if(!strcmp(variable, "stop")) {
      //strcpy(stopServos, "true");
   // }
    //else if(!strcmp(variable, "down")) {
     if(!strcmp(variable, "down")) {
      if(servo1Pos <= 175) {
          servo1Pos += SERVO_STEP;
          servo1.write(servo1Pos);
          //delay(500);
          }
      Serial.println(servo1Pos);
      Serial.println("Down");
      #ifdef Telnet
      TelnetStream.println(servo1Pos);
      TelnetStream.println("Down");
      #endif
     }
      //while (stopServos == false) {
      //while (!strcmp(stopServos, "false")) {
     /* while (!strcmp(variable, "stop")) {
        if(servo1Pos <= 175) {
          servo1Pos += SERVO_STEP;
          servo1.write(servo1Pos);
          delay(500);
          }
        //stopServos = false;
        Serial.println(servo1Pos);
        Serial.println("Up");
      }
    }*/
  /*
    do {
      if(servo1Pos <= 175) {
          servo1Pos += SERVO_STEP;
          servo1.write(servo1Pos);
          TelnetStream.println("Down");
          delay(500);
      }
       if (!strcmp(variable, "stop")) {
          TelnetStream.println("Stop");
          break;
          }
          
        //}
      } while (strcmp(variable, "stop")!= 0);
       //} while (!strcmp(variable, "stop") != 0);
      //} while (!strcmp(variable, "stop") == 0);
    }
   */ 
    else if(!strcmp(variable, "left")) {
        if(servo2Pos <= 175) {
            servo2Pos += SERVO_STEP;
            servo2.write(servo2Pos);
          }
      Serial.println(servo2Pos);
      Serial.println("Left");
      #ifdef Telnet
      TelnetStream.println("Left");
      #endif
     }
     
    else if(!strcmp(variable, "right")) {
        if(servo2Pos >= 5) {
        servo2Pos -= SERVO_STEP;
        servo2.write(servo2Pos);
        
        }
    Serial.println(servo2Pos);
    Serial.println("Right");
    #ifdef Telnet
    TelnetStream.println("Right");
    #endif
      }
    else if(!strcmp(variable, "up")) {
        if(servo1Pos >= 5) {
            servo1Pos -= SERVO_STEP;
            servo1.write(servo1Pos);
        }
        Serial.println(servo1Pos);
        Serial.println("up");
        #ifdef Telnet
        TelnetStream.println("Up");
        #endif
     }
     
    else if(!strcmp(variable, "home")) {
        home_servos();
      }
    else if(!strcmp(variable, "sweep_left")) {
        sweep_left();
      }
    else if(!strcmp(variable, "sweep_right")) {
        sweep_right();
      }
    else if(!strcmp(variable, "sweep_up")) {
        sweep_up();
      }
    else if(!strcmp(variable, "sweep_down")) {
        sweep_down();
      }

    else {
        res = -1;
      }
  if(res){
      return httpd_resp_send_500(req);
    }
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
}



void startCameraServer(){
httpd_config_t config = HTTPD_DEFAULT_CONFIG();
config.server_port = 80;
httpd_uri_t index_uri = {
.uri = "/",
.method = HTTP_GET,
.handler = index_handler,
.user_ctx = NULL
};
httpd_uri_t cmd_uri = {
.uri = "/action",
.method = HTTP_GET,
.handler = cmd_handler,
.user_ctx = NULL
};
httpd_uri_t stream_uri = {
.uri = "/stream",
.method = HTTP_GET,
.handler = stream_handler,
.user_ctx = NULL
};
if (httpd_start(&camera_httpd, &config) == ESP_OK) {
httpd_register_uri_handler(camera_httpd, &index_uri);
httpd_register_uri_handler(camera_httpd, &cmd_uri);
}
config.server_port += 1;
config.ctrl_port += 1;
if (httpd_start(&stream_httpd, &config) == ESP_OK) {
httpd_register_uri_handler(stream_httpd, &stream_uri);
}
}
int pos = 0;

void init_servos(void)
{
  // servoXpos should be set to 0 at start of sketch
  
  if(servo1Pos < 90) {
    Serial.println("Homing Servo1!");
    #ifdef Telnet
         TelnetStream.println("Homing Servo1!");   
    #endif
    for (servo1Pos; servo1Pos <= 90; servo1Pos +=1)
    {
      servo1.write(servo1Pos);
      delay(servo_speed);
      Serial.println(servo1Pos);
      #ifdef Telnet
         TelnetStream.println(servo1Pos);   
      #endif
    }
  }
  else{
    Serial.println("Homing Servo1!");
    #ifdef Telnet
         TelnetStream.println("Homing Servo1!");   
     #endif
    for (servo1Pos; servo1Pos >= 90; servo1Pos -=1)
    {
      servo1.write(servo1Pos);
      delay(servo_speed);
      Serial.println(servo1Pos);
       #ifdef Telnet
         TelnetStream.println(servo1Pos);   
      #endif
    }
  }

  if(servo2Pos < 90) {
    Serial.println("Homing Servo2!");
    #ifdef Telnet
        TelnetStream.println("Homing Servo2!");   
     #endif
    for (servo2Pos; servo2Pos <= 90; servo2Pos +=1)
    {
      servo2.write(servo2Pos);
      delay(servo_speed);
      Serial.println(servo2Pos);
       #ifdef Telnet
         TelnetStream.println(servo2Pos);   
      #endif
    }
  }
  else {
    Serial.println("Homing Servo2!");
    #ifdef Telnet
        TelnetStream.println("Homing Servo2!");   
    #endif
    for (servo2Pos; servo2Pos >= 90; servo2Pos -=1)
    {
      servo2.write(servo2Pos);
      delay(servo_speed);
      Serial.println(servo2Pos);
       #ifdef Telnet
         TelnetStream.println(servo2Pos);   
      #endif
    }
  }
}

void home_servos(void) {
   init_servos();
}
 
void sweep_left(void)
{
  int curr_S2_pos = servo2.read();
  //for (curr_S2_pos; servo2Pos >= 3; servo2Pos -=1) {
  for (servo2Pos; servo2Pos >= servo_min_angle; servo2Pos -=1) {
    servo2.write(servo2Pos);
    delay(servo_speed);
    Serial.println(servo2Pos);
    #ifdef Telnet
       TelnetStream.println(servo2Pos);   
    #endif
  }
}

void sweep_right(void)
{
  int curr_S2_pos = servo2.read();
  //for (curr_S2_pos; servo2Pos <= 178; servo2Pos +=1) {
  for (servo2Pos; servo2Pos <= servo_max_angle; servo2Pos +=1) {
    servo2.write(servo2Pos);
    delay(servo_speed);
    Serial.println(servo2Pos);
    #ifdef Telnet
       TelnetStream.println(servo2Pos);   
    #endif
  }
}

void sweep_up(void)
{
  int curr_S1_pos = servo1.read();
  for (curr_S1_pos; servo1Pos >= servo_min_angle; servo1Pos -=1) {
    servo1.write(servo1Pos);
    delay(servo_speed);
    Serial.println(servo1Pos);
     #ifdef Telnet
       TelnetStream.println(servo1Pos);   
    #endif
  }
}

void sweep_down(void)
{
  int curr_S1_pos = servo1.read();
  for (curr_S1_pos; servo1Pos <= servo_max_angle; servo1Pos +=1) {
    servo1.write(servo1Pos);
    delay(servo_speed);
    Serial.println(servo1Pos);
     #ifdef Telnet
       TelnetStream.println(servo1Pos);   
    #endif
  }
}


void setup() {
  // Allow allocation of all timers
ESP32PWM::allocateTimer(0);
ESP32PWM::allocateTimer(1);
ESP32PWM::allocateTimer(2);
ESP32PWM::allocateTimer(3);
servo1.setPeriodHertz(50);    // standard 50 hz servo
servo2.setPeriodHertz(50);    // standard 50 hz servo
WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

init_servos();
servoN1.attach(2, 1000, 2000);
servoN2.attach(13, 1000, 2000);
servo1.attach(SERVO_1, 500, 2400);
servo2.attach(SERVO_2, 500, 2400);
//servo1.write(servo1Pos);
//servo2.write(servo2Pos);
//setupOTA("TemplateSketch", ssid, password);
setupOTA(ssid, password);

#ifdef Telnet
TelnetStream.begin();
#endif

Serial.begin(115200);
//Serial.setDebugOutput(false);
camera_config_t config;
config.ledc_channel = LEDC_CHANNEL_0;
config.ledc_timer = LEDC_TIMER_0;
config.pin_d0 = Y2_GPIO_NUM;
config.pin_d1 = Y3_GPIO_NUM;
config.pin_d2 = Y4_GPIO_NUM;
config.pin_d3 = Y5_GPIO_NUM;
config.pin_d4 = Y6_GPIO_NUM;
config.pin_d5 = Y7_GPIO_NUM;
config.pin_d6 = Y8_GPIO_NUM;
config.pin_d7 = Y9_GPIO_NUM;
config.pin_xclk = XCLK_GPIO_NUM;
config.pin_pclk = PCLK_GPIO_NUM;
config.pin_vsync = VSYNC_GPIO_NUM;
config.pin_href = HREF_GPIO_NUM;
config.pin_sscb_sda = SIOD_GPIO_NUM;
config.pin_sscb_scl = SIOC_GPIO_NUM;
config.pin_pwdn = PWDN_GPIO_NUM;
config.pin_reset = RESET_GPIO_NUM;
config.xclk_freq_hz = 20000000;
config.pixel_format = PIXFORMAT_JPEG;
if(psramFound()){
config.frame_size = FRAMESIZE_VGA;
config.jpeg_quality = 10;
config.fb_count = 2;
} else {
config.frame_size = FRAMESIZE_SVGA;
config.jpeg_quality = 12;
config.fb_count = 1;
}
// Camera init
esp_err_t err = esp_camera_init(&config);
if (err != ESP_OK) {
Serial.printf("Camera init failed with error 0x%x", err);
return;
}
//TelnetStream.begin();
// OTA Stuff includes wifi setup
//setupOTA("TemplateSketch", mySSID, myPASSWORD);
//setupOTA("TemplateSketch", ssid, password);
setupOTA(ssid, password);

/*
// Wi-Fi connection - Now in setupOta (in OTA.h)
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
Serial.print("Camera Stream Ready! Go to: http://");
Serial.println(WiFi.localIP());
*/
// Start streaming web server
startCameraServer();
//home_servos();
init_servos();
//#ifdef Telnet
//TelnetStream.begin();
//#endif
}
void loop() {
ArduinoOTA.handle();
 entry = micros();
//#ifdef defined(ESP32_RTOS) && defined(ESP32)
//#else // If you do not use FreeRTOS, you have to regulary call the handle method.
//ArduinoOTA.handle();
//#endif
#ifdef Telnet
  switch (TelnetStream.read()) {
    case 'F':
    TelnetStream.println("Fuck you, that HURT!");
    break;
    case 'C':
    TelnetStream.println("bye bye");
    TelnetStream.stop();
    break;
  }
  #endif


    
 #ifdef telnet
  TelnetStream.println(micros()-entry);
  TelnetStream.println("Loop");
  delay(500);
 #endif
}
 
//}
