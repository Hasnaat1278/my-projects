#include <iostream>
#include "Player.hpp"

Player::Player(int (*strat)(int a[], int b[], int val, int pot)) : strategy{strat} {
	restart();
} // constructor

void Player::restart() {
	points = 0;
	for(int i = 1; i <= 13; i++) {
		myCards[i] = i;
		oppCards[i] = i;
	}
} // resets cards in hand

void Player::setBid(int val) {
	try { // checks if the card you select is valid
		if(val >= 1 && val <= 13 && myCards[val] != 0)
			bid = val;
		else
			throw(val);
	} catch(int a) {
		std::cout << "Exception occured: You do not have this card in your hand";
		exit(0); // code exits if you attmpt to use a card you don't have
	}
} // sets the bid for the turn

int* Player::getHand() {
	return myCards;
} // getter for the hand

void Player::setOppBid(int ob) {
	oppBid = ob;
} // getter for opponents bid (set after everyone bids)

void Player::addScore(int a) {
	points += a;
} // increment score by the value of pot

int Player::getScore() {
	return points;
} // get score this game

void Player::update() {
	myCards[bid] = 0;
	oppCards[oppBid] = 0;
} // resets bids

int Player::getBid() {
	return bid;
} // getter for my mad

float Player::getWins() {
	return wins;
} // get wins

void Player::addWin() {
	wins++;
} // increment wins
