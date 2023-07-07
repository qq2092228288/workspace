<template>
  <div>
    <h1>
      {{ title }}
      <el-button class="insert-button" type="success" v-if="insertable" @click="insertButtonClicked">
        <el-icon><CirclePlus /></el-icon>
      </el-button>
    </h1>
    <div>
      <el-table border stripe highlight-current-row
        :data="tableData.slice((currentPage - 1)*pageSize, currentPage*pageSize)" 
        @row-dblclick="copyText">
        <el-table-column v-for="column in columns" :prop="column.en" :label="column.cn" :show-overflow-tooltip="true"/>
        <el-table-column label="操作" width="80px" key="slot">
          <template #default="scope">
            <el-button class="operation-button" type="danger" v-if="deletable" @click="deleteButtonClicked(scope.row)">
              <el-icon><Delete /></el-icon>
            </el-button>
            <el-button class="operation-button" type="primary" v-if="updatable" @click="editButtonClicked(scope.row)">
              <el-icon><Edit /></el-icon>
            </el-button>
            <el-button class="operation-button" type="success" v-if="viewable" @click="viewButtonClicked(scope.row)">
              <el-icon><View /></el-icon>
            </el-button>
          </template>
        </el-table-column>
      </el-table>
    </div>
    <div>
      <el-config-provider :locale="locale">
        <el-pagination
          @size-change="handleSizeChange"
          @current-change="handleCurrentChange"
          :current-page="currentPage"
          :page-sizes="[20, 50, 100, 200]"
          :page-size="pageSize"
          layout="total, sizes, prev, pager, next, jumper"
          :total="tableData.length">
        </el-pagination>
      </el-config-provider>
    </div>
  </div>
</template>

<script>
import { ElConfigProvider } from 'element-plus';
import zhCn from 'element-plus/lib/locale/lang/zh-cn';
import useClipboard from 'vue-clipboard3';

export default {
  components: {
    ElConfigProvider
  },
  setup() {
    return {
      locale: zhCn
    }
  },
  data() {
    let path = this.$route.path
    let tablePermission
    try {
      this.$store.state.menu.forEach(function (item) {
        if (item.path === path) {
          tablePermission = item.permission
          throw Error()
        }
      })
    } catch (error) {

    }
    let name = path.substring(path.lastIndexOf('/') + 1)
    let tableData = this.$store.state.tables[name].data
    let columns = this.$store.state.tables[name].columns
    return {
      title: this.$route.query.title,
      insertable: tablePermission & 0x01,
      deletable: tablePermission & 0x02,
      updatable: tablePermission & 0x04,
      viewable: tablePermission & 0x08,
      tableData,
      columns,
      currentPage: 1,
      pageSize: 20
    }
  },
  methods: {
    handleSizeChange(val) {
      this.currentPage = 1
      this.pageSize = val
    },
    handleCurrentChange(val) {
      this.currentPage = val
    },
    async copyText(row, column, event) {
      try {
        const { toClipboard } = useClipboard()
        await toClipboard(event.target.innerText)
        this.$message({ message: '复制成功', type: 'success', showClose: true })
      } catch (error) {
        
      }
    },
    insertButtonClicked(event) {
      console.log(event)
    },
    deleteButtonClicked(row) {
      console.log(row)
    },
    editButtonClicked(row) {
      console.log(row)
    },
    viewButtonClicked(row) {
      console.log(row)
    }
  }
}

</script>

<style scoped lang="less">
h1 {
  font-size: x-large;
  margin: 0;
  padding-bottom: .3125rem;
  background-color: white;
  border-top-left-radius: .3125rem;
  border-top-right-radius: .3125rem;
}
.el-table {
  height: 80vh;
}
.insert-button {
  padding: .125rem;
  margin-left: .125rem;
  margin-right: .125rem;
  height: auto;
  display: flex;
  margin-left: auto;
  font-size: 1.125rem;
}
.operation-button {
  padding: .125rem;
  margin-left: .125rem;
  margin-right: .125rem;
  height: auto;
}
.el-pagination {
  padding-top: .625rem;
  background-color: white;
  border-bottom-left-radius: .3125rem;
  border-bottom-right-radius: .3125rem;
  justify-content: center;
}
</style>
