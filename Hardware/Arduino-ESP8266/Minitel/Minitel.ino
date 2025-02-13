#include "Minitel1B_Soft.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <string.h>

Minitel minitel(3, 1); // RX, TX

const char *ssid = "3615Twitch";
const char *password = "gouz.dev";

AsyncWebServer server(80);

void setup()
{
  if (minitel.searchSpeed() != 4800) {     // search speed
    if (minitel.changeSpeed(4800) < 0) {   // set to 4800 if different
      minitel.searchSpeed();               // search speed again if change has failed
    }
  }
  //Serial.begin(4800);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    minitel.newScreen();
    minitel.println("Connecting to WiFi...");
    //Serial.println("Connecting to WiFi...");
    delay(1000);
  }
  minitel.newScreen();
  minitel.println(WiFi.localIP().toString());
  minitel.println(String(minitel.currentSpeed()));
  minitel.graphicMode();
  //Serial.println(WiFi.localIP().toString());
  

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { 
    request->send(200, "text/html", "<!doctype html><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>3615 Twitch</title><style>:root{font-family:Inter,system-ui,Avenir,Helvetica,Arial,sans-serif;line-height:1.5;font-weight:400}#app,body,html{width:100%;height:100%;margin:0}#mockup{width:80px;height:72px}#app{display:flex;align-items:center;justify-content:center}input,label{display:block}label{font-weight:700}input{width:300px;height:30px;text-align:center;margin-bottom:1rem}</style><div id=app><form><label for=loadimg>Image</label> <input id=loadimg type=file><canvas height=72 id=mockup width=80></canvas></form></div><script>function CanvasManager(t){this.context=t.getContext(\"2d\"),this.width=t.width,this.height=t.height}function ConvertVideotex(){}function loadFiles(t,e){if(e.length>0){let i=e[0];if(\"undefined\"!=typeof FileReader&&-1!=i.type.indexOf(\"image\")){let n=new FileReader;n.onload=e=>{t.src=e.target.result},n.readAsDataURL(i)}}}function sliceIntoChunks(t,e){let i=[];i.push([t.shift(),t.shift()]);for(let n=0;n<t.length;n+=5*e){let o=t.slice(n,n+5*e);i.push(o)}return i}CanvasManager.prototype.clear=function(){this.context.clearRect(0,0,this.width,this.height),this.context.fillStyle=\"black\",this.context.fillRect(0,0,this.width,this.height)},CanvasManager.prototype.redim=function(t){let e=t.width,i=t.height;i*=this.width/e,e=this.width,i>this.height&&(e*=this.height/i,i=this.height),this.context.drawImage(t,0,0,t.width,t.height,(this.width-e)/2,(this.height-i)/2,e,i)},CanvasManager.prototype.getPixels=function(t){return this.clear(),this.redim(t),this.context.getImageData(0,0,this.width,this.height).data},ConvertVideotex.prototype.convert=function(t){let e=0,i=0,n=[];for(let o=0;o<t.length;o+=4)n.push([t[o],t[o+1],t[o+2]]);let h=[0,4,1,5,2,6,3,7],s=[],r=0;for(i=0;i<24;i++)for(e=0;e<40;e++){let a=this.calcColors(e,i,n);s[r++]=27,s[r++]=h[Math.floor(a[0]/32)]+64,s[r++]=27,s[r++]=h[Math.floor(a[1]/32)]+80,s[r++]=this.calcChar(e,i,a,n)}let l=[12,14];for(let $=0;$<r;$++)l.push(s[$]);return l},ConvertVideotex.prototype.brightness=function(t){return Math.round((299*parseInt(t[0])+587*parseInt(t[1])+114*parseInt(t[2]))/1e3)},ConvertVideotex.prototype.calcColors=function(t,e,i){let n=2*t,o=3*e,h=this.brightness(i[n+80*o]),s=[h,h];for(let r=o;r<=o+2;r++)for(let a=n;a<=n+1;a++)(h=this.brightness(i[a+80*r]))<s[0]&&(s[0]=h),h>s[1]&&(s[1]=h);return s},ConvertVideotex.prototype.calcChar=function(t,e,i,n){let o=2*t,h=3*e,s=0;return s+=1*this.state(o+0,h+0,i,n),s+=2*this.state(o+1,h+0,i,n),s+=4*this.state(o+0,h+1,i,n),s+=8*this.state(o+1,h+1,i,n),s+=16*this.state(o+0,h+2,i,n),s+=32,s+=64*this.state(o+1,h+2,i,n)},ConvertVideotex.prototype.state=function(t,e,i,n){let o=this.brightness(n[t+80*e]);return Math.abs(i[0]-o)>Math.abs(i[1]-o)?0:1};const $loadimg=document.querySelector(\"#loadimg\"),img=document.createElement(\"img\"),CM=new CanvasManager(document.querySelector(\"#mockup\")),CV=new ConvertVideotex;$loadimg.addEventListener(\"change\",t=>{loadFiles(img,t.target.files)},!1),img.addEventListener(\"load\",async()=>{let t=CV.convert(CM.getPixels(img)),e=sliceIntoChunks(t,20),i=`http://${window.location.hostname}`;await fetch(`${i}/new`);for(let n=0;n<e.length;n++){const body=new FormData();body.append('trame', e[n].join(','));await fetch(`${i}/post`,{method:\"post\",body});}await fetch(`${i}/end`)},!1);</script>"); 
  });

  server.on("/new", HTTP_GET, [](AsyncWebServerRequest *request) {
    minitel.newScreen();
    request->send(200); 
  });

  server.on("/end", HTTP_GET, [](AsyncWebServerRequest *request) {
    minitel.noCursor();
    request->send(200); 
  });

  server.on("/post", HTTP_POST, action);
  server.on("/put", HTTP_GET, action);

  // accept CORS
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  // Start server
  server.begin();

  minitel.println("Ready!");
}

void action(AsyncWebServerRequest *request)
{
  AsyncWebParameter* p = request->getParam(0);
  char *code = strtok( (char*) p->value().c_str(), ",");
  while (code != NULL) {
    //Serial.println(atoi(code));
    minitel.writeByte(atoi(code));
    code = strtok(NULL, ",");
  }
  request->send(200);
}

void loop() {}