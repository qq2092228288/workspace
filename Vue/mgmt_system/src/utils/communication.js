import { QWebChannel } from "./qwebchannel/5.12/qwebchannel";
import Router from "@/router";
import store from "@/store";
import { AES_Encrypt } from "./aes";
import { ElMessage, ElMessageBox } from "element-plus";
import { SET_CURRENT_PAGE_PATH, SET_MENU, SET_TABLES, SET_USER_INFO } from "@/store/mutation-types";
import { HOMEPAGE_PATH, LOGIN_PATH } from "@/router/router-path";
import reportDialog from "@/components/report-dialog";

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
    ElMessage.info({
      message: '服务器正在更新维护，请稍后再试。'
    })
  }
  // 连接出错
  socket.onerror = function(error) {
    ElMessage.error({
      message: '服务器连接出错！'
    })
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
            let userInfo = await newdata.userinfo
            // 保存个人信息
            store.commit(SET_USER_INFO, userInfo)
            // 菜单
            store.commit(SET_MENU, await newdata.data.menu)
            if (userInfo.constructor === Object) {
              // 当前页面地址
              store.commit(SET_CURRENT_PAGE_PATH, HOMEPAGE_PATH)
              // 获取数据
              HtmlClientCall(HtmlCallType.SelectData, userInfo)
              // 跳转路由
              Router.push({ 
                path: HOMEPAGE_PATH,
                query: {
                  adminid: userInfo.adminid,
                  password: AES_Encrypt(userInfo.password)
                }
              })
            }
            break;
          case NewDataType.LoginFailed: // 登录失败
            // 弹窗警告 
            ElMessage.error({
              message: '账号或密码错误',
              showClose: true
            })
            break;
          case NewDataType.SendData:  // 获取到服务器的数据
            // 将数据保存到store
            store.commit(SET_TABLES, newdata.data)
            // 弹窗提示
            ElMessage.success({
              duration: 400,
              message: '获取数据成功。', 
              showClose: true,
              onClose: () => {
                if (store.state.currentPagePath !== HOMEPAGE_PATH) {  // 当前页非主页时刷新页面
                  Router.go(0)
                }
              }
            })
            break;
          case NewDataType.Relist:  // 重新登录
            ElMessageBox.alert('用户信息已修改，请重新登录', '警告', {
              confirmButtonText: '确定',
              callback: action => {
                // 跳转路由
                Router.push({
                  path: LOGIN_PATH
                })
              }
            })
            break;
          case NewDataType.ReportData:  // 新建报告页面
            break;
          case NewDataType.OperationSuccessful: // 插入/删除/更新数据 操作成功
            ElMessage.success({
              duration: 300,
              message: '操作成功',
              showClose: true,
              onClose: () => {
                HtmlClientCall(HtmlCallType.SelectData, store.state.userInfo)
              }
            })
            break;
          case NewDataType.OperationFailed: // 操作失败，重新获取数据
            ElMessage.error({
              message: '操作失败',
              showClose: true
            })
            break;
          case NewDataType.InsufficientPermissions: // 权限不足
            ElMessage.error({
              message: '用户权限不够',
              showClose: true
            })
            break;
          default:
            break;
        }
      })
      window.core.reportData.connect(function(reprotdata) {
        try {
          reportDialog(reprotdata)
        } catch (error) {
          ElMessage.error({
            message: '数据格式不正确',
            showClose: true
          })
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
  window.core.htmlCall({
    type: calltype,
    data: data
  })
}
/**
 * 获取报告数据
 * @param {object} obj 
 */
export const getReport = (obj) => {
  window.core.getReport(obj)
}
export const consultation = (obj) => {
  window.core.consultation(obj)
}
/**
 * 向服务器请求的类型
 */
export const HtmlCallType = {
  Login: 0x01,
  InsertData: 0x10,
  DeleteData: 0x20,
  UpdateData: 0x40,
  SelectData: 0x80
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
  OperationSuccessful: 0x20,
  OperationFailed: 0x40,
  InsufficientPermissions: 0x80
}

export const PType = {
  Insert: 0x01,
  Delete: 0x02,
  Update: 0x04,
  View: 0x08
}