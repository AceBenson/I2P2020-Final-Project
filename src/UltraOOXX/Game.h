#pragma once

#include <UltraOOXX/UltraBoard.h>
#include <UltraOOXX/Wrapper/AI.h>
#include <GUI/GUIInterface.h>

#include <iostream>
#include <cassert>
#include <chrono>
#include <cstdarg>
#include <future>
#include <type_traits>

// using std functions
using std::cout;
using std::endl;

namespace TA
{
    class UltraOOXX
    {
    public:
        UltraOOXX(
            std::chrono::milliseconds runtime_limit = std::chrono::milliseconds(1000)
        ):
            m_runtime_limit(runtime_limit),
            m_P1(nullptr),
            m_P2(nullptr),
            MainBoard()
        {
            gui = new ASCII;
            MainBoard = UltraBoard();
        }

        void setPlayer1(AIInterface *ptr) { assert(checkAI(ptr)); m_P1 = ptr; }
        void setPlayer2(AIInterface *ptr) { assert(checkAI(ptr)); m_P2 = ptr; }

        void run()
        {
            gui->title();
            int round = 0;
            if( !prepareState() ) return ;

            //Todo: Play Game;
            MainBoard.setWinTag(BoardInterface::Tag::None);
            updateGuiGame();

            AIInterface *first = m_P2;
            AIInterface *second = m_P1;
            BoardInterface::Tag tag = BoardInterface::Tag::X;

            while (!checkGameover()) {
            // while (round != 81) {
                round++;
                // char c;
                // std::cin >> c;

                if(tag == BoardInterface::Tag::X) {
                    tag = BoardInterface::Tag::O;
                    first = m_P1;
                    second = m_P2;
                } else {
                    tag = BoardInterface::Tag::X;
                    first = m_P2;
                    second = m_P1;
                }
                

                if (!playOneRound(first, tag, second)) {

                }
                putToGui("round =  %d\n", round);
                updateGuiGame();
            } 
        } 

   private:
        void updateGuiGame()
        {
            gui->updateGame(MainBoard);
        }

        bool playOneRound(AIInterface *user, BoardInterface::Tag tag, AIInterface *enemy)
        {
            auto pos = call(&AIInterface::queryWhereToPut, user, MainBoard);
            // cout << pos.first << " " << pos.second << endl; // 1, 2
            int x = pos.first;
            int y = pos.second;
            
            BoardInterface::Tag& t = MainBoard.get(x, y);
            if (t != BoardInterface::Tag::None) {
                // this player will lose
                std::cout << "Wrong here\n";
                return false;
            }

            t = tag;

            updateWinTag(t, x, y);

            call(&AIInterface::callbackReportEnemy, enemy, x, y);
            return true;
        }

        void updateWinTag(BoardInterface::Tag t, int x, int y) {
            bool flag = false; 
            Board save = MainBoard.sub(x/3,y/3);
            if(MainBoard.sub(x/3,y/3).getWinTag() == BoardInterface::Tag::None){
                for(int i = 0; i < 3; i++){
                    if(save.get(i, 0) == t && save.get(i, 1) == t && save.get(i, 2) == t){
                        MainBoard.sub(x/3,y/3).setWinTag(t);
                        flag = true;
                    }
                }
                for(int j = 0; j < 3; j++){
                    if(save.get(0, j) == t && save.get(1, j) == t && save.get(2, j) == t){
                        MainBoard.sub(x/3,y/3).setWinTag(t);
                        flag = true;
                    }
                }
                if(save.get(0, 0) == t && save.get(1, 1) == t && save.get(2, 2) == t){
                    MainBoard.sub(x/3,y/3).setWinTag(t);
                    flag = true;
                }
                if(save.get(2, 0) == t && save.get(1, 1) == t && save.get(0, 2) == t){
                    MainBoard.sub(x/3,y/3).setWinTag(t);
                    flag = true;
                }
                if(flag){
                    for(int i = 0 ; i< 3; i++){
                        if(MainBoard.sub(i,0).getWinTag() == t && MainBoard.sub(i,1).getWinTag() == t && MainBoard.sub(i,2).getWinTag() == t){
                            MainBoard.setWinTag(t);
                        }
                        if(MainBoard.sub(0,i).getWinTag() == t && MainBoard.sub(1,i).getWinTag() == t && MainBoard.sub(2,i).getWinTag() == t){
                            MainBoard.setWinTag(t);
                        }
                    }
                    if(MainBoard.sub(0,0).getWinTag() == t && MainBoard.sub(1,1).getWinTag() == t && MainBoard.sub(2,2).getWinTag() == t){
                        MainBoard.setWinTag(t);
                    }
                    if(MainBoard.sub(2,0).getWinTag() == t && MainBoard.sub(1,1).getWinTag() == t && MainBoard.sub(0,2).getWinTag() == t){
                        MainBoard.setWinTag(t);
                    }
                }
                flag = false;
            }
            if(MainBoard.sub(x/3,y/3).getWinTag() == BoardInterface::Tag::None && MainBoard.sub(x/3,y/3).full()) 
                MainBoard.sub(x/3,y/3).setWinTag(BoardInterface::Tag::Tie);
        }
        
        char toPrintChar(BoardInterface::Tag t){
            switch(t) {
                case BoardInterface::Tag::O: return 'O';
                case BoardInterface::Tag::X: return 'X';
                default:
                    return ' ';
            }
        }

        bool checkGameover()
        {
            // return true; // Gameover!
            if(MainBoard.getWinTag() != BoardInterface::Tag::None){
                putToGui("The player %c is win!!!\n", toPrintChar(MainBoard.getWinTag()));
                return true;
            }
            // std::cout << toPrintChar(MainBoard.getWinTag()) <<"\n";
            return false;
        }

        bool prepareState()
        {
            call(&AIInterface::init, m_P1, true);
            call(&AIInterface::init, m_P2, false);
            return true;
        }

        template<typename Func ,typename... Args, 
            std::enable_if_t< std::is_void<
                    std::invoke_result_t<Func, AIInterface, Args...>
                >::value , int> = 0 >
        void call(Func func, AIInterface *ptr, Args... args)
        {
            std::future_status status;
            auto val = std::async(std::launch::async, func, ptr, args...); //val datatype maybe std::future<std::pair<int, int>>, 就是把function return的值包進去std::future
            status = val.wait_for(std::chrono::milliseconds(m_runtime_limit)); // 等待m_runtime_limit後，判斷status：deferred、ready、timeout

            if( status != std::future_status::ready )
            {
                exit(-1);
            }
            val.get();
        }

        template<typename Func ,typename... Args, 
            std::enable_if_t< std::is_void<
                    std::invoke_result_t<Func, AIInterface, Args...>
                >::value == false, int> = 0 > //invoke_result_t如果不是void(value == false)，則是這個function，return type由auto以及實際invoke_result_t決定
        auto call(Func func, AIInterface *ptr, Args... args)
            -> std::invoke_result_t<Func, AIInterface, Args...>
        {
            std::future_status status;
            auto val = std::async(std::launch::async, func, ptr, args...);
            status = val.wait_for(std::chrono::milliseconds(m_runtime_limit));

            if( status != std::future_status::ready )
            {
                exit(-1);
            }
            return val.get();
        }

        void putToGui(const char *fmt, ...)
        {
            va_list args1;
            va_start(args1, fmt);
            va_list args2;
            va_copy(args2, args1);
            std::vector<char> buf(1+std::vsnprintf(nullptr, 0, fmt, args1));
            va_end(args1);
            std::vsnprintf(buf.data(), buf.size(), fmt, args2);
            va_end(args2);

            if( buf.back() == 0 ) buf.pop_back();
            gui->appendText( std::string(buf.begin(), buf.end()) );
        }

        bool checkAI(AIInterface *ptr) 
        {
            return ptr->abi() == AI_ABI_VER;
        }

        int m_size;
        std::vector<int> m_ship_size;
        std::chrono::milliseconds m_runtime_limit;

        AIInterface *m_P1;
        AIInterface *m_P2;
        GUIInterface *gui;

        UltraBoard MainBoard;
    };
}
