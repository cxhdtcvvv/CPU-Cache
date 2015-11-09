//
//  hashtable.cpp
//  Cache
//
//  Created by Xiaohan Chen on 03/12/2014.
//  Copyright (c) 2014 xiaohan. All rights reserved.
//

#include "mem_sim_*.hpp"
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;
/**********************************BLOCK*******************************/


block::block(const int block_size):
            block_address(-1),block_size_word(block_size),word_size_byte(0),                           //******FIXING
            tag(0),valid(0),dirty(0),LRU_record(0), data(new string*[block_size_word]){
                
                for(int i = 0; i<block_size_word; i++ ){
                    data[i] = new string;
                }
}



bool block::if_hit(const unsigned& tag_num){
    if(tag_num == tag){
        return true;
    }else{
        return false;
    }
}


string block::read_data(const int& offset){                //read doesnt change dirty bit, due to flush.
    
    return *data[offset];

}


void block::wirte_data(const int& offset, const string& data_i){
    
    *data[offset] = data_i;
    dirty = 1;
}




void block::flush(string* memory[], const unsigned& block_addr, const unsigned& tag_n ){
    
    if(dirty == 1){                                     //block has dirty data, so update memory then flush;
        unsigned temp_addr = block_address * block_size_word;
        for(int i = 0; i< block_size_word; i++){
            *memory[temp_addr] = *data[i];                //update memory; N.B. cant connect these two things together
            temp_addr += 1;
        }
        
    }
        tag = tag_n;
        block_address = block_addr;
        
        unsigned temp_addr = block_address * block_size_word;
        for(int i = 0; i< block_size_word; i++){
            *data[i] = *memory[temp_addr];                //update block data from memory;
            temp_addr += 1;
        }
        valid = 1;
        dirty = 0;
        LRU_record = 0;

}


block::~block(){
    
    for(int i = 0; i<block_size_word; i++ ){
        delete data[i];
    }
    delete data;
}

/**********************************CACHE*******************************/

cache::cache(const int& set_size, const int& index_n, const int& block_size, const int& word_size,
             const int& Cycles_Hit_i,const int& Cycles_ReadBlock_i,const int& Cycles_WriteBlock_i):
            set(new block**[index_n]),set_size_block(set_size),set_num(index_n),word_size_byte(word_size),
            block_size_word(block_size), Cycles_Hit(Cycles_Hit_i), Cycles_ReadBlock(Cycles_ReadBlock_i),
            Cycles_WriteBlock(Cycles_WriteBlock_i)
{
    for(int i = 0; i < index_n; i++){
        set[i] = new block*[set_size_block];            //two dimention array get
        for(int j = 0; j<set_size_block; j++ ){
            set[i][j] = new block(block_size_word);
        }
    }
}

cache::~cache(){
    
    for(int i = 0; i < set_num; i++){
        for(int j = 0; j<set_size_block; j++ ){
            delete set[i][j];
        }
    }
    
    for(int i = 0; i < set_num; i++){
        delete [] set[i];
    }
    
    delete set;
    
}

void cache::print_cache(){
    for(int i=0; i<set_num; i++){
        cout<<"S"<<i<<"={";
        for(int j = 0; j< set_size_block;j++){
            if((*set[i][j]).get_block_addr() != -1){
                cout<<"B"<<(*set[i][j]).get_block_addr();
                if((*set[i][j]).get_dirty() == 1) cout<<"/d";
                if(j != (set_size_block-1)) cout<<", ";
            }
        }
        cout<<"}, ";
    }
    
}



block* cache::find_block(const unsigned& byte_addr, string* memory[], string& hit_bool,
                         int& dirty_bit, int& time, const int& global_stamp){
    
    unsigned word_addr = byte_addr / word_size_byte;
    unsigned block_addr = word_addr / block_size_word;
    //unsigned word_offset = word_addr % word_block;
    unsigned index = block_addr % set_num;
    unsigned tag_n = block_addr / set_num;
    
    if(set_size_block == 1){                           //non-associative....
        if( (*set[index][0]).if_hit(tag_n) && (*set[index][0]).get_valid()){
            hit_bool = "hit";
            dirty_bit =(*set[index][0]).get_dirty();
            (*set[index][0]).set_LRU(global_stamp);
            time = Cycles_Hit;
            return set[index][0];        //return pointer pointing to a block
        }else{
            dirty_bit =(*set[index][0]).get_dirty();
            (*set[index][0]).flush(memory, block_addr, tag_n);
            (*set[index][0]).set_LRU(global_stamp);
            hit_bool = "miss";
            time = Cycles_Hit;
            if(dirty_bit == 1){
                time += Cycles_WriteBlock;      //read block to update memory;
            }
            
            time += Cycles_ReadBlock;      //"READ", write memory's data to cache; "Write", write program's data to cache
            
            return set[index][0];         //replce old block;
        }
        
    }else{                                              //associative....
        for(int i = 0; i < set_size_block; i++){                                 //searching the node
            if((*set[index][i]).if_hit(tag_n)&& (*set[index][i]).get_valid()){
                hit_bool = "hit";
                dirty_bit =(*set[index][i]).get_dirty();
                (*set[index][i]).set_LRU(global_stamp);
                time = Cycles_Hit;
                return set[index][i];
            }
        }
                                        //jump out of for loop, no block matched, ***need to find LRU, then replace
        time = Cycles_Hit;  //miss
        int LRU_block = (*set[index][0]).get_LRU_record();
        int LRU_block_index = 0;
        for(int i = 0; i < set_size_block; i++){                             //N.B. if all blocks have the same LRU constant, choose the smallest index
            if(LRU_block > (*set[index][i]).get_LRU_record() ){
                LRU_block_index = i;
            }
        }
        
        cout<<"# set_offset:  "<<LRU_block_index<<endl;
        
        dirty_bit =(*set[index][LRU_block_index]).get_dirty();

        (*set[index][LRU_block_index]).flush(memory, block_addr, tag_n);         //flush block;
        hit_bool = "miss";
        
        if(dirty_bit == 1){
            time += Cycles_WriteBlock;      //read block to update memory;
        }
        time += Cycles_ReadBlock;      //"READ", write memory's data to cache; "Write", write program's data to cache
        (*set[index][LRU_block_index]).set_LRU(global_stamp);

        return set[index][LRU_block_index];         //replce old block;

    }
    
}


int cache::all_flush(string* memory[]){
    int time = 0;
    for(int i = 0; i < set_num; i++){
        for(int j = 0; j < set_size_block; j++){
            unsigned temp_blockaddr = (*set[i][j]).get_blockaddr();
            if(temp_blockaddr == -1){continue;}
            unsigned temp_tag = (*set[i][j]).get_tag();
            
            if((*set[i][j]).get_dirty()){
//                time += Cycles_Hit;
                time += Cycles_WriteBlock;
//                time += Cycles_ReadBlock;
                (*set[i][j]).flush(memory, temp_blockaddr, temp_tag);
            }

        }
    }
    
    
    
    return time;
    
    
}

