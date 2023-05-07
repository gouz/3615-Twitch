import { CanvasManager } from "../common/CanvasManager.mjs";
import { ConvertVideotex } from "../common/ConvertVideotex.mjs";
import dotenv from "dotenv";
import tmi from "tmi.js";
import axios from "axios";
import { createCanvas, loadImage } from "canvas";

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

const sliceIntoChunks = (arr, chunkSize) => {
  const res = [];
  for (let i = 0; i < arr.length; i += chunkSize) {
    const chunk = arr.slice(i, i + chunkSize);
    res.push(chunk);
  }
  return res;
};

const treatImage = async (img) => {
  inTreatment = true;
  const url = `http://${process.env.MINITEL_IP}`;
  const chunks = sliceIntoChunks(CV.convert(CM.getPixels(img)), 100);
  await axios.get(`${url}/new`);
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
  inTreatment = false;
  setTimeout(() => {
    if (messageStack.length) {
      treatMessage(messageStack.shift());
    }
  }, 2000); // we display at least 2 sec the image
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
        .get(
          `https://api.giphy.com/v1/gifs/random?api_key=${
            process.env.GIPHY_API
          }&tag=${encodeURI(prompt)}`
        )
        .then((response) => {
          if (response.data.data.images)
            loadImage(response.data.data.images.original.url).then(treatImage);
        });
    }
  }
};

const messageStack = [];
let inTreatment = false;

client.connect().catch(console.error);
client.on("message", (channel, tags, message, self) => {
  if (self) return;
  if (inTreatment) {
    messageStack.push({ message, tags });
    return;
  } else {
    treatMessage({ message, tags });
  }
});
