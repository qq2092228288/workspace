<template>
  <ElDialog v-model="visible" top="1vh" width="800px" :show-close="false">
    <div id="main">
      <h1>无创心功能监测报告</h1>
      <h1 v-if="strInvalid(primaryPlace)">{{ primaryPlace }}</h1>
      <h2 v-if="strInvalid(secondaryPlace)">{{ "科室: " + secondaryPlace }}</h2>
      <h3 v-if="strInvalid(inspector)">{{ "操作人员: " + inspector }}</h3>
      <ElTable
        :data="patintInfoTable"
        :show-header="false"
        :span-method="infoSpanMethod"
        border>
        <ElTableColumn v-for="col in patintInfoColumns" :prop="col.colname" :align="col.align" />
      </ElTable>
      <ElTable
        :data="tableData"
        :span-method="dataSpanMethod"
        border>
        <ElTableColumn v-for="col in tableDataColumns" :prop="col.value" :label="col.label" :width="col.width" />
      </ElTable>
      <div class="textarea" contenteditable="true">
        {{ reportConclusion }}
      </div>
    </div>
    <ElRow type="flex" justify="end" style="margin-top: 1.25rem;" >
      <ElTooltip effect="dark" content="下载报告" >
        <ElButton type="success" :icon="Download" @click="reportPrint" :loading="loading" circle />
      </ElTooltip>
      <ElTooltip effect="dark" content="关闭窗口">
        <ElButton type="info" :icon="Close" circle @click="visible = false" />
      </ElTooltip>
      <ElTooltip effect="dark" content="确定修改">
        <ElButton type="primary" :icon="Check" circle @click="confirmButtonClicked" />
      </ElTooltip>
    </ElRow>
  </ElDialog>
</template>

<script setup>
import { Check, Close, Download } from '@element-plus/icons';
import { ElButton, ElDialog, ElInput, ElRow, ElTable, ElTableColumn, ElTooltip } from 'element-plus';
import { ref } from 'vue';
import { cBsa, createDataType, DataType, intercept, positionData } from '@/utils/tebcoparamcalculation';
import { consultation } from '@/utils/communication';
import htmlToPdf from '@/utils/htmltopdf';
import router from '@/router';

const strInvalid = (str) => {
  if (str === undefined || str === '' || str === 'unknown') {
    return false
  }
  return true
}

/**
 * @description return position chinese name
 * @param {int} pos 
 * @returns 
 */
const positionCn = (pos) => {
  if (pos == 1) {
    return "半卧";
  }
  else if (pos == 2) {
    return "平躺";
  }
  else if (pos == 3) {
    return "抬腿";
  }
  return null;
}
const infoSpanMethod = ({ row, column, rowIndex, columnIndex }) => {
  if (rowIndex === 2 && columnIndex === 3) {
    return [1, 3]
  }
}

const props = defineProps({
  reporttime: String,
  deviceid: String,
  reportdata: String
})
const reportdata = JSON.parse(props.reportdata)
// 对话窗可见
const visible = ref(true)
// 场所信息
const place = reportdata.place
// 主场所
const primaryPlace = place.primaryPlace
// 次场所
const secondaryPlace = place.secondaryPlace
// 检查人员
const inspector = place.inspector
// 基本信息
const patientInfo = reportdata.patientInfo
const patintInfoTable = [
  {
    col0: '姓名',
    col1: patientInfo.patientName,
    col2: '性别',
    col3: patientInfo.sex,
    col4: '年龄',
    col5: patientInfo.age + ' 岁',
  },
  {
    col0: '身高',
    col1: patientInfo.height + ' cm',
    col2: '体重',
    col3: patientInfo.weight + ' kg',
    col4: '体表面积',
    col5: intercept(cBsa(patientInfo.height, patientInfo.weight ), 2) + ' ㎡'
  },
  {
    col0: '血红蛋白',
    col1: patientInfo.hb === undefined || patientInfo.hb === '0' ? null : patientInfo.hb + ' g/dl',
    col2: '病历号',
    col3: patientInfo.medicalRecordNumber
  }
]
const patintInfoColumns = [
  {
    colname: 'col0',
    align: 'left'
  },
  {
    colname: 'col1',
    align: 'left'
  },
  {
    colname: 'col2',
    align: 'left'
  },
  {
    colname: 'col3',
    align: 'left'
  },
  {
    colname: 'col4',
    align: 'left'
  },
  {
    colname: 'col5',
    align: 'left'
  }
]
// 数据
const pdata = ref([])
for (const pos of reportdata.position) {
  if (Object.keys(pos.data).length !== 0) {
    pdata.value.push(positionData(patientInfo, pos.data))
  }
}
const datEnum = createDataType(DataType)
const baseTable = [
  {
    name: '心排量',
    types: [DataType.Co, DataType.Ci, DataType.Do2, DataType.Sv, DataType.Si, DataType.Hrv]
  },
  {
    name: '前负荷容量',
    types: [DataType.Tfc, DataType.Edi, DataType.Svv, DataType.Vol]
  },
  {
    name: '后负荷阻力',
    types: [DataType.Svr, DataType.Ssvr, DataType.Ssvri, DataType.Svri, DataType.Vas]
  },
  {
    name: '心肌收缩力动力',
    types: [DataType.Pep, DataType.Vet, DataType.Lsw, DataType.Lswi, DataType.Lcw, DataType.Lcwi, DataType.Str, DataType.Epci, DataType.Isi, DataType.Ino]
  },
  {
    name: '常规',
    types: [DataType.Hr, DataType.Sbp, DataType.Dbp, DataType.Map]
  }
]
const tableData = ref([])
for (const part of baseTable) {

  for (let i = 0, len = part.types.length; i < len; ++i) {
    const dtype = part.types[i]
    let obj = {
      class: part.name,
      arg: datEnum.getCn(dtype) + '/' + datEnum.getEn(dtype),
      limit: datEnum.getMin(dtype) !== datEnum.getMax(dtype) ? datEnum.getMin(dtype) + '~' + datEnum.getMax(dtype) : '-',
      unit: datEnum.getUnit(dtype)
    }
    if (pdata.value.length >= 1) {
      Object.assign(obj, { fpos: pdata.value[0][datEnum.getEn(dtype).toLowerCase()] })
    }
    if (pdata.value.length >= 2) {
      Object.assign(obj, { spos: pdata.value[1][datEnum.getEn(dtype).toLowerCase()] })
    }
    tableData.value.push(obj)
  }
}
const tableDataColumns = ref([
  {
    value: 'class',
    label: '',
    width: '40px'
  },
  {
    value: 'arg',
    label: '参数',
    width: pdata.value.length === 2 ? '200px' : '220px'
  }
])
if (pdata.value.length >= 1) {
  tableDataColumns.value.push({
    value: 'fpos',
    label: positionCn(reportdata.position[0].data[DataType.Pos[0]]),
    width: pdata.value.length === 2 ? '100px' : '120px'
  })
}
if (pdata.value.length >= 2) {
  tableDataColumns.value.push({
    value: 'spos',
    label: positionCn(reportdata.position[1].data[DataType.Pos[0]]),
    width: '100px'
  })
}
tableDataColumns.value.push({
  value: 'limit',
  label: '正常值范围',
  width: pdata.value.length === 2 ? '140px' : '160px'
})
tableDataColumns.value.push({
  value: 'unit',
  label: '单位',
  width: 'auto'
})
const spanArr = ref([])
const dataSpanMethod = ({ row, column, rowIndex, columnIndex }) => {
  if (columnIndex === 0) {
    if (rowIndex === 0) {
      return [6, 1]
    }
    else if (rowIndex === 6) {
      return [4, 1]
    }
    else if (rowIndex === 10) {
      return [5, 1]
    }
    else if (rowIndex === 15) {
      return [10, 1]
    }
    else if (rowIndex === 25) {
      return [4, 1]
    }
    else {
      return [0, 0]
    }
  }
}

// 报告结论
const reportConclusion = ref(reportdata.reportConclusion)

const confirmButtonClicked = () => {
  visible.value = false
  consultation({
    reporttime: props.reporttime,
    deviceid: props.deviceid,
    reportConclusion: reportConclusion.value
  })
}
const loading = ref(false)

const reportPrint = () => {
  loading.value = true
  htmlToPdf.getPdf('#main', patientInfo.medicalRecordNumber + '-' + patientInfo.patientName)
  setTimeout(() => {
    loading.value = false
    router.go(0)
  }, 1000)
}

</script>

<style scoped lang="less">
#main {
  text-align: center;
  // padding-left: 15%;
  // padding-right: 15%;
  &/deep/ .el-textarea__inner {
    border: none;
    resize: none;
  }
}
h1 {
  margin: 0;
  margin-bottom: .625rem;
}
h2 {
  margin: 0;
  margin-bottom: .9375rem;
}
h3 {
  margin-top: 0;
  margin-bottom: 1.25rem;
}
/**
改变边框颜色
 */
 .el-table {
  border: 1px solid black;
  margin: 0 auto;
}
/* 表格不出现横向滚动条 */
::v-deep.el-table th {
  border: 1px solid black !important;
  border-right: none !important;
  border-bottom: none !important;
  color: black;
  font-size: 1rem;
}

::v-deep.el-table td {
  border: 1px solid black;
  border-right: none !important;
}

.textarea {
  border: 1px solid black;
  white-space: pre-wrap;
  text-align: left;
  padding: .625rem;
}

</style>