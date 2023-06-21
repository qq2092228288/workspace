<template>
    <div class="header">
        <div class="exit-button">
            <el-button @click="exitBtnClick">退出</el-button>
        </div>
    </div>
</template>

<script>
import { AES_Decrypt } from '@/utils/aes.js'

export default {
    methods: {
        exitBtnClick () {
            this.$router.push({
                path: '/'
            })
        },
        clickButton () {
            const data = this.$route.query
            console.log(AES_Decrypt(data.password))
        }
    }
}
</script>

<style scoped lang="less">
.header {
    display: flex;
}
.exit-button {
    margin-left: auto;
}
</style>