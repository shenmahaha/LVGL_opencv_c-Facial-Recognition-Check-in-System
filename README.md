# 基于Opencv的人脸识别签到系统（lvgl+C++）

## 使用技术
- lvgl9.1
- opencv
- TCP
- 阿里云OSS

## 界面截图

![1](https://github.com/user-attachments/assets/3b916c44-1230-44ba-9d9f-142ab059672a)

![2](https://github.com/user-attachments/assets/8a2fd9a9-37fd-4859-9f7a-44828334eabe)

## 第一步
下载源码到ubuntu20.04（已配置好opencv环境），并在编译器中打开。

## 第二步
更改服务器TCP
![image](https://github.com/user-attachments/assets/28e745f5-3b79-46bb-8ed3-45fe3dfb96e3)

## 第三步
终端输入命令
```
make clean

make -j16
```


