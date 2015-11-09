//
//  hashtable.h
//  Cache
//
//  Created by Xiaohan Chen on 03/12/2014.
//  Copyright (c) 2014 xiaohan. All rights reserved.
//

#ifndef Cache_hashtable_h
#define Cache_hashtable_h
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
class block{
private:
    unsigned block_address;
    int block_size_word;
    int word_size_byte;
    unsigned tag;
    int valid;
    int dirty;
    std::string** data;          //multi_word
    int LRU_record;
    
public:
    
    block(const int block_size);

    
    void flush(std::string* memory[], const unsigned& block_addr, const unsigned& tag_n );
                                                            //flush block(change tag/data/block_addr but not valid;
    unsigned get_tag(){return tag;};
    unsigned get_block_addr(){return block_address;};
    int get_LRU_record(){return LRU_record;}
    int get_valid(){return valid;};
    int get_dirty(){return dirty;};
    unsigned get_blockaddr(){return block_address;};
    void set_valid(){valid = 1;};
    void set_LRU(const int& global_stamp){LRU_record = global_stamp;};
    bool if_hit(const unsigned& tag_num);
    void wirte_data(const int& offset, const std::string& data_i);
    std::string read_data(const int& offset);
    block* LRU_block();
    
    
    ~block();
};


/************************CACHE***************************/




class cache{
    
private:
    block*** set;                    //array of block pointer to block pointers; 2 dimentional arrray;
    int set_size_block;
    int block_size_word;
    int word_size_byte;
    int set_num;
    int Cycles_Hit;
    int Cycles_ReadBlock;
    int Cycles_WriteBlock;
    
public:
    cache(const int& set_size, const int& index_n, const int& block_size, const int& word_size,
          const int& Cycles_Hit_i,const int& Cycles_ReadBlock_i,const int& Cycles_WriteBlock_i);
    block* find_block(const unsigned& byte_addr, std::string* memory[], std::string& hit_bool,
                      int& dirty_bit, int& time,const int& global_stamp);

    int all_flush(std::string* memory[]);
    
    void print_cache();
    
    ~cache();
    
};

#endif



