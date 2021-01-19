#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

// Name: Yagev Levi loginID: yagev

int sets;
int assoc_val;
int bytes;
int misses;
int hits;
int evictions;
int counter;

typedef struct Block { // This struct represents a cache line with a tag, valid bit, and the LRU value
    int tag;
    int valid_bit;
    int lru;
} block;

typedef struct Set { // This struct represents a Set that holds an index and a certain number of Blocks
    unsigned int index;
    block *my_set;
} set;

set *my_cache;

void create_cache(int set_bits, int assoc, int offset_bits) {

    sets = 1 << set_bits; // This does 2 to the power of the value in set_bits using a left shift
    assoc_val = assoc;
    bytes = 1 << offset_bits; // This does 2 to the power of the value in offset_bits using a left shift

    my_cache = (set*) malloc(sets * sizeof(set)); // Allocates space for all the rows in our cache

    int i = 0; // what set we are in
    int j = 0; // what column in the set we are in

    for (i=0; i < sets; i++) { // Loop sets default values for sets inside the block
        my_cache[i].index = i;
        my_cache[i].my_set = (block *) malloc(assoc * sizeof(block));

        for (j=0; j < assoc_val; j++) { // Sets the default values in the column to invalid or unsued
            my_cache[i].my_set[j].tag = -1; 
            my_cache[i].my_set[j].valid_bit = -1;
            my_cache[i].my_set[j].lru = -1;
        }
    }
}
void evict(int set_number, int tag_bits) { // Evict helper function that evicts the LRU block from the set set_number and replaces the tag with the new tag tag_bits
    int i;
    int j;
    for (i=0; i < sets; i++) { // Finds the set we need to evict from
        if (my_cache[i].index == set_number) {
            break;
        }
    }
    int smallest = my_cache[i].my_set[0].lru; // Starts with the first block as the smallest value
    int index_smallest = 0; // Holds the index to the smallest LRU
    for (j=1; j < assoc_val; j++) { // Loops through the set and updates the LRU values accordingly, finds the block with the smallest LRU
        if (my_cache[i].my_set[j].lru < smallest) {
            smallest = my_cache[i].my_set[j].lru;
            index_smallest = j;
        }
    }
    my_cache[i].my_set[index_smallest].tag = tag_bits; // Updates the tag to the new value
    my_cache[i].my_set[index_smallest].lru = counter; // Updates the LRU value because we just accessed it
    counter++;
}

void data_access(int set_number, int tag_bits) { // The data access function that finds misses, hits, or evictions at set set_number and looking for tag tag_bits
    int i;
    int j;
    for (i=0; i < sets; i++) { // Finds the appropriate set index that corresponds to set_number
        if (my_cache[i].index == set_number) {
            break;
        }
    }

    for (j=0; j < assoc_val; j++) { // First loop checks for a hit by checking to see if any tag of the blocks in set_number are equal to the tag we are looking for (tag_bits)
        if (my_cache[i].my_set[j].tag == tag_bits) {
            printf("hit ");
            hits++;
            my_cache[i].my_set[j].lru = counter;
            my_cache[i].my_set[j].valid_bit = 1;
            counter++;
            return;
        }
    }

    printf("miss "); // If the code reaches here, that means we couldn't find a hit so therefore we have a miss
    misses++;
    for (int r=0; r < assoc_val; r++) { // This loop searches for an open block in the set, if one is found it updates its values
        if (my_cache[i].my_set[r].tag == -1) {
            my_cache[i].my_set[r].tag = tag_bits;
            my_cache[i].my_set[r].valid_bit = 1;
            my_cache[i].my_set[r].lru = counter;
            counter++;
            return;
        }
    }

    printf("eviction "); // The code reaches here if no open block was found in the set, so we call our evict helper function to make room for the new tag
    evictions++;
    evict(set_number, tag_bits);
    return;
}

int main(int argc, char **argv) {
    int set_bits = 0;
    int associativity = 0;
    int offset_bits = 0;
    char *tracefile = NULL;

    char opt;
    
    while(1) {

        opt = getopt(argc, argv,"vhs:E:b:t:");

        if (opt == -1) {
            break;
        }

        switch(opt) {
            case 's' : set_bits = atoi(optarg); break;
            case 'E' : associativity = atoi(optarg); break;
            case 'b' : offset_bits = atoi(optarg); break;
            case 't' : tracefile = optarg; break;
            default : break;
        }
    }
    int tag_bits = 64 - (offset_bits + set_bits);

    create_cache(set_bits, associativity, offset_bits); // Creates the inital empty cache

    FILE *traceFILE;

    traceFILE = fopen(tracefile, "r");

    char operation;
    unsigned address;
    int size;

    while (fscanf(traceFILE, " %c %x, %d", &operation, &address, &size)>0) {
        unsigned int set_num = (address & ((0xFFFFFFFF << offset_bits) >> tag_bits)) >> offset_bits; // Isolating the bits inside the address that refer to the set number we are try to access
        int tag = (address & (0xFFFFFFFF << (offset_bits + set_bits)))>>(offset_bits + set_bits); // Isolates the tag bits
        if (operation == 'S' || operation == 'L') { // Both stores and loads access the cache so we call the data_access function that sees if we have a hit or miss
            data_access(set_num, tag);
        }
        if (operation == 'M') { // Since a Modify operation is simply two accesses, we call data_access twice
            data_access(set_num, tag);
            data_access(set_num, tag);
        }
    }

    printSummary(hits, misses, evictions); // We print the counts of our global variables
    return 0;
}
