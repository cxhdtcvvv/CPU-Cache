//
//  mips_mem.cpp
//  Cache
//
//  Created by Xiaohan Chen on 03/12/2014.
//  Copyright (c) 2014 xiaohan. All rights reserved.
//

#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<math.h>
#include <stdio.h>
#include<sstream>
#include "mem_sim_*.hpp"

using namespace std;

int main(int argc, const char * argv[]){
    string a = argv[1] , b = argv[2], c = argv[3], d = argv[4], e = argv[5], f = argv[6], g = argv[7], h = argv[8];
    
    int byte_addr_width = stoi(a.c_str());
    int cache_sets = stoi(b.c_str());
    int set_blocks = stoi(c.c_str());
    int block_words = stoi(d.c_str());
    int word_bytes = stoi(e.c_str());
    int cycle_hit = stoi(f.c_str());
    int cycle_read = stoi(g.c_str());
    int cycle_write = stoi(h.c_str());
    int memory_length = 1 << byte_addr_width;
    
//    int byte_addr_width = 8;
//    int cache_sets = 2;
//    int set_blocks = 2;
//    int block_words = 2;
//    int word_bytes = 2;
//    int cycle_hit = 1;
//    int cycle_read = 3;
//    int cycle_write = 5;
//    int memory_length = 1 << (byte_addr_width) ;
    
    
    int memory_bytesize = 8*1024*1024;
    
    int data_string_length = (memory_bytesize / memory_length) * 2 ;              //two char(hex) is one Byte;
    
    string** memory  = new string*[memory_length];                      //memory size is equal

    for(int i = 0; i< memory_length; i++){
        memory[i] = new string;
    }
    
    cache mips_cache = cache(set_blocks, cache_sets, block_words, word_bytes, cycle_hit, cycle_read, cycle_write);
    
    string command;
    
    int global_stamp = 0;
    while(getline(cin , command)){

        global_stamp += 1;
        
        if(command == ""){      //skip blank line
            continue;
        }
        cout<<endl;
        cout<<"# command: "<<command<<endl;
        if(command[0] == '#'){      //skip command
            continue;
            
        }else{                                                  //processing command
 
            string request = command.substr(0, command.find(' '));
            command.erase(0, request.size()+1);
                    
            string hit_bool;
            int dirty_bool;
            int time;
            if(request == "read-req"){
                
                if( command.substr(0, command.find(' ')).size() > byte_addr_width ){
                    
                    cout<<"# error: byte address width is more than "<< byte_addr_width <<"bits"<<endl;
                    continue;
                    
                }
                
                
                unsigned byte_address = stoi((command.substr(0, command.find(' '))).c_str());
                unsigned word_address = byte_address / word_bytes;
                unsigned block_address = word_address / block_words;
                unsigned index = block_address % cache_sets;
                int block_offset = word_address % block_words;
                string data_out;
                

                
                cout<<"# ";
                mips_cache.print_cache();
                cout<< "read-block: B" << block_address<<endl;
                
                data_out = (*mips_cache.find_block(byte_address, memory, hit_bool, dirty_bool, time, global_stamp)).read_data(block_offset);
                
                cout<<"read-ack data:"<<data_out<<" index:"<<index<<" "<<hit_bool<<" time:"<<time<<endl;

                
            }
            
            else if (request == "write-req"){
                
                if( command.substr(0, command.find(' ')).size() > byte_addr_width ){
                    cout<<"# error: byte address width is more than "<< byte_addr_width <<"bits"<<endl;
                    continue;
                }
                
                string temp = command.substr(0, command.find(' '));
                unsigned byte_address = stoi(temp.c_str());
                command.erase(0, temp.size()+1);
                string data_in = command.substr(0, command.find(' '));
                
                if(data_in.size() > data_string_length){
                    cout<<"# error: data overflow, too large for the memory, this command is ignored"<<endl;
                    continue;
                }
                
                unsigned word_address = byte_address / word_bytes;
                unsigned block_address = word_address / block_words;
                unsigned index = block_address % cache_sets;
                int block_offset = word_address % block_words;
                string data_out;
                
                cout<<"#";
                mips_cache.print_cache();
                cout<< "write-block: B" << block_address<<endl;
                
                (*mips_cache.find_block(byte_address, memory, hit_bool, dirty_bool, time, global_stamp)).wirte_data(block_offset, data_in);
                
                cout<<"write-ack "<<" index:"<<index<<" "<<hit_bool<<" time:"<<time<<endl;
            }
            
            else if(request == "flush-req"){
                time = 0;
                time = mips_cache.all_flush(memory);
                cout<<"flush-ack "<<time<<endl;
            }
            
            else if(request == "debug-req"){
                cout<<"debug-ack-begin"<<endl;
                cout<<"debug-ack-end"<<endl;
            }
            
        }
    }
    
    mips_cache.print_cache();
    cout<<"# End of command"<<endl;
//    input_file.close();
    
    for(int i = 0; i< memory_length; i++){      //delete memory
        delete memory[i];
    }
    
    delete memory;
    
    return 0;
    
}





