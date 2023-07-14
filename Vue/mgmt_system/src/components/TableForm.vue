<template>
  <div>
    <ElTable border stripe highlight-current-row style="height: 80vh;"
      :data="tableData.slice((currentPage - 1)*pageSize, currentPage*pageSize)"
      :header-cell-style="{'text-align':'center'}"
      @row-dblclick="copyText">
      <ElTableColumn v-for="column in columns" :prop="column.en" :label="column.cn" :show-overflow-tooltip="true"/>
      <ElTableColumn label="操作" align="center" width="80px" key="slot">
        <template #default="scope">
          <ElButton class="operation-button" type="danger" v-if="permission & PType.Delete" @click="deleteButtonClicked(scope.row)">
            <ElIcon><Delete /></ElIcon>
          </ElButton>
          <ElButton class="operation-button" type="primary" v-if="permission & PType.Update" @click="emit('update-data', scope.row, scope.$index)">
            <ElIcon><Edit /></ElIcon>
          </ElButton>
          <ElButton class="operation-button" type="success" v-if="permission & PType.View" @click="emit('view-report', scope.row, name)">
            <ElIcon><View /></ElIcon>
          </ElButton>
        </template>
      </ElTableColumn>
    </ElTable>
    <ElConfigProvider :locale="locale">
      <ElPagination
        @size-change="handleSizeChange"
        @current-change="handleCurrentChange"
        :current-page="currentPage"
        :page-sizes="[20, 50, 100, 200]"
        :page-size="pageSize"
        layout="total, sizes, prev, pager, next, jumper"
        :total="tableData.length">
      </ElPagination>
    </ElConfigProvider>
  </div>
</template>

<script setup>
import { ElButton, ElDialog, ElInput, ElIcon, ElRow, ElPagination, ElTable, ElTableColumn, ElConfigProvider, ElMessage, ElMessageBox } from 'element-plus';
import zhCn from 'element-plus/lib/locale/lang/zh-cn';
import useClipboard from 'vue-clipboard3';
import { defineEmits, h, ref } from 'vue';
import { useStore } from 'vuex';
import { useRoute } from 'vue-router';
import { PType, HtmlClientCall, HtmlCallType } from '@/utils/communication';
// import DataEditForm from '@/components/DataEditForm.vue';

// 组件信号
const emit = defineEmits(['update-data', 'view-report'])
const store = useStore()
const route = useRoute()
// 个人信息
const userInfo = store.state.userInfo
// 路由路径
const path = route.path
// 表格权限
const permission = ref(0)
for (const item of store.state.menu) {
  if (item.path === path) {
    permission.value = item.permission
    break
  }
}
// 表格英文名称
const name = path.substring(path.lastIndexOf('/') + 1)
// 表格数据
const tableData = ref(store.state.tables[name].data)
// 表格列数据
const columns = ref(store.state.tables[name].columns)
// 页码中文
const locale = zhCn
// 当前页
const currentPage = ref(1)
// 一页数据量
const pageSize = ref(20)
// 设置一页数据量
const handleSizeChange = (val) => {
  currentPage.value = 1
  pageSize.value = val
}
// 设置当前页
const handleCurrentChange = (val) => {
  currentPage.value = val
}
// 双击复制
const copyText = async (row, column, event) => {
  try {
    const { toClipboard } = useClipboard()
    await toClipboard(event.target.innerText)
    ElMessage.success({ message: '复制成功', type: 'success', showClose: true })
  } catch (error) {
    
  }
}
// 点击表格删除按钮
const deleteButtonClicked = (row) => {
  ElMessageBox.confirm(
    '执行删除操作之后不可恢复，请确认是否要删除此数据？', 
    '警告', 
    {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      draggable: true,
      type: 'warning'
    }
  ).then(() => {
    let newobj = {}
    Object.assign(newobj, userInfo)
    Object.assign(newobj, {
      commstruct: {
        tablename: name,
        criteria: [{
          columnname: columns.value[0].en,
          columnvalue: row[columns.value[0].en]
        },
        {
          columnname: columns.value[1].en,
          columnvalue: row[columns.value[1].en]
        }]
      }
    })
    HtmlClientCall(HtmlCallType.DeleteData, newobj)
    tableData.value.filter((element, index, array) => {
      if (element == row) {
        array.splice(index, 1)
        return
      }
    })
  }).catch(() => {
    ElMessage.info({
      duration: 1000,
      message: '取消操作',
      showClose: true
    })
  })
}

const updateButtonClicked = (row) => {
  ElMessageBox({
    message: h(DataEditForm, { data: row, columns: columns.value }),
    showCancelButton: true,
    confirmButtonText: '确定',
    cancelButtonText: '取消'
  }).then(() => {
    ElMessage.success({
      duration: 1000,
      message: '修改成功',
      showClose: true
    })
  }).catch(() => {
    ElMessage.info({
      duration: 1000,
      message: '取消操作',
      showClose: true
    })
  })
}

</script>

<style scoped lang="less">
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
