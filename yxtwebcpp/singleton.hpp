/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-18 22:24:11
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-06 12:40:53
 * @FilePath: /YXTWebCpp/yxtwebcpp/singleton.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _SINGLETON_HPP_
#define _SINGLETON_HPP_

#include <memory>

namespace YXTWebCpp {

    template<class T, class X, int N>
    T& GetInstanceX() {
        static T v;
        return v;
    }

    template<class T, class X, int N>
    std::shared_ptr<T> GetInstancePtr() {
        static std::shared_ptr<T> v(new T);
        return v;
    }

template<class T, class X = void, int N = 1>
class Singleton {
    public:
        static T* GetInstance() {
            static T v;
            return &v;
        } 
};

template<class T, class X = void, int N = 1>
class SingletonPtr { 
    public:
        static std::shared_ptr<T> GetInstance() {
            static std::shared_ptr<T> v(new T);
            return v;
        }
};

}


#endif