#pragma once

#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

#include <UltraOOXX/UltraBoard.h>

namespace TA
{
    class GUIInterface
    {
    public:
        virtual void title() = 0;
        virtual void appendText(std::string str) = 0;
        virtual void updateGame(UltraBoard b) = 0;
    };

    using std::printf;
    #define ESC "\033"
    class ASCII: public GUIInterface
    {
        const static int GRAPH_HIGHT = 7+15;
        const static int GRAPH_WIDTH = 80;
        
        const static int TEXT_HIGHT = 10;
        const static int TEXT_WIDTH = 80;

        std::string m_preparedText;
        std::string m_textbuf;

        void cls()
        {
            printf( ESC "[H" ESC "[J" ); //清屏並且將游標置頂
        }

        void gotoxy(int y, int x)
        {
            printf( ESC "\033[%d;%df", y, x); //移動游標至指定位置
        }

        void updateTextBuf()
        {
            std::stringstream ss(m_textbuf);
            const std::string ban(TEXT_WIDTH, '-');
            std::string tmp, last;

            m_preparedText.clear();
            for(int L=0 ; L < TEXT_HIGHT - 2; ++L)
            {
                if( last == "" )
                    getline(ss, last);

                tmp = last.substr(0, std::min((size_t)TEXT_WIDTH, last.size()) );
    
                if( tmp.size() == last.size() )
                    last = "";
                else 
                    last = last.substr(TEXT_WIDTH);
    
                m_preparedText = tmp + "\n" + m_preparedText;
            }
            m_textbuf = m_textbuf.substr(0, TEXT_HIGHT * TEXT_WIDTH);
            m_preparedText =  ban + "\n" + m_preparedText + ban;
        }

        void showText()
        {
            gotoxy(GRAPH_HIGHT+1, 0);
            printf( ESC "[J" );
            gotoxy(GRAPH_HIGHT+1, 0);
            puts(m_preparedText.c_str());
            gotoxy(GRAPH_HIGHT+TEXT_HIGHT+1, 0);
            std::fflush(stdout);
        }

    public:
        virtual void title() override
        {
            cls();
            printf( ESC "[1m"); // bold
            puts(
R"( _   _ _ _             _____  _______   ____   __
| | | | | |           |  _  ||  _  \ \ / /\ \ / /
| | | | | |_ _ __ __ _| | | || | | |\ V /  \ V /
| | | | | __| '__/ _` | | | || | | |/   \  /   \
| |_| | | |_| | | (_| \ \_/ /\ \_/ / /^\ \/ /^\ \
 \___/|_|\__|_|  \__,_|\___/  \___/\/   \/\/   \/
)");
            printf( ESC "[m");
        }

        virtual void appendText(std::string str)
        {
            m_textbuf = str + m_textbuf;
            updateTextBuf();
            showText();
        }

        int toPrintChar(BoardInterface::Tag t){ //return char by tag
            switch(t) {
                case BoardInterface::Tag::O: 
                    printf( ESC "[34m" ); // blue
                    return 'O';
                case BoardInterface::Tag::X: 
                    printf( ESC "[31m" ); // red
                    return 'X';
                default:
                    return ' ';
            }
        }

        virtual void updateGame(UltraBoard b)
        {
            gotoxy(7+1, 0);
            const std::string buf(20, ' '); //20 space

            for (int i=0;i<9;++i)
            {
                std::printf("%s", buf.c_str());
                for (int j=0;j<9;++j)
                {
                    std::putchar(toPrintChar(b.get(i, j)));
                    printf( ESC "[m" );
                    if (j == 2 || j == 5) std::putchar('|');
                }
                // add my board
                std::printf("%s", buf.c_str());
                for (int j=0;j<9;++j)
                {
                    switch(toPrintChar(b.sub(i/3, j/3).getWinTag())) {
                        case 'O':
                            if (i%3 == 0 || i%3 == 2) {
                                if (j%3 == 1) {
                                    std::putchar('O');
                                } else {
                                    std::putchar(' ');
                                }
                            } else { // i%3 == 1
                                if (j%3 == 0 || j%3 == 2) {
                                    std::putchar('O');
                                } else {
                                    std::putchar(' ');
                                }
                            }
                            // if(i%3 == 1 && j%3 == 1) {
                            //     std::putchar(' ');
                            // } else {
                            //     std::putchar('O');
                            // }
                            break;
                        case 'X':
                            if ((i%3 == 0 || i%3 == 2) && (j%3 == 0 || j%3 == 2)) {
                                std::putchar('X');
                            } else if (i%3 == 1 && j%3 == 1) {
                                std::putchar('X');
                            } else {
                                std::putchar(' ');
                            }
                            break;
                        case ' ':
                            if (b.sub(i/3, j/3).getWinTag() == TA::BoardInterface::Tag::Tie) {
                                std::putchar('T');
                            } else {
                                std::putchar(' ');
                            }
                            break;
                    }
                    printf( ESC "[m" );
                    if (j == 2 || j == 5) std::putchar('|');
                }

                std::putchar('\n');
                if (i==2 ||i==5) {
                    std::printf("%s", buf.c_str());
                    printf( ESC "[m" );
                    // std::puts(std::string(12,'-').c_str());
                    printf("%s", std::string(11,'-').c_str());

                    // add my board
                    std::printf("%s", buf.c_str());
                    std::puts(std::string(11,'-').c_str());
                }
            }
            gotoxy(GRAPH_HIGHT+TEXT_HIGHT+1, 0);
        }

        // char outputCharPattern(BoardInterface::Tag t) {
        //     switch(t) {
        //         case BoardInterface::Tag::O: 
        //             // printf( ESC "[1;34m" ); // blue text
        //             // printf( ESC "[1;44m" ); // blue background
        //             return 'O';
        //         case BoardInterface::Tag::X: 
        //             // printf( ESC "[1;31m" ); // red text
        //             // printf( ESC "[1;41m" ); // red background
        //             return 'X';
        //         default:
        //             return ' ';
        //     }
        // }
    };
    #undef ESC
}
