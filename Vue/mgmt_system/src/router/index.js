import { createRouter, createWebHashHistory } from 'vue-router'

const pageTitle = '泽耀医疗后台管理系统'
const routes = [
  {
    path: '/',
    name: 'login',
    meta: {
      title: pageTitle + '-登录'
    },
    component: () => import('../views/LoginView.vue')
  },
  {
    path: '/homepage',
    name: 'homepage',
    meta: {
      title: pageTitle + '-主页',
      keepAlive: true
    },
    component: () => import('../views/HomepageView.vue'),
    children: [
      {
        path: 'softwaremanagement',
        name: 'softwaremanagement',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/SoftwareManagement.vue')
      },
      {
        path: 'administratorinfo',
        name: 'administratorinfo',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/AdministratorInfo.vue')
      },
      {
        path: 'agentinfo',
        name: 'agentinfo',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/AgentInfo.vue')
      },
      {
        path: 'allocatedconsumables',
        name: 'allocatedconsumables',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/AllocatedConsumables.vue')
      },
      {
        path: 'device',
        name: 'device',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/Device.vue')
      },
      {
        path: 'placeinfo',
        name: 'placeinfo',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/PlaceInfo.vue')
      },
      {
        path: 'reportinfo',
        name: 'reportinfo',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/ReportInfo.vue')
      }
    ]
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
