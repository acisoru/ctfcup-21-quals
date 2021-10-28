from hashlib import md5
import itertools
import time

a = [['C'],
    ['U'],
    ['P'],
    ['{'],
    ['%','1'],
    ['n','q'],
    ['+','C'],
    ['R','X'],
    ['b','e',],
    ['^','d'],
    ['%','1'],
    ['b','e'],
    ['l'],
    ['b','e'],
    ['+','C'],
    ['R','X'],
    ['b','e'],
    ['v','|'],
    ['b','e'],
    ['r'],
    ['2','5'],
    ['+','C'],
    ['T'],
    ['.','4'],
    ['2','5'],
    ['h','k'],
    ['}']
]

start = time.time()

for i in list(itertools.product(*a)):
    if md5(''.join(i).encode()).hexdigest() == "9db929bc4639cff9aef5b3850322d29b":
        print(''.join(i))
        break 
print("time: {}".format(time.time() - start))