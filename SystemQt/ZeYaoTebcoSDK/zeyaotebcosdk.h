#ifndef ZEYAOTEBCOSDK_H
#define ZEYAOTEBCOSDK_H

#include <iostream>
#include <cstring>
#include <memory>

using namespace std;

class ZeYaoTebcoSDKPrivate;
class ZeYaoTebcoSDK
{
public:
    explicit ZeYaoTebcoSDK();
    virtual ~ZeYaoTebcoSDK();
public:
    /*! 登录服务器 */
    void login(string deviceId, string password);
    /*! 打开串口 */
    bool open(string portname);
    /*! 关闭串口 */
    int close();
    /*! 启动检测  性别、年龄、身高、体重 */
    int start(string name, string id, int gender, int age, int height, int weight);
    /*! 输入血压和体位  收缩压、舒张压 */
    int appendBpAndPostion(int sbp, int dbp, int postion);
    /*! 结束检测 */
    int end();
    /*! 串口已打开 */
    bool isOpen();
    /*! 正在进行检测 */
    bool isDetecting();
    /*! 可用端口列表 */
    string availablePorts();
    /*! 读取数据 */
    string readAll();
    /*! 设备信息（登录成功之后） */
    string deviceInfo();
private:
    unique_ptr<ZeYaoTebcoSDKPrivate> d_ptr;
};

#endif // ZEYAOTEBCOSDK_H
