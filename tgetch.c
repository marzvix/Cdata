#include <curses.h>

/* teste com curses.h ... nao e para usar isso */

int
main(void)
{   
    int ch;
    const char *result;
    char buffer[80];

    filter();
    newterm(NULL, stderr, stdin);
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    ch = getch();
    if ((result = keyname(ch)) == 0) {
        /* ncurses does the whole thing, other implementations need this */
        if ((result = unctrl((chtype)ch)) == 0) {
            sprintf(buffer, "%#x", ch);
            result = buffer;
        }
    }
    endwin();
    printf("%s\n", result);
    return 0;
}
