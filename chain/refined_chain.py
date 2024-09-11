#!/usr/bin/env python
# coding: utf-8
import argparse
from re import match
import numpy as np
from numpy.random import rand
import matplotlib.pyplot as plt
import os, sys
import random
import datetime
import tqdm
from copy import deepcopy

import matplotlib.pyplot as plt

os.chdir("D://About_coding/py/car/chain/data")

parser = argparse.ArgumentParser()
parser.add_argument('--number', default=1000, type=int)

dis_cache = None

#给乘客和车加各上一个id字段，方便索引和debug
class Worker:
    def __init__(self, id, x, y):
        self.id = id
        self.x = x
        self.y = y
    
    def __repr__(self) -> str:
        return '({},{})'.format(self.x, self.y)

class Task:
    def __init__(self, id, x, y, price):
        self.id = id
        self.x = x
        self.y = y
        self.price = price

    def __repr__(self) -> str:
        return '({},{}),{}'.format(self.x, self.y, self.price)

class Pair:
    def __init__(self, worker, task):
        self.worker = worker
        self.task = task

def dist(worker, task, cache=False):
    #空间换时间，理论上basic那次O(N^2)的循环里面这里都算了一遍，后面退火不用再算了
    if cache:
        r = dis_cache[worker.id][task.id]
        if r != -1:
            return r
    r2 = (worker.x - task.x) ** 2 + (worker.y - task.y) ** 2
    r = pow(r2, 0.5)
    if cache:
        dis_cache[worker.id][task.id] = r
    return r

#获取集合中距离某元素最近的一个值
def NN(element, set):
    y = None
    d = 0
    for p in set:
        d1 = dist(p, element)
        if d == 0 or d1 < d:
            d = d1
            y = p
    return y, d

#距离<=threshold的出价最高的task
def highest(w, tasks, threshold=100):
    best_task = None
    price, d = 0, 0
    for t in tasks:
        d1 = dist(t, w)
        if d1 <= threshold:
            if t.price > price:
                best_task = t
                price = t.price
                d = d1
    if best_task == None: #指定距离内无task时，返回最近task
        return NN(w, tasks)
    return best_task, d

#参考《On Efficient Spatial Matching.pdf》第五页
#P=workers, O=tasks
def chain(number, P, O):
    starttime = datetime.datetime.now()
    dis_sum = 0

    #匹配结果
    A = [0 for i in range(number)]
    C = []
    while len(O) != 0:
        o = np.random.choice(O)
        C.append(o)

        while len(C) != 0:
            x = C[-1] #取出C集合最后一个元素
            if x in O:
                #y是P集合中距离x最近的元素
                y, d = NN(x, P)
                #判断y是否是集合C中x前面那个元素
                if len(C) > 1 and C[-2] == y:
                    C.remove(x)
                    C.remove(y)
                    A[x.id] = y
                    dis_sum += d
                    O.remove(x)
                    P.remove(y)
                else:
                    C.append(y)
            else:
                #y是O集合中距离x最近的元素
                y, d = NN(x, O)
                #判断y是否是集合C中x前面那个元素
                if len(C) > 1 and C[-2] == y:
                    C.remove(x)
                    C.remove(y)
                    A[y.id] = x
                    dis_sum += d
                    O.remove(y)
                    P.remove(x)
                else:
                    C.append(y)

    endtime = datetime.datetime.now()
    time_cost = endtime - starttime
    print (str(number) + "个人和车的模拟数据集，链式算法配对消耗总时间：" + str(time_cost) + "，总距离为：" + str(dis_sum))  
    return A, dis_sum 

#链表当前是车时，下个节点存入某一距离范围内的最高价乘客(task), 如果该乘客和倒数第二个乘客是同一个task则匹配
#链表当前是乘客时, 下个节点存入距离最近的车(work), 如果该车和倒数第二个车是同一work则匹配
#P=workers, O=tasks
def stable_chain(number, P, O, threshold=100):
    starttime = datetime.datetime.now()
    dis_sum = 0

    #匹配结果
    A = [0]*number
    C = [] #TODO: 只需要操作C的tail可以使用deque
    while len(O) != 0:
        o = np.random.choice(O)
        C.append(o)

        while len(C) != 0:
            x = C[-1] #取出C集合最后一个元素
            if isinstance(x, Task):
                #y是P集合中距离x最近的元素
                y, d = NN(x, P)
                #判断y是否是集合C中x前面那个元素
                if len(C) > 1 and C[-2] == y:
                    C.remove(x)
                    C.remove(y)
                    A[x.id] = y
                    dis_sum += d
                    O.remove(x)
                    P.remove(y)
                else:
                    C.append(y)
            else:
                #y是O集合中距离小于一定值的出价最高的task
                y, d = highest(x, O, threshold)
                #判断y是否是集合C中x前面那个元素
                if len(C) > 1 and C[-2] == y:
                    C.remove(x)
                    C.remove(y)
                    A[y.id] = x
                    dis_sum += d
                    O.remove(y)
                    P.remove(x)
                else:
                    C.append(y)

    endtime = datetime.datetime.now()
    time_cost = endtime - starttime
    print (str(number) + "个人和车的模拟数据集，稳定链式算法配对消耗总时间：" + str(time_cost) + "，总距离为：" + str(dis_sum))  
    return A, dis_sum, time_cost

def basic(number, workers, tasks):
    match = []

    def price(t):
        return t.price

    tasks.sort(key = price, reverse = True)
    match = [None for i in range(number)]

    starttime = datetime.datetime.now()
    dis_sum = 0
    for t in tqdm.tqdm(tasks):
        w_nearest = None
        d = 0
        for w in workers:
            d1 = dist(w, t)
            if d == 0 or d1 < d:
                d = d1
                w_nearest = w
        match[t.id] = w_nearest
        workers.remove(w_nearest)
        dis_sum += d

    endtime = datetime.datetime.now()
    time_cost = endtime - starttime
    print (str(number) + "个人和车的模拟数据集，传统双层循环配对消耗总时间：" + str(time_cost) + "，总距离为：" + str(dis_sum))    
    return match, dis_sum, time_cost

def load_dataset(number):
    workers = []
    tasks = []
    idx = 0
    with open("worker_" + str(number) + ".txt", "r") as f:
        while True:
            try:
                line = f.readline().split()
                w = Worker(idx, float(line[0]), float(line[1]))
                workers.append(w)
                idx += 1
            except:
                break
    idx = 0
    with open("task_" + str(number) + ".txt", "r") as f:
        while True:
            try:
                line = f.readline().split()
                t = Task(idx, float(line[0]), float(line[1]), float(line[2]))
                tasks.append(t)
                idx += 1
            except:
                break
    return tasks, workers

def main(number):
    global dis_cache
    dis_cache =[[-1 for i in range(number)] for j in range(number)]
    tasks, workers = load_dataset(number)
    #双层for循环基础算法
    match_result_1, dis_sum_1, time_cost_1 = basic(number, deepcopy(workers), deepcopy(tasks))
    #链式算法
    match_result_2, dis_sum_2 = chain(number, deepcopy(workers), deepcopy(tasks))
    #稳定链式算法
    match_result_4, dis_sum_4, time_cost_4 = stable_chain(number, deepcopy(workers), deepcopy(tasks))

if __name__ == '__main__':
    args = parser.parse_args()
    main(args.number)