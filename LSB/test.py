from PIL import Image

img = Image.open('./images/eva.bmp')
## 这里遇到一个问题，与输入图片有关，有些图片并不是输出一个tuple，而是输出一个int类型的值
pixel = img.getpixel((22, 22))
print(pixel)
