export function CanvasManager(canvas) {
  this.context = canvas.getContext("2d");
  this.width = canvas.width;
  this.height = canvas.height;
}

CanvasManager.prototype.clear = function () {
  this.context.clearRect(0, 0, this.width, this.height);
  this.context.fillStyle = "black";
  this.context.fillRect(0, 0, this.width, this.height);
};

CanvasManager.prototype.redim = function (img) {
  let newWidth = img.width;
  let newHeight = img.height;
  newHeight *= this.width / newWidth;
  newWidth = this.width;
  if (newHeight > this.height) {
    newWidth *= this.height / newHeight;
    newHeight = this.height;
  }
  this.context.drawImage(
    img,
    0,
    0,
    img.width,
    img.height,
    (this.width - newWidth) / 2,
    (this.height - newHeight) / 2,
    newWidth,
    newHeight
  );
};

CanvasManager.prototype.getPixels = function (img) {
  this.clear();
  this.redim(img);
  return this.context.getImageData(0, 0, this.width, this.height).data;
};
