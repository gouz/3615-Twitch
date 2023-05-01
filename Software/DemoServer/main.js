import { ConvertVideotex } from "../common/ConvertVideotex.mjs";
import { CanvasManager } from "../common/CanvasManager.mjs";
import { loadFiles, sliceIntoChunks } from "./helpers";

const $loadimg = document.querySelector("#loadimg");
const img = document.createElement("img");

const CM = new CanvasManager(document.querySelector("#mockup"));
const CV = new ConvertVideotex();

img.addEventListener(
  "load",
  async () => {
    const chunks = sliceIntoChunks(CV.convert(CM.getPixels(img)), 100);
    const url = `http://${window.location.hostname}`;
    await fetch(`${url}/new`);
    for (let i = 0; i < chunks.length; i++) {
      const body = new FormData();
      body.append("encrypt", new Blob([Uint8ClampedArray.from(chunks[i])]));
      await fetch(`${url}/post`, {
        method: "post",
        body,
      });
    }
    await fetch(`${url}/end`);
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
