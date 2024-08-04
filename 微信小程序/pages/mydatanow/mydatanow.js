// pages/mydatanow/mydatanow.js
Page({
  data: {
    height: null,
    weight: null,
    bmi: null,
    state: '',
    symbol: '>',
    show: false,
    mystate: ["过轻", "正常", "过重", "肥胖"]
  },

  onLoad(options) {
    var that = this
    wx.request({
      //设备ID
      //api-key
      url: 'http://api.heclouds.com/devices/952687417/datapoints?', //xxxxxxxxxx这里填写你的设备id,删掉？后面的时间后的返回回来的是最新数据
      header: {
        "api-key": "gLvq6SSLYwOA=7E3leJzHhiEe=Y=" //这里写你的api-key
      },
      method: "GET",
      //获取成功
      success: function (res) {
        console.log(res)
        console.log(res.data.data.datastreams[0].datapoints[0].value)
        that.setData({
          weight: res.data.data.datastreams[0].datapoints[0].value,
          state: res.data.data.datastreams[1].datapoints[0].value,
          height: res.data.data.datastreams[3].datapoints[0].value,
          bmi: res.data.data.datastreams[2].datapoints[0].value
        })
      }
    })
  },
  refresh() {
    var that = this
    wx.request({
      //设备ID
      //api-key
      url: 'http://api.heclouds.com/devices/952687417/datapoints?', //xxxxxxxxxx这里填写你的设备id,删掉？后面的时间后的返回回来的是最新数据
      header: {
        "api-key": "gLvq6SSLYwOA=7E3leJzHhiEe=Y=" //这里写你的api-key
      },
      method: "GET",
      //获取成功
      success: function (res) {
        console.log(res)
        console.log(res.data.data.datastreams[0].datapoints[0].value)
        that.setData({
          weight: res.data.data.datastreams[0].datapoints[0].value,
          state: res.data.data.datastreams[1].datapoints[0].value,
          height: res.data.data.datastreams[3].datapoints[0].value,
          bmi: res.data.data.datastreams[2].datapoints[0].value
        })
      }
    })
  },
  more() {
    if (this.data.symbol == '>') {
      this.setData({
        symbol: '∨',
        show: true
      })
    } else {
      this.setData({
        symbol: '>',
        show: false
      })
    }
  },
  back() {
    wx.navigateTo({
      url: '/pages/catalogue/catalogue',
    })
  }
})