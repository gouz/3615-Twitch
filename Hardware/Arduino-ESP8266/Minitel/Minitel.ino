#include "Minitel1B_Soft.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>

Minitel minitel(3, 1); // RX, TX

const char *ssid = "bobox";
const char *password = "mon joli mot de passe";

AsyncWebServer server(80);

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  for (size_t i = 0; i < len; i++)
  {
    minitel.writeByte(data[i]);
  }
  request->send(200);
}

void setup()
{
  minitel.changeSpeed(minitel.searchSpeed());
  minitel.newScreen();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    minitel.newScreen();
    minitel.println("Connecting to WiFi...");
    delay(1000);
  }
  minitel.newScreen();
  minitel.println(WiFi.localIP().toString());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", "<!doctype html><html lang=en><head><meta charset=UTF-8><meta name=viewport content=\"width=device-width,initial-scale=1\"><title>3615 Twitch</title><style>:root{font-family:Inter,system-ui,Avenir,Helvetica,Arial,sans-serif;line-height:1.5;font-weight:400}#app,body,html{width:100%;height:100%;margin:0}#mockup{width:80px;height:72px}#app{display:flex;align-items:center;justify-content:center}input,label{display:block}label{font-weight:700}input{width:300px;height:30px;text-align:center;margin-bottom:1rem}</style></head><body><div id=app><form><label for=loadimg>Image</label><input type=file id=loadimg><canvas width=80 height=72 id=mockup></canvas></form></div><script>function a(){}function l(t){this.context=t.getContext(\"2d\"),this.width=t.width,this.height=t.height}function f(t,e){if(e.length>0){const o=e[0];if(typeof FileReader<\"u\"&&-1!=o.type.indexOf(\"image\")){const e=new FileReader;e.onload=e=>{t.src=e.target.result},e.readAsDataURL(o)}}}function u(t,e){const o=[];for(let i=0;i<t.length;i+=e){const n=t.slice(i,i+e);o.push(n)}return o}!function(){const t=document.createElement(\"link\").relList;if(!(t&&t.supports&&t.supports(\"modulepreload\"))){for(const t of document.querySelectorAll('link[rel=\"modulepreload\"]'))e(t);new MutationObserver((t=>{for(const o of t)if(\"childList\"===o.type)for(const t of o.addedNodes)\"LINK\"===t.tagName&&\"modulepreload\"===t.rel&&e(t)})).observe(document,{childList:!0,subtree:!0})}function e(t){if(t.ep)return;t.ep=!0;const e=function(t){const e={};return t.integrity&&(e.integrity=t.integrity),t.referrerPolicy&&(e.referrerPolicy=t.referrerPolicy),\"use-credentials\"===t.crossOrigin?e.credentials=\"include\":\"anonymous\"===t.crossOrigin?e.credentials=\"omit\":e.credentials=\"same-origin\",e}(t);fetch(t.href,e)}}(),a.prototype.convert=function(t){let e=0,o=0;const i=[];for(let e=0;e<t.length;e+=4)i.push([t[e],t[e+1],t[e+2]]);const n=[0,4,1,5,2,6,3,7],r=[];let s=0;for(o=0;o<24;o++)for(e=0;e<40;e++){const t=this.calcColors(e,o,i);r[s++]=27,r[s++]=n[Math.floor(t[0]/32)]+64,r[s++]=27,r[s++]=n[Math.floor(t[1]/32)]+80,r[s++]=this.calcChar(e,o,t,i)}const h=[12,14];for(let t=0;t<s;t++)h.push(r[t]);return h},a.prototype.brightness=function(t){return Math.round((299*parseInt(t[0])+587*parseInt(t[1])+114*parseInt(t[2]))/1e3)},a.prototype.calcColors=function(t,e,o){const i=2*t,n=3*e;let r=this.brightness(o[i+80*n]),s=[r,r];for(let t=n;t<=n+2;t++)for(let e=i;e<=i+1;e++)r=this.brightness(o[e+80*t]),r<s[0]&&(s[0]=r),r>s[1]&&(s[1]=r);return s},a.prototype.calcChar=function(t,e,o,i){const n=2*t,r=3*e;let s=0;return s+=Math.pow(2,0)*this.state(n+0,r+0,o,i),s+=Math.pow(2,1)*this.state(n+1,r+0,o,i),s+=Math.pow(2,2)*this.state(n+0,r+1,o,i),s+=Math.pow(2,3)*this.state(n+1,r+1,o,i),s+=Math.pow(2,4)*this.state(n+0,r+2,o,i),s+=1*Math.pow(2,5),s+=Math.pow(2,6)*this.state(n+1,r+2,o,i),s},a.prototype.state=function(t,e,o,i){const n=this.brightness(i[t+80*e]);return Math.abs(o[0]-n)>Math.abs(o[1]-n)?0:1},l.prototype.clear=function(){this.context.clearRect(0,0,this.width,this.height),this.context.fillStyle=\"black\",this.context.fillRect(0,0,this.width,this.height)},l.prototype.redim=function(t){let e=t.width,o=t.height;o*=this.width/e,e=this.width,o>this.height&&(e*=this.height/o,o=this.height),this.context.drawImage(t,0,0,t.width,t.height,(this.width-e)/2,(this.height-o)/2,e,o)},l.prototype.getPixels=function(t){return this.clear(),this.redim(t),this.context.getImageData(0,0,this.width,this.height).data};const p=document.querySelector(\"#loadimg\"),d=document.createElement(\"img\"),g=new l(document.querySelector(\"#mockup\")),w=new a;d.addEventListener(\"load\",(async()=>{const t=u(w.convert(g.getPixels(d)),100),e=`http://${window.location.hostname}`;await fetch(`${e}/new`);for(let o=0;o<t.length;o++){const i=new FormData;i.append(\"encrypt\",new Blob([Uint8ClampedArray.from(t[o])])),await fetch(`${e}/post`,{method:\"post\",body:i})}await fetch(`${e}/end`)}),!1),p.addEventListener(\"change\",(t=>{f(d,t.target.files)}),!1)</script></body></html>"); });

  server.on("/new", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    minitel.newScreen();
    request->send(200); });

  server.on("/end", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    minitel.noCursor();
    request->send(200); });

  server.on(
      "/post", HTTP_POST, [](AsyncWebServerRequest *request) {}, handleUpload);

  // accept CORS
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  // Start server
  server.begin();

  minitel.println("Ready!");
}

void loop() {}