#ifndef _NOCOPYABLE_HPP_
#define _NOCOPYABLE_HPP_

namespace YXTWebCpp {

class Nocopyable {
public:
    Nocopyable() = default;
    virtual ~Nocopyable() = default;

    Nocopyable(const Nocopyable&) = delete;
    Nocopyable(Nocopyable&&) = delete;
    Nocopyable& operator=(const Nocopyable&) = delete;
    Nocopyable& operator=(Nocopyable&&) = delete;
};

}

#endif