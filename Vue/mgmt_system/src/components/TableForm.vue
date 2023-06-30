<template>
  <div>
    <h1>{{ title }}</h1>
    <el-table :data="tableData" border stripe>
      <el-table-column v-for="column in columns" :prop="column" :label="column" />
    </el-table>
  </div>
</template>

<script>
import { toRaw } from 'vue';
export default {
  data () {
    let path = this.$route.path
    let name = path.substring(path.lastIndexOf('/') + 1)
    let tableData = this.$store.state.data[name]
    let columns = Object.keys(toRaw(tableData[0]))
    return {
      title: this.$route.query.title,
      tableData,
      columns
    }
  }
}

</script>

<style scoped lang="less">
h1 {
  font-size: x-large;
  margin-top: 0;
}
.el-table {
  box-shadow: .125rem .125rem gray;
}
.el-table-column {
  width: auto;
}
</style>
