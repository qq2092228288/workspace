import { createStore } from 'vuex'
import { stateInit, RESET_STATE, SET_CURRENT_PAGE_PATH, SET_USER_INFO, SET_MENU, SET_TABLES } from './mutation-types'

export default createStore({
  state: stateInit,
  getters: {
  },
  mutations: {
    [RESET_STATE] (state) {
      Object.assign(state, stateInit)
    },
    [SET_CURRENT_PAGE_PATH] (state, path) {
      state.currentPagePath = path
    },
    [SET_USER_INFO] (state, userInfo) {
      state.userInfo = userInfo
    },
    [SET_MENU] (state, menu) {
      state.menu = null
      state.menu = menu
    },
    [SET_TABLES] (state, tables) {
      state.tables = null
      state.tables = tables
    }
  },
  actions: {
  },
  modules: {
  }
})
