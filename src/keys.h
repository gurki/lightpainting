#ifndef KEYS_H
#define KEYS_H

#include <unordered_set>


class Keys
{
    public:

        Keys();

        void down( const int key );
        bool isPressed( const int key ) const;

    private:

        std::unordered_set<int> pressed_;
};


#endif
