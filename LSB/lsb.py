# _*_ coding: UTF-8 _*_

from PIL import Image

def plus(str):
    # Python zfill() 方法返回指定长度的字符串，原符串右对齐，前面填充0。
    return str.zfill(8)

def toasc(strr):
    # 将strr转换成二进制
    return int(strr, 2)
#因为asscii的二进制第一位为 0b故将其替换
def get_bin(strr):
    mes = ''
    for i in range(len(strr)):
        mes = mes + plus(bin(ord(strr[i])).replace('0b',''))
    return mes
#测试函数用
# print(get_bin('FLAGFLAGFLAG'))

def mod(x, y):
    return x % y;

# str1为载体图片路径，str2为隐写信息字符串，str3为加密图片保存的路径
def Embed(str1, str2, str3):
    im = Image.open(str1)
    # 获取图片的宽和高并且将其输出
    width = im.size[0]
    print("width:" + str(width) + "\n")
    height = im.size[1]
    print("height:" + str(height) + "\n")
    count = 0
    # 获取需要隐藏的二进制信息
    key = get_bin(str2)
    # print(key)
    keylen = len(key)
    for h in range(0, height):
        for w in range(0, width):
            pixel = im.getpixel((w, h))
            print(w, h)
            a = pixel[0]
            b = pixel[1]
            c = pixel[2]
            if count == keylen:
                break
            # 下面的操作是将信息隐藏进去
            # 分别将每个像素点的RGB值余2，这样可以去掉最低位的值
            # 再从需要隐藏的信息中取出一位，转换为整型
            # 两值相加，就把信息隐藏起来了
            a = a - mod(a, 2) + int(key[count])
            count += 1
            if count == keylen:
                im.putpixel((w, h), (a, b, c))
                break

            b = b - mod(b, 2) + int(key[count])
            count += 1
            if count == keylen:
                im.putpixel((w, h), (a, b, c))
                break

            c = c - mod(c, 2) + int(key[count])
            count += 1
            if count == keylen:
                im.putpixel((w, h), (a, b, c))
                break

            if count % 3 == 0:
                im.putpixel((w, h), (a, b, c))
    im.save(str3)
# str1为加密载体图片的路径，然后直接输出解密内容，并加入了一个验证
def Extract(str1):
    list = []
    a = ""
    b = ""
    im = Image.open(str1)
    lenth = 32 * 8
    width = im.size[0]
    height = im.size[1]
    count = 0

    for h in range(0, height):
        for w in range(0, width):
            # 获得(w,h)点像素的值
            print(w, h)
            pixel = im.getpixel((w, h))
            # 此处余3，依次从R、G、B三个颜色通道获得最低位的隐藏信息
            if count % 3 == 0:
                count += 1
                b = b + str((mod(int(pixel[0]), 2)))
                if count == lenth:
                    break
            if count % 3 == 1:
                count += 1
                b = b + str((mod(int(pixel[1]), 2)))
                if count == lenth:
                    break
            if count % 3 == 2:
                count += 1
                b = b + str((mod(int(pixel[2]), 2)))
                if count == lenth:
                    break
        if count == lenth:
            break
    for i in range(0, len(b), 8):
        stra = toasc(b[i:i+8])
        list.append(chr(stra))
        # print(chr(stra))
    str2 = "".join(list)
    # 将解密的结果和原始隐藏信息进行对比
    if str2 == flag:
        print("加密内容与解密内容一致，程序运行成功!")
    print(str2)

# 原图
old = "./images/eva.bmp"
# 处理后输出的图片路径
new = "./images/eva_test.bmp"
# 需要隐藏的信息
flag = "FLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAG"
#用lsb对图片进行信息隐藏
# be_time = time()

Embed(old, flag, new)
#解密输出在屏幕上
Extract(new)
