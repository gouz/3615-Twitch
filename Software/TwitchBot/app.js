import { CanvasManager } from "../common/CanvasManager.mjs";
import { ConvertVideotex } from "../common/ConvertVideotex.mjs";
import dotenv from "dotenv";
import tmi from "tmi.js";
import axios from "axios";
import { createCanvas, loadImage, Image } from "canvas";
import terminalImage from "terminal-image";

dotenv.config();

const client = new tmi.Client({
  options: { debug: false, messagesLogLevel: "info" },
  connection: {
    reconnect: true,
    secure: true,
  },
  channels: [`${process.env.TWITCH_CHANNEL}`],
});

const canvas = createCanvas(80, 72);
const CM = new CanvasManager(canvas);
const CV = new ConvertVideotex();
const url = `http://${process.env.MINITEL_IP}`;

function sliceIntoChunks(arr, chunkSize) {
  const res = [];
  res.push([arr.shift(), arr.shift()]);
  for (let i = 0; i < arr.length; i += chunkSize * 5) {
    const chunk = arr.slice(i, i + chunkSize * 5);
    res.push(chunk);
  }
  return res;
}

const prepareImage = (img) => {
  const chunks = sliceIntoChunks(CV.convert(CM.getPixels(img)), 20);
  const arr = [`${url}/new`];
  for (let i = 0; i < chunks.length; i++)
    arr.push(
      `${url}/put?${new URLSearchParams({
        trame: chunks[i].join(","),
      }).toString()}`
    );
  arr.push(`${url}/end`);
  return arr;
};

let isCalling = false;
const minitel_array = [];

const call = () => {
  isCalling = true;
  if (minitel_array.length) {
    const plo = minitel_array.shift();
    fetch(plo).then(() => {
      if (minitel_array.length) call();
      else isCalling = false;
    });
  }
  else isCalling = false;
};

const treatImage = async (img) => {
  minitel_array.push(...prepareImage(img));
  if (!isCalling) call();
};

const treatMessage = ({ message, tags }) => {
  const parts = message.toString().split(" ");
  const cmd = parts.shift();
  if (cmd.toLowerCase() === "!3615") {
    const prompt = parts.join(" ");
    if ((tags.mod || tags.badges?.broadcaster) && prompt.startsWith("http")) {
      // is moderator, then this viewer can send image directly
      loadImage(prompt).then(treatImage);
    } else {
      axios
        .post(`${process.env.STABLE_DIFFUSION_URL}/sdapi/v1/txt2img`, {
          width: 512,
          height: 512,
          steps: 10,
          prompt: `${prompt}, black and white`,
          negative_prompt: "nsfw",
          cfg_scale: 7,
        })
        .then(async (response) => {
          if (response.data.images.length) {
            const img = new Image();
            img.src = `data:image/png;base64, ${response.data.images[0]}`;
            treatImage(img);
            console.log(prompt);
            console.log();
            console.log(
              await terminalImage.buffer(
                Buffer.from(response.data.images[0], "base64")
              )
            );
          }
        });
    }
  }
};

client.connect().catch(console.error);
client.on("message", (_, tags, message, self) => {
  if (self) return;
  treatMessage({ message, tags });
});
