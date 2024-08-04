Page({
  data: {
    //文本框的数据模型
    input: '',
    //任务清单数据模型 
    todos: [{
        name: '熟记26英文字母', //任务名称
        complete: false //任务完成状态
      },
      {
        name: '背会九九乘法口诀', //任务名称
        omplete: false //任务完成状态
      },
      {
        name: '按时干饭', //任务名称  
        complete: true //任务完成状态 
      }
    ],
    leftCount: 2
  },
  //1,先让按钮点击时执行一段代码
  addTodoHandle: function () {
    //点击按钮之后执行事件
    // console.log(this.data.input) //拿到文本框的值
    if (!this.data.input) return
    var todos = this.data.todos
    todos.push({
      name: this.data.input,
      complete: false,
      allComplete: false
    })
    wx.setStorage({
      key: "todos",
      data: todos
    })
    console.log(wx.getStorageSync('todos'))
    //必须显式的通过setData来改变数据，这样界面才会得到变化
    this.setData({
      todos: todos,
      input: '',
      leftCount: this.data.leftCount + 1
    })
  },
  //2，拿到文本框里面的值
  //2.1由于小程序的数据绑定是单向的，必须要给文本注册改变事件
  inputChangeHandle: function (e) {
    // console.log(e.detail)
    this.setData({
      input: e.detail.value
    })
  },
  toggleToHandle: function (e) {
    //切换当前点中item的完成状态
    //console.log(e.currentTarget)
    var item = this.data.todos[e.currentTarget.dataset.index]
    item.complete = !item.complete
    var leftCount = this.data.leftCount + (item.complete ? -1 : 1)
    //console.log(item)
    this.setData({
      todos: this.data.todos,
      leftCount: leftCount
    })
  },
  //这个方面需要注意冒泡问题
  DeleteHandle: function (e) {
    //删除任务操作
    console.log(e.currentTarget)
    var todos = this.data.todos
    //item就是splice方法中删除掉的元素
    var item = todos.splice(e.currentTarget.dataset.index, 1)[0] //删除数组中对象的方法，1是从固定下标开始删除几个数组元素
    var leftCount = this.data.leftCount - (item.complete ? 0 : 1)
    this.setData({
      todos: todos,
      leftCount: leftCount
    })
  },
  toggleAllHandle: function () {
    //this在这里永远指向的当前的页面对象  
    this.data.allComplete = !this.data.allComplete
    var todos = this.data.todos
    var that = this
    todos.forEach(function (item) { //for循环  
      item.complete = that.data.allComplete
    })
    var leftCount = this.data.allComplete ? 0 : this.data.todos.length
    this.setData({
      todos: todos,
      leftCount: leftCount
    })
  },
  clearHandle: function () {
    var todos = [] //定义空数组
    this.data.todos.forEach(function (item) {
      if (!item.complete) {
        todos.push(item) //把所有未完成的任务存储到一个新的数组
      }
    })
    this.setData({
      todos: todos
    })
  }
})