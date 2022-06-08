#include <iostream>
#include <iomanip>
#include "element.h"
#include "document.h"
#include <fstream>
#include <string>

int main()
{
    std::ifstream myFile("example.txt");

    Document d1(myFile);
    myFile.close();
    Document d2;
    Document d3;
    d3 = d2 = d1;
    
    std::cout << d3.render();
    
    std::ifstream forUpdate("update.txt");
    d3.remove(2);
    std::cout << d3.render();
    d3.update(4, forUpdate);

    std::cout << d3.render();


    

    return 0;
}