import sys
import random
import statistics as st
import math
import numpy as np 
random.seed(2)

def place_balls_in_bins(n, b, s):

    bins = [0] * b
    
    for _ in range(n):
        bin_num = random.randint(0, b - 1)
        bins[bin_num] += 1
    
    # print(bins)
    overflowing = [x > s for x in bins]
    
    return sum(overflowing) / b

def place_balls_in_bins_p2_theory(n, b, s, p=2):

    bins = [0] * b
    
    for _ in range(n):
        
        bin_nums = [random.randint(0, b - 1) for _ in range(p)]
        potentials = [bins[i] for i in bin_nums]
        
        to_add_to = np.argmin(potentials)
        # print(bin_nums, bin_nums[to_add_to])
        bins[bin_nums[to_add_to]] += 1
    
    # print(bins)
    overflowing = [x > s for x in bins]
    
    return sum(overflowing) / b

def place_balls_in_bins_p2_practice(n, b, s, p=2):

    bins = [0] * b
    
    for _ in range(n):
        start = random.randint(0, b - 1)
        bin_nums = list(range(start, min(b, start + p)))
        potentials = [bins[i] for i in bin_nums]
        
        to_add_to = np.argmin(potentials)
        # print(bin_nums, bin_nums[to_add_to])
        bins[bin_nums[to_add_to]] += 1
    
    # print(bins)
    overflowing = [x > s for x in bins]
    
    return sum(overflowing) / b
      
# need to make this more efficient
def find_valid_bins(n, b, s, current_placement, placements):
    if n == 0:  # All balls are placed
        placements.append(list(current_placement))
        return

    if b == 0 or n > b * s:  # No more buckets or not enough buckets to place remaining balls
        return

    for balls_in_bucket in range(min(n, s) + 1):  # Allow empty buckets
        current_placement.append(balls_in_bucket)
        # sofar[(balls_in_bucket, )]
        find_valid_bins(n - balls_in_bucket, b - 1, s, current_placement, placements)
        current_placement.pop()
        

def one_step_of_summation(n, l):
    return math.factorial(n) / np.prod([math.factorial(x) for x in l])
    
def full_summation(valid_bins, n, b):
    my_sum = sum([one_step_of_summation(n, x) for x in valid_bins])
    return my_sum / (b**n)

def compute_binomial_cmf(n, b, s):
    def inner(i):
        return math.comb(n, i) * (1/b)**i * (1 - (1/b))**(n-i)
    return 1- sum([inner(x) for x in range(0, s+1)])

REPS = 10000
P = 2
   
def main():
    n = int(sys.argv[1]) # items 
    s = int(sys.argv[3]) # bin size (i.e threshold)
    # s = math.floor(math.log(n))
    # b = int(sys.argv[2]) # bins b >= n/s
    b = math.floor((n / s) * 1.05)

    print("n:", n, "b:", b, "s:", s)
    
    
    trials = [place_balls_in_bins(n, b, s) for _ in range(REPS)]
    
    print("Emperical P(no bucket overflow)",  1 - (sum([1 for x in trials if x > 0]) / REPS))
    print("Emperical P(any given bucket overflows)", st.mean(trials))
    

    trials_p2 = [place_balls_in_bins_p2_theory(n, b, s, p=P) for _ in range(REPS)]

    
    print("P2 (Theory) Emperical P(no bucket overflow)",  1 - (sum([1 for x in trials_p2 if x > 0]) / REPS))
    print("P2 (Theory) Emperical P(any given bucket overflows)", st.mean(trials_p2))
    
    trials_p2_practice = [place_balls_in_bins_p2_practice(n, b, s, p=P) for _ in range(REPS)]

    
    print("P2 (Practice) Emperical P(no bucket overflow)",  1 - (sum([1 for x in trials_p2_practice if x > 0]) / REPS))
    print("P2 (Practice) Emperical P(any given bucket overflows)", st.mean(trials_p2_practice))
    
    print("Mathematical P(any given bucket overflows)", compute_binomial_cmf(n,b,s))
    
    
    # placements = []
    # find_valid_bins(n, b, s, [], placements)
    # padded = [x + ([0] * (b - len(x))) for x in placements]
    # # print("bins found")

    # out = full_summation(placements, n, b)
    
    # print("Mathematic P(no bucket overflow)", out)
    
if __name__ == "__main__":
    main()