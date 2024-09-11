#!/usr/bin/env python
# coding: utf-8
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

os.chdir("D://academic//match2")

step = 1000000
number = 1000
betas = [20, 40, 80, 160, 200]
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

'''
判断配对后的两对是否是不稳定对
不稳定的判断条件是同时满足：
pair1中的task1，相对于worker1，更喜欢pair2中的worker2，判断依据是worker2比worker1更近
pair2中的worker2，相对于task1，更喜欢pair1中的task1，判断依据是task1的price比task2更高

反之亦然
'''
def isUnstablePair(pair1, pair2):
    #只判断价钱高的那个task
    if pair1.task.price > pair2.task.price:
        if dist(pair1.worker, pair1.task) > dist(pair2.worker, pair1.task) and pair1.task.price > pair2.task.price:
            return True
    else:    
        if dist(pair2.worker, pair2.task) > dist(pair1.worker, pair2.task) and pair2.task.price > pair1.task.price:
            return True
    return False

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
def chain( number, P, O):
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

#P=workers, O=tasks
def fast_chain(number, P, O):
    starttime = datetime.datetime.now()
    dis_sum = 0

    #匹配结果
    A = [0] * number
    from collections import deque
    C = deque() #doubly-linkedlist
    while len(O) != 0:
        o = np.random.choice(O)
        C.append(o)

        while len(C) != 0:
            #x = C[-1] #取出C集合最后一个元素
            x = C[-1] #peek the last element
            if isinstance(x, Task):
                #y是P集合中距离x最近的元素
                y, d = NN(x, P)
                #判断y是否是集合C中x前面那个元素
                if len(C) > 1 and C[-2] == y:
                    #C.remove(x)
                    #C.remove(y)
                    C.pop()
                    C.pop()
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
                    #C.remove(x)
                    #C.remove(y)
                    C.pop()
                    C.pop()
                    A[y.id] = x
                    dis_sum += d
                    O.remove(y)
                    P.remove(x)
                else:
                    C.append(y)

    endtime = datetime.datetime.now()
    time_cost = endtime - starttime
    print (str(number) + "个人和车的模拟数据集，快速链式算法配对消耗总时间：" + str(time_cost) + "，总距离为：" + str(dis_sum))  
    return A, dis_sum 

#链表当前是车时，下个节点存入某一距离范围内的最高价乘客(task), 如果该乘客和倒数第二个乘客是同一个task则匹配
#链表当前是乘客时, 下个节点存入距离最近的车(work), 如果该车和倒数第二个车是同一work则匹配
#P=workers, O=tasks
# 问题：1.重复的y。2.输出的链
def stable_chain(number, P, O, threshold=100):
    starttime = datetime.datetime.now()
    dis_sum = 0

    #匹配结果
    A = [0]*number
    C = [] #TODO: 只需要操作C的tail可以使用deque
    chain=[]
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
                    chain.append(x.id)
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
                    chain.append([y.id])
                    dis_sum += d
                    O.remove(y)
                    P.remove(x)
                else:
                    C.append(y)

    endtime = datetime.datetime.now()
    time_cost = endtime - starttime
    print (str(number) + "个人和车的模拟数据集，稳定链式算法配对消耗总时间：" + str(time_cost) + "，总距离为：" + str(dis_sum))  
    return A, chain,dis_sum, time_cost


def basic(number, workers, tasks):
    workers_num = len(workers)
    tasks_num = len(tasks)

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

        #随机交换之后不再满足稳定，直接continue
        #if isUnstablePair(Pair(w1, t2), Pair(w2, t1)):
            #continue

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


def anneal2(number, match, tasks, chain, dist_sum_first):
    starttime = datetime.datetime.now()
    dis_sum_start = dist_sum_first
    n=len(chain)
    for m in range(len(chain) - 2):
        m1, m2, m3 = chain[m], chain[m + 1], chain[m + 2]
        t1, t2, t3 = tasks[m1], tasks[m2], tasks[m3]
        w1, w2, w3 = match[m1], match[m2], match[m3]
        l0 = dist(w1, t1) + dist(w2, t2) + dist(w3, t3)
        l12 = dist(w1, t2) + dist(w2, t1) + dist(w3, t3)
        l13 = dist(w1, t3) + dist(w2, t2) + dist(w3, t1)
        l23 = dist(w1, t1) + dist(w2, t3) + dist(w3, t2)
        l_min = min(l0, l12, l13, l23)

        if l_min == l0:
            continue
        elif l_min == l12:
            match[m1] = w2
            match[m2] = w1
            dist_sum_first = dist_sum_first - l0 + l12
            n-=1
        elif l_min == l13:
            match[m1] = w3
            match[m3] = w1
            dist_sum_first = dist_sum_first - l0 + l13
            n -= 1
        elif l_min == l23:
            match[m3] = w2
            match[m2] = w3
            dist_sum_first = dist_sum_first - l0 + l23
            n -= 1
    endtime = datetime.datetime.now()
    time_cost = endtime - starttime

    print(str(number) + "个人和车的模拟数据集，使用" + str(len(chain) - 2) + "步退火消耗总时间：" + str(time_cost) + "，退火后总距离为：" + str(
        dist_sum_first) + "，优化比例为：" + "{:.2%}".format(dist_sum_first / dis_sum_start),"，稳定比例为：" + "{:.2%}".format(n/len(chain)))
    return time_cost, dist_sum_first,round(dist_sum_first / dis_sum_start,2),round(n/len(chain),2)

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
    #zero-based，方便空间换时间索引
    
    workers_c = deepcopy(workers)
    tasks_c = deepcopy(tasks)
    workers_fc = deepcopy(workers)
    tasks_fc = deepcopy(tasks)
    workers_sc = deepcopy(workers)
    tasks_sc = deepcopy(tasks)
    #双层for循环基础算法
    match_result_1, dis_sum_1, time_cost_1 = basic(number, workers, tasks)
    #基于baseline出来的数据进行退火
    #anneal(number, match_result_1, workers, tasks, dis_sum_1)
    #链式算法
    #match_result_2, dis_sum_2 = chain(number, workers_c, tasks_c)
    #快速链式算法
    #match_result_3, dis_sum_3 = fast_chain(number, workers_fc, tasks_fc)
    #print('chain & fast-chain distances: {},{}'.format(dis_sum_2, dis_sum_3))
    #稳定链式算法
    match_result_4, chain_4,dis_sum_4, time_cost_4 = stable_chain(number, workers_sc, tasks_sc)
    #基于chain出来的数据进行退火
    #anneal(number, match_result_2, workers, tasks, dis_sum_2)
    d=0
    for i in tasks:
        d+=dist(i,match_result_4[i.id])
    print(d)
    return dis_sum_1, dis_sum_4, time_cost_1, time_cost_4

def statistic():
    basic_dists = []
    stable_chain_dists = []
    basic_times = []
    stable_chain_times = []
    nums = [1000,2000,3000,4000,5000,6000,7000,8000,9000,10000,11000,12000,13000,14000,15000]
    for n in [1000]:
        basic_dist, stable_chain_dist, basic_time, stable_chain_time = main(n)
        basic_dists.append(basic_dist)
        basic_times.append(basic_time.seconds)
        stable_chain_dists.append(stable_chain_dist)
        stable_chain_times.append(stable_chain_time.seconds)
    import pandas as pd
    result1 = pd.DataFrame(index=nums, data={'basic': basic_dists, 'stable-chain': stable_chain_dists})
    result2 = pd.DataFrame(index=nums, data={'basic': basic_times, 'stable-chain': stable_chain_times})
    # result1.to_parquet('result1.parq')
    # result2.to_parquet('result2.parq')
    
    plt.figure()
    plt.plot(result1)
    plt.legend(['basic','stable-chain'])
    plt.title("basic & stable-chain distance comparation")
    plt.savefig("result1.png")
    
    plt.figure()
    plt.plot(result2)
    plt.legend(['basic','stable-chain'])
    plt.title("basic & stable-chain time(seconds) comparation")
    plt.savefig("result2.png")

def find_win_set(number, sample=5):
    tasks, workers = load_dataset(number) 
    while True:
        import random
        sample_workers = random.sample(workers, k=sample)
        sample_tasks = random.sample(tasks, k=sample)
        for idx in range(sample):
            sample_workers[idx].id = idx
            sample_tasks[idx].id = idx 
        sample_workers_sc = deepcopy(sample_workers)
        sample_tasks_sc = deepcopy(sample_tasks)
        workers_copy = deepcopy(sample_workers)
        tasks_copy = deepcopy(sample_tasks)
        #双层for循环基础算法
        match_result_1, dis_sum_1, time_cost_1 = basic(sample, sample_workers, sample_tasks)
        #稳定链式算法
        match_result_2, dis_sum_2, time_cost_2 = stable_chain(sample, sample_workers_sc, sample_tasks_sc)
        if dis_sum_1 > dis_sum_2*2:
            print('workers:', workers_copy)
            print('tasks:', tasks_copy)
            print('basic match:', match_result_1)
            print('stable chain match:', match_result_2)
            #print(sum([dist(tasks_copy[idx], match_result_1[idx]) for idx in range(sample)]))
            #print(sum([dist(tasks_copy[idx], match_result_2[idx]) for idx in range(sample)]))
            break

def find_threshold(number):
    import math
    #score = stableCount/number / totalDistance - log(time)
    tasks, workers = load_dataset(number)
    def score(threshold):
        match, dis_sum, time_cost = stable_chain(number, deepcopy(workers), deepcopy(tasks), threshold)
        unstable = count_unstable(match, tasks, workers)
        s = (number - unstable) / dis_sum - math.log(time_cost.microseconds)
        print('score:{}, threshold:{}, unsable:{}'.format(s, threshold, unstable))
        return -s
    #from scipy.optimize import minimize
    #return minimize(score, x0=100)
    return {threshold: -score(threshold) for threshold in range(0, 200, 1)}

def count_unstable(match, tasks, workers):
    #原始match: task.id -> worker
    #调整match: task.id -> worker.id
    match = [w.id for w in match]
    w2t = [-1] * len(match)
    for t in range(len(tasks)):
        w2t[match[t]] = t
    count = 0
    for t in tqdm.tqdm(range(len(tasks))):
        #t和新的w配对判断是否比t和match[t]配对更满意
        for w in range(len(workers)):
            if (dist(tasks[t], workers[match[t]]) > dist(tasks[t], workers[w])) and (tasks[t].price > tasks[w2t[w]].price):
                count += 1
                break
    return count


# if __name__ == '__main__':
    # argv = len(sys.argv)
    # if argv >= 3:
    #     step = int(sys.argv[2])
    # if argv >= 2:
    #     number = int(sys.argv[1])

main(number)
# statistic()
#find_win_set(number)
# r = find_threshold(number)
# import pandas as pd
# df = pd.DataFrame.from_dict(r, orient='index', columns=['score'])
# df.to_parquet('threshold.parq')