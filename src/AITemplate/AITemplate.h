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
#include <queue>


class AI : public AIInterface
{
    
public:

    void init(bool order) override // prepareState will call this
    {
        gui = new TA::ASCII;

        main_pos = std::make_pair(-1, -1);
        is_Fisrt = order;
        phase = 0;
    }

    void callbackReportEnemy(int x, int y) override
    {
        (void) x;
        (void) y;
        // give last step

        main_pos = std::make_pair(x%3, y%3);
        last_pos = std::make_pair(x, y);
    }

    std::pair<int,int> queryWhereToPut(TA::UltraBoard MainBoard) override
    {
        if(!is_Fisrt) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); //simulate computing time, can't more than 1000
            auto pair = backhand(MainBoard);
            // auto pair = select_random_pair(MainBoard);
            return pair;
        } else { // predefined answer
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); //simulate computing time, can't more than 1000
            auto pair = predefined_pair(MainBoard);
            // auto pair = select_random_pair(MainBoard);
            return pair;
        }
    }
private:
    typedef struct _point{          // for backhand
        int points;
        std::pair<int, int> pair;
    } point;

    int get_points(TA::UltraBoard MainBoard, int i, int j,int p,int level){
        int cnt = 0;
        int max = -100;
        int min = 100;
        int ii = i%3;
        int jj = j%3;
        //bool flag = false;
        if(level == 3) return 0;
        if(level%2==0){
            TA::BoardInterface::Tag t_enemy = MainBoard.get(last_pos.first, last_pos.second);
            TA::BoardInterface::Tag &t = MainBoard.get(i,j);
            if(t_enemy == TA::BoardInterface::Tag::O) t = TA::BoardInterface::Tag::X;
            else t = TA::BoardInterface::Tag::O;
            TA::Board save = MainBoard.sub(main_pos.first, main_pos.second);
            if(save.getWinTag() == TA::BoardInterface::Tag::None){
                for(int k = 0; k < 3; k++){
                    if(save.get(k, 0) == t && save.get(k, 1) == t && save.get(k, 2) == t)p += 5;//flag = true;
                    if(save.get(0, k) == t && save.get(1, k) == t && save.get(2, k) == t)p += 5;//flag = true;
                }
                if(save.get(0, 0) == t && save.get(1, 1) == t && save.get(2, 2) == t)p += 5;//flag = true;
                if(save.get(2, 0) == t && save.get(1, 1) == t && save.get(0, 2) == t)p += 5;//flag = true;
                t = t_enemy;
                for(int k = 0; k < 3; k++){
                    if(save.get(k, 0) == t && save.get(k, 1) == t && save.get(k, 2) == t)p += 2;//flag = true;
                    if(save.get(0, k) == t && save.get(1, k) == t && save.get(2, k) == t)p += 2;//flag = true;
                }
                if(save.get(0, 0) == t && save.get(1, 1) == t && save.get(2, 2) == t)p += 2;//flag = true;
                if(save.get(2, 0) == t && save.get(1, 1) == t && save.get(0, 2) == t)p += 2;//flag = true;
                if(ii == main_pos.first && jj == main_pos.second) p += 3;
                if(i%3 == 1 && j%3 == 1)p -= 5;
                else if (i%3 == 1 || j%3 == 1)p += 1;
            }
            else{
                // have win tag
            }
            for(int _i = ii ; _i < ii + 3; _i++){
                for(int _j = jj;_j < jj + 3; _j++){
                    if (MainBoard.get(_i, _j) == TA::BoardInterface::Tag::None){
                        cnt = get_points(MainBoard,_i,_j,p,level+1);
                        if(cnt > max)   max = cnt;
                        t = TA::BoardInterface::Tag::None;
                    }
                }
            }
            if(max != -100) return p + max;
            return p;
        }
        else{
            TA::BoardInterface::Tag t1 = MainBoard.get(last_pos.first, last_pos.second);
            TA::BoardInterface::Tag &t_enemy1 = MainBoard.get(i,j);
            if(t1 == TA::BoardInterface::Tag::O) t_enemy1 = TA::BoardInterface::Tag::X;
            else t_enemy1 = TA::BoardInterface::Tag::O;
            TA::Board save = MainBoard.sub(main_pos.first, main_pos.second);
            //TA::Board next = MainBoard.sub(i%3,j%3);
            if(save.getWinTag() == TA::BoardInterface::Tag::None){
                for(int k = 0; k < 3; k++){
                    if(save.get(k, 0) == t_enemy1 && save.get(k, 1) == t_enemy1 && save.get(k, 2) == t_enemy1)p -= 5;//flag = true;
                    if(save.get(0, k) == t_enemy1 && save.get(1, k) == t_enemy1 && save.get(2, k) == t_enemy1)p -= 5;//flag = true;
                }
                if(save.get(0, 0) == t_enemy1 && save.get(1, 1) == t_enemy1 && save.get(2, 2) == t_enemy1)p -= 5;//flag = true;
                if(save.get(2, 0) == t_enemy1 && save.get(1, 1) == t_enemy1 && save.get(0, 2) == t_enemy1)p -= 5;//flag = true;
                t_enemy1 = t1;
                for(int k = 0; k < 3; k++){
                    if(save.get(k, 0) == t_enemy1 && save.get(k, 1) == t_enemy1 && save.get(k, 2) == t_enemy1)p -= 2;//flag = true;
                    if(save.get(0, k) == t_enemy1 && save.get(1, k) == t_enemy1 && save.get(2, k) == t_enemy1)p -= 2;//flag = true;
                }
                if(save.get(0, 0) == t_enemy1 && save.get(1, 1) == t_enemy1 && save.get(2, 2) == t_enemy1)p -= 2;//flag = true;
                if(save.get(2, 0) == t_enemy1 && save.get(1, 1) == t_enemy1 && save.get(0, 2) == t_enemy1)p -= 2;//flag = true;
                if(i%3 == 1 && j%3 == 1) p += 1;
                //else if ((i%3 == 2 && j%3 == 1) || (i%3 == 0 && j%3 == 1)) p -= 2;
                else if (i%3 == 1 || j%3 == 1)p -= 1;
            }
            else{
                // have win tag
            }
            for(int _i = ii ; _i < ii + 3; _i++){
                for(int _j = jj;_j < jj + 3; _j++){
                    if (MainBoard.get(_i, _j) == TA::BoardInterface::Tag::None){
                        cnt = get_points(MainBoard,_i,_j,p,level+1);
                        if(cnt < min)   min = cnt;
                        t_enemy1 = TA::BoardInterface::Tag::None;
                    }
                }
            }
            if(min != 100) return p + min;
            return p;
        }
    }
    std::pair<int, int> backhand (TA::UltraBoard MainBoard) {
        std::pair<int, int> b_pair;

        std::set<std::pair<int, int>> candidates;
        if ( (main_pos.first == -1 && main_pos.second == -1) || MainBoard.sub(main_pos.first, main_pos.second).full()) { // no constrain
            return select_random_pair(MainBoard);
            // check next step
        }else {
            std::vector<point> candidate;
            int x = main_pos.first*3;
            int y = main_pos.second*3;
            
            for (int i=x; i<x+3; ++i) {
                for (int j=y; j<y+3; ++j) {
                    if (MainBoard.get(i, j) == TA::BoardInterface::Tag::None) {
                        point tmp;
                        tmp.pair = std::make_pair(i, j);
                        tmp.points = get_points(MainBoard, i, j, 0, 0);
                        candidate.push_back(tmp);
                    }
                }
            }
            int max = -50;
            int n = candidate.size();
            auto it = candidate.begin();
            auto iter = candidate.begin();
            for(int i = 0; i < n; i++){
                if(it->points > max){
                    iter = it;
                    max = it->points;
                }
                it++;
            }
            candidate.clear();
            return iter->pair;
        }
    }

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

    std::pair<int, int> select_correspond_pair (TA::UltraBoard MainBoard, std::pair<int, int> my_main_pos) {
        if (MainBoard.get(my_main_pos.first*3 + my_main_pos.first, my_main_pos.second*3 + my_main_pos.second) == TA::BoardInterface::Tag::None)
            return std::make_pair(my_main_pos.first*3 + my_main_pos.first, my_main_pos.second*3 + my_main_pos.second);
        else // should not here;
            return select_random_pair(MainBoard);
    }

    std::pair<int, int> predefined_pair (TA::UltraBoard MainBoard) {
        if (phase == 0) {
            if ( (main_pos.first == -1 && main_pos.second == -1) ) { // first move;
                send_back_pos_queue.push(std::make_pair(1, 1));
                return std::make_pair(4, 4);;
            }
            if ( MainBoard.sub(1, 1).full() ) {
                phase++;
                send_back_pos_queue.pop();
                send_back_pos_queue.push(std::make_pair(main_pos.first, main_pos.second));
                return select_correspond_pair(MainBoard, main_pos);
            }
            return std::make_pair(main_pos.first*3 + 1, main_pos.second*3 + 1); // 1 == send_back_pos_queue.front().first
        } else if (phase == 1) {
            std::pair <int, int> special_center = std::make_pair( send_back_pos_queue.front().first*3 + 1, send_back_pos_queue.front().second*3 + 1 );
            std::pair <int, int> special_diagonal = std::make_pair( send_back_pos_queue.front().first*3 + (2 - send_back_pos_queue.front().first), send_back_pos_queue.front().second*3 + (2 - send_back_pos_queue.front().second) );
            if (last_pos == special_center || last_pos == special_diagonal) { // the two special case
                std::pair <int, int> new_main_pos = std::make_pair((2 - send_back_pos_queue.front().first), (2 - send_back_pos_queue.front().second)); // move to diagonal
                if (MainBoard.get(new_main_pos.first*3 + send_back_pos_queue.front().first, new_main_pos.second*3 + send_back_pos_queue.front().second) == TA::BoardInterface::Tag::None) { // if I can send him back
                    return std::make_pair(new_main_pos.first*3 + send_back_pos_queue.front().first, new_main_pos.second*3 + send_back_pos_queue.front().second);
                }
                // I can't send him back so go to phase 2
                phase++;
                send_back_pos_queue.push(std::make_pair(new_main_pos.first, new_main_pos.second));
                return select_correspond_pair(MainBoard, new_main_pos);
            } else //send him back
                return std::make_pair(main_pos.first*3 + send_back_pos_queue.front().first, main_pos.second*3 + send_back_pos_queue.front().second);
        } else if (phase == 2) { // now send him back to first one board, otherwise the second one
            if (MainBoard.sub(send_back_pos_queue.front().first, send_back_pos_queue.front().second).full()) {
                send_back_pos_queue.pop();
            }
            
            if(MainBoard.get(main_pos.first*3 + send_back_pos_queue.front().first, main_pos.second*3 + send_back_pos_queue.front().second) == TA::BoardInterface::Tag::None)
                return std::make_pair(main_pos.first*3 + send_back_pos_queue.front().first, main_pos.second*3 + send_back_pos_queue.front().second);
            else if (MainBoard.get(main_pos.first*3 + send_back_pos_queue.back().first, main_pos.second*3 + send_back_pos_queue.back().second) == TA::BoardInterface::Tag::None )
                return std::make_pair(main_pos.first*3 + send_back_pos_queue.back().first, main_pos.second*3 + send_back_pos_queue.back().second);
            else { // Now you should already win
                phase++;
                return select_random_pair(MainBoard); 
            }
                // std::cout << "Should not be here.\n";
        } else if (phase == 3) {
            return select_random_pair(MainBoard);
        }

        return select_random_pair(MainBoard);
    }

    std::pair<int, int> main_pos;
    std::pair<int, int> last_pos;
    std::queue<std::pair<int, int>> send_back_pos_queue;
    int phase;
    bool is_Fisrt;

    TA::GUIInterface *gui;
};
