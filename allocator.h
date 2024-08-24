//  Allocator (atl::allocator)

/*
    The atl::allocator class template provides a simple memory allocator
    with basic functionalities such as allocation, deallocation, 
    construction, and destruction of objects.
*/

#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <cstddef>
#include <utility>

namespace atl {

template <typename Type>
class allocator {
public:

    using value_type = Type; // value_type: Type of the elements that the allocator handles.

    using pointer = Type*; // pointer: Pointer to the allocated memory.

    using const_pointer = const Type*; // const_pointer: Pointer to the allocated constant memory.

    using reference = Type&; // reference: Reference to the allocated object.

    using const_reference = const Type&; //const_reference: Reference to the constant allocated object.


    
    // allocator(): Default constructor.
    allocator() = default;

    // Template copy constructor for converting between different allocator types.
    template<typename U>
    allocator(const allocator<U>&) noexcept {}

    // Allocates memory for n objects of type Type.
    pointer allocate(std::size_t n) {
        return static_cast<pointer>(::operator new(n * sizeof(Type)));
        
    }
    
    // Deallocates the memory pointed to by p.
    void deallocate(pointer p, std::size_t) {
        ::operator delete(p);
    }

    // Constructs an object of type Type in the allocated memory using the provided arguments.
    template<typename U, typename... Args>
    void construct(U p, Args&&... args) {
        ::new(static_cast<void*>(p)) Type(std::forward<Args>(args)...);
    }

    // Destroys the object pointed to by p.
    template<typename U>
    void destroy(pointer p) {
        p->~Type();
    }
    
    // Destructor.
    ~allocator() {}

};


} // namespace atl

#endif // ALLOCATOR_H

