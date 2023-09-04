/*
                ######  ####### #     #  #####  #     #    #    ######  
                #     # #     #  #   #  #     # #     #   # #   #     # 
                #     # #     #   # #   #       #     #  #   #  #     # 
                ######  #     #    #    #       ####### #     # ######  
                #     # #     #   # #   #       #     # ####### #   #   
                #     # #     #  #   #  #     # #     # #     # #    #  
                ######  ####### #     #  #####  #     # #     # #     #
    
    Boxchar is a simple header only library for creating text based user interfaces 
    in C and C++. It is designed to be lightweight and easy to use.

    MIT License

    Copyright (c) 2023 Dane Madsen

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#ifndef BOXCHAR_H
#define BOXCHAR_H

#include <stdio.h>
#include <wchar.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#ifdef _WIN32
static HANDLE hConsole;
#else
static struct termios old_tio;
#endif

static void bcinit() {
    // Set the locale to the user's default
    setlocale(LC_ALL, "");
    
    // Initialize the terminal for boxchar
    #ifdef _WIN32
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 

        GetConsoleScreenBufferInfo(hConsole, &csbiInfo);
        SetConsoleMode(hConsole, ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    #else
        struct termios new_tio;

        // get the terminal settings for stdin
        tcgetattr(STDIN_FILENO, &old_tio);

        // we want to keep the old setting to restore them at the end
        new_tio = old_tio;

        // disable canonical mode (buffered i/o) and local echo
        new_tio.c_lflag &=(~ICANON & ~ECHO);

        // set the new settings immediately
        tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
        
        // Hide cursor and clear screen
        printf("\033[?25l\033[H\033[J");
    #endif
}

static void bcend() {
    // Cleanup the terminal after boxchar
    #ifdef _WIN32
        // Restore original console mode
        SetConsoleMode(hConsole, 0);
    #else
        // restore the former settings
        tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
        
        // Show cursor
        printf("\033[?25h\033[H\033[J");
    #endif
}

void bctermsize(int* width, int* height) {
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    #else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        *width = w.ws_col;
        *height = w.ws_row;
    #endif
}

static void bcputchar(int x, int y, wchar_t ch, int color) {
    #ifdef _WIN32
        COORD coord = {x, y};
        SetConsoleCursorPosition(hConsole, coord);
        SetConsoleTextAttribute(hConsole, color);
        wprintf(L"%lc", ch);
    #else
        printf("\033[%d;%dH\033[38;5;%dm%lc\033[0m", y, x, color, ch);
    #endif
}

#endif // BOXCHAR_H