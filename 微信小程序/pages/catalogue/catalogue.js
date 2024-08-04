// pages/catalogue/catalogue.js
Page({
  data: {

  },
  bmi() {
    wx.switchTab({
      url: '/pages/mydatanow/mydatanow',
    })
  },
  weather() {
    wx.navigateTo({
      url: '/pages/weather/weather',
    })
  },
  clock() {
    wx.navigateTo({
      url: '/pages/clock/clock',
    })
  },
  notebook() {
    wx.navigateTo({
      url: '/pages/notebook/notebook',
    })
  }

})