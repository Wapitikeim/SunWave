#pragma once
#include <string>


class Component
{
    private:
    protected:
        std::string componentName;
    public:
        virtual ~Component();
        //virtual void remove();
        std::string getName(){return this->componentName;};
        virtual void update(){};
};