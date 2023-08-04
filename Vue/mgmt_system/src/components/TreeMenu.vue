<template>
  <div class="tree-menu">
    <transition name="moveL">
      <nav v-if="isMenuVisible">
        <ElSubMenu index="1">
          <template #title>
            <ElIcon><Menu /></ElIcon>
            <span>菜单</span>
          </template>
          <ElMenuItem v-for="menu in treeMenu" 
            :index="menu.path" 
            :route="{ path: menu.path, query: { adminid: adminid, password: password, title: menu.name } }">
            <ElIcon><component :is="menu.icon" /></ElIcon>
            <span>{{  menu.name }}</span>
          </ElMenuItem>
        </ElSubMenu>
        <ElSubMenu index="2">
          <template #title>
            <ElIcon><Tools /></ElIcon>
            <span>系统</span>
          </template>
          <ElMenuItem :index="route.path" :route="route" @click="retrieveData">
            <ElIcon><Refresh /></ElIcon>
            <span>重新获取</span>
          </ElMenuItem>
          <ElMenuItem :index="route.path" :route="route" @click="changePasswordVisible = true">
            <ElIcon><User /></ElIcon>
            <span>修改密码</span>
          </ElMenuItem>
          <ElMenuItem :index="loginPath" @click="logOut">
            <ElIcon><SwitchButton /></ElIcon>
            <span>退出登录</span>
          </ElMenuItem>
        </ElSubMenu>
      </nav>
    </transition>
    <ElButton class="shrink-expand-menu" @click="shrinkExpandMenu">
      <ElIcon><DArrowLeft v-if="isMenuVisible" /><DArrowRight v-else/></ElIcon>
    </ElButton>
    <ElDialog v-model="changePasswordVisible" width="350px" title="修改密码" style="text-align: left;" >
      <ElForm :rules="rules" :model="changePassword">
        <ElFormItem label="原密码:" label-width="80px" prop="original">
          <ElInput v-model="changePassword.original" type="password" />
        </ElFormItem>
        <ElFormItem label="新密码:" label-width="80px" prop="fresh">
          <ElInput v-model="changePassword.fresh" type="password" />
        </ElFormItem>
        <ElFormItem label="确认密码:" label-width="80px" prop="confirm">
          <ElInput v-model="changePassword.confirm" type="password" />
        </ElFormItem>
        <ElFormItem>
          <div style="display: flex; margin-left: auto;">
            <ElButton type="info" @click="changePasswordVisible = false">取消</ElButton>
            <ElButton type="primary" @click="confirmButtonClicked">确认</ElButton>
          </div>
        </ElFormItem>
      </ElForm>
    </ElDialog>
  </div>
</template>

<script setup>
import { LOGIN_PATH } from '@/router/router-path';
import { RESET_STATE } from '@/store/mutation-types';
import { AES_Encrypt } from '@/utils/aes';
import { HtmlClientCall, HtmlCallType } from '@/utils/communication';
import { ElButton, ElDialog, ElForm, ElFormItem, ElIcon, ElInput, ElMenuItem, ElMessage, ElSubMenu } from 'element-plus';
import { useStore } from 'vuex';
import { ref } from 'vue';
import { useRoute } from 'vue-router';

const store = useStore()
const route = useRoute()

const userInfo = store.state.userInfo
const adminid = userInfo.adminid
const password = AES_Encrypt(userInfo.password)
const treeMenu = store.state.menu
const isMenuVisible = ref(true)
const changePasswordVisible = ref(false)
const loginPath = LOGIN_PATH

const shrinkExpandMenu = () => {
  isMenuVisible.value = !isMenuVisible.value
}
const retrieveData = () => {
  try {
    HtmlClientCall(HtmlCallType.SelectData, userInfo)
  } catch (error) {
    ElMessage.error({ message: '获取失败，请重新登录', showClose: true })
  }
}

const changePassword = ref({
  original: '',
  fresh: '',
  confirm: ''
})

const passwordTip = (rule, value, callback) => {
  if (!value || value.trim() === '') {
    return (new Error('密码不能为空'))
  }
  if (value.trim().length > 32) {
    return callback(new Error('密码长度不能超过32位'))
  }
  callback()
}

const confirmPasswordTip = (rule, value, callback) => {
  if (value.trim() !== changePassword.value.fresh.trim()) {
    return callback(new Error('密码不一致'))
  }
  callback()
}

const rules = {
  original: [{ validator: passwordTip, trigger: onblur }],
  fresh: [{ validator: passwordTip, trigger: onblur }],
  confirm: [{ validator: confirmPasswordTip, trigger: onblur }]
}

const confirmButtonClicked = () => {
  console.log(userInfo, changePassword.value)
  if (userInfo.password !== changePassword.value.original) {
    ElMessage.warning({
      duration: 3000,
      message: '原密码错误',
      showClose: true
    })
  }
  else if (changePassword.value.fresh !== changePassword.value.confirm) {
    ElMessage.warning({
      duration: 3000,
      message: '密码不一致',
      showClose: true
    })
  }
  else {
    let obj = userInfo;
    Object.assign(obj, { fresh: changePassword.value.fresh })
    HtmlClientCall(HtmlCallType.UpdatePassword, obj)
    changePassword.value.original = ''
    changePassword.value.fresh = ''
    changePassword.value.confirm = ''
    changePasswordVisible.value = false
  }
}

const logOut = () => {
  store.commit(RESET_STATE)
}
</script>

<style scoped lang="less">
.moveL-enter-active,
.moveL-leave-active {
  transition: all 0.3s linear;
  transform: translateX(0%);
}
.moveL-leave-to {
  transform: translateX(-100%);
}
.tree-menu {
  height: 100vh;
  display: flex;
}
.el-submenu {
  text-align: left;
}
.shrink-expand-menu {
  height: 100vh;
  border-right: .0625rem;
  border-radius: 0;
  padding-left: 0;
  padding-right: 0;
}

</style>
