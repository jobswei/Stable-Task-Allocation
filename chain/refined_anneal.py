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

os.chdir("./data")

parser = argparse.ArgumentParser()
parser.add_argument('--number', default=1000, type=int)
parser.add_argument('--step', default=1000000, type=int)
parser.add_argument('--percent', default=0.9, type=float)


step = 1000000
number = 10
betas = [20, 40, 80, 160, 200]
dis_cache = None
percent = 0.9

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

'''
判断配对后的两对是否是不稳定对
不稳定的判断条件是同时满足：
pair1中的task1，相对于worker1，更喜欢pair2中的worker2，判断依据是worker2比worker1更近
pair2中的worker2，相对于task1，更喜欢pair1中的task1，判断依据是task1的price比task2更高

反之亦然
'''
def is_unstable(pair1, pair2):
    if dist(pair1.worker, pair1.task) > dist(pair2.worker, pair1.task) and pair1.task.price > pair2.task.price:
        return True
    if dist(pair2.worker, pair2.task) > dist(pair1.worker, pair2.task) and pair2.task.price > pair1.task.price:
        return True
    return False

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

def anneal(number, match, workers, tasks, dis_sum_start):
    starttime = datetime.datetime.now()

    H2 = H1 = dis_sum_start
    for i in tqdm.tqdm(range(step)):
        beta = betas[i // 200000]
        t1 = tasks[np.random.randint(0, number)]
        t2 = tasks[np.random.randint(0, number)] 
        w1 = match[t1.id]
        w2 = match[t2.id]

        #随机交换之后不再满足稳定，以一定概率continue
        if is_unstable(Pair(w1, t2), Pair(w2, t1)):
            if np.random.rand() < percent:
                continue

        H2 = H1 - dist(w2, t2) + dist(w2, t1) + dist(w1, t2) - dist(w1, t1)
        if H1 - H2 > 0:
            H1 = H2
            match[t1.id] = w2
            match[t2.id] = w1
        elif rand() < np.exp(-beta * H1):
            H1 = H2
            match[t1.id] = w2
            match[t2.id] = w1
    
    endtime = datetime.datetime.now()
    time_cost = endtime - starttime
    print (str(number) + "个人和车的模拟数据集，使用" + str(step) + "步退火消耗总时间：" + str(time_cost) + "，退火后总距离为：" + str(H1) + "，优化比例为：" + "{:.2%}".format(H1 / dis_sum_start))

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
    #基于baseline出来的数据进行退火
    anneal(number, match_result_1, deepcopy(workers), deepcopy(tasks), dis_sum_1)

if __name__ == '__main__':
    args = parser.parse_args()
    step = args.step
    number = args.number
    percent = args.percent
    main(number)