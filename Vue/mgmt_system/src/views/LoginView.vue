<template>
  <div class="login-main">
    <h1>泽耀医疗管理系统</h1>
    <LoginForm class="login-form" @login="loginEvent($event)" />
    <p class="tail-p">广东泽耀医疗技术有限公司 备案号: <a href="https://beian.miit.gov.cn/" target="_blank">粤ICP备2022137474号</a></p>
  </div>
</template>

<script setup>
import LoginForm from '@/components/LoginForm.vue';
import { HtmlClientCall, HtmlCallType } from '@/utils/communication';

const loginEvent = (userInfo) => {
  HtmlClientCall(HtmlCallType.Login, userInfo)
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
  margin-bottom: 0;
  bottom: 0;
  left: 50%;
  transform: translate(-50%, -50%);
  font-size: .75rem;
}

</style>
