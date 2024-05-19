#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <bitset>

#define SHIFTED_MASK 4
#define CONTINUATION_MASK 2
#define OCCUPIED_MASK 1



uint_fast64_t typedef u64; 

using namespace std;


hash<string> string_hash;

void shift_blocks(u64* filter, u64 curr_u64, u64 curr_bit, int bits_per_entry, int num_u64s) {

    u64 block_mask = (1lu << bits_per_entry) - 1;
    u64 block_no_occ_mask = (1lu << (bits_per_entry - 1)) - 1;

    u64 curr_block_data = ((filter[curr_u64] >> curr_bit) & block_mask);


    u64 next_u64 = (curr_u64 + (curr_bit + bits_per_entry) / 64) % num_u64s;
    u64 next_bit = (curr_bit + bits_per_entry) % 64;
    u64 next_block_data = ((filter[next_u64] >> next_bit) & block_mask);
    do {
        u64 temp = curr_block_data;
        if ((temp & 7) == 1) {
            // goes from first element of a run in the right place to the first in a run in the wrong place
            temp = (temp & (~7)) | SHIFTED_MASK | CONTINUATION_MASK;
        } else {
            // mask 
            temp = temp & ~OCCUPIED_MASK;
        }

        curr_block_data = next_block_data;

        // u64* to_edit = &filter[next_u64];
        // u64 edit_bits = next_bit;

        filter[next_u64] &= ~(block_no_occ_mask << (next_bit + 1));
        filter[next_u64] |= (temp << next_bit);
        
        next_u64 = (next_u64 + (next_bit + bits_per_entry) / 64) % num_u64s;
        next_bit = (next_bit + bits_per_entry) % 64;
        next_block_data = ((filter[next_u64] >> next_bit) & block_mask);

    } while ((curr_block_data & 7) > 0);
}

void add_to_run_from_start(u64* filter, u64 curr_u64, u64 curr_bit, int bits_per_entry, int num_u64s, u64 rem) {

    u64 block_mask = (1lu << bits_per_entry) - 1;
    u64 rmask = (1lu << (bits_per_entry - 3)) - 1;


    u64 curr_block_data = ((filter[curr_u64] >> curr_bit) & block_mask);
    do {
        
        u64 block_rem_data = curr_block_data >> 3;
        if (rem == block_rem_data) {
            cout << "should break\n";
            // element already exisits in filter so we dont need to add
            // goto continue_outer;
            return;

        } else if (rem < block_rem_data) {
            // We will move everything over by one and correctly set the bits until we reach an empty block
            // u64 inserting_block_u64 = curr_u64;
            // u64 inserting_block_bit = curr_bit;

            // note: occupied bits never change

            shift_blocks(filter, curr_u64, curr_bit, bits_per_entry, num_u64s);

            // cout << "to_replace" << hex << filter[inserting_block_u64] << endl;
            filter[curr_u64] &= ~(rmask << (curr_bit + 3));
            filter[curr_u64] |= (rem << (curr_bit + 3));

            // goto continue_outer;
            return;
        } else {
            // cout << "here\n";
            curr_u64 = (curr_u64 + (curr_bit + bits_per_entry) / 64) % num_u64s;
            curr_bit = (curr_bit + bits_per_entry) % 64;
            curr_block_data = ((filter[curr_u64] >> curr_bit) & block_mask);
        }
    } while ((curr_block_data & (SHIFTED_MASK)) > 0);

    u64 newBlock = (rem << 3) | SHIFTED_MASK | CONTINUATION_MASK;
    filter[curr_u64] |= newBlock << curr_bit;
}

// the filter is stored as an array of 64 bit unsigned numbers
// the first 3 bits of a given slot are designated as the metadata bits and the next r bits are the remainder
// something like: actually this is reversed

// is_occupied | is_continuation | is_shifted | remainder | ... | is_occupied | is_continuation | is_shifted | remainder | 
//      1               0              0        100101      ...
template <typename T>
u64* build_filter(hash<T> hash_f, int q, int r, vector<T> input) {

    u64 filter_entries = 1 << q;
    int bits_per_entry = r + 3;
    u64 block_mask = (1lu << bits_per_entry) - 1;
    u64 block_no_occ_mask = (1lu << (bits_per_entry - 1)) - 1;

    cout << "filter_entries: "<< filter_entries << endl <<"bits_per_entry: "<< bits_per_entry << endl;

    u64 qmask = ((1lu << q) - 1) << (64 - q);
    u64 rmask = ((1lu << r) - 1); 
    int num_u64s = ((filter_entries * bits_per_entry) / 64) + 1;
    u64* filter = (u64*)calloc(num_u64s, sizeof(u64));
    
    // one iteration of this is just add element 
    // TODO write add_element as a helper
    for (T ele : input) {
        
        u64 fingerprint = static_cast<u64>(hash_f(ele));
        u64 quotient = (fingerprint & qmask) >> (64 - q);
        u64 rem = fingerprint & rmask;
        
        // note: / 64 and % 64 are all very fast with better bit tricks / high compiler optimizations
        int start_u64 = (quotient * bits_per_entry) / 64;
        int start_bit = (quotient * bits_per_entry) % 64;
        int end_u64 = ((quotient * bits_per_entry) + (bits_per_entry - 1)) / 64;
        // cout << quotient << " " << start_block << " " << end_block << endl;  
        if (start_u64 == end_u64) {

            u64* cannon_u64 = &filter[start_u64];
            // cout << "filter " << hex << filter[start_u64] << endl;

            // cout << dec << "Startbit" << start_bit << endl;
            u64 cannonical_block_data = ((filter[start_u64] >> start_bit) & block_mask);
            // cout << "cbd " << hex << cannonical_block_data << endl;

            // u64 is_clean = cannonical_block_data & 0x7;
            // cout << is_clean << endl;


            // cout << "filter " << hex << filter[start_u64] << endl << endl;
            // cout << "rem " << hex << rem << endl;
            // cout << "quot " << dec << quotient << endl;
            // cout << "filter " << bitset<64>(filter[start_u64]) << endl;

            *cannon_u64 |= (1lu << start_bit);
            // Notes: (Current understanding)
            // - when we go to add an element we start at the cannoical slot
            // - if it is is filled but marked as occupied then we go look to the right if it is empty then we add 
            //   and set shifted and continuation to 1
            // - if the is filled and marked as not empty then we move right to next empty spot, add, and set shifted bit to 1
            // - if we insert into an exisiting run we insert in sorted order and move over elements as necesary (note need to update bits)
            // - i think a continuation is also always shifted...?
            // - if cannonical slot is marked as occupied it may be there so we have to look for it:
            // - consider 2 cases;
            //     - if the shifted bit is set to a 0 then while the continuation bit is set to 1
            //     we search linearly "forward" until we either find the value in which case we are done because we don't need to add
            //     or we reach the end of the continuation bits being set
            //     - if the shifted bit is set to a 1 then this is not the start of the run for the quotient we care about
            //     we must go back to the start of this cluster by looking linearly "backwards" until we find a bucket without shifted = 1. Then
            //     we go linearly "forward" ... 
            
            
            // keeping track of the sum of the bits along the way until occupied + continuation = shifted. 
            //     when this is done, all possible runs that could have originated from this spot have been accounted for 

            // we may have to still look then until the end of the last run? 


            u64 curr_block_data = cannonical_block_data; // sometimes we will have to look backwards to actually find this first
            u64 curr_bit = start_bit;
            u64 curr_u64 = start_u64;
            



            // this checks if there is an existing run
            if ((cannonical_block_data & OCCUPIED_MASK) > 0) {
                
                // this check if this is the start of the exisiting run
                if ((cannonical_block_data & SHIFTED_MASK) == 0) {
                    
                    // remainders match
                    add_to_run_from_start(filter, curr_u64, curr_bit, bits_per_entry, num_u64s, rem);
                    goto continue_outer;
                
                // there is a run somewhere, but this is not the start of it... now we need to find the run and then search
                } else {
                    // first we look backwards to find the start

                    // currently this includes the starting run but not the run we want to end up in
                    int num_runs = 0;
                    while ((curr_block_data & SHIFTED_MASK) > 0) {

                        int spill = (curr_bit - bits_per_entry) < 0;
                        cout << spill << endl;
                        curr_u64 -= spill;
                        curr_bit = (curr_bit - bits_per_entry) + (64 * spill);
                        curr_block_data = ((filter[curr_u64] >> curr_bit) & block_mask);

                        if ((curr_block_data & OCCUPIED_MASK) > 0) {
                            num_runs++;
                        }
                    }
                    while (num_runs > 0) {

                        curr_u64 = (curr_u64 + (curr_bit + bits_per_entry) / 64) % num_u64s;
                        curr_bit = (curr_bit + bits_per_entry) % 64;
                        curr_block_data = ((filter[curr_u64] >> curr_bit) & block_mask);

                        if ((curr_block_data & CONTINUATION_MASK) == 0) {
                            num_runs--;
                        }
                        
                        add_to_run_from_start(filter, curr_u64, curr_bit, bits_per_entry, num_u64s, rem);
                    }

                    // curr_block now should be the spot to insert at
                        
                    // TODO
                }
            // no exisiting run  
            } else {
                
                // this block is not part of a cluster so we can trivally add it
                if ((cannonical_block_data & SHIFTED_MASK) == 0) {
                    // *cannon_u64 |= (rem << (curr_bit + 3));
                    filter[start_u64] |= (rem << (curr_bit + 3));
                    
                    goto continue_outer;
                // block is part of a cluster -- need to find next available block and add it there
                } else {

                    // Note: I think this is untested still

                    while ((curr_block_data & 7) > 0) {
                        curr_u64 = (curr_u64 + (curr_bit + bits_per_entry) / 64) % num_u64s;
                        curr_bit = (curr_bit + bits_per_entry) % 64;
                        curr_block_data = ((filter[curr_u64] >> curr_bit) & block_mask);
                    }

                    filter[curr_u64] |= (rem << (curr_bit + 3));
                    
                }   

                
            }
            // if (cannonical_block_data == 0) {
            //     *curr_u64 |= (rem << (start_bit + 3));
                
            // } else {
            //     u64 curr_block_data = cannonical_block_data;
            //     u64 curr_bit = start_bit;
            //     u64 curr_u64 = start_u64;

            //     // find start of a run : i.e. find first element without is_shifted set
            //     // while ((curr_block_data & SHIFTED_MASK) > 0) {
            //     //     int spill = (curr_bit - bits_per_entry) < 0;
            //     //     cout << spill << endl;
            //     //     curr_u64 -= spill;
            //     //     curr_bit = (curr_bit - bits_per_entry) + (64 * spill);
            //     //     curr_block_data = ((filter[curr_u64] >> curr_bit) & block_mask);
            //     // }



            //     u64 running_count = 0;
            //     do {
            //         int is_occupied = (curr_block_data & OCCUPIED_MASK) > 0;
            //         int is_continuation = (curr_block_data & CONTINUATION_MASK) > 0;
            //         running_count += is_occupied - is_continuation;

            //         // check if this is the element
            //         if (rem == (((filter[curr_u64] >> curr_bit) & rem_and_block_mask)) >> 3) {
            //             // element already exists so we are good
            //             break;
            //         }

            //     } while (running_count != 0);

            //     // now we need to add
            //     cout << "not done\n";
            // }

            

            // u64 is_occupied = cannonical_block_data & 1;






            

        } else {

            cout << "error bad build\n";
            // this is where the stuff doesn't align
            // TODO, harder
        }


        continue_outer:;
    }



    // u64 hash_value = static_cast<u64>(hash_f(input[0]));
    
    // cout << hash_value << endl;
    
    return filter;
}

template <typename T>
bool test_filter(u64* filter, hash<T> hash_f, int q, int r, T query) {

    u64 filter_entries = 1 << q;
    int bits_per_entry = r + 3;
    u64 block_mask = (1lu << bits_per_entry) - 1;

    u64 qmask = ((1lu << q) - 1) << (64 - q);
    u64 rmask = ((1lu << r) - 1); 
    int num_u64s = ((filter_entries * bits_per_entry) / 64) + 1;
        
    u64 fingerprint = static_cast<u64>(hash_f(query));
    u64 quotient = (fingerprint & qmask) >> (64 - q);
    u64 rem = fingerprint & rmask;


    // note: / 64 and % 64 are all very fast with better bit tricks / high compiler optimizations
    int start_u64 = (quotient * bits_per_entry) / 64;
    int start_bit = (quotient * bits_per_entry) % 64;
    int end_u64 = ((quotient * bits_per_entry) + (bits_per_entry - 1)) / 64;

    // cout << "rem " << hex << rem << endl;
    // cout << "quot " << dec << quotient << endl;
    // cout << "start_u64 " << dec << start_u64 << endl;
    // cout << "start_bit " << dec << start_bit << endl;
    

    // cout << quotient << " " << start_block << " " << end_block << endl;  
    if (start_u64 == end_u64) {

        u64* cannon_u64 = &filter[start_u64];

        u64 cannonical_block_data = ((filter[start_u64] >> start_bit) & block_mask);

        *cannon_u64 |= (1lu << start_bit);

        u64 curr_block_data = cannonical_block_data; // sometimes we will have to look backwards to actually find this first
        u64 curr_bit = start_bit;
        u64 curr_u64 = start_u64;

        // this checks if there is an existing run
        if ((cannonical_block_data & OCCUPIED_MASK) > 0) {
            
            // this check if this is the start of the exisiting run
            if ((cannonical_block_data & SHIFTED_MASK) == 0) {
                
                // remainders match

                do {
                    u64 block_rem_data = curr_block_data >> 3;
                    if (rem == block_rem_data) {
                        // element already exisits in filter so we dont need to add
                        return true;
                    // } else if (rem > block_rem_data) {
                    //     // cout << "here\n";
                    //     // return false;

                    //     // continue;
                    } else {
                        // cout << "reping\n";

                        curr_u64 = (curr_u64 + (curr_bit + bits_per_entry) / 64) % num_u64s;
                        curr_bit = (curr_bit + bits_per_entry) % 64;
                        curr_block_data = ((filter[curr_u64] >> curr_bit) & block_mask);
                    }

                } while ((curr_block_data & SHIFTED_MASK) > 0 && ((curr_block_data >> 3) < rem));

                if ((curr_block_data >> 3) == rem){
                    return true;
                }
            
            // there is a run somewhere, but this is not the start of it... now we need to find the run and then search
            } else {
                
                int num_runs = 0;
                while ((curr_block_data & SHIFTED_MASK) > 0) {

                    int spill = (curr_bit - bits_per_entry) < 0;
                    // cout << spill << endl;
                    curr_u64 -= spill;
                    curr_bit = (curr_bit - bits_per_entry) + (64 * spill);
                    curr_block_data = ((filter[curr_u64] >> curr_bit) & block_mask);

                    if ((curr_block_data & OCCUPIED_MASK) > 0) {
                        num_runs++;
                    }
                }
                while (num_runs > 0) {

                    curr_u64 = (curr_u64 + (curr_bit + bits_per_entry) / 64) % num_u64s;
                    curr_bit = (curr_bit + bits_per_entry) % 64;
                    curr_block_data = ((filter[curr_u64] >> curr_bit) & block_mask);

                    if ((curr_block_data & CONTINUATION_MASK) == 0) {
                        num_runs--;
                    }
                    
                    // this is copy paste from above -- make helper
                    do {
                        u64 block_rem_data = curr_block_data >> 3;
                        if (rem == block_rem_data) {
                            // element already exisits in filter so we dont need to add
                            return true;
                        // } else if (rem > block_rem_data) {
                        //     // cout << "here\n";
                        //     // return false;

                        //     // continue;
                        } else {
                            // cout << "reping\n";

                            curr_u64 = (curr_u64 + (curr_bit + bits_per_entry) / 64) % num_u64s;
                            curr_bit = (curr_bit + bits_per_entry) % 64;
                            curr_block_data = ((filter[curr_u64] >> curr_bit) & block_mask);
                        }

                    } while ((curr_block_data & SHIFTED_MASK) > 0 && ((curr_block_data >> 3) < rem));

                    if ((curr_block_data >> 3) == rem){
                        return true;
                    }
                    // add_to_run_from_start(filter, curr_u64, curr_bit, bits_per_entry, num_u64s, rem);
                }
            }
        // no exisiting run  
        } else {
            return false;
        }
    } else {
        cout << "error bad test\n";

        // this is where the stuff doesn't align
        // TODO, harder
    }


    return false;
}

string generateRandomString(int length) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::string randomString;
    randomString.reserve(length);

    for (int i = 0; i < length; ++i) {
        randomString += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return randomString;
}


template <typename T>

void run_tests(u64* filter, hash<T> hash_f, int q, int r, vector<T> inputs) {

    for (T word : inputs) {
        bool result = test_filter(filter, hash_f, q, r, word);

        if (result) {
            // cout << word << " in filter" << endl;
        } else {
            cout << word << " not in filter" << endl;

        }
    }

    for (int i = 0; i < 100; i++) {

        string word = generateRandomString(5);
        bool result = test_filter(filter, hash_f, q, r, word);

        if (result) {
            cout << word << " in filter" << endl;
        } else {
            // cout << word << " not in filter" << endl;

        }
    }
}


int main(int argc, char* argv[]) {

    // vector<string> words = {"test", "goober", "hello", "pizza", "sample", "pizza"};
    // vector<string> words = {"11111111", "hello", "adfa"};


    vector<string> words = {
        // "hello",
        // "5PN2q",
        // "Q9wQj",
        // "5ZuGX",
        // "lemon"
        // "dNMK6",
        // "bavJv",
        // "S0hoI",
        // "SpM2T",
        // "botuQ",
        // "LMZRO",
        // "1Mguo",
        // "l5noe"
    };

    for (int i = 0; i < 5; ++i) {
        words.push_back(generateRandomString(5)); // Change 8 to desired length of random string
    }

    int r = 5;
    int q = 4;
    int len_filter = (((1lu << q) * (r + 3)) / 64) + 1;

    u64* filter = build_filter<string>(string_hash, q, r, words);
    cout << "filter:" << endl;
    for (int i = 0; i < len_filter; i ++) {
        cout << hex << filter[i] << endl;
    }


    run_tests<string>(filter, string_hash, q, r, words);

    // for (int i = 0; i < 100; ++i) {
    //     words.push_back(generateRandomString(5)); // Change 8 to desired length of random string
    // }


    // u64 qmask = ((1lu << q) - 1) << (64 - q);

    
    // for (string word : words) {
    //     u64 fingerprint = static_cast<u64>(string_hash(word));
    //     u64 quotient = (fingerprint & qmask) >> (64 - q);
    //     if (quotient == 0)
    //         cout << word << endl;
    // }
    

    // cout << bool << test_filter<string>(filter, string_hash, q, r, "lemon") << endl;
    // string to_test = argv[1];
    // if (test_filter<string>(filter, string_hash, q, r, to_test)) {
    //     cout << to_test << " in filter" << endl;
    // } else {
    //     cout << to_test << " not in filter" << endl;

    // }

    
    return 0;
}
