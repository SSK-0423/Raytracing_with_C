#include "mymath.hpp"
#include "log.hpp"
#include <iostream>

using namespace std;

#define N 1000000

int main()
{
    initLogFile("myRandTest.txt");

    for (int i = 0; i < N; i++)
    {
        float random = myRand();
        recordLine("%f\n",random);
        if(random < 0)
        {
            cout << "負数エラー発生" << endl;
        }
    }

    finalLogFile();
}