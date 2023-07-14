<template>
  <div>
    <el-form class="main" :rules="rules" :model="userInfo" ref="LoginForm" @keyup.enter="loginButtonClick">
      <!-- <el-form-item> -->
      <img class="logo" src="../static/images/logo.png" />
      <!-- </el-form-item> -->
      <el-form-item prop="username">
        <el-input type="text" v-model="userInfo.username" placeholder="请输入账号" prefix-icon="User"/>
      </el-form-item>
      <el-form-item prop="password">
        <el-input type="password" v-model="userInfo.password" placeholder="请输入密码" prefix-icon="Lock"/>
      </el-form-item>
      <!-- <el-row type="flex" justify="end">
        <el-checkbox v-model="userInfo.remember" label="记住密码" />
      </el-row> -->
      <el-row type="flex" justify="center">
        <el-button class="login-button" type="primary" @click="loginButtonClick">登录</el-button>
      </el-row>
    </el-form>
    <div class="v-code">
        <v-code :show="puzzleShow" :canvasWidth="300" :canvasHeight="180" @close="closeCallback" @success="vCodeSuccessListener" @fail="vCodeFailListener" />
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue';
import VCode from 'vue3-puzzle-vcode';
import { defineEmits } from 'vue';

const emit = defineEmits(['login'])
/**
 * 账号字符串检测
 * @param {*} rule 
 * @param {*} value 
 * @param {*} callback 
 */
  let checkUsername = (rule, value, callback) => {
  if (!value || value.trim() === '') {
    return callback(new Error('账号不能为空'))
  }
  if (value.trim().length > 32) {
    return callback(new Error('账号长度不能超过32位'))
  }
  callback()
}
/**
 * 密码字符检测
 * @param {*} rule 
 * @param {*} value 
 * @param {*} callback 
 */
let checkPassword = (rule, value, callback) => {
  if (!value || value.trim() === '') {
    return (new Error('密码不能为空'))
  }
  callback()
}

const userInfo = ref({
  username: '',
  password: ''
  // remember: false
})
const rules = {
  username: [{ validator: checkUsername, trigger: onblur }],
  password: [{ validator: checkPassword, trigger: onblur }]
}
const puzzleShow = ref(false)
const puzzleCount = ref(0)
// const isAuthenticated = ref(false)

const loginButtonClick = () => {
  // if (!isAuthenticated.value) {
  //   puzzleShow.value = true
  //   return
  // }
  puzzleShow.value = true
}
/**
 * 点击其它位置关闭拼图验证
 */
const closeCallback = () => {
  setTimeout(() => {
    puzzleCount.value = 0
    puzzleShow.value = false
  }, 300)
}
/**
 * 拼图验证成功
 */
const vCodeSuccessListener = () => {
  setTimeout(() => {
    puzzleShow.value = false
    // isAuthenticated.value = true
  }, 300)
  emit('login', userInfo.value)
}
/**
 * 拼图验证次数检测
 */
const vCodeFailListener = () => {
  ++puzzleCount.value
  if (puzzleCount.value > 2) {
    puzzleShow.value = false
    puzzleCount.value = 0
    alert('验证失败，请重试')
  }
}

</script>

<style scoped lang="less">
.main {
  box-shadow: .125rem .125rem .125rem .125rem gray;
  padding-left: 3.125rem;
  padding-right: 3.125rem;
  padding-top: .625rem;
  padding-bottom: .625rem;
  border-radius: .625rem;
}
.logo {
  height: 6.25rem;
  padding: .625rem;
}
.el-input {
  font-size: .9375rem;
}
/deep/ .el-form-item {
  display: block !important;
  color: pink;
}
.login-button {
  border: 0;
  border-radius: .625rem;
  background-color: rgb(52, 152, 245);
  box-shadow: .125rem .125rem .125rem .0625rem gray;
  margin: 0;
}
.login-button:hover {
  color: black;
}
</style>