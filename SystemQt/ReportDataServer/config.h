#ifndef CONFIG_H
#define CONFIG_H

/*! TCP监听端口 */
#define LISTEN_PORT             18080
/*! 最大连接数 */
#define MAX_NUM_CONNECTION      256
/*! 子TCP服务线程最大客户端数量 */
#define SUB_SERVER_MAX          16
/*! 最大数据长度(1MB) */
#define MAX_DATA_LENGTH         1048576

#endif // CONFIG_H
