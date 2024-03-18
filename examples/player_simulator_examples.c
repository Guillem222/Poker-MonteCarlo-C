/******************************************************************************
 * File: player_simulator_examples.c
 * Author: Guillem
 * Date: July 1, 2023
 * Description: Examples of using the simulation function that uses
 * the Monte Carlo method for calculating probabilities in Texas Hold'em games,
 * from a player perspective.
 ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "../src/simulation.h"

void print_probabilities(double** probs);


int main(){
    
    printf("\n\nInitialize Simulator...\n");
    
    // Meant to be executed from the root directory of the project
    if (init_simulator("data/eq_classes.csv") == -1){
        printf("Error initializing simulator: Can't read file.\n");
        return -1;
    }

    printf("Simulator Initialized\n\n");
    
    /*
    ====================================
    |      PLAYER SIMULATION TESTS     |
    ====================================
    */
    /*
       ------  TEST 1  ------
    */
    {
        int num_players = 2;
        int num_known_cards = 2;
        int num_games = 50000;

        char* known_cards[] = {
                                
                                "2H","7S" //Player's cards
                                
                                
                                };


        double** probs = simulate_player(known_cards,num_known_cards,num_players,num_games);        
        
        printf("                              ----  TEST 1  ----                              \n");
        print_probabilities(probs); 

        // Free allocated memory for probs
        free(probs[0]);
        free(probs[1]);
        free(probs);
    
    }
    

    /*
       ------  TEST 2  ------
    */
    {
        int num_players = 5;
        int num_known_cards = 5;
        int num_games = 100000;

        char* known_cards[] = {
                                
                                "AH","JS", //Player's cards
                                "2C","JD","QH"  // Board cards
                                
                                
                                };


        double** probs = simulate_player(known_cards,num_known_cards,num_players,num_games);        
        
        printf("                              ----  TEST 2  ----                              \n");
        print_probabilities(probs); 

        // Free allocated memory for probs
        free(probs[0]);
        free(probs[1]);
        free(probs);
    
    }


    /*
       ------  TEST 3  ------
    */
    {
        int num_players = 10;
        int num_known_cards = 6;
        int num_games = 100000;

        char* known_cards[] = {
                                
                                "TH","JH", //Player's cards
                                "2C","JD","QH","KH"  // Board cards
                                
                                
                                };


        double** probs = simulate_player(known_cards,num_known_cards,num_players,num_games);        
        
        printf("                              ----  TEST 3  ----                              \n");
        print_probabilities(probs); 

        // Free allocated memory for probs
        free(probs[0]);
        free(probs[1]);
        free(probs);
    
    }


    /*
       ------  TEST 4  ------
    */
    {
        int num_players = 10;
        int num_known_cards = 7;
        int num_games = 100000;

        char* known_cards[] = {
                                
                                "TH","JH", //Player's cards
                                "2C","JD","QH","KH","AH"  // Board cards
                                
                                
                                };


        double** probs = simulate_player(known_cards,num_known_cards,num_players,num_games);        
        
        printf("                              ----  TEST 4  ----                              \n");
        print_probabilities(probs); 

        // Free allocated memory for probs
        free(probs[0]);
        free(probs[1]);
        free(probs);
    
    }

    return 0;
    

}

/**
 *  @brief  Procedure for printing the probabilities results
 *  for player perspective simulations.
 * 
 * @param probs Matrix with probabilities of the simulated game.
 */
void print_probabilities(double** probs){
    printf("==============================================================================\n");
    printf("|                             GAME PROBABILITIES                             |\n");
    printf("==============================================================================\n\n");
        
    printf(" - Player - \n\n");
    printf("\tVictory : %f%%\n",probs[0][0]);
    printf("\tDefeat  : %f%%\n",probs[0][1]);
    printf("\tTie     : %f%%\n\n",probs[0][2]);

    printf("\tStraight Flush  : %f%% \n",probs[0][3]);
    printf("\tFour of a Kind  : %f%% \n",probs[0][4]);
    printf("\tFull House      : %f%% \n",probs[0][5]);
    printf("\tFlush           : %f%% \n",probs[0][6]);
    printf("\tStraight        : %f%% \n",probs[0][7]);
    printf("\tThree of a Kind : %f%% \n",probs[0][8]);
    printf("\tTwo Pair        : %f%% \n",probs[0][9]);
    printf("\tOne Pair        : %f%% \n",probs[0][10]);
    printf("\tHigh Card       : %f%% \n\n\n",probs[0][11]);


    printf(" - Opponents - \n\n");
    printf("\tStraight Flush  : %f%% \n",probs[1][3]);
    printf("\tFour of a Kind  : %f%% \n",probs[1][4]);
    printf("\tFull House      : %f%% \n",probs[1][5]);
    printf("\tFlush           : %f%% \n",probs[1][6]);
    printf("\tStraight        : %f%% \n",probs[1][7]);
    printf("\tThree of a Kind : %f%% \n",probs[1][8]);
    printf("\tTwo Pair        : %f%% \n",probs[1][9]);
    printf("\tOne Pair        : %f%% \n",probs[1][10]);
    printf("\tHigh Card       : %f%% \n\n\n",probs[1][11]);

    
    printf("|=============================================================================|\n");
    printf("|=============================================================================|\n\n\n\n\n");
}


