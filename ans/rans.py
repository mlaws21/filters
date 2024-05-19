#C(s, x) = mbx/lsc + bs + mod(x, ls) D(x) = (s, lsbx/mc + mod(x, m) − bs) where s = s(mod(x, m))

import numpy as np
# m = num_elements
# ls is the emperical number of appearences of the ith number in the series
# bs is sum of all ls from 0, s - 1

# dont pass s bc we do alr
def encode(state, ls, m, bs):
    # (x//freq[s])*M + cumul[s] + x%freq[s]
    return m * (state // ls) + bs + (state % ls)

#should be tabled
def s_func(x, bs, m):
    for i, ele in enumerate((bs[1:] + [m])):
        # print((bs[1:] + [m]))
        # print((bs))
        
        # print(i, ele)
        if x < ele:
            return i

def decode(state, ls_list, m, bs_list):
    s = s_func(state % m, bs_list, m)
    state = ls_list[s] * (state // m) + (state % m) - bs_list[s]
    return s, state

def decode_series(state, ls: np.array, m: int, c_freqs):
    
    series = []
    while state > 0:
        s, state = decode(state, ls, m, c_freqs)
        series.append(s)
        # print(state)
    return series


def encode_series(series: list, ls: np.array, m: int, c_freqs):
    state = 0
    for i in series:
        state = encode(state, ls[i], m, c_freqs[i])
        # print(state)
    return state

def main():
    
    N = 16
    generic_freqs = (6, 5, 3, 1, 1)
    freqs = np.random.multinomial(N, (0.33, 0.33, 0.2, 0.1, 0.04), size=None)
    nums = []
    for i, ele in enumerate(freqs):
        nums.extend([i] * ele)
    
    np.random.shuffle(nums)
    # print(nums)
    # cnums = [x + (sum(nums[0:i-1]) * int(i > 0)) for i, x in enumerate(nums)]
    # bs = []
    # bsp1 = []
    # sofar = 0
    # for i in nums:
    #     bs.append(sofar)
    #     bsp1.append(i + sofar)
        
        
    #     sofar += i
    # print(bs)
    # print(bsp1)
    
    c_freqs = []
    sofar = 0
    for i in generic_freqs:
        c_freqs.append(sofar)
        sofar += i
    print(generic_freqs, c_freqs)
    encoded = encode_series(nums, generic_freqs, len(nums), c_freqs)
    print(encoded)
    out = decode_series(encoded, generic_freqs, len(nums), c_freqs)
    out += [0] * (N - len(out))
    out.reverse()
    print(out)
    print(nums)
    print(nums == out)
    # print("encoded:", encoded)

if __name__ == "__main__":
    main()