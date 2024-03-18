/******************************************************************************
 * File: simulation.c
 * Author: Guillem
 * Date: July 1, 2023
 * Description: This file implements two Monte-Carlo based probabilistic algorithms
 * for obtaining the probabilites of Texas Hold'em Games.
 * The first method provides results from a player's perspective,
 * while the second method provides results from a spectator's perspective.
 ****************************************************************************/

#include "simulation.h"

#include <stdlib.h>
#include <time.h>


#define PERMUTATIONS 21
#define NUM_OF_HAND_TYPES 9


void shuffle(int *array, size_t n);
void init_deck(int deck[]);
void init_score_to_hand_num();

/* Deck of cards */

int deck[TOTAL_CARDS];

/* Support array, see init_score_to_hand_num() function */

unsigned char score_hand_to_num[NUM_OF_EQUIVALENCES + 1];

/* All possible groups of 5 cards from a set of 7, without repetition. choose(7,5) */

int groups_5[PERMUTATIONS][5] =
{
    { 0, 1, 2, 3, 4 },
    { 0, 1, 2, 3, 5 },
    { 0, 1, 2, 3, 6 },
    { 0, 1, 2, 4, 5 },
    { 0, 1, 2, 4, 6 },
    { 0, 1, 2, 5, 6 },
    { 0, 1, 3, 4, 5 },
    { 0, 1, 3, 4, 6 },
    { 0, 1, 3, 5, 6 },
    { 0, 1, 4, 5, 6 },
    { 0, 2, 3, 4, 5 },
    { 0, 2, 3, 4, 6 },
    { 0, 2, 3, 5, 6 },
    { 0, 2, 4, 5, 6 },
    { 0, 3, 4, 5, 6 },
    { 1, 2, 3, 4, 5 },
    { 1, 2, 3, 4, 6 },
    { 1, 2, 3, 5, 6 },
    { 1, 2, 4, 5, 6 },
    { 1, 3, 4, 5, 6 },
    { 2, 3, 4, 5, 6 }
};


/**
 * @brief Calculation of the user's probability of winning, losing, and tying in poker games by simulating them.
 *  The games are played from the user's perspective, meaning the player's cards and the community cards on the table
 *  are fixed. Both the cards of other players and the table cards that have not yet been revealed are randomly selected.
 *
 * @param known_cards Cards that are known, including both user's and community cards.
 * known_cards[0..1] = player's cards, known_cards[2..n] = community cards.
 * @param num_known_cards The number of known cards, i.e., the sum of the user's cards and the cards that have been revealed on the table.
 * @param num_players The number of players.
 * @param num_games The number of games to be simulated.
 * @return A matrix storing the probabilities of winning, losing, and tying [0][0..2],
 * and the probabilities of obtaining various poker hands. [0..1][3..11].
 * [0][...] represents the user's probabilities, and [1][3..11] represents the opponent's probabilities.
 * [1][0..2] are not used because they are the complementary of the user's.
 */
double** simulate_player(char* known_cards[], int num_known_cards, int num_players, int num_games){




    /*  
        [0][...] User player
        [1][...] Opponents

        [1][0] Don't care
        [1][1] Don't care
        [1][2] Don't care

        [0][0] Victory
        [0][1] Defeat
        [0][2] Tie

        [...][3] Straight Flush
        [...][4] Four of a Kind
        [...][5] Full Houses
        [...][6] Flush
        [...][7] Straight
        [...][8] Three of a Kind
        [...][9] Two Pair
        [...][10] One Pair
        [...][11] High Card
     */

    double** probabilities = (double**) malloc(2 * sizeof(double*));
    probabilities[0] = (double*) malloc((3 + NUM_OF_HAND_TYPES) * sizeof(double));
    probabilities[1] = (double*) malloc((3 + NUM_OF_HAND_TYPES) * sizeof(double));


    srand (time(NULL));

    
    int num_unknown_cards = TOTAL_CARDS - num_known_cards; 
    
    int known_cards_num[num_known_cards];

    /* We transform the cards from 3 char string to [0,51] integers */

    for(int i = 0; i < (num_known_cards);i++){
        known_cards_num[i] = (int) cardtype_to_num(known_cards[i]);
    }


    

    /* Create a deck with all the cards except those that are known. */

    int random_vec[num_unknown_cards]; // Baralla
    for(int i = 0, founded = 0; i < TOTAL_CARDS; i++){
        char equal = 0;
        for(int j = 0; j < (num_known_cards);j++){
            if(known_cards_num[j] == i){
                equal = 1;
                founded++;
                break;
            }
        }
        if(equal == 0) random_vec[i - founded] = i;
        
    }


    // players_cards[0][0..1] = user cards

    int players_cards[num_players][2];

    players_cards[0][0] = known_cards_num[0];
    players_cards[0][1] = known_cards_num[1];



    /* --- Game simulations --- */

    int num_of_wins[num_players];
    int num_of_draws[num_players];
    int num_of_hand_types_user[NUM_OF_HAND_TYPES];
    int num_of_hand_types_opponents[NUM_OF_HAND_TYPES];

    /* Counter arrays initialization */

    for(int i = 0; i < NUM_OF_HAND_TYPES;i++){
        num_of_hand_types_user[i] = 0;
        num_of_hand_types_opponents[i] = 0;
    }
    for(int i = 0; i < num_players; i++){
        num_of_wins[i] = 0;
        num_of_draws[i] = 0;
    } 


    for(int it = 0; it < num_games;it++){

        /* We shuffle the deck */

        shuffle(random_vec,num_unknown_cards);

        /* Dealing cards to the opponents */

        int given_cards = 0;

        for(int i = 1; i < num_players;i++){
            players_cards[i][0] = random_vec[given_cards++];
            players_cards[i][1] = random_vec[given_cards++];
        }


       /* We calculate the best hand (score) for each player and the winner. */

        unsigned short best_score_game = 0xFFFF;
        unsigned short player_i_best_score[num_players];
        for(int i = 0; i < num_players;i++) player_i_best_score[i] = 0xFFFF;
        int winner = -1;

        for(int player_i = 0; player_i < num_players; player_i++){

            int seven_card_hand[7];
            
            /* We put the cards of the player we are processing into our hand */

            seven_card_hand[0] = players_cards[player_i][0];
            seven_card_hand[1] = players_cards[player_i][1];

            /* We collect the cards on the table (if there are any) into the hand */

            for(int j = 2; j < num_known_cards;j++){
                seven_card_hand[j] = known_cards_num[j];
            }

            /* If there are cards missing to complete a 7-card hand, we add some from the random vector */

            for(int j = num_known_cards, k = 0; j < 7; j++, k++){
                // It is important that all players receive the same cards at this point
                seven_card_hand[j] = random_vec[given_cards + k];
            }

            /* We calculate the score of the hand for each group of 5 cards from the 7-card hand of player_i */
            
            int cards[5];
            int player_i_best_hand;
            

            for(int i = 0; i < PERMUTATIONS; i++){

                
                
                for(int j = 0; j < 5;j++){
                    cards[j] = deck[seven_card_hand[groups_5[i][j]]];
                }
                

                int rank = get_score(cards);

                if(rank < player_i_best_score[player_i]){
                    player_i_best_score[player_i] = rank;
                    player_i_best_hand = i;
                }

            }

            if(player_i == 0){
                num_of_hand_types_user[score_hand_to_num[player_i_best_score[player_i]]]++;
            } else {
                num_of_hand_types_opponents[score_hand_to_num[player_i_best_score[player_i]]]++;
            }

            if(player_i_best_score[player_i] < best_score_game){
                best_score_game = player_i_best_score[player_i];
                winner = player_i;
            }
            
            
        }

        /* We sum up the wins and ties */

        int num_of_winners = 0;
        for(int i = 0; i < num_players;i++){
            if(player_i_best_score[i] == best_score_game){ num_of_winners++; }
        }
        
        if(num_of_winners == 1){
            num_of_wins[winner]++;
        } else {
            for(int i = 0; i < num_players;i++){
                if(player_i_best_score[i] == best_score_game){ num_of_draws[i]++; }
            }
        }

        
        
    }


    
    // probabilities[0][0] = % victory
    // probabilities[0][1] = % defeat
    // probabilities[0][2] = % tie
    
    probabilities[0][0] = ((double) num_of_wins[0] / (double) num_games) * 100.0;
    probabilities[0][1] = ((double) (num_games - num_of_wins[0] - num_of_draws[0]) / (double) num_games) * 100.0;
    probabilities[0][2] = ((double) num_of_draws[0] / (double) num_games) * 100.0;

    for(int i = 0; i < NUM_OF_HAND_TYPES;i++){
        probabilities[0][i + 3] = ((double) num_of_hand_types_user[i] / (double) num_games) * 100.0;
        probabilities[1][i + 3] = (((double) num_of_hand_types_opponents[i] / (double) (num_players - 1)) / (double) num_games) * 100.0;
    }


    return probabilities;


}


/**
 * @brief Calculation of the probability of winning, losing, and tying for each player from a spectator's perspective.
 * The spectator is aware of the cards held by all the players and those on the table.
 *
 * Probabilities are derived from simulating poker games. Games are played with all the active players' cards and the
 * community cards, discarding the cards of players who have folded from the deck and randomly selecting the community
 * cards that have not yet appeared.
 *
 * @param player_cards Cards of the active players. players_cards[0..1] = first player's cards,
 * players_cards[2..3] = second player's cards, and so on.
 * @param board_cards Community cards.
 * @param discarded_cards Discarded cards. These are the cards that were in the hands of players who
 * no longer play (have folded).
 * @param num_discarded_cards Number of discarded cards.
 * @param num_board_cards Number of community cards.
 * @param num_players Number of players.
 * @param num_games Number of games to simulate.
 * @return A matrix with as many rows as there are players and three columns. In row "i," you will find the probabilities
 * of winning (rtn[i][0]), losing (rtn[i][1]), and tying (rtn[i][2]) for player "i." and for rtn[i][3...11] the probabilities
 * of having each of the different types of poker hands.
 */
double** simulate_spectator(char* players_cards[], char* board_cards[], char* discarded_cards[],int num_discarded_cards, int num_board_cards, int num_players, int num_games){

    // Memory allocation for the probabilities

    /*  
        [...][0] Victory
        [...][1] Defeat
        [...][2] Tie

        [...][3] Straight Flush
        [...][4] Four of a Kind
        [...][5] Full Houses
        [...][6] Flush
        [...][7] Straight
        [...][8] Three of a Kind
        [...][9] Two Pair
        [...][10] One Pair
        [...][11] High Card
     */


    
    double** probabilities = (double**) malloc(num_players * sizeof(double*));
    for(int i = 0; i < num_players;i++) probabilities[i] = (double*) malloc((3 + NUM_OF_HAND_TYPES) * sizeof(double));


    srand (time(NULL));

    
    int num_known_cards = num_players * 2 + num_board_cards + num_discarded_cards;
    
    int num_unknown_cards = TOTAL_CARDS - num_known_cards; 
    
    int players_cards_num[num_players][2];
    int board_cards_num[num_board_cards];
    int discarded_cards_num[num_discarded_cards];
    int seven_card_hand[num_players][7];


    /* We convert the cards strings into integers from 0 to 51 */
    /* We add to each player's hand of 7 cards the cards that are known */


    for(int i = 0; i < num_discarded_cards;i++){
        discarded_cards_num[i] = cardtype_to_num(discarded_cards[i]);
    }

    for(int i = 0; i < (num_players);i++){
        players_cards_num[i][0] = cardtype_to_num(players_cards[i * 2]);
        players_cards_num[i][1] = cardtype_to_num(players_cards[i * 2 + 1]);
        seven_card_hand[i][0] = players_cards_num[i][0];
        seven_card_hand[i][1] = players_cards_num[i][1];
    }

    for(int i = 0; i < num_board_cards;i++){
        board_cards_num[i] = cardtype_to_num(board_cards[i]);
        for(int j = 0; j < num_players;j++){
            seven_card_hand[j][i + 2] = board_cards_num[i];
        }
    }


    

    /* Create a deck with all the cards except the ones the spectator knows */

    int random_vec[num_unknown_cards]; // Deck
    for(int i = 0, founded = 0; i < TOTAL_CARDS; i++){
        char equal = 0;
        for(int j = 0; j < (num_board_cards);j++){
            if(board_cards_num[j] == i){
                equal = 1;
                founded++;
                break;
            }
        }
        if(equal == 0){
            for(int j = 0; j < (num_players);j++){
                if(players_cards_num[j][0] == i || players_cards_num[j][1] == i){
                    equal = 1;
                    founded++;
                    break;
                }
            }
        }
        if(equal == 0){
            for(int j = 0; j < (num_discarded_cards);j++){
                if(discarded_cards_num[j] == i){
                    equal = 1;
                    founded++;
                    break;
                }
            }   
        }
        
        if(equal == 0) random_vec[i - founded] = i;
        
    }



    /* --- Game simulations --- */

    int num_of_wins[num_players];
    int num_of_draws[num_players];
    int num_of_hand_types[num_players][NUM_OF_HAND_TYPES];
    for(int i = 0; i < num_players; i++){
        num_of_wins[i] = 0;
        num_of_draws[i] = 0;
        for(int j = 0; j < NUM_OF_HAND_TYPES;j++) num_of_hand_types[i][j] = 0;
    } 


    for(int it = 0; it < num_games;it++){

        // We shuffle the deck
        shuffle(random_vec,num_unknown_cards);


        

       /* Calculating the best hand (score) for each player and the winner */

        unsigned short best_score_game = 0xFFFF;
        unsigned short player_i_best_score[num_players];
        for(int i = 0; i < num_players;i++) player_i_best_score[i] = 0xFFFF;
        int winner = -1;

        for(int player_i = 0; player_i < num_players; player_i++){


            /* If there are cards left to complete the hand of 7 cards, we add some from the random vector. */

            for(int j = (num_board_cards + 2), k = 0; j < 7; j++, k++){
                // It is important that all players receive the same cards at this point
                seven_card_hand[player_i][j] = random_vec[k];
            }

            /* We calculate the score of the hand for each group of 5 cards from the 7-card hand of player_i*/
            
            int cards[5];
            int player_i_best_hand;
            

            for(int i = 0; i < PERMUTATIONS; i++){
                
                for(int j = 0; j < 5;j++){
                    cards[j] = deck[seven_card_hand[player_i][groups_5[i][j]]];
                }

                int rank = get_score(cards);

                if(rank < player_i_best_score[player_i]){
                    player_i_best_score[player_i] = rank;
                    player_i_best_hand = i;
                }

            }

            num_of_hand_types[player_i][score_hand_to_num[player_i_best_score[player_i]]]++;

            if(player_i_best_score[player_i] < best_score_game){
                best_score_game = player_i_best_score[player_i];
                winner = player_i;
            }
            
        }

        /* We sum up the wins and ties */

        int num_of_winners = 0;
        for(int i = 0; i < num_players;i++){
            if(player_i_best_score[i] == best_score_game){ num_of_winners++; }
        }
        
        if(num_of_winners == 1){
            num_of_wins[winner]++;
        } else {
            for(int i = 0; i < num_players;i++){
                if(player_i_best_score[i] == best_score_game){ num_of_draws[i]++; }
            }
        }


        
    }

    for(int i = 0; i < num_players;i++){
        probabilities[i][0] = ((double) num_of_wins[i] / (double) num_games) * 100.0;
        probabilities[i][1] = ((double) (num_games - num_of_wins[i] - num_of_draws[i]) / (double) num_games) * 100.0;
        probabilities[i][2] = ((double) num_of_draws[i] / (double) num_games) * 100.0;
        for(int j = 0; j < NUM_OF_HAND_TYPES;j++){
            probabilities[i][j + 3] = ((double) num_of_hand_types[i][j] / (double) num_games) * 100.0;
        }
    }

    return probabilities;


}


/**
 * @brief  Necessary initializations for the proper functioning of the simulator.
 * Initialization of tables and the encoded deck.
 *  
 * @param  csv_file Address of the CSV file where equivalence classes of hands are stored.
 * @return -1 if reading csv_file fails, 0 if success.
 */
int init_simulator(const char *csv_file){
    if (create_lookup_tables(csv_file) == -1){
        return -1;
    }
    init_deck(deck);
    init_score_to_hand_num();

    return 0;
}



/**
 * @brief  Initialization of the deck of cards. Each card
 * follows the following encoding scheme in 4 Bytes of Cactus Kev:
 *
 * |xxxbbbbb|bbbbbbbb|cdhsrrrr|xxpppppp|
 *
 * Where
 * p = the prime number assigned to the card (two=2, three=3, four=5,..., Ace=41),
 * r = the rank of the card (two=0, three=1, four=2, five=3,..., Ace=12), not used, they are don't cares,
 * cdhs = the suit of the card (1000 clubs, 0100 diamonds, 0010 hearts, 0001 spades),
 * b = a bit activated based on the rank of the card in the following order (xxxAKQJT 98765432)
 *
 * @param  deck Array where the deck is to be stored.
 */
void init_deck(int deck[])
{
    int i, j, n = 0, suit = 0x8000;

    for (i = 0; i < 4; i++, suit >>= 1)
        for (j = 0; j < 13; j++, n++)
            deck[n] = PRIMES[j] | (j << 8) | suit | (1 << (16+j));
}



/**
        *   @brief  Initializes the global support array score_hand_to_num.
        *    It uses the order of the different equivalence classes.
        * 
        *    Straight Flush ->  0
        *    Four of a Kind ->  1
        *    Full Houses    ->  2
        *    Flush          ->  3
        *    Straight       ->  4
        *    Three of a Kind->  5 
        *    Two Pair       ->  6
        *    One Pair       ->  7
        *    High Card      ->  8
*/
void init_score_to_hand_num(){
    int i = 1;
    for(; i < 11;i++){
        score_hand_to_num[i] = 0;
    }
    for(; i < 167;i++){
        score_hand_to_num[i] = 1;
    }
    for(; i < 323;i++){
        score_hand_to_num[i] = 2;
    }
    for(; i < 1600;i++){
        score_hand_to_num[i] = 3;
    }
    for(; i < 1610;i++){
        score_hand_to_num[i] = 4;
    }
    for(; i < 2468;i++){
        score_hand_to_num[i] = 5;
    }
    for(; i < 3326;i++){
        score_hand_to_num[i] = 6;
    }
    for(; i < 6186;i++){
        score_hand_to_num[i] = 7;
    }
    for(; i < (NUM_OF_EQUIVALENCES + 1);i++){
        score_hand_to_num[i] = 8;
    }

}   


/**
 *  @brief  Converts the English encoding of poker cards to a numerical encoding.
 *          The resulting number represents the index in the deck[] array.
 * 
 * @param  card_type  Two-character English encoding of the card. Ex: "AC" -> Ace of clubs.
 * @return Index of the card in the deck array.
 */
int cardtype_to_num(char* card_type){
    char suit = card_type[1];
    char type = card_type[0];
    unsigned long num = 0;
    // Increment of suit
    switch (suit)
    {
    case 'C':
        num += 0;
        break;
    case 'D':
        num += 13;
        break;
    case 'H':
        num += 26;
        break;
    case 'S':
        num += 39;
        break;
    default:
        break;
    }
    // Increment of type (-2 because it begins with 0 and Ace is the last)

    switch (type)
    {
    case 'J':
        num += 11 - 2;
        break;
    case 'Q':
        num += 12 - 2;
        break;
    case 'K':
        num += 13 - 2;
        break;
    case 'A':
        num += 14 - 2;
        break;
    case 'T':
        num += 10 - 2;
        break;
    
    default:
        //is a number
        num += type - '0' - 2;
        break;
    }
    
    return num;
}


/**
 *  @brief  Shuffles an array of ints, using the benpfaff's method:
 *  https://benpfaff.org/writings/clc/shuffle.html
 * 
 * @param array array to be sorted
 * @param  n  size of the array to be sorted
 */
void shuffle(int *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}


