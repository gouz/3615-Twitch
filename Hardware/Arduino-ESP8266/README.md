# 3615-Twitch :: ESP8266 Code

## Requirements

This project uses 3 librairies : 

- [Minitel1B_Soft](https://github.com/eserandour/Minitel1B_Soft) (included in the source code)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP) (required by `ESPAsyncWebServer`)

## Configuration

To connect to the Minitel, we use the RX/TX/GND pins of the ESP8266.

```c
Minitel minitel(3, 1); // RX, TX
```

To connect the ESP8266 to your WiFi, then change values here:

```c
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
```

## How to use the server?

The code present here with libraries take a lot of memories on the ESP8266, then we can't push the `Videotex` image in one `POST`.

The code below defines 3 methods to do that:

```c
server.on("/new", HTTP_GET, [](AsyncWebServerRequest *request) {
    minitel.newScreen();
    request->send(200);
});

server.on("/end", HTTP_GET, [](AsyncWebServerRequest *request) {
    minitel.noCursor();
    request->send(200);
});

server.on("/post", HTTP_POST, [](AsyncWebServerRequest * request) {}, handleUpload);
```

and this code write the image to the Minitel

```c
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  for(size_t i = 0; i < len; i++){
    minitel.writeByte(data[i]);
  }
  request->send(200);
}
```

So, to display an image on the Minitel, we have to respect a sequence and push the Videotex (an array of bytes) in many step (per 100 bytes).

```js
// using axios in JavaScript

await axios.get(`${url}/new`);
const chunks = [[...], [...], ...]; // an array of array of 100 bytes
for (let i = 0; i < chunks.length; i++) {
    await axios.post(
    `${url}/post`,
    {
        encrypt: Uint8Array.from(chunks[i]),
    },
    {
        headers: {
        "Content-Type": "multipart/form-data",
        },
    }
    );
}
await axios.get(`${url}/end`);
```