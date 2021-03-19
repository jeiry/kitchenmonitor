const app = getApp()
import {
  $wuxNotification
} from '../../miniprogram_npm/wux-weapp/index'
var request = require('../../utils/http.js')
var mqtt = require('../../utils/mqtt.min.js') //根据自己存放的路径修改
var client = null;
var points = []
Page({
      data: {
        temparr: [
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0]
        ],
        alarr: [
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0],
          [0, 0, 0, 0, 0, 0, 0, 0]
        ],
        countTime: 0,
        maxTemp:0
      },
      onLoad: function (options) {
        this.connectMqtt()
      },
      onUnload: function (options) {
        client.end()
      },
      connectMqtt: function () {
        const options = {
          connectTimeout: 4000, // 超时时间
          clientId: '123212',
          port: 8083, 
          username: 'xxxxx',
          password: 'xxxxx',
        }

        client = mqtt.connect('wx://t.xxxxx.fun/mqtt', options)
        client.on('reconnect', (error) => {
          console.log('正在重连:', error)
        })

        client.on('error', (error) => {
          console.log('连接失败:', error)
        })

        let that = this;
        client.on('connect', (e) => {
          console.log('成功连接服务器')
          //订阅一个主题
          client.subscribe('/home/r/kitchenTemp', {
            qos: 0
          }, function (err) {
            if (!err) {
              // client.publish('message.queue', 'Hello MQTT')
              console.log("订阅成功")
            }
          })
        })
        client.on('message', function (topic, message) {
            // console.log('received msg:' + message.toString());
            let arr = message.toString().split('_')
            // console.log(arr);
            let tempArr_ = that.data.temparr;
            // console.log(tempArr_)
            let tempAlArr_ = that.data.alarr
            let findMax = []
            for (var i = 0; i < arr.length; i++) {
              if (arr[0] == i) {
                tempArr_[i] = [arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8]]
                for (var j = 0; j < 8; j++) {
                  let a = Math.floor(tempArr_[i][j]) / 100
                  // let red = Math.floor(tempArr_[i][j])
                  tempAlArr_[i][j] = 1 - a
                }
              }
            }
            // console.log(tempArr_)
            let countTime_ = that.data.countTime
            if (that.data.countTime < 7) {
              countTime_++;
            } else {
              countTime_ = 0
              let arr_ = []
              for (var i = 0; i < tempArr_.length; i++) {
                for (var j = 0; j < tempArr_[i].length; j++) {
                  arr_.push(Math.floor(tempArr_[i][j]))
                }
              }
              that.setData({
                maxTemp: Math.max(...arr_)
              })

            }
              that.setData({
                temparr: tempArr_,
                alarr: tempAlArr_,
                countTime: countTime_
              })

            }.bind(this))

        },
      })
