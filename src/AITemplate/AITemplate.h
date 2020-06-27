#pragma once

#include <UltraOOXX/Wrapper/AI.h>
#include <UltraOOXX/UltraBoard.h>
#include <algorithm>
#include <random>
#include <ctime>

#include <vector>
#include <set>
#include <random>
#include <chrono>
#include <thread>
#include <cstdlib>

class AI : public AIInterface
{
    
public:
    void init(bool order) override
    {
        // any way
        Main_x = -1;
        Main_y = -1;
    }

    void callbackReportEnemy(int x, int y) override
    {
        (void) x;
        (void) y;
        // give last step
        this->Main_x = x%3;
        this->Main_y = y%3;
    }

    std::pair<int,int> queryWhereToPut(TA::UltraBoard MainBoard) override
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); //simulate computing time, can't more than 1000
        auto pair = select_random_pair(MainBoard);
        return pair;
    }
private:
    std::pair<int, int> select_random_pair (TA::UltraBoard MainBoard) {
        std::pair<int, int> random_pair;
        srand( time(NULL) );

        std::set<std::pair<int, int>> candidates;
        if ( (Main_x == -1 && Main_y == -1) || MainBoard.sub(Main_x, Main_y).full()) { // no constrain
            for (int i=0; i<9; ++i) {
                for (int j=0; j<9; ++j) {
                    if (MainBoard.get(i, j) == TA::BoardInterface::Tag::None) {
                        candidates.insert( std::make_pair(i, j) );
                    }
                }
            }
            int n = rand() % candidates.size();
            auto it = candidates.begin();
            std::advance(it, n);
            random_pair = *it;
            
        } else {
            int x = Main_x*3;
            int y = Main_y*3;
            for (int i=x; i<x+3; ++i) {
                for (int j=y; j<y+3; ++j) {
                    if (MainBoard.get(i, j) == TA::BoardInterface::Tag::None) {
                        candidates.insert( std::make_pair(i, j) );
                    }
                }
            }
            int n = rand() % candidates.size();
            auto it = candidates.begin();
            std::advance(it, n);
            random_pair = *it;
        }
        // std::cout << "random_pair.first = " << random_pair.first << "random_pair.second = " << random_pair.second << "\n";
        candidates.clear();
        return random_pair;
    }

    int Main_x;
    int Main_y;
};
