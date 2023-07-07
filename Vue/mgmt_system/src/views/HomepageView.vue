<template>
  <div class="main">
    <el-container>
      <el-aside class="aside">
        <el-menu router :default-openeds="['1']">
          <TreeMenu />
        </el-menu>
      </el-aside>
      <el-container>
        <el-main>
          <router-view/>
        </el-main>
      </el-container>
    </el-container>
  </div>
</template>

<script>
import TreeMenu from '@/components/TreeMenu.vue'

export default {
  name: 'Homepage',
  components: {
    TreeMenu
  }
}
</script>

<style scoped lang="less">
.main {
  background-color: white;
}
.aside {
  width: auto;
}
</style>
