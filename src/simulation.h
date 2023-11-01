/******************************************************************************
 * File: simulation.h
 * Author: Guillem
 * Date: July 1, 2023
 * Description: Header file for simulation.c, which implements two Monte-Carlo
 * based probabilistic algorithms for obtaining the probabilites of
 * Texas Hold'em Games.
 ****************************************************************************/

#pragma once
#include "hand_evaluator.h"

/* These functions are meant to be called from outside the current module. */ 

int init_simulator(const char *csv_file);
int cardtype_to_num(char* card_type);


double** simulate(char* known_cards[], int num_known_cards, int num_players, int num_games);
double** simulate_spectator(char* players_cards[], char* board_cards[],char* discarded_cards[],int num_discarded_cards, int num_board_cards, int num_players, int num_games);