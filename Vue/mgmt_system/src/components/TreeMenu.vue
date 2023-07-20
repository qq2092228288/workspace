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
        <ElSubMenu index="">
          <template #title>
            <ElIcon><Tools /></ElIcon>
            <span>系统</span>
          </template>
          <ElMenuItem :index="route.path" :route="route" @click="retrieveData">
            <ElIcon><Refresh /></ElIcon>
            <span>重新获取</span>
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
  </div>
</template>

<script setup>
import { LOGIN_PATH } from '@/router/router-path';
import { RESET_STATE } from '@/store/mutation-types';
import { AES_Encrypt } from '@/utils/aes';
import { HtmlClientCall, HtmlCallType } from '@/utils/communication';
import { ElButton, ElIcon, ElMenuItem, ElMessage, ElSubMenu } from 'element-plus';
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
const loginPath = LOGIN_PATH

const shrinkExpandMenu = () => {
  isMenuVisible.value = !isMenuVisible.value
}
const retrieveData = async () => {
  try {
    await HtmlClientCall(HtmlCallType.SelectData, store.state.userInfo)
  } catch (error) {
    ElMessage.error({ message: '获取失败，请重新登录', showClose: true })
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