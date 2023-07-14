import { createRouter, createWebHashHistory } from 'vue-router'
import { ADMINISTRATORINFO_PATH, AGENTINFO_PATH, ALLOCATEDCONSUMABLES_PATH, DEVICE_PATH, HOMEPAGE_PATH, LOGIN_PATH, PLACEINFO_PATH, REPORTINFO_PATH, SOFTWAREMANAGEMENT_PATH } from './router-path'

const pageTitle = '泽耀医疗后台管理系统'
const routes = [
  {
    path: LOGIN_PATH,
    name: 'login',
    meta: {
      title: pageTitle + '-登录'
    },
    component: () => import('../views/LoginView.vue')
  },
  {
    path: HOMEPAGE_PATH,
    name: 'homepage',
    meta: {
      title: pageTitle + '-主页',
      keepAlive: true
    },
    component: () => import('../views/HomepageView.vue'),
    children: [
      {
        path: SOFTWAREMANAGEMENT_PATH,
        name: 'softwaremanagement',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/SoftwareManagement.vue')
      },
      {
        path: ADMINISTRATORINFO_PATH,
        name: 'administratorinfo',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/AdministratorInfo.vue')
      },
      {
        path: AGENTINFO_PATH,
        name: 'agentinfo',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/AgentInfo.vue')
      },
      {
        path: ALLOCATEDCONSUMABLES_PATH,
        name: 'allocatedconsumables',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/AllocatedConsumables.vue')
      },
      {
        path: DEVICE_PATH,
        name: 'device',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/Device.vue')
      },
      {
        path: PLACEINFO_PATH,
        name: 'placeinfo',
        meta: {
          keepAlive: true
        },
        component: () => import('../views/menu/PlaceInfo.vue')
      },
      {
        path: REPORTINFO_PATH,
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
