<template>
    <div class="login-main">
        <h1>泽耀医疗管理系统</h1>
        <LoginForm class="login-form" @login="loginEvent($event)" />
        <p class="tail-p">Welcome to Ze Yao Yi Liao Management System.</p>
    </div>
</template>

<script>
import LoginForm from '@/components/LoginForm.vue'
import { AES_Encrypt } from '@/utils/aes.js'

export default {
    name: 'LoginView',
    components: {
        LoginForm
    },
    methods: {
        loginEvent (userInfo) {
            // 用户信息校验
            // 校验成功跳转
            this.$router.push({ 
                path: '/homepage', 
                query: {
                    id: userInfo.username,
                    password: AES_Encrypt(userInfo.password)
                } 
            })
        }
    }
}

</script>

<style scoped lang="less">
h1 {
    font-size: 2.8125rem;
    // padding-top: 1.875rem;
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
