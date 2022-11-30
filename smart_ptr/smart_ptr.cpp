// smart_ptr.cpp 
#include <iostream>
#include <string>
#include <stdexcept>

#include <cassert>

using namespace std;



class null_ptr_exception : public runtime_error
{
public:
    null_ptr_exception(const char* s) : runtime_error(s)
    {
    }
};

template <typename T>
class smart_ptr {
public:
    smart_ptr() noexcept; 
    // Create a smart_ptr that is initialized to nullptr. The reference count
    // should be initialized to nullptr.

    explicit smart_ptr(T*& raw_ptr); 
    // Create a smart_ptr that is initialized to raw_ptr. The reference count
    // should be one. No change is made to raw_ptr.

    explicit smart_ptr(T*&& raw_ptr); 
    // Create a smart_ptr that is initialized to raw_ptr. The reference count
    // should be one. If the constructor fails raw_ptr is deleted.	

    smart_ptr(const smart_ptr& rhs) noexcept; 
    // Copy construct a pointer from rhs. The reference count should be 
    // incremented by one.

    smart_ptr(smart_ptr&& rhs) noexcept; 
    // Move construct a pointer from rhs.

    smart_ptr& operator=(const smart_ptr& rhs); 
    // This assignment should make a shallow copy of the right-hand side's
    // pointer data. The reference count should be incremented as appropriate.

    smart_ptr& operator=(smart_ptr&& rhs) noexcept; 
    // This move assignment should steal the right-hand side's pointer data.

    bool clone(); 
    // If the smart_ptr is either nullptr or has a reference count of one, this
    // function will do nothing and return false. Otherwise, the referred to
    // object's reference count will be decreased and a new deep copy of the
    // object will be created. This new copy will be the object that this
    // smart_ptr points and its reference count will be one. 

    int ref_count() const noexcept; 
    // Returns the reference count of the pointed to data.

    T& operator*(); 
    // The dereference operator shall return a reference to the referred object.
    // Throws null_ptr_exception on invalid access. 

    T* operator->(); 
    // The arrow operator shall return the pointer ptr_. Throws null_ptr_exception
    // on invalid access.

    ~smart_ptr() noexcept;          // deallocate all dynamic memory // TODO


private:
    T* ptr_;               // pointer to the referred object
    int* ref_;             // pointer to a reference count
};




//=========================================================

// Create a smart_ptr that is initialized to nullptr. The reference count
// should be initialized to nullptr.
template<typename T>
smart_ptr<T>::smart_ptr() noexcept
{
    ref_ = nullptr;
    ptr_ = nullptr;
}

//=========================================================


 // Create a smart_ptr that is initialized to raw_ptr. The reference count
 // should be one. No change is made to raw_ptr.
template<typename T>
smart_ptr<T>::smart_ptr(T*& raw_ptr)
{
    ptr_ = raw_ptr;
    ref_ = new int(1);
}


//==========================================================

 // Create a smart_ptr that is initialized to raw_ptr. The reference count
 // should be one. If the constructor fails raw_ptr is deleted.	
template<typename T>
smart_ptr<T>::smart_ptr(T*&& raw_ptr)
{
    ptr_ = raw_ptr;
    try {
        ref_ = new int(1);   // If ref_ throws a bad alloc, rethrow so main can catch the bad_alloc.
    }
    catch (bad_alloc)
    {
        delete raw_ptr;
        throw;
    }
    raw_ptr = nullptr;  // Steal data from raw_ptr so it's set to null by the end.
}


//==========================================================

// Copy construct a pointer from rhs. The reference count should be 
// incremented by one. (Shallow Copy)
template<typename T>
smart_ptr<T>::smart_ptr(const smart_ptr& rhs) noexcept
{

    if (rhs.ptr_ == nullptr)  // If we are copying an empty pointers data make this pointer null in ptr_ and ref_
    {
        ptr_ = nullptr;
        ref_ = nullptr;   // reference count remains unchanged.
    }
    else
    {
        ptr_ = rhs.ptr_;   // Copy rhs's data and increment reference count
        ref_ = rhs.ref_;
        *ref_ = *ref_ + 1; 
    }
}

//=========================================================

// Move construct a pointer from rhs.
template<typename T>
smart_ptr<T>::smart_ptr(smart_ptr&& rhs) noexcept // move constructor should not throw
{


    if (rhs.ptr_ == nullptr)
    {
        ptr_ = nullptr;
        ref_ = nullptr;
    }
    else
    {
        ptr_ = rhs.ptr_;
        ref_ = rhs.ref_;
    }
    rhs.ref_ = nullptr;  // After data is moved set ref_ of rhs to nullptr and its ptr_ to nullptr as well
    rhs.ptr_ = nullptr;
}

//==========================================================

// This assignment should make a shallow copy of the right-hand side's
// pointer data. The reference count should be incremented as appropriate.
template<typename T>
smart_ptr<T>& smart_ptr<T>::operator=(const smart_ptr<T>& rhs)
{

    if (ptr_ == rhs.ptr_) {
        return *this;       // If the pointer is pointing to the same data as the right hand side, return.
    }

    if (ptr_ != nullptr) // If the pointer is pointing to existing data reduce those data's reference count by 1 because this left hand side no longer shares that data
    {
        *ref_ = *ref_ - 1;
    }

    if (rhs.ptr_ == nullptr)
    {
        ptr_ = nullptr;
        ref_ = nullptr;
    }
    else {
        ptr_ = rhs.ptr_;
        ref_ = rhs.ref_;
        *ref_ = *ref_ + 1;    // Copy and increment reference count
    }
    return *this;

}

//============================================================

// This move assignment should steal the right-hand side's pointer data.
template<typename T>
smart_ptr<T>& smart_ptr<T>::operator=(smart_ptr&& rhs) noexcept
{

    if (rhs.ptr_ == nullptr) 
    {
        ptr_ = nullptr;
        ref_ = nullptr;
        
    }
    else {
        ptr_ = rhs.ptr_;
        ref_ = rhs.ref_; // don't decrement ref count?
    }
    rhs.ref_ = nullptr; // Move assignment operator sets rhs ref and ptr_ to null because data has been stolen 
    rhs.ptr_ = nullptr;
    return *this;
}

//============================================================

// If the smart_ptr is either nullptr or has a reference count of one, this
// function will do nothing and return false. Otherwise, the referred to
// object's reference count will be decreased and a new deep copy of the
// object will be created. This new copy will be the object that this
// smart_ptr points and its reference count will be one. 


template<typename T>
bool smart_ptr<T>::clone()
{
    if (ptr_ == nullptr || *ref_ == 1) // Condition: if the pointer that is being cloned is pointing to nothing, it cannot clone, function returns false.
    {                                  // You also cannot clone if the reference count of the given object is 1
        cout << "Cannot clone" << endl;
        return false;
    }
    else
    {
         smart_ptr<T>* deepCopy = new smart_ptr(*this);  // Construct a deep copy of *this so that deepCopy and *this point to the same data and share a reference count with other data increasing overall reference count by 1 
         *ref_ = *ref_ - 2;
         this->ptr_ = deepCopy->ptr_;   //*this takes deepCopy's pointer data
         this->ref_ = new int(1);
         return true;                   
    }


}

//=============================================================

// Returns the reference count of the pointed to data.

template<typename T>
int smart_ptr<T>::ref_count() const noexcept
{
    if (ref_ == nullptr)
    {
        return 0;
    }
    else
    {
        return *ref_;
    }
}
//=============================================================

// The dereference operator shall return a reference to the referred object.
// Throws null_ptr_exception on invalid access. 
template<typename T>
T& smart_ptr<T>::operator*()
{
    if (ptr_ == nullptr)
    {
        throw (null_ptr_exception("Error: null_ptr_exception!"));
    }
    else
    {
        return *ptr_;
    }
}

//===============================================================

 // The arrow operator shall return the pointer ptr_. Throws null_ptr_exception
 // on invalid access.


template<typename T>
T* smart_ptr<T>::operator->()
{
    if (ptr_ == nullptr || ref_ == nullptr)
    {
        throw (null_ptr_exception("Error: null_ptr_exception!"));
    }
    return ptr_;

}


//=================================================================

// Destructor
template<typename T>
smart_ptr<T>::~smart_ptr() noexcept
{

    if (ref_ == nullptr || ptr_ == nullptr)
    {
        delete ref_;                            // If the smart pointer is default, delete its nullptr and refernce count
        delete ptr_;
        return;
    }

    if (*ref_ != 1)                 //Reference count is then decremented each time the reference count is not 1 (The sign that everything should be deleted for smart pointers
    {
        *ref_ = *ref_ - 1;          // - Note: This will never dereference NULL because of the previous if statement
    }
    else
    {
        delete ptr_;            // When reference count is 1, the ptr_ and ref_ are deleted 
        delete ref_;
    }
}


int main()
{

}
