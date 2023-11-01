
/******************************************************************************
 * File: hand_evaluator.c
 * Author: Guillem
 * Date: July 1, 2023
 * Description: This file contains all the data structures and algorithms to
 * evaluate a Poker 5-hand. It implements the Cactus Kev design:
 * http://suffe.cool/poker/evaluator.html
 ****************************************************************************/



/*
    CODIFICATION OF CACTUS KEV CARDS INTO 4 BYTES
    Encoding Scheme:
    +--------+--------+--------+--------+
    |xxxbbbbb|bbbbbbbb|cdhsrrrr|xxpppppp|
    +--------+--------+--------+--------+
    +-----------------------------------+
    |xxxAKQJT 98765432 CDHSrrrr xxPPPPPP|
    +-----------------------------------+
    p = prime number assigned to the card (two=2, three=3, four=5, ..., Ace=41)
    r = rank of the card (two=0, three=1, four=2, five=3, ..., Ace=12), not used, they are don't cares.
    cdhs = suit of the card (1000 clubs, 0100 diamonds, 0010 hearts, 0001 spades)
    b = bit activated depending on the rank of the card in the following order (xxxAKQJT 98765432)
*/



#define FLUSH "F"
#define STRAIGHT_FLUSH "SF"
#define STRAIGHT "S"
#define HIGH_CARD "HC"
#define HIGHEST_ASCII_RANK (int)'T'
#define HIGHEST_5CARD_BIT_RANK (0x1F00 + 1) 
#define PRIME_PROD_TABLE_SIZE 4888
#define NUM_MAX_SHORT_HAND_NAME 3
#define MAX_LINE_LENGTH 64



#include "hand_evaluator.h"
#include <string.h>
#include <stdio.h>


/* Declaration of functions for look tables creation */

void create_rank_lookup_table(unsigned short card_ranks[]);
void create_flushes_lookup_table(char hands[NUM_OF_EQUIVALENCES][5], char short_hand_names[NUM_OF_EQUIVALENCES][NUM_MAX_SHORT_HAND_NAME],unsigned short flushes_table[], unsigned short coded_card_ranks[]);
void create_unique5_lookup_table(char hands[NUM_OF_EQUIVALENCES][5], char short_hand_names[NUM_OF_EQUIVALENCES][NUM_MAX_SHORT_HAND_NAME], unsigned short unique5[], unsigned short coded_card_ranks[]);
void create_prime_product_lookup_tables(char hands[NUM_OF_EQUIVALENCES][5], char short_hand_names[NUM_OF_EQUIVALENCES][NUM_MAX_SHORT_HAND_NAME], int prime_product_table[], unsigned short score_table[]);


/* Support functions declaration */

void quickSort(int arr[], int low, int high);
int binary_search(int v[], int To_Find);

/* Look up tables */

unsigned short flushes_table[HIGHEST_5CARD_BIT_RANK];
unsigned short unique5_table[HIGHEST_5CARD_BIT_RANK];
int prime_product_table[PRIME_PROD_TABLE_SIZE];
unsigned short prime_product_score_table[PRIME_PROD_TABLE_SIZE];

/* Array of full names of the equivalence table */

char full_hand_names[NUM_OF_EQUIVALENCES][MAX_LINE_LENGTH]; // array[equivalence value - 1] = "name"



/* Support arrays */

int PRIMES[NUM_RANKS] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41 };
char CARD_RANKS[NUM_RANKS] = {'2','3','4','5','6','7','8','9','T','J','Q','K','A'};

char* card_names[TOTAL_CARDS] = {   "2C","3C","4C","5C","6C","7C","8C","9C","TC","JC","QC","KC","AC",
                                        "2D","3D","4D","5D","6D","7D","8D","9D","TD","JD","QD","KD","AD",
                                        "2H","3H","4H","5H","6H","7H","8H","9H","TH","JH","QH","KH","AH",
                                        "2S","3S","4S","5S","6S","7S","8S","9S","TS","JS","QS","KS","AS"};



/**
 * @brief Procedure for creating all the lookup tables for the hand evaluator from the CSV file
 * of hand class equivalence tables.
 *
 * @param csv_file The location of the CSV file storing hand equivalence classes.
 * @param full_hand_names (Global) Full names of poker hands, indexed by score.
 * @param flushes_table (Global) lookup table containing scores for flush hands.
 * @param unique5_table (Global) lookup table containing scores for straight and high card hands.
 * @param prime_product_table (Global) Sorted array containing products of prime numbers for the hands with repeated ranks.
 * @param prime_product_score_table (Global) Array containing scores for hands with repeated hands.
 * @return -1 for failure opening the data file, 0 for success.
 */
int create_lookup_tables(const char *csv_file){
    
    /* CSV file reading and storing its information into the different structures */

    char short_hand_names[NUM_OF_EQUIVALENCES][NUM_MAX_SHORT_HAND_NAME];// array[Equivalencia - 1] = "nom"
    char hands[NUM_OF_EQUIVALENCES][5];

    // Support Lookup table

    unsigned short coded_card_ranks[HIGHEST_ASCII_RANK + 1];
    
    FILE *eq_file = fopen(csv_file, "r");
    char line[MAX_LINE_LENGTH];

    
    if (eq_file != NULL) {
        int num_eq = 0;
        while (fgets(line, sizeof(line), eq_file) != NULL) {
            char *token;
            int i = 0;
            char *saveptr = line;
            
            while ((i <= 7) && ((token = strtok_r(saveptr, ",", &saveptr)) != NULL)) {
                if (i >= 1 && i <= 5) { // 1 2 3 4 5
                    hands[num_eq][i - 1] = token[0];
                } else if (i == 6) {
                    strcpy(short_hand_names[num_eq], token);
                } else if (i == 7) {
                    strcpy(full_hand_names[num_eq], token);
                }
                i++;
            }
            num_eq++;
        }
        fclose(eq_file);
    } else {
        fprintf(stderr,"Error when reading the Equivalence classes table.\n");
        return -1;
    }

    
    // Look up tables creation

    create_rank_lookup_table(coded_card_ranks);

    create_flushes_lookup_table(hands,short_hand_names,flushes_table,coded_card_ranks);

    create_unique5_lookup_table(hands,short_hand_names,unique5_table,coded_card_ranks);
    
    create_prime_product_lookup_tables(hands,short_hand_names,prime_product_table,prime_product_score_table);

    return 0;
}

/**
 * @brief Function that obtains the score from the equivalence table of a 5-card hand
 * using all the lookup tables.
 *
 * @param cards Array of cards, encoded with the Cactus Kev encoding.
 * @param flushes_table (Global) lookup table containing the scores of flush hands.
 * @param unique5_table (Global) lookup table containing the scores of normal straights and high card hands.
 * @param prime_product_table (Global) sorted array containing the products of the prime numbers of the hands with repeated ranks.
 * @param prime_product_score_table (Global) Array containing the scores of hands with repeated ranks.
 * @return Score or rank of the equivalence class to which the hand in cards[] belongs.
 */

unsigned short get_score(int cards[]){

    if((cards[0] & cards[1] & cards[2] & cards[3] & cards[4] & 0xF000)){ // it is flush? SF, F
        return flushes_table[ ( cards[0] | cards[1] | cards[2] | cards[3] | cards[4] ) >> 16 ];
    } else if((( cards[0] | cards[1] | cards[2] | cards[3] | cards[4] ) >> 16 ) ==
            ((cards[0] >> 16) + (cards[1] >> 16) + (cards[2] >> 16) + (cards[3] >> 16) + (cards[4] >> 16))){ // ranks are unique?. S,HC
        
            return unique5_table[( cards[0] | cards[1] | cards[2] | cards[3] | cards[4] ) >> 16];
            

    } else { // 4K,3K,2P,1P, FH
            // Extraction and product of the prime numbers
            int hand_prime_product = ((cards[0] & 0x00FF) * (cards[1] & 0x00FF) * (cards[2] & 0x00FF) *
                                     (cards[3] & 0x00FF) * (cards[4] & 0x00FF));

            // Binary search over the ordered vector of prime numbers
            int idx = binary_search(prime_product_table,hand_prime_product);
            return prime_product_score_table[idx];

            
    }
    return -1;

}


/**
 * @brief Procedure for creating the table to obtain, from the representative character of the range,
 *  its bit encoding as in the 4-byte card encoding. 
 *  For example:
 *                if 'A' -> 00010000 00000000.
 *                then coded_card_ranks['A'] = 00010000 00000000.
 *
 * @param coded_card_ranks Lookup table that stores the encoded values of the card ranks.
 */
void create_rank_lookup_table(unsigned short coded_card_ranks[]){
    for(int i = 0; i < NUM_RANKS; i++){
        coded_card_ranks[CARD_RANKS[i]] = 1 << i;
    }
    
}


/**
 * @brief Creation of lookup tables for hands that do not have 5 different cards ranks. 4k,3K,2P,1P,FH.
 * 
 * 
 * @param hands Char matrix where all the hands of the equivalence table are stored.
 * @param short_hand_names Array where abbreviated names of hands from the equivalence table are stored.
 * @param prime_product_table Sorted array that contains the products of prime numbers for the hands.
 * @param score_table Array containing scores for hands that do not have 5 different cards ranks.
 */
void create_prime_product_lookup_tables(char hands[NUM_OF_EQUIVALENCES][5], char short_hand_names[NUM_OF_EQUIVALENCES][NUM_MAX_SHORT_HAND_NAME], int prime_product_table[], unsigned short score_table[]){
    int prime_product_hand_rank[PRIME_PROD_TABLE_SIZE][2]; // [][0] -> Prime product,  [][1] -> Poker hand score
    for (int i = 0,  idx = 0; i < NUM_OF_EQUIVALENCES ; i++) {
        if(!(strcmp(short_hand_names[i],STRAIGHT_FLUSH) == 0) && !(strcmp(short_hand_names[i],FLUSH) == 0) &&
        !(strcmp(short_hand_names[i],STRAIGHT) == 0) && !(strcmp(short_hand_names[i],HIGH_CARD) == 0) ){
            int res_number = 1;
            for(int j = 0; j < 5; j++){
                int k = 0;
                for(; k < NUM_RANKS; k++){
                    if(CARD_RANKS[k] == hands[i][j]) break;
                }
                res_number *= PRIMES[k];
            }
            prime_product_table[idx] =  res_number;
            prime_product_hand_rank[idx][0] = res_number;
            prime_product_hand_rank[idx][1] = i + 1;
            
            idx++;
            
        }
    }

    quickSort(prime_product_table, 0, PRIME_PROD_TABLE_SIZE - 1);


    for(int i = 0; i < PRIME_PROD_TABLE_SIZE; i++){
        for(int j = 0; j < PRIME_PROD_TABLE_SIZE; j++){
            if(prime_product_table[i] == prime_product_hand_rank[j][0]){
                score_table[i] = prime_product_hand_rank[j][1];
            }
        }
    }

}


/**
* @brief Creation of the lookup table for non-flush Straight and High Card hands.
*
*
* @param hands Char array where all the hands from the equivalence table are stored.
* @param short_hand_names Array where abbreviated names of hands from the equivalence table are stored.
* @param unique5 lookup table containing the scores of hands that are non-flush Straights and High Cards.
* @param coded_hand_ranks lookup table that stores the encoded values of card ranks.
*/
void create_unique5_lookup_table(char hands[NUM_OF_EQUIVALENCES][5], char short_hand_names[NUM_OF_EQUIVALENCES][NUM_MAX_SHORT_HAND_NAME], unsigned short unique5[], unsigned short coded_card_ranks[]){
    for(int i = 0; i < NUM_OF_EQUIVALENCES; i++){
        if((strcmp(short_hand_names[i],STRAIGHT) == 0) || (strcmp(short_hand_names[i],HIGH_CARD) == 0)){
            
            int idx = (coded_card_ranks[hands[i][0]] | coded_card_ranks[hands[i][1]] | coded_card_ranks[hands[i][2]] |
                     coded_card_ranks[hands[i][3]] | coded_card_ranks[hands[i][4]]);
            unique5[idx] = i + 1;

        }

        
    }
}


/**
 * @brief  Creation of the lookup table for flush hands.
 * 
 * 
 * @param  hands Matrix of characters where all hands of the equivalence table are stored.
 * @param  short_hand_names Array where abbreviated names of hands from the equivalence table are stored.
 * @param  flushes_table Lookup table that contains the scores of hands that are flushes.
 * @param  coded_hand_ranks Lookup table that stores the encoded values of card ranks.
 */
void create_flushes_lookup_table(char hands[NUM_OF_EQUIVALENCES][5], char short_hand_names[NUM_OF_EQUIVALENCES][NUM_MAX_SHORT_HAND_NAME], unsigned short flushes_table[], unsigned short coded_card_ranks[]){
    for(int i = 0; i < NUM_OF_EQUIVALENCES; i++){
        if((strcmp(short_hand_names[i],STRAIGHT_FLUSH) == 0) || (strcmp(short_hand_names[i],FLUSH) == 0)){
            
            int idx = (coded_card_ranks[hands[i][0]] | coded_card_ranks[hands[i][1]] | coded_card_ranks[hands[i][2]] |
                     coded_card_ranks[hands[i][3]] | coded_card_ranks[hands[i][4]]);
            flushes_table[idx] = i + 1;
        }

        
    }
}


/**
        *  @brief  Obtaining the full hand name from the score.
        *           
        * 
        * @param  score Hand score.
        * @param name variable where the hand name is saved
        * @return Full hand name.
*/
void get_full_hand_name_by_score(int score,char name[],int name_size){
    strncpy(name,full_hand_names[score],name_size - 1); //Avoid overflow buffering
    name[name_size - 1] = '\0'; // Ensure null termination
}


/**
 * @brief Binary search for an integer in an array of integers.
 * Extracted from Geeksforgeeks.org.
 *
 * @param  v Array in which the integer is to be searched.
 * @param  To_find Integer for which you want to know the position in the array.
 * @return Position in the array where the searched value has been found.
 */
int binary_search(int v[], int To_Find)
{
    int lo = 0, hi = PRIME_PROD_TABLE_SIZE - 1;
    int mid;

    while (hi - lo > 1) {
        int mid = (hi + lo) / 2;
        if (v[mid] < To_Find) {
            lo = mid + 1;
        }
        else {
            hi = mid;
        }

    }
    if (v[lo] == To_Find) {
        return lo;
    }
    else if (v[hi] == To_Find) {
        return hi;
    }
    else {
        return -1;
    }
    
}


/**
        *  @brief  Given two integer pointer swaps its contents.
        *  Extracted from https://www.javatpoint.com/quick-sort-in-c
        * 
        * @param  a value to swap
        * @param  b value to swap
*/
void swap(int* a, int* b) {  
    int t = *a;  
    *a = *b;  
    *b = t;  
}




/**
        *  @brief  Makes the partition from the Quick sort algorithm
        *  Extracted from https://www.javatpoint.com/quick-sort-in-c
        * 
        * @param  arr array in which the partition is made
        * @param  low low boundary from the partition
        * @param  high high boundary from the partition
*/
int partition(int arr[], int low, int high) {  
    int pivot = arr[high];  
    int i = (low - 1);  
  
    for (int j = low; j <= high - 1; j++) {  
        if (arr[j] < pivot) {  
            i++;  
            swap(&arr[i], &arr[j]);  
        }  
    }  
    swap(&arr[i + 1], &arr[high]);  
    return (i + 1);  
}

/**
        *  @brief  Quick sort algorithm (recursive implementation). Sorts an array of ints.
        *  Extracted from https://www.javatpoint.com/quick-sort-in-c
        * 
        * @param  arr array to sort
        * @param  low low boundary to sort the array
        * @param  high high boundary to sort the array.
*/
void quickSort(int arr[], int low, int high) {  
    if (low < high) {  
        int pi = partition(arr, low, high);  
        quickSort(arr, low, pi - 1);  
        quickSort(arr, pi + 1, high);  
    }  
}  
