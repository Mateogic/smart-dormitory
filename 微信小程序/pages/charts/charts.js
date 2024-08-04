import * as echarts from '../../ec-canvas/echarts';

const app = getApp();
var receive = [];

function initChart(canvas, width, height, dpr) {
  const chart = echarts.init(canvas, null, {
    width: width,
    height: height,
    devicePixelRatio: dpr // new
  });
  canvas.setChart(chart);
  console.log(Page)
  //这里的option可以去echarts官网里面的option一起使用
  var option = {
    xAxis: {
      type: 'category',
      data: receive[0] //['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun']
    },
    yAxis: {
      type: 'value'
    },
    series: [{
      data: receive[1], //[120, 200,150, 80, 70, 110, 130],
      type: 'line'
    }]
  };

  chart.setOption(option);
  return chart;
}

function initChart2(canvas, width, height, dpr) {
  const chart2 = echarts.init(canvas, null, {
    width: width,
    height: height,
    devicePixelRatio: dpr // new
  });
  canvas.setChart(chart2);
  //这里的option可以去echarts官网里面的option一起使用
  var option2 = {
    xAxis: {
      type: 'category',
      data: receive[0] //['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun']
    },
    yAxis: {
      type: 'value'
    },
    series: [{
      data: receive[2], //[120, 200,150, 80, 70, 110, 130],
      type: 'line'
    }]
  };

  chart2.setOption(option2);
  return chart2;
}

function initChart3(canvas, width, height, dpr) {
  const chart3 = echarts.init(canvas, null, {
    width: width,
    height: height,
    devicePixelRatio: dpr // new
  });
  canvas.setChart(chart3);
  //这里的option可以去echarts官网里面的option一起使用
  var option3 = {
    xAxis: {
      type: 'category',
      data: receive[0] //['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun']
    },
    yAxis: {
      type: 'value'
    },
    series: [{
      data: receive[3], //[120, 200,150, 80, 70, 110, 130],
      type: 'line'
    }]
  };

  chart3.setOption(option3);
  return chart3;
}

Page({
  onShareAppMessage: function (res) {
    return {
      title: 'ECharts 可以在微信小程序中使用啦！',
      path: '/pages/index/index',
      success: function () {},
      fail: function () {}
    }
  },
  data: {
    Hei: true,
    Wei: false,
    Bmi: false,
    receive: false,
    ec: {
      onInit: initChart
    },
    ec2: {
      onInit: initChart2
    },
    ec3: {
      onInit: initChart3
    }
  },

  onLoad(options) {
    console.log(JSON.stringify(options))
    if (JSON.stringify(options) == "{}") {
      wx.showToast({
        title: '暂无数据',
        duration: 3000
      })
    }
    var a = JSON.parse(options.data);

    receive = a;
    this.setData({
      receive: true
    })
  },
  points: function (e) {
    var that = this
    wx.request({
      //设备ID
      //api-key
      url: 'http://api.heclouds.com/devices/952687417/datapoints?start=2017-01-01T00:00:00', //xxxxxxxxxx这里填写你的设备id,删掉？后面的时间后的返回回来的是最新数据
      header: {
        "api-key": "gLvq6SSLYwOA=7E3leJzHhiEe=Y=" //这里写你的api-key
      },
      method: "GET",
      //获取成功
      success: function (res) {
        console.log(res)
      }
    })
  },
  hei() {
    this.setData({
      Hei: true,
      Wei: false,
      Bmi: false,
    })
  },
  wei() {
    this.setData({
      Hei: false,
      Wei: true,
      Bmi: false,
    })
  },
  bmi() {
    this.setData({
      Hei: false,
      Wei: false,
      Bmi: true,
    })
  }
});
