#!/usr/bin/env python
# -*- coding: utf-8 -*-
import time
import paho.mqtt.client as mqtt
import json
import os
import requests

client = mqtt.Client()
# 连接mqtt
def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))
    client.subscribe('/home/r/kitchenTemp', qos=0)
arr = [] #重组数组
flag = 0 #重组数组标记
sentMsgFlag = 0
def checkIp():
    ips = os.popen("arp -a")
    ips = ips.readlines()
    ipArr = []
    for obj in ips:
        ip = obj.split(' ')[1]
        ipArr.append(ip[1:-1])
    print(ipArr)
    checkIps = ['192.168.31.197', '192.168.31.250']
    ipList = list(set(ipArr).intersection(set(checkIps)))
    if len(ipList) == 0:
        # print('家里没人')
        data = {"to": "1363333333", "msg": "没关煤气", "token": "xx"}
        rep = requests.post(url='http://noteapi.yoyolife.fun/api/sms/sent_sms', data=data)
        print(rep.json())
        return False
    else:
        return True
def on_message(client, userdata, msg):
    global arr,flag,sentMsgFlag
    data = msg.payload.decode().split("_")
    for index in range(1,len(data)-1):
        arr.append(data[index])
    if flag >= 7:
        maxTemp = float(max(arr))
        print(maxTemp)
        if maxTemp > 45:
            # print(maxTemp, flag)
            if checkIp() == False:
                if sentMsgFlag == 0:
                    print("家里没人 发短信")
                    sentMsgFlag += 1
                elif sentMsgFlag >= 60:
                    # 一分钟发一次短信
                    sentMsgFlag = 0
                else:
                    sentMsgFlag += 1
        arr = []
        flag = 0
    else:
        flag += 1

if __name__ == "__main__":
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect('t.haha.com', 1883, 600)  # 600为keepalive的时间间隔
    client.username_pw_set('username', password='yourpw')
    client.loop_forever()  # 保持连接
