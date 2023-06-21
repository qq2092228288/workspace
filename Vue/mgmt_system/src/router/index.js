import { createRouter, createWebHashHistory } from 'vue-router'

const pageTitle = '泽耀医疗后台管理系统'
const routes = [
  {
    path: '/',
    name: 'login',
    meta: {
      title: pageTitle + '-登录'
    },
    component: () => import(/* webpackChunkName: "login" */ '../views/LoginView.vue')
  },
  {
    path: '/homepage',
    name: 'homepage',
    meta: {
      title: pageTitle + '-主页',
      keepAlive: false
    },
    component: () => import(/* webpackChunkName: "homepage" */ '../views/HomepageView.vue')
  }
]

const router = createRouter({
  history: createWebHashHistory(),
  routes
})

router.beforeEach((to, from, next) => {
  window.document.title = to.meta.title
  next()
})

export default router
