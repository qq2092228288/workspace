<template>
  <div class="main">
    <el-container>
      <el-aside class="aside">
        <el-menu router :default-openeds="['1']" @select="setCurrentPagePath">
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

<script setup>
import TreeMenu from '@/components/TreeMenu.vue';
import { SET_CURRENT_PAGE_PATH } from '@/store/mutation-types';
import { useStore } from 'vuex';

const store = useStore()

const setCurrentPagePath = (index) => {
  store.commit(SET_CURRENT_PAGE_PATH, index)
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
