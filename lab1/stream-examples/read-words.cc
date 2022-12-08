/* An example program that reads words from the standard input
 * and prints them each on a separate line to standard out.
 *
 * If running this from the terminal, note two things:
 * 1. Text is buffered and not sent to the program until
 *    you hit enter. 
 *
 * 2. The program reads until it reaches the end of the stream.
 *    For the terminal, you need to send an end-of-file (EOF)
 *    character by pressing <Ctrl>-d as the first character on
 *    a line.
 */
#include <iostream>
#include <string>

int main()
{
    std::string s;
    while(std::cin >> s){
        std::cout << s << '\n';
    }
}
