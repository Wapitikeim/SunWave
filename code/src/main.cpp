#include "GameEnvironment.h"

int main()
{   
    try 
    {
        GameEnvironment sunWave;
        sunWave.run();
    }
    catch(const std::exception &e) 
    {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
    }
}

