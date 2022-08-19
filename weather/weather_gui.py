from tkinter import *
import requests
import time
LOG_LINE_NUM = 0


class Weather_GUI():
    def __init__(self, init_window_name):
        self.init_window_name = init_window_name

    def set_init_window(self):
        self.init_window_name.title("天气查询工具")
        self.init_window_name.geometry('1068x681+10+10')
        self.init_data_label = Label(self.init_window_name, text="输入城市")
        self.init_data_label.grid(row=0, column=0)
        self.result_data_label = Label(self.init_window_name, text="天气")
        self.result_data_label.grid(row=0, column=12)
        self.log_label = Label(self.init_window_name, text="日志")

        self.log_label.grid(row=12, column=0)

        # 文本框
        self.init_data_Text = Text(self.init_window_name, width=67, height=35)  # 原始数据录入框
        self.init_data_Text.grid(row=1, column=0, rowspan=10, columnspan=10)
        self.result_data_Text = Text(self.init_window_name, width=70, height=49)  # 处理结果展示
        self.result_data_Text.grid(row=1, column=12, rowspan=15, columnspan=10)
        self.log_data_Text = Text(self.init_window_name, width=66, height=9)  # 日志框
        self.log_data_Text.grid(row=13, column=0, columnspan=10)
        self.str_get_weather_button = Button(self.init_window_name, text="获取天气", bg="lightblue", width=10,
                                             command=self.str_get_weather_button)
        self.str_get_weather_button.grid(row=1, column=11)

    def set_init_request(self):
        self.headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36 QIHU 360SE'
        }
        self.wKey = "1f1884f3b8ea4863b7ef79d8ee7b45a5"
        self.cityname = ""

    def str_get_weather_button(self):
        src = self.init_data_Text.get(1.0, END).strip().replace("\n", "")
        self.set_init_request()
        print(src)
        if src:
            try:
                data = self.get_weather_msg(src)
                self.result_data_Text.delete(1.0, END)
                print(data["text"])
                self.result_data_Text.insert(1.0, data)
                self.write_log_to_Text("INFO:爬取天气成功")

            except:
                self.result_data_Text.delete(1.0, END)
                self.result_data_Text.insert(1.0, "爬取天气失败")
        else:
            self.write_log_to_Text("ERROR:str_weather failed")

    def get_location_Id(self, cityname):
        location_id = ""
        self.cityurl = "https://geoapi.qweather.com/v2/city/lookup?location=" + cityname + "&key=" + self.wKey
        r = requests.get(url=self.cityurl, headers=self.headers)
        data = r.json()
        print(type(data["location"]))
        for location in data["location"]:
            print(location["name"])
            if location["name"] == cityname:
                location_id = location["id"]
                return location_id
        return location_id

    def get_weather_msg(self, cityname):
        self.weatherurl = "https://devapi.qweather.com/v7/weather/now?location=" + self.get_location_Id(
            cityname) + "&key=" + self.wKey
        r = requests.get(url=self.weatherurl, headers=self.headers)
        data = r.json()
        return data["now"]

    def get_current_time(self):
        current_time = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time()))
        return current_time

    def write_log_to_Text(self, logmsg):
        global LOG_LINE_NUM
        current_time = self.get_current_time()
        logmsg_in = str(current_time) + " " + str(logmsg) + "\n"  # 换行
        if LOG_LINE_NUM <= 7:
            self.log_data_Text.insert(END, logmsg_in)
            LOG_LINE_NUM = LOG_LINE_NUM + 1
        else:
            self.log_data_Text.delete(1.0, 2.0)
            self.log_data_Text.insert(END, logmsg_in)


def gui_start():
    init_window = Tk()
    weathre = Weather_GUI(init_window)
    weathre.set_init_window()
    init_window.mainloop()


if __name__ == "__main__":
    gui_start()