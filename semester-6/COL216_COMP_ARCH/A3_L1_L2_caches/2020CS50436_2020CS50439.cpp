#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <bitset>
#include <cmath>
using namespace std;

class cache
{
    private:
        int block_size;
        int L1_size;
        int L1_assoc;
        int L2_size;
        int L2_assoc;
        vector<vector<vector<int>>> L1_cache;
        vector<vector<int>> L1_tag;
        vector<vector<vector<int>>> L2_cache;
        vector<vector<int>> L2_tag;

        //if cache has any value written (for writeback)
        vector<vector<int>> L1_updated;
        vector<vector<int>> L2_updated;

        //LRU priorities
        vector<vector<int>> L1_lru;
        vector<vector<int>> L2_lru;


    public:
        int L1_reads = 0;
        int L1_read_misses = 0;
        int L1_writes = 0;
        int L1_write_misses = 0;
        int L1_writeback = 0;

        int L2_reads = 0;
        int L2_read_misses = 0;
        int L2_writes = 0;
        int L2_write_misses = 0;
        int L2_writeback = 0;


        cache(int block_size_in, int L1_size_in, int L1_assoc_in, int L2_size_in, int L2_assoc_in) : block_size(block_size_in), 
        L1_size(L1_size_in/L1_assoc_in), L1_assoc(L1_assoc_in), 
        L2_size(L2_size_in/L2_assoc_in), L2_assoc(L2_assoc_in), 
        L1_cache(L1_size/block_size, vector<vector<int>>(L1_assoc ,vector<int>(block_size, 0))), 
        L1_tag(L1_size/block_size, vector<int>(L1_assoc,0)),
        L1_updated(L1_size/block_size, vector<int>(L1_assoc,0)),
        L1_lru(L1_size/block_size, vector<int>(L1_assoc,0)),
        L2_cache(L2_size/block_size, vector<vector<int>>(L2_assoc ,vector<int>(block_size, 0))),
        L2_tag(L2_size/block_size, vector<int>(L2_assoc,0)),
        L2_updated(L2_size/block_size, vector<int>(L2_assoc,0)),
        L2_lru(L2_size/block_size, vector<int>(L2_assoc,0))
        { }
        
        int check_L1(string rw, bitset<32> address)
        {
            int block_byte = address.to_ulong() & ((1 << static_cast<int>(log2(block_size))) - 1);
            int set_number = (address.to_ulong() >> static_cast<int>(log2(block_size))) & ((1 << static_cast<int>(log2(L1_size/block_size))) -1);
            int tag_value = address.to_ulong() >> static_cast<int>(log2(L1_size));
            int set_way = 0;
            int index = L1_assoc;
            while(set_way < L1_assoc)
            {
                if (L1_lru[set_number][set_way] == 0){}
                else{
                    L1_lru[set_number][set_way] +=1;
                
                    if(L1_tag[set_number][set_way] == tag_value)
                    {
                        L1_lru[set_number][set_way] = 1;
                        index = set_way;
                    }
                }
                
                set_way+=1;
            }

            set_way = index;
            if(set_way >= L1_assoc)
            {
                
                if(rw == "r")
                {
                    L1_read_misses +=1;
                    L1_reads +=1;
                }
                else
                {
                    L1_writes +=1;
                    L1_write_misses +=1;
                }

                update_L1(rw, address);
                check_L2("r" , address);
            }
            else
            {
                if(rw == "r")
                {
                    L1_reads +=1;
                }
                else
                {
                    L1_writes +=1;
                    L1_updated[set_number][set_way] = 1;
                }
            }

            // cout << "\n Checking L1";
            // for (int i = 0; i< L1_tag.size(); i++){
            //     cout << "\n set " << i << " : ";
            //     for (int j = 0; j<L1_assoc ; j++){
            //         cout << L1_tag[i][j] << " " << L1_lru[i][j]  << " " << L1_updated[i][j] << " ";
            //     }
            // }
            return 0;
        }

        int update_L1(string rw, bitset<32> address)
        {

            int block_byte = address.to_ulong() & ((1 << static_cast<int>(log2(block_size))) - 1);
            int set_number = (address.to_ulong() >> static_cast<int>(log2(block_size))) & ((1 << static_cast<int>(log2(L1_size/block_size))) -1);
            int tag_value = address.to_ulong() >> static_cast<int>(log2(L1_size));
            
            int set_way = 0;
            while(set_way < L1_assoc)
            {
                if(L1_lru[set_number][set_way] == 0)
                {
                    break;
                }
                set_way +=1;
            }
            if(set_way >= L1_assoc)
            {
                set_way = 0;
                int max = 0;
                int index = 0;
                while(set_way < L1_assoc)
                {
                    if(L1_lru[set_number][set_way] > max)
                    {
                        max = L1_lru[set_number][set_way];
                        index = set_way;
                    }
                    set_way +=1;
                }
                set_way = index;
                if (L1_updated[set_number][set_way] == 1)
                {
                    // cout << "hi ";
                    L1_updated[set_number][set_way] = 0;
                    L1_writeback +=1;
                    bitset<32> evic_address = (L1_tag[set_number][set_way] << static_cast<int>(log2(L1_size))) + (set_number << static_cast<int>(log2(block_size))) + block_byte;
                    check_L2("w", evic_address); 
                    // cout << "write back: " << L1_tag[set_number][set_way] << "\n";
                }
            }
            // cout << " " << L1_lru[set_number][set_way] << " "<< L1_tag[set_number][set_way] <<" " << tag_value<< "\n";
            L1_lru[set_number][set_way] = 1;
            if (rw == "w")
            {
                L1_updated[set_number][set_way] = 1;
            }
            L1_tag[set_number][set_way] = tag_value;
            return 0;
        }

        int check_L2(string rw, bitset<32> address)
        {
            int block_byte = address.to_ulong() & ((1 << static_cast<int>(log2(block_size))) - 1);
            int set_number = (address.to_ulong() >> static_cast<int>(log2(block_size))) & ((1 << static_cast<int>(log2(L2_size/block_size))) -1);
            int tag_value = address.to_ulong() >> static_cast<int>(log2(L2_size));
            
            int set_way = 0;
            int index = L2_assoc;
            while(set_way < L2_assoc)
            {
                if (L2_lru[set_number][set_way] == 0){}
                else 
                {
                    L2_lru[set_number][set_way] +=1;
                    if(L2_tag[set_number][set_way] == tag_value)
                    {
                        L2_lru[set_number][set_way] = 1;
                        index = set_way;
                    }
                }
                set_way+=1;
            }
            // cout << "index is : " << index << " " << set_number << " " << L2_tag[set_number][index]  << " " << tag_value << "\n";
            set_way = index;
            if(set_way >= L2_assoc)
            {
                // cout << "hi\n";
                
                if(rw == "r")
                {
                    L2_read_misses +=1;
                    L2_reads +=1;
                }
                else
                {
                    L2_write_misses +=1;
                    L2_writes +=1;
                    
                }
                update_L2(rw, address);
            }
            else
            {
                if(rw == "r")
                {
                    L2_reads +=1;
                }
                else
                {
                    L2_writes +=1;
                    L2_updated[set_number][set_way] = 1;
                }
                // cout << "L2 True";
            }
            // cout << "\n Checking L2";
            // for (int i = 0; i< L2_tag.size(); i++){
            //     cout << "\n set " << i << " : ";
            //     for (int j = 0; j<L2_assoc ; j++){
            //         cout << L2_tag[i][j] << " " << L2_lru[i][j] << " ";
            //     }
            // }
            return 0;
        }

        int update_L2(string rw, bitset<32> address)
        {
            int block_byte = address.to_ulong() & ((1 << static_cast<int>(log2(block_size))) - 1);
            int set_number = (address.to_ulong() >> static_cast<int>(log2(block_size))) & ((1 << static_cast<int>(log2(L2_size/block_size))) -1);
            int tag_value = address.to_ulong() >> static_cast<int>(log2(L2_size));
            
            int set_way = 0;
            while(set_way < L2_assoc)
            {
                if(L2_lru[set_number][set_way] == 0)
                {
                    break;
                }
                set_way +=1;
            }
            if(set_way >= L2_assoc)
            {
                set_way = 0;
                int max = 0;
                int index = 0;
                while(set_way < L2_assoc)
                {
                    if(L2_lru[set_number][set_way] > max)
                    {
                        max = L2_lru[set_number][set_way];
                        index = set_way;
                    }
                    set_way +=1;
                }
                set_way = index;
                if (L2_updated[set_number][set_way] == 1)
                {
                    L2_updated[set_number][set_way] = 0;
                    L2_writeback +=1;
                }
            }
            
            L2_lru[set_number][set_way] = 1;
            if (rw == "w")
            {
                L2_updated[set_number][set_way] = 1;
            }
            L2_tag[set_number][set_way] = tag_value;
            
            return 0;
        }

};

int main(int argc, char* argv[])
{
    int block_size_in = stoi(argv[1]);
    int L1_size_in = stoi(argv[2]);
    int L1_assoc_in =stoi(argv[3]);
    int L2_size_in = stoi(argv[4]);
    int L2_assoc_in =stoi(argv[5]);
    string input = argv[6];

    int n = 0;
    cache caches(block_size_in, L1_size_in, L1_assoc_in,L2_size_in, L2_assoc_in);

    string text;
    string rw;
    ifstream input_file(input);
    while(getline(input_file, text)){
        rw = text[0];
        bitset<32> address(stoul(text.substr(2), nullptr, 16));
        caches.check_L1(rw, address);
        // cout << "number of L2 read misses: " << caches.L2_read_misses << " " << address << "\n";
        // cout << "number of L2 write misses: " << caches.L2_write_misses << " " << address << "\n" ;

        n+=1;
        // if (n>1000)break;
    }
    cout << "number of L1 reads: " << caches.L1_reads << "\n" ;
    cout << "number of L1 read misses: " << caches.L1_read_misses << "\n";
    cout << "number of L1 writes: " << caches.L1_writes << "\n" ;
    cout << "number of L1 write misses: " << caches.L1_write_misses << "\n" ;
    cout << "L1 miss rate: " << (float)(caches.L1_read_misses + caches.L1_write_misses)/ (float)n << "\n" ;
    cout << "number of writebacks from L1 memory: " << caches.L1_writeback << "\n" ;
    cout << "number of L2 reads: " << caches.L2_reads << "\n" ;
    cout << "number of L2 read misses: " << caches.L2_read_misses << "\n" ;
    cout << "number of L2 writes: " << caches.L2_writes << "\n" ;
    cout << "number of L2 write misses: " << caches.L2_write_misses << "\n" ;
    cout << "L2 miss rate: " << (float)(caches.L2_read_misses + caches.L2_write_misses)/(float)(caches.L2_reads+caches.L2_writes) << "\n" ;
    cout << "number of writebacks from L2 memory: " << caches.L2_writeback << "\n" ;

    cout << "total time is (in ns): " << n*1 + 20*(caches.L1_writeback + caches.L1_read_misses + caches.L1_write_misses) + 200*(caches.L2_read_misses + caches.L2_write_misses + caches.L2_writeback)<< "\n";
}