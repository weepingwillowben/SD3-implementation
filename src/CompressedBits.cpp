#include <cassert>
#include "CompressedBits.h"

void BlockSet::add(int64_t element){
    assert(element < BLOCK_SIZE && element >= 0);
    bits[element] = true;
}
bool BlockSet::has(int64_t element){
    assert(element < BLOCK_SIZE && element >= 0);
    return bits[element];
}
void BlockSet::operator |= (const BlockSet &outer){
    bits |= outer.bits;
}
void BlockSet::operator &= (const BlockSet &  outer){
    bits &= outer.bits;
}
void BlockSet::subtract(const BlockSet & outer){
    bits &= ~outer.bits;
}
bool BlockSet::any(){
    return bits.any();
}
int64_t BlockSet::count(){
    return bits.count();
}


void CompressedBits::add(int64_t element){
    data[uint64_t(element) / BLOCK_SIZE].add(uint64_t(element) % BLOCK_SIZE);
}

void CompressedBits::add_block(int64_t element,int64_t size){
    for(int64_t i = 0; i < size; i++){
        this->add(element+i);
    }
}
bool CompressedBits::has(int64_t element){
    return data[uint64_t(element) / BLOCK_SIZE].has(uint64_t(element) % BLOCK_SIZE); 
}

bool CompressedBits::has_all_block(int64_t element,int64_t size){
    for(int64_t i = 0; i < size; i++){
        if(!has(i+element)){
            return false;
        }
    }
    return true;
}
bool CompressedBits::has_any_in_block(int64_t element,int64_t size){
    for(int64_t i = 0; i < size; i++){
        if(has(i+element)){
            return true;
        }
    }
    return false;
}    
void CompressedBits::and_with_optional_neg(CompressedBits & outer,bool neg){
    for(set_iterator iter = data.begin(); iter != data.end(); ){
        int64_t key = iter->first;
        BlockSet & value = iter->second;
        if(outer.data.count(key)){
            if(!neg){
                value &= outer.data[key];
            }
            else{
                value.subtract(outer.data[key]);
            }
            if(!value.any()){
                //be careful, erases current element. 
                data.erase(iter++);
            }
            else{
                ++iter;
            }
        }else{
            //be careful, erases current element.
            data.erase(iter++);
        }
        //if(data.size() == 0){
        //    break;
        //}
    }
}
void CompressedBits::operator &=(CompressedBits & outer){
    and_with_optional_neg(outer,false);
}

void CompressedBits::subtract(CompressedBits & outer){
    and_with_optional_neg(outer,true);
}
void CompressedBits::operator|=(CompressedBits & outer){
    for(set_iterator iter = outer.data.begin(); iter != outer.data.end(); ++iter){
        int64_t key = iter->first;
        if(this->data.count(key)){
            this->data[key] |= iter->second;
        }
        else{
            this->data[key] = iter->second;
        }
    }
}

bool CompressedBits::any(){
    for(set_iterator iter = data.begin(); iter != data.end(); ++iter){
        if(iter->second.any()){
            return true;
        }
    }
    return false;
}
int64_t CompressedBits::count(){
    int64_t sum = 0;
    for(set_iterator iter = data.begin(); iter != data.end(); ++iter){
        sum += iter->second.count();
    }
    return sum;
}
void CompressedBits::clear(){
    data.clear();
}
