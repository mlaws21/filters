import numpy as np
from math import log2

def make_table(probs, L, cycles=10000):
    table = []
    counter_probs = [0] * len(probs)
    
    for c in range(1, cycles + 1):
        cp_index = 0
        for i in range(L):
            while counter_probs[cp_index] >= probs[cp_index] * c:
                # print(cp_index, counter_probs)
                cp_index = (cp_index + 1) % len(probs)
            col = [-1] * len(probs)
            col[cp_index] = counter_probs[cp_index]
            counter_probs[cp_index] += 1
            table.append(col)
            cp_index = (cp_index + 1) % len(probs)
        
    
    return table

def make_stream_table(probs, L):
    table = []
    counter_probs = [0] * len(probs)

        
    
    return table
        
def tprint(table, num_rows):
    rows = [""] * num_rows
    rows[0] += "C(x, s) | "
    for i in range(0, len(table)):
        rows[0] +=  str(i) + " | "
    for i in range(1, num_rows):
        rows[i] += f"  s = {i-1} | "
    for i, col in enumerate(table):
        builtin = (int((max(col) ** 1/10)) + 1)
        for j, ele in enumerate([str(x) if x != -1 else " " * (builtin) for x in col]):
            fill = " " * (int(i ** 1/10) - builtin)
            rows[j+ 1] += f"{ele}{fill} | "
            
    for r in rows:
        print(r)


def stream_encode(nums, table):
    

def encode(nums, table):
    x = 0

    
    for s in nums:
        # print("STARTING", s)
        
        found = False
        for i, ele in enumerate(table):
            # print(ele[s], x)
            if ele[s] == x:
                x = i
                found = True
                # print("found")
                break
                
        if found == False:
            print("error")
        # x = 
    return x


def decode(x, table):
    out = []
    while x > 0:
        col = table[x]
        x = max(col)
        out.append(col.index(x))
        
    return out

def nbits(n):
    if n == 0:
        return 1
    return int(log2(n)) + 1

def main():
    # probs = [23, 24, 12, 5] # 1/(e * n!)
    # L = 64
    
    probs = [1, 3, 6]
    real_probs = [.1, .3, .6]
    
    L = 10
    
    freqs = np.random.multinomial(L, real_probs, size=None)
    nums = []
    for i, ele in enumerate(freqs):
        nums.extend([i] * ele)
    
    np.random.shuffle(nums)
    print(nums)
    t = make_table(probs, L)
    # for i, ele in enumerate(t):
    #     print(i, ele)
    # print(enumerate(t))
    # tprint(t, len(probs) + 1)
    seq = nums
    print("start:", seq)
    e = encode(seq, t)
    print("encoded:", e)
    d = decode(e, t)
    d.reverse()
    print("decoded:", d)
    print("min bits:", sum([nbits(x) for x in seq]))
    print("used bits:", nbits(e))
    

if __name__ == "__main__":
    main()