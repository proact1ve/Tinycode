import requests
import time
import ssl
import hashlib
import datetime
def md5value(s):
    md5 = hashlib.md5()
    md5.update(s)
    return md5.hexdigest()

password = "lan20000715"
password = md5value(password.encode(encoding='utf-8'))
print(password)
userAgent = "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36"
header = {
    'User-Agent': userAgent,
}
login = {
    'username':"NoahSuming",
    'password': password
}
loginSession = requests.session()

backendurl = "http://localhost:8000/login"
res = loginSession.post(backendurl, data = login, headers= header, allow_redirects=True)
print(res.text)

problemPutUrl = "http://localhost:8000/judgestatusput/"
code = """
#include <iostream>
using namespace std;
int main () {
    int a, b;
    cin >> a >> b;
   cout << a + b << endl;
    return 0;
}
"""

codeData = {
    "user": "NoahSuming",
    "oj": "WHUOJ",
    "problem": "2",
    "result": -1,
    "time": 2,
    "memory": 1000,
    "length": len(code),
    "language": "C++",
    "submittime": datetime.datetime.now(),
    "judger": "127.0.0.1",
    "contest": 0,
    "code" : code,
    "testcase": "0",
    "message" : "WHUOJ - 2 A + B",
    "problemtitle": "WHUOJ - 2 A + B",
    "rating": 1500,
    "ip": "127.0.0.1",
}
putres = loginSession.post(problemPutUrl, data = codeData, headers= header)
print(putres.text)
