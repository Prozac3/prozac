
#include <iostream>
#include "Rech.h"

int main(int agrc, char **argv)
{
   BaseShape* shape =  Factory::CreateObj<BaseShape,std::string>("R","nmsl");
   shape->draw();
   return 0;
}