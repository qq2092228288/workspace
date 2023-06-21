<template>
  <div id="building">
    <router-view />
  </div>
</template>

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
