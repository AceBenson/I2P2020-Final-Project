#pragma once

#include <UltraOOXX/Wrapper/AI.h>
#include <UltraOOXX/UltraBoard.h>
#include <GUI/GUIInterface.h>
#include <algorithm>
#include <random>
#include <ctime>

#include <vector>
#include <set>
#include <random>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <stack>


class AI : public AIInterface
{
    
public:

    void init(bool order) override // prepareState will call this
    {
        // any way
        // Main_x = -1;
        // Main_y = -1;
        main_pos = std::make_pair(-1, -1);
        // last_Main_x = -1;
        // last_Main_y = -1;
        is_Fisrt = order;

        gui = new TA::ASCII;
    }

    void callbackReportEnemy(int x, int y) override
    {
        (void) x;
        (void) y;
        // give last step
        main_pos = std::make_pair(x%3, y%3);
        last_single_main_pos = std::make_pair(x/3, y/3);
        if(last_main_pos.empty())
            last_main_pos.push(std::make_pair(x/3, y/3));
        else if(std::make_pair(x/3, y/3) != last_main_pos.top())
            last_main_pos.push(std::make_pair(x/3, y/3));
    }

    std::pair<int,int> queryWhereToPut(TA::UltraBoard MainBoard) override
    {
        if(!is_Fisrt) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200)); //simulate computing time, can't more than 1000
            auto pair = select_random_pair(MainBoard);
            return pair;
        } else { // predefined answer
            std::this_thread::sleep_for(std::chrono::milliseconds(200)); //simulate computing time, can't more than 1000
            auto pair = predefined_pair(MainBoard);
            return pair;
        }
    }
private:
    std::pair<int, int> select_random_pair (TA::UltraBoard MainBoard) {
        std::pair<int, int> random_pair;
        srand( time(NULL) );

        std::set<std::pair<int, int>> candidates;
        if ( (main_pos.first == -1 && main_pos.second == -1) || MainBoard.sub(main_pos.first, main_pos.second).full() ) { // no constrain
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
            int x = main_pos.first*3;
            int y = main_pos.second*3;
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

    std::pair<int, int> predefined_pair (TA::UltraBoard MainBoard) {
        std::pair<int, int> my_pair;

        if ( (main_pos.first == -1 && main_pos.second == -1) ) { // first move;
            my_pair = std::make_pair(4, 4);
            return my_pair;
        }

        while(!last_main_pos.empty() && MainBoard.sub(last_main_pos.top().first, last_main_pos.top().second).full()) {
            std::cout << "POP last_main_pos.top() = " << last_main_pos.top().first << " " << last_main_pos.top().second << "\n";
            last_main_pos.pop();
            if(!last_main_pos.empty()) {
                std::cout << "new last_main_pos.top() = " << last_main_pos.top().first << " " << last_main_pos.top().second << "\n";
            }
        }
        if (MainBoard.sub(main_pos.first, main_pos.second).full()) { // no constrain
            std::cout << "No constrain\n";
            if(!last_main_pos.empty()) { // I want to send him back
                std::cout << "I want to send him back to " << last_main_pos.top().first << " " << last_main_pos.top().second << "\n";
                my_pair = send_him_back(MainBoard, false, last_main_pos.top());
            } else {
                std::cout << "I don't want to send him back\n";
                // 但是還是要找對面的
                int diagonalX = 2 - last_single_main_pos.first;
                int diagonalY = 2 - last_single_main_pos.second;
                my_pair = select_from_the_same_block(MainBoard, std::make_pair(diagonalX, diagonalY));
                if(my_pair != std::make_pair(-1, -1)) {
                    for (int i=0; i<3; ++i) {
                        for (int j=0; j<3; ++j) {
                            my_pair = select_from_the_same_block(MainBoard, std::make_pair(i, j));
                            if(my_pair != std::make_pair(-1, -1)) 
                                break;
                        }
                    }
                    if (my_pair == std::make_pair(-1, -1)) 
                        my_pair = select_random_pair(MainBoard);
                }
            }
        } else { // constrain to main_pos
            std::cout << "Constrain to main_pos\n";
            if(!last_main_pos.empty()) { // I want to send him back
                std::cout << "I want to send him back to " << last_main_pos.top().first << " " << last_main_pos.top().second << "\n";
                my_pair = send_him_back(MainBoard, true, last_main_pos.top());
            } else { // I don't want to send him back, otherwise he can choose wherever he want.
                std::cout << "I don't want to send him back\n";
                my_pair = select_from_the_same_block(MainBoard, main_pos);
                if (my_pair == std::make_pair(-1, -1)) 
                    my_pair = select_random_pair(MainBoard);
            }
        }

        return my_pair;
    }

    std::pair<int, int> send_him_back(TA::UltraBoard MainBoard, bool isConstrained, std::pair<int, int> goal_main_pos) {
        int Main_x;
        int Main_y;
        if (!isConstrained) {
            Main_x = 2 - goal_main_pos.first; // diagonal main_x
            Main_y = 2 - goal_main_pos.second; // diagonal main_y
        } else {
            Main_x = main_pos.first;
            Main_y = main_pos.second;
        }
        int tempX = Main_x*3 + goal_main_pos.first; // the position which I can send him back
        int tempY = Main_y*3 + goal_main_pos.second; // the position which I can send him back
        if( MainBoard.get(tempX, tempY) ==  TA::BoardInterface::Tag::None) { // I can send him back;
            return std::make_pair(tempX, tempY);
        } else { // I can't send him back
            if (!isConstrained) {
                for (int i=0; i<3; ++i) {
                    for (int j=0; j<3; ++j) {
                        tempX = i*3 + goal_main_pos.first; // the position which I can send him back
                        tempY = j*3 + goal_main_pos.second; // the position which I can send him back
                        if( MainBoard.get(tempX, tempY) ==  TA::BoardInterface::Tag::None) { // I can send him back;
                            return std::make_pair(tempX, tempY);
                        }
                    }
                }
            }
            int newX = tempX + (Main_x*3+1 - tempX)*2; // diagonal tempX
            int newY = tempY + (Main_y*3+1 - tempY)*2; // diagonal tempY
            if( MainBoard.get(newX, newY) == TA::BoardInterface::Tag::None) {
                return std::make_pair(newX, newY);
            } else {
                return select_random_pair(MainBoard);
            }
        }
    }

    std::pair<int, int> select_from_the_same_block(TA::UltraBoard MainBoard, std::pair<int, int> goal_main_pos) {
        int tempX = goal_main_pos.first*3 + goal_main_pos.first;
        int tempY = goal_main_pos.second*3 + goal_main_pos.second;
        if( MainBoard.get(tempX, tempY) == TA::BoardInterface::Tag::None ) {
            return std::make_pair(tempX, tempY);
        } else {
            // return select_random_pair(MainBoard);
            return std::make_pair(-1, -1);
        }
    }

    std::pair<int, int> main_pos;
    std::stack<std::pair<int, int>> last_main_pos;
    std::pair<int, int> last_single_main_pos;
    bool is_Fisrt;

    TA::GUIInterface *gui;
};
