import multiprocessing
import os
from concurrent.futures import ThreadPoolExecutor

def inst0():
    os.system('./ccehc -r ../data/wscpc/test.txt >> ../log/wlog.txt')
def inst1():
    os.system('./ccehc -r ../data/wscpc/maxcat.cnf >> ../log/wlog.txt')
def inst2():
    os.system('./ccehc -r ../data/wscpc/20000/100000_0.2_500_1.txt >> ../log/wlog.txt')
def inst3():
    os.system('./ccehc -r ../data/wscpc/20000/100000_0.1_500_1.txt >> ../log/wlog.txt')
def inst4():
    os.system('./ccehc -r ../data/wscpc/20000/100000_0.15_500_100.txt >> ../log/wlog.txt')
def inst5():
    os.system('./ccehc -r ../data/wscpc/20000/100000_0.05_500_100.txt >> ../log/wlog.txt')
def inst6():
    os.system('./ccehc -r ../data/wscpc/20000/100000_0.2_500_10000.txt >> ../log/wlog.txt')
def inst7():
    os.system('./ccehc -r ../data/wscpc/20000/100000_0.1_500_100.txt >> ../log/wlog.txt')
def inst8():
    os.system('./ccehc -r ../data/wscpc/20000/100000_0.2_500_100.txt >> ../log/wlog.txt')
def inst9():
    os.system('./ccehc -r ../data/wscpc/20000/100000_0.05_500_10000.txt >> ../log/wlog.txt')
def inst10():
    os.system('./ccehc -r ../data/wscpc/20000/100000_0.15_500_10000.txt >> ../log/wlog.txt')
def inst11():
    os.system('./ccehc -r ../data/wscpc/20000/100000_0.05_500_1.txt >> ../log/wlog.txt')
def inst12():
    os.system('./ccehc -r ../data/wscpc/20000/100000_0.1_500_10000.txt >> ../log/wlog.txt')
def inst13():
    os.system('./ccehc -r ../data/wscpc/20000/100000_0.15_500_1.txt >> ../log/wlog.txt')
def inst14():
    os.system('./ccehc -r ../data/wscpc/2000/10000_0.2_50_100.txt >> ../log/wlog.txt')
def inst15():
    os.system('./ccehc -r ../data/wscpc/2000/10000_0.05_50_1.txt >> ../log/wlog.txt')
def inst16():
    os.system('./ccehc -r ../data/wscpc/2000/10000_0.05_50_100.txt >> ../log/wlog.txt')
def inst17():
    os.system('./ccehc -r ../data/wscpc/2000/10000_0.15_50_100.txt >> ../log/wlog.txt')
def inst18():
    os.system('./ccehc -r ../data/wscpc/2000/10000_0.1_50_100.txt >> ../log/wlog.txt')
def inst19():
    os.system('./ccehc -r ../data/wscpc/2000/10000_0.15_50_10000.txt >> ../log/wlog.txt')
def inst20():
    os.system('./ccehc -r ../data/wscpc/2000/10000_0.2_50_1.txt >> ../log/wlog.txt')
def inst21():
    os.system('./ccehc -r ../data/wscpc/2000/10000_0.15_50_1.txt >> ../log/wlog.txt')
def inst22():
    os.system('./ccehc -r ../data/wscpc/2000/10000_0.05_50_10000.txt >> ../log/wlog.txt')
def inst23():
    os.system('./ccehc -r ../data/wscpc/2000/10000_0.2_50_10000.txt >> ../log/wlog.txt')
def inst24():
    os.system('./ccehc -r ../data/wscpc/2000/10000_0.1_50_1.txt >> ../log/wlog.txt')
def inst25():
    os.system('./ccehc -r ../data/wscpc/2000/10000_0.1_50_10000.txt >> ../log/wlog.txt')

pool = ThreadPoolExecutor(max_workers=10)

pool.submit(inst0)
pool.submit(inst1)
pool.submit(inst2)
pool.submit(inst3)
pool.submit(inst4)
pool.submit(inst5)
pool.submit(inst6)
pool.submit(inst7)
pool.submit(inst8)
pool.submit(inst9)
pool.submit(inst10)
pool.submit(inst11)
pool.submit(inst12)
pool.submit(inst13)
pool.submit(inst14)
pool.submit(inst15)
pool.submit(inst16)
pool.submit(inst17)
pool.submit(inst18)
pool.submit(inst19)
pool.submit(inst20)
pool.submit(inst21)
pool.submit(inst22)
pool.submit(inst23)
pool.submit(inst24)
pool.submit(inst25)

pool.shutdown()
