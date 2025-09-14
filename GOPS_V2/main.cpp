#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include "Player.hpp" // vscode is dumb and wants me to write .cpp, so if this doesn't work try this fix
#include "DefaultStrategies" // some premade strategies to test your function against
// header stuff

const unsigned reps = 1000; // number of games to play

using namespace std;

void shuffle(int[]); // dont worry about this now, all will be revealed
void debugGame(int, int, int);

void printSequence(int arr[]){
    for(int i = 0; i < 13; i++){
        cout << arr[i] << " ";
    }
} // for seeing how each game played out (the sequence that the cards were drawn)

int main(){

    srand(time(NULL));

    Player p1{&randomSequence};
    Player p2{&plusN};
    // the two strategy functions go here. the &function is the adress of a strategy function

    int values[13] = {0};
    int turn = 0;
    int pot;

    int trials = reps;

    while(trials > 0){
        initStrats(); // this is for setting up the default strategies
        
        for(int i = 0; i < 13; i++){
            values[i] = i+1;
        } // setting up the 13 cards to be dealt

        shuffle(values); // shuffle the cards so dealing order is random
        pot = 0;
        p1.restart();
        p2.restart();

        for(int i = 0; i < 13; i++){ // going through a game

            pot += values[i]; // this is for ties, the pot is the previous pot plus the new card

            p1.setBid(p1.strategy(p1.getHand(), p2.getHand(), values[i], pot));
            p2.setBid(p2.strategy(p2.getHand(), p1.getHand(), values[i], pot));
            // run each strategy
            
            // debugGame(pot, p1.getBid(), p2.getBid());
            // this line can be used to debug exactly what each player did for a specific turn


            if(p1.getBid() > p2.getBid()){
                p1.addScore(pot);
                pot = 0;
            } else if(p1.getBid() < p2.getBid()){
                p2.addScore(pot);
                pot = 0;
            } // check who won the bid
            
            p1.setOppBid(p2.getBid());
            p2.setOppBid(p1.getBid());

            p1.update();
            p2.update();     
            // update the information about which cards was used this round for each player
        }

        if(p1.getScore() > p2.getScore()){
            p1.addWin();
        } else if(p1.getScore() < p2.getScore()){
            p2.addWin();
        } // after game is played, check who won and increment the number of wins for the winner
        
        cout << "Game #" << reps - trials + 1 << ": ";
        printSequence(values); // this prints the sequence of cards for the game
        cout << ": " << p1.getScore() << " - " << p2.getScore() << endl; // prints the score for each player
        // this block of 3 lines is also optional and only used for displaying the game


        trials--; // decrement goes BRRRR
    }

    cout << endl;
    cout << setprecision(3) << fixed; // set to 3 decimals, can be edited to suit needs
    cout << "Player 1: " << (static_cast<double>(p1.getWins()*100.00)/reps) << endl;
    cout << "Player 2: " << (static_cast<double>(p2.getWins()*100)/reps);
    // display overall data neatly (in my opinion)
    
    /* ----------------------------------------------------------------------------------------
    Note: the sum of win percentages may not be equal in the case there is a tie, where
    at some point the game was tied and each player played the same cards for the last few
    turns, in this case the game ends as a tie and nobody wins the last few cards 
     ------------------------------------------------------------------------------------------*/

    return 0; // we are FINALLY done
}

void shuffle(int arr[]){
    int temp, j;
    for(int i = 0; i < 13; i++){
        j = rand() % 13;
        temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
} // standard shuffle algorithm

void debugGame(int pot, int bid1, int bid2){
cout << "Pot:" << setw(3) << pot <<
"  Player 1 Bid:" << setw(3) << bid1 <<
"  Player 2 Bid:" << setw(3) << bid2 << endl;
} // this outputs more information in this middle of rounds for debugging strategies