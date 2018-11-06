#ifndef _UNMOVABLE_H_
#define _UNMOVABLE_H_

///
//
//  Class:      Unmovable
//  
//  Purpose:    Using inheritance, this will make a derived class unable to be moved
//              (unallowed move ctor/assignment)
//
///
class Unmovable
{
    // Move Ctor/Assignment Deleted
    Unmovable(Unmovable&&)                  = delete;
    Unmovable& operator=(Unmovable&&)       = delete;

protected:
    Unmovable( )                            = default;
    Unmovable(const Unmovable&)             = default;
    ~Unmovable( )                           = default;

    Unmovable& operator=(const Unmovable&)  = default;
};

#endif // _UNMOVABLE_H_