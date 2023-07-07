<template>
  <div class="login-main">
    <h1>泽耀医疗管理系统</h1>
    <LoginForm class="login-form" @login="loginEvent($event)" />
    <p class="tail-p">Welcome to Ze Yao Yi Liao Management System.</p>
  </div>
</template>

<script>
import LoginForm from '@/components/LoginForm.vue'
import { HtmlClientCall, CallType } from '@/utils/communication'

export default {
  name: 'LoginView',
  components: {
    LoginForm
  },
  methods: {
    // 向服务器请求登录
    loginEvent (userInfo) {
      HtmlClientCall(CallType.Login, userInfo)
    }
  }
}

</script>

<style scoped lang="less">
h1 {
  font-size: 2.8125rem;
}
.login-form {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  width: 25rem;
}
.tail-p {
  position: absolute;
  bottom: 0;
  left: 50%;
  transform: translate(-50%, -50%);
  font-size: .3125rem;
}

</style>
