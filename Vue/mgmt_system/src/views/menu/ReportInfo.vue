<template>
  <h1>{{ title }}</h1>
  <TableForm @view-report="getReport" />
</template>

<script setup>
import TableForm from '@/components/TableForm.vue';
import { ref } from 'vue';
import { useRoute } from 'vue-router';
import { getReport } from '@/utils/communication'

const route = useRoute()
// 页面标题
const title = ref(route.query.title)
// 路由路径
const path = route.path
// 表格英文名称
const name = path.substring(path.lastIndexOf('/') + 1)

</script>

<style scoped lang="less" src="../../static/less/subform.less"></style>
