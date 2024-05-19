#include <bits/stdc++.h>
#include <iostream>
#include <random>
#include <cmath>

#include <iomanip> // for std::setprecision
#include <sstream> // for std::ostringstream

uint64_t typedef u64;

struct {
    float lower;
    float upper;
} typedef bounds;

using namespace std;

double binomial_cmf(int n, int k, double p) {
    double q = 1.0 - p; // Probability of failure
    double cdf = 0.0;
    for (int i = 0; i <= k; ++i) {
        double binomial_coeff = std::exp(std::lgammal(n + 1) - std::lgammal(i + 1) - std::lgammal(n - i + 1));
        cdf += binomial_coeff * std::pow(p, i) * std::pow(q, n - i);
    }
    return cdf;
}

void printBits(u64 number) {
    for (int i = 63; i >= 0; i--) {
        cout << ((number >> i) & 1);
    }
    cout << endl;
}

u64 arithmetic_encode(float* pmf, float* cmf, int* numbers, int num_nums) {
    
    float encode = 0;
    float curr_base_prob = 1.0;
    for (int i = 0; i < num_nums; i++) {
        
        int current_num = numbers[i];
        encode += curr_base_prob * cmf[current_num];
        curr_base_prob *= pmf[current_num];
        // cout << encode << endl;

    }

    bounds out = {};
    out.lower = encode;
    out.upper = encode + curr_base_prob;
    // cout << out.lower << ", " << out.upper << endl;

    float binary_spot = 0.5;
    float binary_base_prob = 0.25;
    u64 inserter = 1;
    u64 binary_out = 0;
    while (binary_spot > out.upper || (binary_spot) < out.lower) {
    // for (int i = 0; i < 4; i++) {
        // cout << binary_spot << endl;
        if (binary_spot < out.lower) {
            binary_spot += binary_base_prob;
            binary_out |= inserter;
        } else {
            binary_spot -= binary_base_prob;
        }

        inserter <<= 1;
        binary_base_prob *= 0.5;

    }

    // now the number is in the range
    
    while (binary_spot + binary_base_prob > out.upper || (binary_spot) < out.lower) {
        // cout << "here" << endl;
        // cout << binary_spot << endl;
        if (binary_spot - out.lower < out.upper - binary_spot) {
            binary_spot += binary_base_prob;
            binary_out |= inserter;
        } else {
            binary_spot -= binary_base_prob;
        }

        inserter <<= 1;
        binary_base_prob *= 0.5;

    }

    // take the bottom of this interval
    inserter <<= 1;

    // && (binary_spot - out.lower > out.upper - binary_spot)
    printBits(inserter);
    return binary_out;
}



// return math.comb(n, k) * (1/b)**k * (1 - (1/b))**(n-k)
int main(int argc, char* argv[]) {
    int num_balls = 64;
    // float max_balls_float = log2(num_balls) / log2(log2(num_balls));
    // int max_balls = (int)ceil(max_balls_float);
    int num_possible = 10;
    float* probs = (float*)malloc(sizeof(float) * num_possible);
    for (int i = 0; i < num_possible; i++) {
        probs[i] = binomial_cmf(num_balls, i - 1, 1.0/num_balls);
        // cout << probs[i] << endl;
        // need pmf and cdf
    }

    // float pmf[4] = {0.05, 0.05, 0.5, 0.4};
    // float cmf[4] = {0, 0.05, 0.1, 0.6};

    float pmf[4] = {0.2, 0.4, 0.4};
    float cmf[4] = {0, 0.2, 0.6};
    
    int n[4] = {2, 1, 0};

    u64 code = arithmetic_encode(pmf, cmf, n, 3);
    cout << code << endl;

    printBits(code);

    return 0;
}
