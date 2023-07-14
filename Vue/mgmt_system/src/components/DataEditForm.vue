<template>
  <ElDialog>
    <ElTable border stripe highlight-current-row
      :show-header="false"
      :data="data">
      <ElTableColumn prop="name" width="120px" align="right" />
      <ElTableColumn prop="value">
        <template #default="scope">
          <ElInput type="textarea" :autosize="{ minRows: 1, maxRows: 5 }" v-model="scope.row['value']" />
        </template>
      </ElTableColumn>
    </ElTable>
    <ElRow type="flex" justify="end" style="margin-top: 1.25rem;" >
      <ElButton type="info">取消</ElButton>
      <ElButton type="primary">确定</ElButton>
    </ElRow>
  </ElDialog>
</template>

<script setup>
import { ElButton, ElDialog, ElInput, ElRow, ElTable, ElTableColumn } from 'element-plus';
import { defineEmits, defineProps, ref } from 'vue';

// const emit = defineEmits(['modified-value'])

const props = defineProps({
  data: Object,
  columns: Array
})
const data = ref([])
props.columns.filter((column, index, array) => {
  data.value.push({
    name: ref(column.cn),
    value: ref('')
  })
})

</script>