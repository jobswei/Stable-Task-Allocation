import os
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
    
def load_dataset(number:int)->list:
    os.chdir("D://About_coding/py/car/newCompare")
    workers = []
    tasks = []
    idx = 0
    with open("./data/worker_" + str(number) + ".txt", "r") as f:
        while True:
            try:
                line = f.readline().split()
                w = Worker(idx, float(line[0]), float(line[1]))
                workers.append(w)
                idx += 1
            except:
                break
    idx = 0
    with open("./data/task_" + str(number) + ".txt", "r") as f:
        while True:
            try:
                line = f.readline().split()
                t = Task(idx, float(line[0]), float(line[1]), float(line[2]))
                tasks.append(t)
                idx += 1
            except:
                break
    return tasks, workers

def dist(worker, task):
    r2 = (worker.x - task.x) ** 2 + (worker.y - task.y) ** 2
    r = pow(r2, 0.5)
    return r

def toSeconds(time)->float:
    h,m,s = str(time).strip().split(":")
    return int(h) * 3600 + int(m) * 60 + float(s)
