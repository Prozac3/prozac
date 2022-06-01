#pragma once
#include <iostream>
#include <prozac/factory.h>
class BaseShape
{
public:
    virtual void draw() = 0;
};

class Rectangle : public BaseShape
{
public:
    std::string m_userData;
    Rectangle(const std::string& userDate)
    {
        m_userData = userDate;
    }
    ~Rectangle()
    {
    }

    void draw() override
    {
        std::cout << "I'm Rectangle" << std::endl;
        std::cout << "m_userData:" << m_userData << std::endl;
    }
};

CLASS_STRING_REGISTER(Rectangle, "R",std::string)
