<template>
  <div class="tree-menu">
    <nav v-if="isMenuVisible">
      <el-submenu index="1">
        <template #title>
          <i class="el-icon-menu"></i>
          <span>菜单</span>
        </template>
        <el-menu-item v-for="menu in treeMenu" 
          :index="menu.path" 
          :route="{ path: menu.path, query: { permission: permission, adminid: adminid, password: password, title: menu.name } }">
          <i :class="menu.icon"></i>
          {{  menu.name }}
        </el-menu-item>
      </el-submenu>
      <el-submenu index="2">
        <template #title>
          <i class="el-icon-setting"></i>
          <span>系统</span>
        </template>
        <el-menu-item index="/">
          <i class="el-icon-caret-left"></i>
          <span>退出登录</span>
        </el-menu-item>
      </el-submenu>
    </nav>
    <el-button class="shrink-expand-menu" @click="shrinkExpandMenu">
      <nav v-if="isMenuVisible"><i class="el-icon-d-arrow-left"></i></nav>
      <nav v-if="!isMenuVisible"><i class="el-icon-d-arrow-right"></i></nav>
    </el-button>
  </div>
</template>

<script>
import { AES_Encrypt } from '@/utils/aes'

export default {
  name: 'TreeMenu',
  data () {
    let userInfo = this.$store.state.userInfo
    return {
      adminid: userInfo.adminid,
      password: AES_Encrypt(userInfo.password),
      permission: userInfo.permission,
      treeMenu: this.$store.state.menu,
      isMenuVisible: true
    }
  },
  methods: {
    shrinkExpandMenu () {
      this.isMenuVisible = !this.isMenuVisible
    }
  }
}
</script>

<style scoped lang="less">
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