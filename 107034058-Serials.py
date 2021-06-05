from os import write
import serial
import matplotlib.pyplot as plt
import time

#################################################################
# 硬體通訊設定：指定序列名稱與傳輸速率

COM_PORT = 'COM4'    # 指定通訊埠名稱
BAUD_RATES = 9600    # 設定傳輸速率
ser = serial.Serial(COM_PORT, BAUD_RATES)   # 初始化序列通訊埠

#################################################################
# 監控數據檔案資訊

path = './datas/'               # 儲存檔案路徑，預設與程式同一層目錄

#################################################################
# 即時更新太陽能 數據可視化 設定

plt.ion()
plt.figure(1)

#################################################################
# 主監控程式

contents = []
solarValue = []

try:
    while True:
        # 向 Arduino Serial 監控資料
        while ser.in_waiting:          
            data_raw = ser.readline()  # 讀取一行
            data = data_raw.decode()   # 用預設的UTF-8解碼

            # 將接收到的資料 解封包
            infos = data.split(', ')
            ldrs = [int(i[6:]) for i in infos]  # 獲得各 ldr 的測量值

            # 將測量值記錄下來，後續將會儲存至指定數據檔案內
            contents.append(ldrs)
            solarValue.append(sum(ldrs)/4)
            
            # 新增連線獲得資料，並更新至可視化介面上
            plt.clf()
            plt.xlabel('epochs')
            plt.ylabel('avg solarValue')
            plt.plot(solarValue, '-r')
            plt.pause(0.01)



except KeyboardInterrupt:
    ser.close()    # 清除序列通訊物件
    
    # 開啟數據檔案，儲存本次連線的資料
    Date = time.strftime("%Y%m%d-%H%M%S", time.localtime())
    fileName = f'{path}{Date}.csv' 
    with open(fileName, 'w') as fileObj:
        # 寫入欄位名稱
        fileObj.write(', '.join(['topl', 'topr', 'botl', 'botr']))
        fileObj.write('\n')
        # 寫入欄位資訊
        for content in contents:
            strContent = [str(i) for i in content]
            fileObj.write(', '.join(strContent))
            fileObj.write('\n')

    print('System Close, disconnect with Arduino Serials')

#################################################################