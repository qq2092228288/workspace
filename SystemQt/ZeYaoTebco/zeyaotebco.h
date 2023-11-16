/**
 * @file ZeYaoTebco.h
 * @author 广东泽耀医疗技术有限公司
 * @brief 泽耀TEBCO设备数据处理类
 */

#ifndef ZEYAOTEBCO_H
#define ZEYAOTEBCO_H

#include <string>

class ZeYaoTebcoPrivate;
class ZeYaoTebco
{
public:
    /**
     * @brief construct a new ZeYaoTebco object 构造函数
     *
     */
    ZeYaoTebco();
    /**
     * @brief destroy the ZeYaoTebco object 析构函数
     *
     */
    ~ZeYaoTebco();
    /**
     * @brief manually releasing resources 手动释放资源，与析构函数作用一致
     *
     */
    void release();
public:
    /**
     * @brief login 登录
     *
     * @param deviceId [in] the device id length is 1 to 32 bytes 设备编号长度1到32个字节
     * @param password [in] the password length is 1 to 32 bytes 密码长度1到32个字节
     * @return
     * @retval 0 no error 无错误，请求登录
     * @retval 1 input error 输入错误
     * @retval 2 server connection failed 服务器连接失败
     */
    int login(std::string deviceId, std::string password);

    /**
     * @brief open serial port 打开串口
     *
     * @param portName [in] the port name 串口名称 Windows:COM1 Linux:/dev/ttyS0
     * @return
     * @retval true open success 打开成功
     * @retval false open failed 打开失败
     */
    bool openSerialPort(std::string portName);

    /**
     * @brief start detection 开始检测
     *
     * @param name [in] the name length is 1 to 32 bytes 姓名长度1到32个字节
     * @param id [in] the id length is 1 to 32 bytes 编号长度1到32个字节
     * @param gender [in] male input 0, female input 1 男性输入0, 女性输入1
     * @param age [in] the age between 10 and 120 年龄在10到120岁之间
     * @param height [in] the height between 50 and 250 centimeters 身高在50到250厘米之间
     * @param weight [in] the weight between 20 and 300 kilograms 体重在20到300千克之间
     * @return
     * @retval 0 no error 无错误，开始检测
     * @retval 1 input error 输入错误
     * @retval 3 serial port not opened 串口未打开
     * @retval 4 detecting 正在进行检测
     * @retval 5 insufficient quantity 可使用数量不足
     */
    int start(const std::string &name, const std::string &id, int gender, int age, int height, int weight);

    /**
     * @brief append blood pressure and body position 新增血压和体位
     *
     * @param sbp [in] systolic blood pressure between 0 and 300 收缩压在0到300之间
     * @param dbp [in] diastolic blood pressure between 0 and 300 舒张压在0到300之间
     * @param position [in] half lying input 1, lying flat input 2, leg raising input 3
     *        半躺输入1、平躺输入2、抬腿输入3
     * @return
     * @retval 0 no error 无错误，新增体位
     * @retval 1 input error 输入错误
     * @retval 6 not detected 未在进行检测
     * @retval 7 no data detected 没有检测到数据
     */
    int appendBpAndPosition(int sbp, int dbp, int position);

    /**
     * @brief end detection 结束检测
     * @details the param and return values are the same as the function appendBpAndPosition
     *          参数和返回值与函数appendBpAndPosition相同
     *
     */
    int end(int sbp, int dbp, int position);

    /**
     * @brief close serial port 关闭串口
     *
     */
    void closeSerialPort();

    /**
     * @brief read all data 读取所有数据
     *
     * @return return the JSON string of the detection result 返回检测结果的JSON字符串
     */
    std::string readAll();

    /**
     * @brief is the server connected 服务器是否已连接
     *
     * @return
     * @retval true connected 已连接
     * @retval false not connected 未连接
     */
    bool serverIsConnected();

    /**
     * @brief if serial port is open success 串口是否打开成功
     *
     * @return
     * @retval true serial port open success 串口打开成功
     * @retval false serial port open failed 串口打开失败
     */
    bool serialPortIsOpen();

    /**
     * @brief is detecting 是否正在进行检测
     *
     * @return
     * @retval true detecting 正在进行检测
     * @retval false not detected 未在检测
     */
    bool isDetecting();

    /**
     * @brief available quantity 获取可用数量
     *
     * @return return the available quantity of consumables 返回耗材可用数量
     */
    int availableQuantity();
    /**
     * @brief available ports 获取串口名称列表
     * @return return a list of available serial port names JSON string 返回可用串口名称列表JSON字符串
     */
    std::string availablePorts();

    /**
     * @brief get serial port error message 获取串口错误信息
     *
     * @return return serial port error message 返回串口错误信息
     */
    std::string serialPortErrorMsg();
private:
    ZeYaoTebcoPrivate *d_ptr;
};

#endif // ZEYAOTEBCO_H
