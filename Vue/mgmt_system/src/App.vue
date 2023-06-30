<template>
  <div id="building">
    <router-view />
  </div>
</template>

<script>
export default {
  // 缓存vuex数据
  created () {
    //在页面加载时读取sessionStorage里的状态信息
    if (sessionStorage.getItem('store')) {
      this.$store.replaceState(Object.assign({}, this.$store.state, JSON.parse(sessionStorage.getItem('store'))));
    }
    //在页面刷新时将vuex里的信息保存到sessionStorage里
    window.addEventListener('beforeunload', () => {
      sessionStorage.setItem('store', JSON.stringify(this.$store.state));
    });
  }
}
</script>

<style>
body{
	margin: 0;
}
#building {
  position: fixed;
  width: 100%;
  height: 100%;
  background-size: 100% 100%;
  background: linear-gradient(to bottom right, rgb(13, 214, 137), rgb(37, 129, 172));
}
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
}
</style>
