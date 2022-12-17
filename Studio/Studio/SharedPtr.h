/*******************************************************************
 * Copyright (c) 2022~2023 XMuli  All rights reserved.
 * GitHub: https://github.com/XMuli
 * Description: C++ ʵ��һ�����ĵ� shared_ptr ����ָ��ģ���ࣻʵ�������ü������̰߳�ȫ�ģ������ʹ������Դ�����̰߳�ȫ�ġ�
 * Note: ��д����ʱ����ʹ��Ĭ�Ϲ��캯��, ��Ա���� _ptr��_refCount��_pMutex �� release() �����ױ������Ƽ����εĹ��캯�����޴�
 * Ref: https://juejin.cn/post/7111726931301072910#heading-8
 *      https://cloud.tencent.com/developer/article/1688444
 *      https://blog.csdn.net/Z_Stand/article/details/98512756
 ******************************************************************/

#pragma once
#include <iostream>
#include <mutex>
using namespace std;

template <typename T>
class SharedPtr
{
public:
    SharedPtr() : _ptr(nullptr), _refCount(nullptr), _pMutex(nullptr) { cout << "default constructor" << endl; };
    SharedPtr(T* obj) : _ptr(obj), _refCount(new int(1)), _pMutex(new mutex) { cout << "no default constructor" << endl; };

    SharedPtr(const SharedPtr<T>& obj)  // �� _refCount ����ͨ������һ��ָ�����޸ģ�ָ�����ͬһ����ַ
        : _ptr(obj._ptr)
        , _refCount(obj._refCount)
        , _pMutex(obj._pMutex)
    {
        cout << "copy constructor" << endl;
        addRefCount();
    };

    SharedPtr<T>& operator=(const SharedPtr<T>& obj)
    {
        cout << "copy assignment constructor" << endl;
        if (&obj != this) {
            if (_ptr != obj._ptr) {
                release();            // ���ͷžɵ���Դ

                _ptr = obj._ptr;
                _refCount = obj._refCount;
                _pMutex = obj._pMutex;

                addRefCount();        // �ټ����� +1
            }
        }

        return *this;
    }

    //SharedPtr(SharedPtr<T>&& obj) noexcept;
    //SharedPtr<T>& operator=(SharedPtr<T>&& obj)noexcept;

    ~SharedPtr() { cout << "destructor" << endl; release(); }
    T& operator*() { return *_ptr; }
    T* operator->() { return _ptr; }

    int useCount() { return *_refCount; }
    T* get() { return _ptr; }

private:
    void addRefCount()
    {
        cout << "addRefCount" << endl;
        _pMutex->lock();
        ++*_refCount;
        _pMutex->unlock();
    }

    void release()
    {
        cout << "release" << endl;
        bool bDelMutex = false;
        _pMutex->lock();

        if (_ptr && --*_refCount == 0) {  // ��У���Ƿ���ڣ�������Ϊ 0 ���ͷ�
            delete _ptr;
            delete _refCount;
            _ptr = nullptr;
            _refCount = nullptr;

            bDelMutex = true;
        }

        _pMutex->unlock();
        if (bDelMutex)
            delete _pMutex;
    }

private:                  // ���ڹ��캯���г�ʼ��
    T* _ptr;
    int* _refCount;
    mutex* _pMutex;       // ����������ԭ�Ӳ���������������߳�
};

//int main()
//{
//    SharedPtr<int> sp1(new int(10));
//    SharedPtr<int> sp2(sp1);
//    *sp2 = 20;
//                                                                //sp1 �� sp2 �ڹ����ⲿ����Դ�����ü���Ϊ 2
//    cout << sp1.useCount() << "  *ptr:" << *sp1 << endl;		//2	 20
//    cout << sp2.useCount() << "  *ptr:" << *sp2 << endl;		//2	 20
//                                                                  
//    SharedPtr<int> sp3(new int(30));                              
//    sp2 = sp3;		                                            //sp3 ��ֵ�������ͷŹ���ľ���Դ�����ü���-1��   
//    cout << sp1.useCount() << "  *ptr:" << *sp1 << endl;        //1	 20
//    cout << sp2.useCount() << "  *ptr:" << *sp2 << endl;        //2	 30
//    cout << sp3.useCount() << "  *ptr:" << *sp3 << endl;        //2	 30
//                                                                  
//    sp1 = sp3;                                                    
//    cout << sp1.useCount() << "  *ptr:" << *sp1 << endl;        //3	 30
//    cout << sp2.useCount() << "  *ptr:" << *sp2 << endl;        //3	 30
//    cout << sp3.useCount() << "  *ptr:" << *sp3 << endl;        //3	 30
//
//    std::cout << "Hello World!\n";
//    return 0;
//}

/*******************************************************************
 * ��ӡ���:
 * no default constructor
 * copy constructor
 * addRefCount
 * 2  *ptr:20
 * 2  *ptr:20
 * no default constructor
 * copy assignment constructor
 * release
 * addRefCount
 * 1  *ptr:20
 * 2  *ptr:30
 * 2  *ptr:30
 * copy assignment constructor
 * release
 * addRefCount
 * 3  *ptr:30
 * 3  *ptr:30
 * 3  *ptr:30
 * Hello World!
 * destructor
 * release
 * destructor
 * release
 * destructor
 * release
 ******************************************************************/