import { QWebChannel } from "./qwebchannel"
import Router from '@/router'
import { AES_Encrypt } from "./aes"
import store from "@/store"
import { ElMessage } from "element-plus";

/**
 * 连接服务器
 * @param {string} url 
 */
export const ConnectToServer = (url) => {
  // 设置服务器地址
  if (location.search != '') {
    var baseUrl = (/[?&]webChannelBaseUrl=([A-Za-z0-9\-:/\.]+)/.exec(location.search)[1])
  }
  else {
    var baseUrl = url
  }
  // 创建web套接字
  var socket = new WebSocket(baseUrl)
  // 服务器关闭
  socket.onclose = function() {
    console.error('服务器关闭')
  }
  // 连接出错
  socket.onerror = function(error) {
    console.error('服务器出错：' + error)
  }
  // 连接成功
  socket.onopen = function() {
    // 创建对象
    new QWebChannel(socket, function(channel) {
      // 客户端的注册对象为core
      window.core = channel.objects.core
      // 连接newData信号
      window.core.newData.connect(async function(newdata) {
        switch (newdata.type) {
          case NewDataType.LoginSucceeded:  // 登录成功
            let userInfo = newdata.userinfo
            // 保存个人信息
            store.state.userInfo = userInfo
            // 菜单
            store.state.menu = newdata.data.menu
            if (userInfo.constructor === Object) {
              // 获取数据
              HtmlClientCall(CallType.Data, userInfo)
              // 跳转路由
              Router.push({ 
                path: '/homepage',
                query: {
                  adminid: userInfo.adminid,
                  password: AES_Encrypt(userInfo.password)
                }
              })
            }
            break;
          case NewDataType.LoginFailed: // 登录失败
            // 弹窗警告 
            alert('账号或密码错误')
            // 登录页面重载
            location.reload()
            break;
          case NewDataType.SendData:  // 获取到服务器的数据
            // 将数据保存到store
            if (store.state.tables !== null) {
              store.state.tables = await newdata.data
              location.reload()
            }
            else {
              store.state.tables = await newdata.data
            }
            break;
          case NewDataType.Relist:  // 重新登录
            alert('用户信息已修改，请重新登录')
            // 跳转路由
            Router.push({
              path: '/'
            })
            break;
          case NewDataType.ReportData:  // 新建报告页面
            break;
          default:
            break;
        }
      })
    })
  }
}
/**
 * 向服务器请求的接口
 * @param {CallType} type 
 * @param {JSON} data 
 */
export const HtmlClientCall = (calltype, data) => {
  let obj = {
    'type': calltype,
    'data': data
  }
  return window.core.htmlCall(obj)
}
/**
 * 向服务器请求的类型
 */
export const CallType = {
  Login: 0,
  Data: 1
}
/**
 * 服务器返回的数据类型
 */
export const NewDataType = {
  LoginSucceeded: 0x01,
  LoginFailed: 0x02,
  SendData: 0x04,
  Relist: 0x08,
  ReportData: 0x10,
  OperationSuccessful: 0x20
}