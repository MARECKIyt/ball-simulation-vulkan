#include "app.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
    engine::App app{};
    try
    {
        app.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr<<e.what()<<'\n';
        return 1;
    }
    
    return 0;
}