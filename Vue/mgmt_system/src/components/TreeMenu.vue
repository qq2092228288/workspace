<template>
  <div class="tree-menu">
    <transition name="moveL">
      <nav v-if="isMenuVisible">
        <el-sub-menu index="1">
          <template #title>
            <el-icon><Menu /></el-icon>
            <span>菜单</span>
          </template>
          <el-menu-item v-for="menu in treeMenu" 
            :index="menu.path" 
            :route="{ path: menu.path, query: { adminid: adminid, password: password, title: menu.name } }">
            <el-icon><component :is="menu.icon" /></el-icon>
            {{  menu.name }}
          </el-menu-item>
        </el-sub-menu>
        <el-sub-menu index="">
          <template #title>
            <el-icon><Tools /></el-icon>
            <span>系统</span>
          </template>
          <el-menu-item :index="this.$route.path" :route="this.$route" @click="retrieveData">
            <el-icon><Refresh /></el-icon>
            <span>重新获取</span>
          </el-menu-item>
          <el-menu-item :index="loginPath" @click="logOut">
            <el-icon><SwitchButton /></el-icon>
            <span>退出登录</span>
          </el-menu-item>
        </el-sub-menu>
      </nav>
    </transition>
    <el-button class="shrink-expand-menu" @click="shrinkExpandMenu">
      <el-icon><DArrowLeft v-if="isMenuVisible" /><DArrowRight v-else/></el-icon>
    </el-button>
  </div>
</template>

<script setup>
import { LOGIN_PATH } from '@/router/router-path';
import { RESET_STATE } from '@/store/mutation-types';
import { AES_Encrypt } from '@/utils/aes';
import { HtmlClientCall, HtmlCallType } from '@/utils/communication';
import { ElMessage } from 'element-plus';
import { useStore } from 'vuex';
import { ref } from 'vue';

const store = useStore()

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