import { ConvertVideotex } from "../common/ConvertVideotex.mjs";
import { CanvasManager } from "../common/CanvasManager.mjs";
import { loadFiles, sliceIntoChunks } from "./helpers";
import axios from "axios";
import "./style.css";

const $loadimg = document.querySelector("#loadimg");
const $ip = document.querySelector("#ip");
const img = document.createElement("img");

const CM = new CanvasManager(document.querySelector("#mockup"));
const CV = new ConvertVideotex();

img.addEventListener(
  "load",
  async () => {
    const chunks = sliceIntoChunks(CV.convert(CM.getPixels(img)), 100);
    const url = `http://${$ip.value}`;
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
  },
  false
);

$loadimg.addEventListener(
  "change",
  (e) => {
    loadFiles(img, e.target.files);
  },
  false
);
