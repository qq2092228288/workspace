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
        <el-sub-menu index="2">
          <template #title>
            <el-icon><Tools /></el-icon>
            <span>系统</span>
          </template>
          <el-menu-item :index="this.$route.path" :route="this.$route" @click="retrieveData">
            <el-icon><Refresh /></el-icon>
            <span>重新获取</span>
          </el-menu-item>
          <el-menu-item index="/" @click="logOut">
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

<script>
import { AES_Encrypt } from '@/utils/aes'
import { HtmlClientCall, CallType } from '@/utils/communication'

export default {
  name: 'TreeMenu',
  data () {
    let userInfo = this.$store.state.userInfo
    return {
      adminid: userInfo.adminid,
      password: AES_Encrypt(userInfo.password),
      treeMenu: this.$store.state.menu,
      isMenuVisible: true
    }
  },
  methods: {
    shrinkExpandMenu() {
      this.isMenuVisible = !this.isMenuVisible
    },
    async retrieveData() {
      try {
        await HtmlClientCall(CallType.Data, this.$store.state.userInfo)
      } catch (error) {
        this.$message({ message: '获取失败，请重新登录', type: 'error', showClose: true })
      }
    },
    logOut() {
      let state = this.$store.state
      state.userInfo = null
      state.menu = null
      state.tables = null
      this.$router.push({
        path: '/'
      })
    }
  }
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