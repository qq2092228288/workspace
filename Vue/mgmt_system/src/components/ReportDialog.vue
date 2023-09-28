<template>
  <ElDialog v-model="visible" top="1vh" style="width: 22cm;" :show-close="false">
    <div id="main">
      <h1>无创心功能监测报告</h1>
      <h1 v-if="strInvalid(primaryPlace)">{{ primaryPlace }}</h1>
      <ElTable
        :data="patintInfoTable"
        :show-header="false"
        :span-method="infoSpanMethod"
        :header-cell-style="{ padding: '0' }"
        :cell-style="{ padding: '0' }"
        border>
        <ElTableColumn v-for="col in patintInfoColumns" :prop="col.colname" :align="col.align" />
      </ElTable>
      <div>
        <div>
          <ElTable
            :data="tableData"
            :span-method="dataSpanMethod"
            :header-cell-style="{ padding: '0' }"
            :cell-style="{ padding: '0' }"
            border>
            <ElTableColumn v-for="col in tableDataColumns" :prop="col.value" :label="col.label" :width="col.width" />
          </ElTable>
        </div>
        <!-- <div class="right">
          
        </div> -->
      </div>
      <div class="report-conclusion" contenteditable="true" v-html="reportConclusion" @blur="reportConclusion=$event.target.innerText"></div>
      <ElDivider />
      <div style="text-align: left;">
        <span>会诊建议:</span>
      </div>
      <div class="report-suggestion" contenteditable="true" v-html="suggestion" @blur="suggestion=$event.target.innerText"></div>
    </div>
    <ElRow type="flex" justify="end" style="margin-top: 1.25rem;" >
      <ElButton type="primary" round @click="suggestionDrawer = true">会诊建议</ElButton>
      <ElTooltip effect="dark" content="未上传签名" :disabled="!signatureDisabled">
        <ElButton type="primary" round @click="confirmButtonDisabled = false" :disabled="signatureDisabled">签名</ElButton>
      </ElTooltip>
    </ElRow>
    <ElRow type="flex" justify="end" style="margin-top: 1.25rem;" >
      <ElTooltip effect="dark" content="下载报告" >
        <ElButton type="success" :icon="Download" @click="reportPrint" :loading="loading" circle />
      </ElTooltip>
      <ElTooltip effect="dark" content="关闭窗口">
        <ElButton type="info" :icon="Close" circle @click="visible = false"/>
      </ElTooltip>
      <ElTooltip effect="dark" :content="confirmButtonDisabled ? '未签名' : '确认修改'">
        <ElButton type="primary" :icon="Check" circle @click="confirmButtonClicked" :disabled="confirmButtonDisabled" />
      </ElTooltip>
    </ElRow>
    <ElDrawer v-model="suggestionDrawer" :direction="drawerDirection"  :size="isMobile() ? '50%' : '40%'">
      <template #header>
        <h4>请选择会诊建议</h4>
      </template>
      <template #default>
        <div class="topic">
          <span>{{ lcoTopic }}</span>
          <ElRadioGroup v-model="lco">
            <ElRadio label="正常" />
            <ElRadio label="增强：可见于高血压性心脏病、肥厚性心肌病早期、甲亢性心脏病等疾病，也可见于正常人，根据具体情况判断。" />
            <ElRadio label="减少：可见于左室收缩力下降、主动脉瓣狭窄、左室流出道狭窄、心房颤动、左室舒张功能减弱等疾病，建议结合患者症状（气促、胸闷、头晕、乏力等）、体征（双肺湿罗音、瓣膜杂音、双下肢浮肿、肝颈静脉回流征等）、胸片（胸淤血、肺水肿）、心脏超声（EF下降，重度二尖瓣或主动脉瓣狭窄或关闭不全）、检验结果（NT-ProBNP、肌钙蛋白、DD），判断是否存在心力衰竭的情况，必要时可考虑强心治疗" />
          </ElRadioGroup>
        </div>
        <div class="topic">
          <span>{{ preloadTopic }}</span>
          <ElRadioGroup v-model="preload">
            <ElRadio label="正常" />
            <ElRadio label="增高：说明可能存在水钠潴留，建议结合患者症状（气促、胸闷、咳嗽、尿量减少、体重增加等）、体征（双肺湿罗音、双下肢浮肿、肝颈静脉回流征等）、胸片（胸淤血、肺水肿），必要时可考虑加强利尿治疗。" />
            <ElRadio label="减低：说明细胞内外液体可能不足，建议结合患者进食情况、血压、尿量等，必要时可考虑适当补液。" />
          </ElRadioGroup>
        </div>
        <div class="topic">
          <span>{{ afterloadTopic }}</span>
          <ElRadioGroup v-model="afterload">
            <ElRadio label="正常" />
            <ElRadio label="增高：说明可能存在主动脉瓣狭窄、左室流出道梗阻、高血压等情况，建议结合患者症状（气促、胸闷、头晕等）、体征（血压、心脏杂音等）、超声心动图，根据具体情况决定治疗策略。" />
            <ElRadio label="减低：说明可能存在神经源性休克、迷走神经反射、严重感染等情况，建议加强补液或血管收缩剂（去甲肾上腺素等）,根据具体情况决定治疗策略。" />
          </ElRadioGroup>
        </div>
        <div class="topic">
          <span>{{ lvsfTopic }}</span>
          <ElRadioGroup v-model="lvsf">
            <ElRadio label="正常" />
            <ElRadio label="增高：临床意义等同于正常，注意排查甲亢、贫血等高代谢性疾病。" />
            <ElRadio label="减低：说明可能存在左室心肌病变，如扩张型心肌病、缺血性心肌病等，建议结合患者症状（气促、胸闷、头晕等）、体征（双肺湿罗音、瓣膜杂音、双下肢浮肿、肝颈静脉回流征等）、胸片（胸淤血、肺水肿）、心脏超声、检验结果（NT-ProBNP、肌钙蛋白、DD），必要时可考虑强心、利尿治疗,根据具体情况决定治疗策略。" />
          </ElRadioGroup>
        </div>
        <div class="topic">
          <span>{{ lvdfTopic }}</span>
          <ElRadioGroup v-model="lvdf">
            <ElRadio label="正常" />
            <ElRadio label="减退：常见于老年人，可见于高血压性心脏病、肥厚型心肌病、限制型心肌病等疾病。主要是病因治疗。" />
          </ElRadioGroup>
        </div>
      </template>
      <template #footer>
        <ElButton type="info" round @click="suggestionDrawer = false">取消</ElButton>
        <ElButton type="primary" round @click="confirmSuggestion">确定</ElButton>
      </template>
    </ElDrawer>
  </ElDialog>
</template>

<script setup>
import { Check, Close, Download } from '@element-plus/icons';
import { ElButton, ElDialog, ElDivider, ElDrawer, ElInput, ElMessage, ElRadio, ElRadioGroup, ElRow, ElTable, ElTableColumn, ElTooltip } from 'element-plus';
import { ref } from 'vue';
import { cBsa, createDataType, DataType, intercept, positionData } from '@/utils/tebcoparamcalculation';
import { consultation } from '@/utils/communication';
import htmlToPdf from '@/utils/htmltopdf';
import router from '@/router';
import Waveform from './Waveform.vue';
import store from "@/store";

const isMobile = () => {
	 let flag = navigator.userAgent.match(/(phone|pad|pod|iPhone|iPod|ios|iPad|Android|Mobile|BlackBerry|IEMobile|MQQBrowser|JUC|Fennec|wOSBrowser|BrowserNG|WebOS|Symbian|Windows Phone)/i)
   return flag
}
const drawerDirection = ref(isMobile() ? 'btt' : 'rtl')

const lco = ref('')
const preload = ref('')
const afterload = ref('')
const lvsf = ref('')
const lvdf = ref('')

const lcoTopic = "一、左心输出量："
const preloadTopic = "二、前负荷："
const afterloadTopic = "三、后负荷："
const lvsfTopic = "四、左室收缩功能："
const lvdfTopic = "五、左室舒张功能 ："

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
    col0: '病历号',
    col1: patientInfo.medicalRecordNumber,
    col2: '科室',
    col3: secondaryPlace
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
  // if (Object.keys(pos.waveform).length !== 0) {
    
  // }
}
// const fWaveform = ref()
// const sWaveform = ref()
// if (reportdata.position.length === 2) {
//   fWaveform.value = reportdata.position[0].waveform
//   sWaveform.value = reportdata.position[1].waveform
// }
// else if (reportdata.position.length === 1) {
//   fWaveform.value = reportdata.position[0].waveform
// }


const datEnum = createDataType(DataType)
const baseTable = [
  {
    name: '心排量',
    types: [DataType.Co, DataType.Ci, DataType.Sv, DataType.Si]
  },
  {
    name: '前负荷',
    types: [DataType.Tfc, DataType.Edi, DataType.Svv, DataType.Vol]
  },
  {
    name: '后负荷阻力',
    types: [DataType.Svr, DataType.Ssvr, DataType.Ssvri, DataType.Svri, DataType.Vas]
  },
  {
    name: '心肌收缩力',
    types: [DataType.Pep, DataType.Vet, DataType.Lsw, DataType.Lswi, DataType.Lcw, DataType.Lcwi, DataType.Str, DataType.Epci, DataType.Isi, DataType.Ino]
  },
  {
    name: '常规',
    types: [DataType.Hr, DataType.Hrv, DataType.Sbp, DataType.Dbp, DataType.Map]
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
const pLen = pdata.value.length
const tableDataColumns = ref([
  {
    value: 'class',
    label: '',
    width: '40px'
  },
  {
    value: 'arg',
    label: '参数',
    width: pLen == 2 ? '160px' : '180px'
  }
])
if (pLen >= 1) {
  tableDataColumns.value.push({
    value: 'fpos',
    label: positionCn(reportdata.position[0].data[DataType.Pos[0]]),
    width: pLen == 2 ? '60px' : '80px'
  })
}
if (pLen == 2) {
  tableDataColumns.value.push({
    value: 'spos',
    label: positionCn(reportdata.position[1].data[DataType.Pos[0]]),
    width: '60px'
  })
}
tableDataColumns.value.push({
  value: 'limit',
  label: '正常值范围',
  width: pLen == 2 ? '100px' : '120px'
})
tableDataColumns.value.push({
  value: 'unit',
  label: '单位',
  width: 'auto'
})
const dataSpanMethod = ({ row, column, rowIndex, columnIndex }) => {
  if (columnIndex === 0) {
    if (rowIndex === 0) {
      return [4, 1]
    }
    else if (rowIndex === 4) {
      return [4, 1]
    }
    else if (rowIndex === 8) {
      return [5, 1]
    }
    else if (rowIndex === 13) {
      return [10, 1]
    }
    else if (rowIndex === 23) {
      return [5, 1]
    }
    else {
      return [0, 0]
    }
  }
}
const confirmButtonDisabled = ref(true)
// 报告结论
const reportConclusion = ref(reportdata.reportConclusion)
// 签名
const signature = ref()
const signatureDisabled = ref(false)
try {
  signature.value = JSON.parse(store.state.userInfo.remarks).signature
} catch (error) {
  
}
if (signature.value === undefined || signature.value.length === 0) {
  signatureDisabled.value = true
}
// 会诊建议
const suggestionDrawer = ref(false)
const suggestion = ref('')
try {
  suggestion.value = reportdata.consultation.suggestion
} catch (error) {
  
}
const confirmButtonClicked = () => {
  visible.value = false
  consultation({
    reporttime: props.reporttime,
    deviceid: props.deviceid,
    reportConclusion: reportConclusion.value,
    signature: signature.value,
    suggestion: suggestion.value
  })
}
const loading = ref(false)

const reportPrint = async () => {
  loading.value = true
  htmlToPdf.getPdf('#main', patientInfo.medicalRecordNumber + '-' + patientInfo.patientName)
  setTimeout(() => {
    loading.value = false
  }, 3000)
}
const confirmSuggestion = () => {
  if (lco.value.length === 0 || preload.value.length === 0 || afterload.value.length === 0 || lvsf.value.length === 0 || lvdf.value.length === 0) {
    ElMessage.error({
      message: '所有项为必选',
      showClose: true
    })
    return
  }
  let str = lcoTopic + "\n" + lco.value + "\n" 
          + preloadTopic + "\n" + preload.value + "\n" 
          + afterloadTopic + "\n" + afterload.value + "\n" 
          + lvsfTopic + "\n" + lvsf.value + "\n" 
          + lvdfTopic + "\n" + lvdf.value
  suggestion.value += str
  suggestionDrawer.value = false
}

</script>

<style scoped lang="less">
#main {
  width: 21cm;
  height: 39.7cm;
  text-align: center;
  &/deep/ .el-textarea__inner {
    border: none;
    resize: none;
  }
}
.left {
  float: left;
  width: 152mm;
  height: auto;
}
.right {
  width: auto;
  height: 200mm;
}
h1 {
  margin: 0;
  // margin-bottom: 5.64mm;
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
  font-size: 2.1mm;
}
/* 表格不出现横向滚动条 */
::v-deep.el-table th {
  border: 1px solid black !important;
  border-right: none !important;
  border-bottom: none !important;
  color: black;
}

::v-deep.el-table td {
  border: 1px solid black;
  border-right: none !important;
}

.report-conclusion {
  white-space: pre-wrap;
  text-align: left;
  padding: .3125rem;
  font-size: 2.1mm;
  height: 60mm;
}
.report-suggestion {
  white-space: pre-wrap;
  text-align: left;
  padding: .3125rem;
  font-size: 2.1mm;
  min-height: 60mm;
}

.el-radio-group {
  display: flex;
  flex-flow: column nowrap;
  align-items: flex-start;
}

.topic {
  text-align: left;
  font-weight: bold;
}

</style>