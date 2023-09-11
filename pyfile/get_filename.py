# -*- coding: utf-8 -*-
import os

folder_path = "../data/wscpc" # 替换成你要获取的文件夹路径

# 获取该文件夹下的所有文件名（包括子文件夹中的文件名）
file_names = []
for root, dirs, files in os.walk(folder_path):
    for filename in files:
        file_names.append(os.path.join(root, filename))

# print(file_names)

# 将获取到的文件名写入 run 文件中
count = 0
with open("pool.py", "w") as f:
    f.write("import multiprocessing" "\n")
    f.write("import os" "\n")
    f.write("from concurrent.futures import ThreadPoolExecutor" "\n" "\n")

    for name in file_names:
        f.write("def inst" + str(count) + "():" "\n")
        f.write("    os.system('./ccehc -r " + name + " >> ../log/wlog.txt')" "\n")
        count += 1
    
    f.write("\n")
    f.write("pool = ThreadPoolExecutor(max_workers=10)" "\n" "\n")
    for index in range(0, count):
        f.write("pool.submit(inst" + str(index) + ")" "\n")
    
    f.write("\n" "pool.shutdown()" "\n")