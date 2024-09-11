from algorithm import *
from tools import *
import pickle
import os
import tqdm

os.chdir("D://About_coding/py/car/newCompare")
if not os.path.exists("results"):
    os.mkdir("results")
Distance=[]
Runtime=[]
for num in [1000]:
    print("Num= ",num," begin!")
    tasks,workers=load_dataset(num)

    match1=Algorithm04()
    pairs=match1.run(tasks,workers)
    distance=match1.getDist()
    runtime=match1.getRuntime()
    print("Total distance: ",distance)
    print("Total runtime: ",runtime)
    Distance.append(distance)
    Runtime.append(runtime)

    with open("results/alg04_dist.pkl","wb") as fp:
        pickle.dump(Distance,fp)
    with open("results/alg04_runtime.pkl","wb") as fp:
        pickle.dump(Runtime,fp)
