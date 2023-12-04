#include "GameEnvironment.h"

int main()
{
    try 
    {
        GameEnvironment hiddenInstinct;
        hiddenInstinct.run();
    }
    catch(const std::exception &e) 
    {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
    }
}

