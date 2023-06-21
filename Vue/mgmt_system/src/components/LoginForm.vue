<template>
  <div>
    <el-form class="main" :rules="rules" :model="userInfo" ref="LoginForm">
      <el-form-item>
        <img class="logo" src="../static/images/logo.png" />
      </el-form-item>
      <el-form-item prop="username">
        <el-input type="text" v-model="userInfo.username" placeholder="请输入账号" prefix-icon="el-icon-user" />
      </el-form-item>
      <el-form-item prop="password">
        <el-input type="password" v-model="userInfo.password" placeholder="请输入密码" prefix-icon="el-icon-lock" />
      </el-form-item>
      <!-- <el-row type="flex" justify="end">
        <el-checkbox v-model="userInfo.remember" label="记住密码" />
      </el-row> -->
      <el-row type="flex" justify="center">
        <el-button class="login-button" type="primary" @click="loginButtonClick('LoginForm')">登录</el-button>
      </el-row>
    </el-form>
    <div class="v-code">
        <v-code :show="puzzleShow" :canvasWidth="300" :canvasHeight="180" @close="closeCallback" @success="vCodeSuccessListener" @fail="vCodeFailListener" />
    </div>
  </div>
</template>

<script>
import VCode from 'vue3-puzzle-vcode'

export default {
  name: 'LoginForm',
  components: {
    VCode
  },
  data () {
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
    return {
      userInfo: {
        username: '',
        password: '',
        remember: false
      },
      rules: {
      username: [{ validator: checkUsername, trigger: onblur }],
      password: [{ validator: checkPassword, trigger: onblur }]
      },
      puzzleShow: false,
      puzzleCount: 0,
      isAuthenticated: false
    }
  },
  methods: {
    loginButtonClick (formName) {
        this.$refs[formName].validate(valid => {
            if (!valid) {
                return
            }
        })
        if (!this.isAuthenticated) {
            this.puzzleShow = true
            return
        }
    },
    /**
     * 点击其它位置关闭拼图验证
     */
    closeCallback () {
        setTimeout(() => {
            this.puzzleCount = 0
            this.puzzleShow = false
        }, 300)
    },
    /**
     * 拼图验证成功
     */
    vCodeSuccessListener () {
        setTimeout(() => {
            this.puzzleShow = false
            this.isAuthenticated = true
        }, 300)
        this.$emit('login', this.userInfo)
    },
    /**
     * 拼图验证次数检测
     */
    vCodeFailListener () {
        ++this.puzzleCount
        if (this.puzzleCount > 3) {
            this.puzzleShow = false
            this.puzzleCount = 0
            alert('验证失败，请重试')
        }
    }
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
  margin-top: .3125rem;
}
</style>