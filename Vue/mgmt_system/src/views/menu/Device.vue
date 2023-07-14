<template>
  <h1>{{ title }}</h1>
  <ElButton class="insert-button" type="success" v-if="permission & PType.Insert" @click="insertDialogButtonClicked">
    <ElIcon><CirclePlus /></ElIcon>
  </ElButton>
  <TableForm @update-data="updateDataReceived" />
  <ElDialog v-model="insertDialogVisible" width="500px" title="新增数据">
    <ElTable stripe highlight-current-row
      :show-header="false"
      :data="insertDialogData">
      <ElTableColumn prop="name" width="150px" align="right" />
      <ElTableColumn prop="value" width="300px">
        <template #default="scope">
          <ElSelect v-if="scope.row.en === 'placename'" v-model="scope.row['value']">
            <ElOption v-for="item in placeName" :key="item.value" :label="item.label" :value="item.value"/>
          </ElSelect>
          <ElSelect v-else-if="scope.row.en === 'agentname'" v-model="scope.row['value']">
            <ElOption v-for="item in agentName" :key="item.value" :label="item.label" :value="item.value"/>
          </ElSelect>
          <ElSelect v-else-if="scope.row.en === 'type'" v-model="scope.row['value']">
            <ElOption v-for="item in type" :key="item.value" :label="item.label" :value="item.value"/>
          </ElSelect>
          <ElSelect v-else-if="scope.row.en === 'status'" v-model="scope.row['value']">
            <ElOption v-for="item in status" :key="item.value" :label="item.label" :value="item.value"/>
          </ElSelect>
          <ElInput v-else :type="scope.row['type']" :autosize="{ minRows: 1, maxRows: 5 }" v-model="scope.row['value']"/>
        </template>
      </ElTableColumn>
    </ElTable>
    <ElRow type="flex" justify="end" style="margin-top: 1.25rem;" >
      <ElButton type="info" @click="insertDialogVisible = false" >取消</ElButton>
      <ElButton type="primary" @click="insertDialogConfirmButtonClicked">确定</ElButton>
    </ElRow>
  </ElDialog>
  <ElDialog v-model="updateDialogVisible" width="500px" title="修改数据">
    <ElTable stripe highlight-current-row
      :show-header="false"
      :data="updateDialogData">
      <ElTableColumn prop="name" width="150px" align="right" />
      <ElTableColumn prop="value" width="300px">
        <template #default="scope">
          <ElSelect v-if="scope.row.en === 'placename'" v-model="scope.row['value']">
            <ElOption v-for="item in placeName" :key="item.value" :label="item.label" :value="item.value"/>
          </ElSelect>
          <ElSelect v-else-if="scope.row.en === 'agentname'" v-model="scope.row['value']">
            <ElOption v-for="item in agentName" :key="item.value" :label="item.label" :value="item.value"/>
          </ElSelect>
          <ElSelect v-else-if="scope.row.en === 'type'" v-model="scope.row['value']">
            <ElOption v-for="item in type" :key="item.value" :label="item.label" :value="item.value"/>
          </ElSelect>
          <ElSelect v-else-if="scope.row.en === 'status'" v-model="scope.row['value']">
            <ElOption v-for="item in status" :key="item.value" :label="item.label" :value="item.value"/>
          </ElSelect>
          <ElInput v-else :type="scope.row['type']" :autosize="{ minRows: 1, maxRows: 5 }" v-model="scope.row['value']" :disabled="scope.$index === 0"/>
        </template>
      </ElTableColumn>
    </ElTable>
    <ElRow type="flex" justify="end" style="margin-top: 1.25rem;" >
      <ElButton type="info" @click="updateDialogVisible = false" >取消</ElButton>
      <ElButton type="primary" @click="updateDialogConfirmButtonClicked">确定</ElButton>
    </ElRow>
  </ElDialog>
</template>

<script setup>
import TableForm from '@/components/TableForm.vue';
import { HtmlCallType, HtmlClientCall, PType } from '@/utils/communication';
import { useStore } from 'vuex';
import { ElButton, ElDialog, ElIcon, ElInput, ElMessage, ElTable, ElTableColumn, ElRow, ElSelect, ElOption } from 'element-plus';
import { ref } from 'vue';
import { useRoute } from 'vue-router';
import { AGENTINFO_PATH, PLACEINFO_PATH } from '@/router/router-path';

const store = useStore()
const route = useRoute()
// 个人信息
const userInfo = store.state.userInfo
// 页面标题
const title = ref(route.query.title)
// 路由路径
const path = route.path
// 表格英文名称
const name = path.substring(path.lastIndexOf('/') + 1)
// 表格权限
const permission = ref(0)
for (const item of store.state.menu) {
  if (item.path === path) {
    permission.value = item.permission
    break
  }
}
// 插入数据对话窗可见标记
const insertDialogVisible = ref(false)
// 修改数据对话窗可见标记
const updateDialogVisible = ref(false)
// 表格数据
const tableData = ref(store.state.tables[name].data)
// 表格列数据
const columns = ref(store.state.tables[name].columns.filter((item, index, array) => {
  let ignored = ['adminname', 'totalcount', 'usedcount']
  if (ignored.indexOf(item.en) !== -1) {
    return false
  }
  return true
}))
// 选择框数据
const getSelectData = (table, val, ...columns) => {
  let data = []
  for(let i = 0, length = table.length; i < length; ++i) {
    let label = table[i][columns[0]]
    data.push({
      value: table[i][val],
      label: columns.length === 1 ? label : label + '&' + table[i][columns[1]]
    })
  }
  return data
}
// 场所
const placeInfo = ref(store.state.tables[PLACEINFO_PATH].data)
const placeName = getSelectData(placeInfo.value, 'placeid', 'hostname', 'secondaryname')
// 经销商姓名
const agentInfo = ref(store.state.tables[AGENTINFO_PATH].data)
const agentName = getSelectData(agentInfo.value, 'agentid', 'name')
// 设备类型
const type = [
  {
    value: 'YZ-TEBCO-01A',
    label: 'YZ-TEBCO-01A'
  },
  {
    value: 'YZ-TEBCO-01B',
    label: 'YZ-TEBCO-01B'
  },
  {
    value: 'YZ',
    label: '其它'
  }
]
// 设备状态
const status = [
  {
    value: 0,
    label: '未激活(0)'
  },
  {
    value: 1,
    label: '已激活(1)'
  }
]
// 数据初始化
const initData  = (rowdata) => {
  if (rowdata === undefined) {
    rowdata = {}
  }
  let data = []
  for(let i = 0, length = columns.value.length; i < length; ++i) {
    let column = columns.value[i]
    data.push({
      name: column.cn,
      value: rowdata[column.en] === undefined ? '' : rowdata[column.en],
      en: column.en,
      type: column.en.indexOf('content') === -1 ? 'text' : 'textarea'
    })
  }
  return data
}
// 插入弹窗数据
const insertDialogData = ref(initData({}))
// 修改弹窗数据
const updateDialogData = ref(initData({}))
/// 修改对话窗index
const updateIndex = ref(0)
// 检查数据完整性
const checkDataIntegrity = (data) => {
  const temp = data.filter((item, index, array) => {
    if (item.value.length === 0) {
      ElMessage.warning({
        message: columns.value[index].cn + '不能为空',
        showClose: true
      })
      return false
    }
    return true
  })
  if (temp.length === data.length) {
    return true
  }
  return false
}
const insertDialogButtonClicked = () => {
  insertDialogVisible.value = true
}
// 点击插入弹窗确认按钮
const insertDialogConfirmButtonClicked = () => {
  // 输入判断
  if (!checkDataIntegrity(insertDialogData.value)) {
    return
  }
  // 关闭弹窗
  insertDialogVisible.value = false
  // 上传数据
  let criteria = [{ columnname: 'adminid', columnvalue: route.query.adminid }]
  for (const item of insertDialogData.value) {
    let columnname = item.en
    if (item.en === 'placename') {
      columnname = 'placeid'
    }
    else if (item.en === 'agentname') {
      columnname = 'agentid'
    }
    criteria.push({ columnname: columnname, columnvalue: item.value })
  }
  let newdata = {}
  Object.assign(newdata, userInfo)
  Object.assign(newdata, {
    commstruct: {
      tablename: name,
      criteria: criteria
    }
  })
  HtmlClientCall(HtmlCallType.InsertData, newdata)
  // 插入表格的行数据
  let newrow = {
    totalcount: 0, 
    usedcount: 0,
    adminname: store.state.userInfo.name
  }
  for (const item of insertDialogData.value) {
    let key = item.en
    let value = item.value
    if (key === 'placename') {
      for (const item of placeName) {
        if (item.value === value) {
          newrow['placename'] =item.label
          break
        }
      }
    }
    else if (key === 'agentname') {
      for (const item of agentName) {
        if (item.value === value) {
          newrow['agentname'] = item.label
          break
        }
      }
    }
    else {
      newrow[key] = value
    }
  }
  tableData.value.unshift(newrow)
  // 初始化
  insertDialogData.value = initData({})
}
// 更新数据
const updateDataReceived = (row, index) => {
  updateDialogData.value = initData(row)
  updateIndex.value = index
  // 弹出对话窗
  updateDialogVisible.value = true
}
// 点击更新弹窗确认按钮
const updateDialogConfirmButtonClicked = () => {
  // 输入判断
  if (!checkDataIntegrity(updateDialogData.value)) {
    return
  }
  updateDialogVisible.value = false
  const setData = updateDialogData.value.filter((item, index, array) => { return index !== 0 })
  let set = []
  for (const item of setData) {
    let columnname = item.en
    let columnvalue = item.value
    if (item.en === 'placename') {
      columnname = 'placeid'
      for (const item of placeName) {
        if (item.label === columnvalue) {
          columnvalue = item.value
          break
        }
      }
    }
    else if (item.en === 'agentname') {
      columnname = 'agentid'
      for (const item of agentName) {
        if (item.label === columnvalue) {
          columnvalue = item.value
          break
        }
      }
    }
    set.push({ columnname: columnname, columnvalue: columnvalue })
  }
  let newdata = {}
  Object.assign(newdata, userInfo)
  Object.assign(newdata, {
    commstruct: {
      tablename: name,
      set: set,
      criteria: [{
        columnname: updateDialogData.value[0].en,
        columnvalue: updateDialogData.value[0].value
      }]
    }
  })
  HtmlClientCall(HtmlCallType.UpdateData, newdata)
  // 修改表格的行数据
  let updaterow = tableData.value[updateIndex.value]
  for (const element of updateDialogData.value) {
    let key = element.en
    let value = element.value
    if (key === 'placename') {
      for (const item of placeName) {
        if (item.value === value) {
          value = item.label
          break
        }
      }
    }
    else if (key === 'agentname') {
      for (const item of agentName) {
        if (item.value === value) {
          value = item.label
          break
        }
      }
    }
    updaterow[key] = value
  }
  tableData.value.splice(updateIndex.value, 1, updaterow)
}

</script>

<style scoped lang="less" src="../../static/less/subform.less"></style>
