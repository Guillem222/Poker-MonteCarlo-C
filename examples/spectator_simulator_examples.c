/******************************************************************************
 * File: spectator_simulator_examples.c
 * Author: Guillem
 * Date: July 1, 2023
 * Description: Examples of using the simulation function that uses
 * the Monte Carlo method for calculating probabilities in Texas Hold'em games,
 * from a spectator perspective.
 ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "../src/simulation.h"


void print_probabilities(double** probs, int num_players);

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
    |    SPECTATOR SIMULATION TESTS    |
    ====================================
    */
    /*
       ------  TEST 1  ------
    */
    {
        int num_players = 2;
        int num_board_cards = 3;
        int num_discarded_cards = 0;
        int num_games = 50000;

        char* players_cards[] = {
                                
                                "AS","AD",
                                "JC","JD"
                                
                                
                                };
        char* board_cards[] = {"QH","2D","KD"};

        double** probs = simulate_spectator(players_cards,
                                            board_cards,
                                            NULL,
                                            num_discarded_cards,
                                            num_board_cards,
                                            num_players,
                                            num_games);
        
        printf("                              ----  TEST 1  ----                              \n");
        print_probabilities(probs,num_players);

        // Free allocated memory for probs
        for (int i = 0; i < num_players; i++) {
            free(probs[i]);
        }
        free(probs);
    
    }
    


    /*
       ------  TEST 2  ------
    */
    {

        int num_players = 3;
        int num_board_cards = 4;
        int num_discarded_cards = 4;
        int num_games = 100000;

        char* players_cards[] = {
                                
                                "4S","4D",
                                "AC","JD",
                                "2C","2S"
                                
                                
                                };
        char* board_cards[] = {"QH","AD","KD","3S"};

        char* discarded_cards[] = {"7S","2H","KC","TC"};
        double** probs = simulate_spectator(players_cards,
                                            board_cards,
                                            discarded_cards,
                                            num_discarded_cards,
                                            num_board_cards,
                                            num_players,
                                            num_games);


        printf("                              ----  TEST 2  ----                              \n");
        print_probabilities(probs,num_players); 

        // Free allocated memory for probs
        for (int i = 0; i < num_players; i++) {
            free(probs[i]);
        }
        free(probs);

    }
    
    
    /*
       ------  TEST 3  ------
    */
    {

        int num_players = 10;
        int num_board_cards = 3;
        int num_discarded_cards = 2;
        int num_games = 100000;

        char* players_cards[] = {
                                
                                "KS","QD",
                                "3S","3D",
                                "JS","JC",
                                "7S","AD",
                                "5C","5D",
                                "4S","4D",
                                "9S","9D",
                                "AC","JD",
                                "2C","2S",
                                "8S","8D"
                                
                                
                                };
        char* board_cards[] = {"2H","7D","KD"};

        char* discarded_cards[] = {"AS","AH"};
        double** probs = simulate_spectator(players_cards,
                                            board_cards,
                                            discarded_cards,
                                            num_discarded_cards,
                                            num_board_cards,
                                            num_players,
                                            num_games);


        printf("                              ----  TEST 3  ----                              \n");
        print_probabilities(probs,num_players); 

        // Free allocated memory for probs
        for (int i = 0; i < num_players; i++) {
            free(probs[i]);
        }
        free(probs);

    }

    return 0;                        
    

}


/**
 *  @brief  Procedure for printing the probabilities results
 *  for spectator simulations.
 * 
 * @param probs Matrix with probabilities of the simulated game.
 * @param num_players Number of players in the simulated game. 
 */
void print_probabilities(double** probs, int num_players){
    printf("==============================================================================\n");
    printf("|                             GAME PROBABILITIES                             |\n");
    printf("==============================================================================\n\n");
    for(int i = 0; i < num_players; i++){
        
        printf(" - Player %d: \n\n",i);
        printf("\tVictory : %f%%\n",probs[i][0]);
        printf("\tDefeat  : %f%%\n",probs[i][1]);
        printf("\tTie     : %f%%\n\n",probs[i][2]);

        printf("\tStraight Flush  : %f%% \n",probs[i][3]);
        printf("\tFour of a Kind  : %f%% \n",probs[i][4]);
        printf("\tFull House      : %f%% \n",probs[i][5]);
        printf("\tFlush           : %f%% \n",probs[i][6]);
        printf("\tStraight        : %f%% \n",probs[i][7]);
        printf("\tThree of a Kind : %f%% \n",probs[i][8]);
        printf("\tTwo Pair        : %f%% \n",probs[i][9]);
        printf("\tOne Pair        : %f%% \n",probs[i][10]);
        printf("\tHigh Card       : %f%% \n\n\n",probs[i][11]);

    }
    printf("|=============================================================================|\n");
    printf("|=============================================================================|\n\n\n\n\n");
}

