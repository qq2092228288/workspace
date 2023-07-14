import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import * as ELIcons from '@element-plus/icons-vue'


const app = createApp(App)
for (let iconName in ELIcons) {
  app.component(iconName, ELIcons[iconName])
}
app.use(store).use(router).use(ElementPlus).mount('#app')

// 处理改变窗口大小出现错误
const debounce = (fn, delay) => {
  let timer = null;
  return function () {
    let context = this;
    let args = arguments;
    clearTimeout(timer);
    timer = setTimeout(function () {
    fn.apply(context, args);
    }, delay);
  }
}

const _ResizeObserver = window.ResizeObserver;
window.ResizeObserver = class ResizeObserver extends _ResizeObserver {
  constructor(callback) {
  callback = debounce(callback, 16);
  super(callback);
  }
}
  