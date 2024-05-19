import sys
import random
import statistics as st
import math
import numpy as np 
random.seed(3)

def place_balls_in_bins(n, b):

    bins = [0] * b
    
    for _ in range(n):
        bin_num = random.randint(0, b - 1)
        bins[bin_num] += 1
    
    # print(bins)
    
    return bins

def compute_binomial_pmf(k, n=64, b=64):
    return math.comb(n, k) * (1/b)**k * (1 - (1/b))**(n-k)
    



def main():
    # all_bins = [print(place_balls_in_bins(64, 64))]
    
    [print(x, compute_binomial_pmf(x)) for x in range(100)]


    
if __name__ == "__main__":
    main()