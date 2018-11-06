#ifndef _UNCOPYABLE_H_
#define _UNCOPYABLE_H_

///
//
//  Class:      Uncopyable
//  
//  Purpose:    Using inheritance, this will make a derived class unable to be copied 
//              (unallowed copy ctor/assignment).
//
///
class Uncopyable
{
    // Copy Ctor/Assignment Deleted
    Uncopyable(const Uncopyable&)               = delete;
    Uncopyable& operator=(const Uncopyable&)    = delete;

protected:
    Uncopyable()                                = default;
    Uncopyable(Uncopyable&&)                    = default;
    ~Uncopyable()                               = default;

    Uncopyable& operator=(Uncopyable&&)         = default;
};

#endif // _UNCOPYABLE_H_