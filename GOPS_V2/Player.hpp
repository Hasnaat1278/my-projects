class Player{
public:    
    explicit Player(int (*strat)(int a[], int b[], int val, int pot));
    int (*strategy)(int myC[], int opC[], int val, int pot);
    void restart();
    void setBid(int val);
    int getBid();
    int* getHand();
    void setOppBid(int ob);
    void update();
    void addScore(int a);
    int getScore();
    float getWins();
    void addWin();
    
private:
    int points;
    int myCards[14] = {0};
    int oppCards[14] = {0};
    int bid;
    int oppBid;
    int wins = 0;
};