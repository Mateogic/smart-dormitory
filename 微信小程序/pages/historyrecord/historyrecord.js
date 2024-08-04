// pages/historyrecord/historyrecord.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    mystate: ["过轻", "正常", "过重", "肥胖"],
    number: 0,
    height: [],
    weight: [],
    bmi: [],
    state: [],
    choice: true,
    selected: [],
    selenum: [],
    seleallnum: 0
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {
    var that = this
    wx.request({
      //设备ID
      //api-key
      url: 'http://api.heclouds.com/devices/952687417/datapoints?start=2017-01-01T00:00:00&sort=DESC', //xxxxxxxxxx这里填写你的设备id,删掉？后面的时间后的返回回来的是最新数据
      header: {
        "api-key": "gLvq6SSLYwOA=7E3leJzHhiEe=Y=" //这里写你的api-key
      },
      method: "GET",
      //获取成功
      success: function (res) {
        console.log(res)
        that.setData({
          number: res.data.data.count / 4,
          weight: res.data.data.datastreams[0],
          state: res.data.data.datastreams[1],
          height: res.data.data.datastreams[3],
          bmi: res.data.data.datastreams[2]
        })
      }
    })
  },
  mychoice() {
    if (this.data.choice) {
      this.setData({
        choice: false
      })
    } else {
      this.setData({
        choice: true
      })
    }

  },
  change(e) {
    console.log(e)
    this.setData({
      selenum: e.detail.value,
      seleallnum: e.detail.value.length
    })
  },
  switch (e) {
    //将对象转为string
    var selecteddata = []
    var t = []
    var h = []
    var w = []
    var b = []
    for (let i = 0; i < this.data.seleallnum; i++) {
      w.push(this.data.weight.datapoints[this.data.selenum[i]].value),
        h.push(this.data.height.datapoints[this.data.selenum[i]].value),
        b.push(this.data.bmi.datapoints[this.data.selenum[i]].value),
        t.push(this.data.weight.datapoints[this.data.selenum[i]].at.substring(0, 19).replace(' ', '\n'))

    }
    selecteddata.push(t)
    selecteddata.push(h)
    selecteddata.push(w)
    selecteddata.push(b)
    console.log(selecteddata)
    var senddata = JSON.stringify(selecteddata)
    wx.reLaunch({
      url: '/pages/charts/charts?data=' + senddata,

    })
  },
  refresh() {
    var that = this
    wx.request({
      //设备ID
      //api-key
      url: 'http://api.heclouds.com/devices/952687417/datapoints?start=2017-01-01T00:00:00&sort=DESC', //xxxxxxxxxx这里填写你的设备id,删掉？后面的时间后的返回回来的是最新数据
      header: {
        "api-key": "gLvq6SSLYwOA=7E3leJzHhiEe=Y=" //这里写你的api-key
      },
      method: "GET",
      //获取成功
      success: function (res) {
        console.log(res)
        that.setData({
          number: res.data.data.count / 4,
          weight: res.data.data.datastreams[0],
          state: res.data.data.datastreams[1],
          height: res.data.data.datastreams[3],
          bmi: res.data.data.datastreams[2]
        })
      }
    })
  }
})