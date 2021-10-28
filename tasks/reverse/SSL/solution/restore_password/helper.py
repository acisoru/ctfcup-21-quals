import itertools
from hashlib import md5
import time

a = [['C','q'],
['U'],
['P'],
['G', '{'],
['B', '2'],
['8'],
['Z', 'i'],
['R'],
['A'],
['y'],
['t'],
['W'],
['j'],
['r'],
['e'],
['8'],
['A'],
['U'],
['b'],
['0'],
['y'],
['W'],
['z', 'F'],
['u'],
['L'],
['J'],
['I'],
['K'],
['A'],
['H'],
['k','w'],
['l'],
['Y'],
['X'],
['y'],
['s'],
['}']]

start = time.time()

for i in list(itertools.product(*a)):
    if md5(''.join(i).encode()).hexdigest() == "56361cb66d9cb9f61fb8ab6440df5e32":
        print(''.join(i))
        break 
print("time: {}".format(time.time() - start))