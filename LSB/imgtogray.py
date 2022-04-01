import matplotlib.pyplot as plt
from PIL import Image

'''

'''
def convert_to_Gray(path, size = None):
    img = Image.open(path)
    # img.show()
    img_gray = img.convert("L")
    # img_gray.show()
    if (size != None):
        img_gray.thumbnail(size, Image.ANTIALIAS)
    filename = path[9:].split('.')[0]
    img_gray.save("./images/" + filename + "_gray.bmp", "BMP")

def test():
    path = "./images/bulesky.bmp"
    convert_to_Gray(path)
    
if __name__ == "__main__":
    test()
