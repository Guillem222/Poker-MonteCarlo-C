/******************************************************************************
 * File: hand_evaluator.h
 * Author: Guillem
 * Date: July 1, 2023
 * Description: Header file from the Poker hand evaluator implemented in
 * hand_evaluator.c. 
 ****************************************************************************/

#pragma once

#define NUM_OF_EQUIVALENCES 7462
#define NUM_RANKS 13
#define TOTAL_CARDS 52


/* These functions and structures are meant to be called from outside the current module. */ 


int create_lookup_tables(const char *csv_file);
unsigned short get_score(int cards[]);
void get_full_hand_name_by_score(int score,char name[],int name_size);

extern char* card_names[TOTAL_CARDS];
extern int PRIMES[NUM_RANKS];
