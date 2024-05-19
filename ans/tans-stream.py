from math import ceil, log2

def shrink_state_num_out_bits_base(s):
    # calculate the power of 2 lying in [freq[s], 2freq[s] - 1]
    y = ceil(log2(freq[s]))
    return r + 1 - y

### build the tables ###
# NOTE: this is a one time thing which can be done at the beginning of encoding
# or at compile time

base_encode_step_table = {} #M rows, each storing x_next in [L,H]
for s in Alphabet:
    for x_shrunk in Interval[freq[s], 2*freq[s] - 1]:
        base_encode_step_table[x_shrunk, s] = rans_base_encode_step(x_shrunk,s)

shrink_state_num_out_bits_base = {} #stores the exponent y values as described above
shrink_state_thresh = {} # stores the thresh values
for s in Alphabet:
    shrink_state_num_out_bits_base[s] = shrink_state_num_out_bits_base(s)
    shrink_state_thresh[s] = freq[s] << (shrink_state_num_out_bits_base[s] + 1)

### the cached encode_step ########
def encode_step_cached(x,s):
    # shrink state x before calling base encode
    num_out_bits = shrink_state_num_out_bits_base[s]
    if x >= shrink_state_thresh[y]:
        num_out_bits += 1
    x_shrunk = x >> num_out_bits
    out_bits = to_binary(x)[-num_out_bits:]

    # perform the base encoding step
    x_next = base_encode_step_table[x_shrunk,s]
   
    return x_next, out_bits