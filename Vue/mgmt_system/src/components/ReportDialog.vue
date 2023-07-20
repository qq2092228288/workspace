<template>
  <ElDialog v-model="visible" top="1vh" width="700px" :show-close="false">
    <div id="main">
      <h1 v-if="primaryPlace !== undefined">{{ primaryPlace }}</h1>
      <!-- <h1>无创心功能监测报告</h1> -->
      <h2 v-if="secondaryPlace !== undefined">{{ "科室: " + secondaryPlace }}</h2>
      <h3 v-if="inspector !== 'unknown'">{{ "操作人员: " + inspector }}</h3>
      <ElTable :data="patintInfoTable" :show-header="false" border>
        <ElTableColumn v-for="col in patintInfoColumns" :prop="col.colname" :align="col.align" />
      </ElTable>
      <ElTable :data="tableData" border>
        <ElTableColumn v-for="col in tableDataColumns" :prop="col.value" :label="col.label" :width="col.width" />
      </ElTable>
      <ElInput type="textarea" :autosize="{ minRows: 1, maxRows: 10 }" v-model="reportConclusion" />
    </div>
    <ElRow type="flex" justify="end" style="margin-top: 1.25rem;" >
      <ElTooltip effect="dark" content="打印报告" >
        <ElButton type="success" :icon="Printer" @click="reportPrint" circle />
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
import { Check, Close, Printer } from '@element-plus/icons';
import { ElButton, ElCol, ElDialog, ElInput, ElRow, ElTable, ElTableColumn, ElTooltip } from 'element-plus';
import { h, ref } from 'vue';
import { cBsa, createDataType, DataType, intercept, positionData } from '@/utils/tebcoparamcalculation';
import { consultation } from '@/utils/communication';
import printJS from 'print-js';
import html2canvas from 'html2canvas';

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
const reportPrint = () => {
  printJS({
    printable: 'main',
    type: 'html',
    style: `@page{
      size: landscape;
      margin: 0 ;
    }`
  })
  // html2canvas(document.querySelector("#main")).then(canvas => {
  //   const imgUrl = canvas.toDataURL('image/png'); // 获取生成的图片的url
  //   let save = document.createElement('a')
  //   save.href = imgUrl
  //   save.download = 'test'
  //   save.click()
  // })
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
    col1: patientInfo.hb !== undefined ? patientInfo.hb + ' g/dl' : null,
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
  for (const dtype of part.types) {
    let obj = {
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
const tableDataColumns = ref([{
  value: 'arg',
  label: '参数',
  width: pdata.value.length === 2 ? '200px' : '220px'
}])
if (pdata.value.length >= 1) {
  tableDataColumns.value.push({
    value: 'fpos',
    label: positionCn(reportdata.position[0].data[DataType.Pos[0]]),
    width: pdata.value.length === 2 ? '80px' : '100px'
  })
}
if (pdata.value.length >= 2) {
  tableDataColumns.value.push({
    value: 'spos',
    label: positionCn(reportdata.position[1].data[DataType.Pos[0]]),
    width: '80px'
  })
}
tableDataColumns.value.push({
  value: 'limit',
  label: '正常值范围',
  width: pdata.value.length === 2 ? '120px' : '140px'
})
tableDataColumns.value.push({
  value: 'unit',
  label: '单位',
  width: 'auto'
})
// 报告结论
const reportConclusion = ref(reportdata.reportConclusion)
const confirmButtonClicked = () => {
  visible.value = false
  console.log(reportConclusion.value)
  consultation({
    reporttime: props.reporttime,
    deviceid: props.deviceid,
    reportConclusion: reportConclusion.value
  })
}

</script>

<style scoped lang="less">
#main {
  text-align: center;
}
h1 {
  margin: 0;
}
h2 {
  margin: 0;
}
h3 {
  margin-top: 0;
  margin-bottom: .3125rem;
}
</style>