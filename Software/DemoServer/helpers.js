export function loadFiles(img, files) {
  if (files.length > 0) {
    const file = files[0];
    if (typeof FileReader !== "undefined" && file.type.indexOf("image") != -1) {
      const reader = new FileReader();
      reader.onload = (e) => {
        img.src = e.target.result;
      };
      reader.readAsDataURL(file);
    }
  }
}

export function sliceIntoChunks(arr, chunkSize) {
  const res = [];
  for (let i = 0; i < arr.length; i += chunkSize) {
    const chunk = arr.slice(i, i + chunkSize);
    res.push(chunk);
  }
  return res;
}
