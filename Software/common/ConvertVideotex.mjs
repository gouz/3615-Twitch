export function ConvertVideotex() {}

ConvertVideotex.prototype.convert = function (pixels) {
  let x = 0;
  let y = 0;
  const pixelsArray = [];
  for (let i = 0; i < pixels.length; i += 4) {
    pixelsArray.push([pixels[i], pixels[i + 1], pixels[i + 2]]);
  }

  const colorLum = [0, 4, 1, 5, 2, 6, 3, 7];
  /*
    Colors from dark to light:
      black   += 0
      blue    += 4
      red     += 1
      magenta += 5
      green   += 2
      cyan    += 6
      yellow  += 3
      white   += 7
  */
  const str = [];

  let index = 0;

  for (y = 0; y < 24; y++) {
    for (x = 0; x < 40; x++) {
      const colors = this.calcColors(x, y, pixelsArray);
      str[index++] = 0x1b; // ESC char
      str[index++] = colorLum[Math.floor(colors[0] / 32)] + 0x40; // 0x40 = Black (front)
      str[index++] = 0x1b; // ESC char
      str[index++] = colorLum[Math.floor(colors[1] / 32)] + 0x50; // 0x50 = Black (background)
      str[index++] = this.calcChar(x, y, colors, pixelsArray);
    }
  }

  const result = [0x0c, 0x0e]; // clear screen & semi graphic mode
  for (let i = 0; i < index; i++) result.push(str[i]);

  return result;
};

ConvertVideotex.prototype.brightness = function (rgb) {
  return Math.round(
    (parseInt(rgb[0]) * 299 + parseInt(rgb[1]) * 587 + parseInt(rgb[2]) * 114) /
      1000
  );
};

ConvertVideotex.prototype.calcColors = function (x, y, pixels) {
  const xPix = x * 2;
  const yPix = y * 3;
  let lum = this.brightness(pixels[xPix + yPix * 80]);
  let color = [lum, lum];
  for (let b = yPix; b <= yPix + 2; b++) {
    for (let a = xPix; a <= xPix + 1; a++) {
      lum = this.brightness(pixels[a + b * 80]);
      if (lum < color[0]) color[0] = lum;
      if (lum > color[1]) color[1] = lum;
    }
  }
  return color;
};

ConvertVideotex.prototype.calcChar = function (x, y, colors, pixels) {
  const xPix = x * 2;
  const yPix = y * 3;
  let car = 0;
  car += Math.pow(2, 0) * this.state(xPix + 0, yPix + 0, colors, pixels);
  car += Math.pow(2, 1) * this.state(xPix + 1, yPix + 0, colors, pixels);
  car += Math.pow(2, 2) * this.state(xPix + 0, yPix + 1, colors, pixels);
  car += Math.pow(2, 3) * this.state(xPix + 1, yPix + 1, colors, pixels);
  car += Math.pow(2, 4) * this.state(xPix + 0, yPix + 2, colors, pixels);
  car += Math.pow(2, 5) * 1;
  car += Math.pow(2, 6) * this.state(xPix + 1, yPix + 2, colors, pixels);
  return car;
};

ConvertVideotex.prototype.state = function (x, y, colors, pixels) {
  const color = this.brightness(pixels[x + y * 80]);
  return Math.abs(colors[0] - color) > Math.abs(colors[1] - color) ? 0 : 1;
};
