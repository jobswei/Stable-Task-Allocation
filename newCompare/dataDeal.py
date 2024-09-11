import pickle
import pandas as pd
import matplotlib.pyplot as plt
import os
os.chdir("D://About_coding/py/car/newCompare")


with open("results/alg04_dist.pkl","rb") as fp:
    Distance=pickle.load(fp)
with open("results/alg04_runtime.pkl","rb") as fp:
    Runtime=pickle.load(fp)

# 将数据转换为数据框
df = pd.DataFrame(Distance)

# 将数据框写入Excel文件
writer = pd.ExcelWriter('results/alg04_dist.xlsx')
df.to_excel(writer,'Sheet1',index=False)
writer.save()

# 将数据转换为数据框
df = pd.DataFrame(Runtime)

# 将数据框写入Excel文件
writer = pd.ExcelWriter('results/alg04_runtime.xlsx')
df.to_excel(writer,'Sheet1',index=False)
writer.save()

print("数据已写入Excel文件")