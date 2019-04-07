# flir

## 新刷机:

**0x01** --> 通过SWD刷入Boot.zip内的hex;

**0x02** --> 使用串口连接电脑,打开ISP_Downloader.exe,选择bin文件,点击下载;

**0x03** --> 按住KEY4的同时开机进行握手,等待刷机完成.
***

## V2.02后覆盖刷机:

**0x02** --> 使用串口连接电脑,并使机器处于开机状态;

**0x01** --> 打开ISP_Downloader.exe,选择bin文件,点击下载.
***

![flir](https://github.com/AnalogDragon/flir/blob/master/1.jpg)

![flir](https://github.com/AnalogDragon/flir/blob/master/readme.png)
***

**注:**

ISP_Downloader.exe并非使用ISP协议下载，只是这样命名;

传输未校验,可能因误码而失败;

开机图片尺寸:160*128,路径//sys/BootGrap.bmp; 

To_clean_old_BMP.exe放在 //picture/To_clean_old_BMP.exe 下运行可以将BMP文件的附加数据删除;

PCB文件位于layer.zip;

