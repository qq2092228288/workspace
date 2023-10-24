<template>
  <div class="waveform" ref="main" ></div>
</template>

<script setup>
import { ref, onMounted } from "vue";
//  按需引入 echarts
import * as echarts from "echarts";
const main = ref() // 使用ref创建虚拟DOM引用，使用时用main.value

function init() {
  // 基于准备好的dom，初始化echarts实例
  var myChart = echarts.init(main.value);
  // 指定图表的配置项和数据
  var option = {
    title: {
      text: 'ECharts 入门示例'
    },
    tooltip: {},
    xAxis: {
      data: ['衬衫', '羊毛衫', '雪纺衫', '裤子', '高跟鞋', '袜子']
    },
    yAxis: {},
    series: [
      {
        name: '销量',
        type: 'bar',
        data: [5, 20, 36, 10, 10, 20]
      }
    ]
  };
  // 使用刚指定的配置项和数据显示图表。
  myChart.setOption(option);
}
onMounted(
  () => {
    init()
  }
)
</script>

<style scoped lang="less">
.waveform {
  float: right;
  width: 200px;
  height: 200px;
}
</style>
