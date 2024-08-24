// Forward List (atl::forward_list)

/*
    The atl::forward_list class template is a simple implementation of a singly linked list.
*/

#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H

#include "Allocator.h"
#include <memory>
#include <cstddef>
#include <utility>
#include <iostream>

namespace atl {

    /*
        Node (atl::fwd_list_node)
        This struct defines a node in the singly linked list:
    */
    template<typename Type>
    struct fwd_list_node {

        
        Type value; // Type value: Stores the value of the node.
        fwd_list_node* next{nullptr}; // fwd_list_node next*: Pointer to the next node in the list.

        // Default constructor.
        fwd_list_node() = default; 

        // Move constructor.
        fwd_list_node(fwd_list_node&& x) noexcept 
            : next(x.next), value(std::move(x.value)) {
            x.next = nullptr;
        }

        // Move assignment operator.
        fwd_list_node& operator=(fwd_list_node&& x) noexcept {
            if (this != &x) {
                next = x.next;
                value = std::move(x.value);
                x.next = nullptr;
            }
            return *this;
        }

        // Default destructor.
        ~fwd_list_node() = default;
    };


    /* 
        Iterator (atl::fwd_list_iterator)
        This struct defines an iterator for traversing the singly linked list:
    */
    template<typename Type>
    struct fwd_list_iterator {

        using iterator = fwd_list_iterator<Type>;
        using const_iterator = fwd_list_iterator<const Type>;
        using Node = fwd_list_node<Type>;

        using value_type = Type;
        using pointer = Type*;
        using reference = Type&;

        Node* node; // Node node*: Pointer to the current node.
        
        // Constructor initializing the iterator to point to the node n.
        fwd_list_iterator(Node* n = nullptr) noexcept : node(n) {}

        // Dereference operator to access the value of the current node.
        reference operator*() const {
            return node->value;
        }

        // Member access operator to access the value of the current node.
        pointer operator->() const {
            return &(node->value);
        }
        
        // Pre-increment operator to move the iterator to the next node.
        iterator& operator++() {
            node = node->next;
            return *this;
        }

        // Post-increment operator to move the iterator to the next node.
        iterator operator++(int) {
            iterator tmp = *this;
            node = node->next;
            return tmp;
        }

        // Equality operator to compare two iterators.
        bool operator==(const iterator& other) const {
            return node == other.node;
        }

        //  Inequality operator to compare two iterators.
        bool operator!=(const iterator& other) const {
            return node != other.node;
        }

    };


    /*
        Base Class (atl::forward_list_base)
        This class provides the basic functionalities required by the forward list, such as memory management, node creation, and destruction.
    */
    template<typename Type, typename Allocator = allocator<fwd_list_node<Type>>>
    class forward_list_base {
    protected:
        using Node = fwd_list_node<Type>;

        Node* head; // Pointer to the head node of the list.
        Allocator alloc; // Allocator used to allocate and deallocate memory for nodes.
        typename std::allocator_traits<Allocator>::template rebind_alloc<Type> value_alloc; // Rebound allocator for the type Type

    public:
        // Constructor initializing the base with the given allocator.
        forward_list_base(const Allocator& a = Allocator()) 
        : head(nullptr), alloc(a), value_alloc(typename std::allocator_traits<Allocator>::template rebind_alloc<Type>(a)) {}
        
       
        // Destructor that clears the list.
        ~forward_list_base() {
            clear();
        }

        // Move constructor.
        forward_list_base(forward_list_base&& other) noexcept
            : head(other.head), alloc(std::move(other.alloc)) {
            other.head = nullptr;
        }

        // Move assignment operator.
        forward_list_base& operator=(forward_list_base&& other) noexcept {
            if (this != &other) {
                clear();
                head = other.head;
                alloc = std::move(other.alloc);
                other.head = nullptr;
            }
            return *this;
        }

        // Allocates memory for a new node.
        Node* allocate_node() {
            return static_cast<Node*>(alloc.allocate(1));
        }

        // Deallocates memory for the given node.
        void deallocate_node(Node* node) {
            alloc.deallocate(node, 1);
        }

         // Clears the list by destroying all nodes.
        void clear() {
            while (this->head) {
                Node* tmp = this->head;
                this->head = this->head->next;
                this->destroy_node(tmp);
            }
        }

        // Creates a new node with the given value.
        Node* create_node(const Type& value) {
            Node* node = allocate_node();                 
            std::allocator_traits<decltype(value_alloc)>::construct(value_alloc, &node->value, value); 
            return node;
        }

        // Creates a new node with the given value, moving the value.
        Node* create_node(Type&& value) {
            Node* node = allocate_node();
            std::allocator_traits<decltype(value_alloc)>::construct(value_alloc, &node->value, std::move(value));
            return node;
        }

        // Destroys the given node and deallocates its memory.
        void destroy_node(Node* node) {
            std::allocator_traits<decltype(value_alloc)>::destroy(value_alloc, &node->value);
            deallocate_node(node);
        }
    
    };

    /*
        Forward List Class (atl::forward_list)
        This class provides the interface for the forward list, 
        including operations like inserting, removing, and accessing elements.
    */
    template<typename Type, typename Allocator = allocator<fwd_list_node<Type>>>
    class forward_list : public forward_list_base<Type, Allocator> {
    public:
        using Base = forward_list_base<Type, Allocator>;
        using Node = fwd_list_node<Type>;
        using Iterator = fwd_list_iterator<Type>;
        using ConstIterator = fwd_list_iterator<const Type>;

        // Constructor initializing the list with the given allocator.
        forward_list(const Allocator& a = Allocator()) noexcept
            : Base(a) {}

        // Destructor.
        ~forward_list() = default;

        // Copy constructor.
        forward_list(const forward_list& other) : Base(other.alloc) {
            for (Node* current = other.head; current; current = current->next) {
                push_front(current->value);
            }
        }

        // Move constructor.
        forward_list(forward_list&& other) noexcept
            : Base(std::move(other)) {}

        // Copy assignment operator.
        forward_list& operator=(const forward_list& other) {
            if (this != &other) {
                forward_list_base<Type, Allocator>::clear();
                for (Node* current = other.head; current; current = current->next) {
                    push_front(current->value);
                }
            }
            return *this;
        }

        // Move assignment operator.
        forward_list& operator=(forward_list&& other) noexcept {
            if (this != &other) {
                forward_list_base<Type, Allocator>::clear();
                this->head = other.head;
                this->alloc = std::move(other.alloc);
                other.head = nullptr;
            }
            return *this;
        }

        // Assigns count elements with the given value to the list.
        void assign(size_t count, const Type& value) {
            forward_list_base<Type, Allocator>::clear();
            for (size_t i = 0; i < count; ++i) {
                push_front(value);
            }
        }

        // Returns the allocator used by the list.
        Allocator get_allocator() const noexcept {
            return this->alloc;
        }

        // Returns the allocator used by the list.
        Type& front() {
            return this->head->value;
        }
        
        // Returns a constant reference to the first element in the list.
        const Type& front() const {
            return this->head->value;
        }

        // Checks if the list is empty.
        bool empty() const noexcept {
            return this->head == nullptr;
        }
        
        // Inserts a new element at the front of the list.
        void push_front(const Type& value) {
            Node* new_node = this->create_node(value);
            new_node->next = this->head;
            this->head = new_node;
        }
        
        // Inserts a new element at the front of the list, moving the value.
        void push_front(Type&& value) {
            Node* new_node = this->create_node(std::move(value));
            new_node->next = this->head;
            this->head = new_node;
        }

        // Constructs and inserts a new element at the front of the list with the given arguments.
        template<typename... Args>
        void emplace_front(Args&&... args) {
            Node* new_node = this->allocate_node();
            this->alloc.construct(&new_node->value, std::forward<Args>(args)...);
            new_node->next = this->head;
            this->head = new_node;
        }

        // Removes the first element from the list.
        void pop_front() {
            if (this->head) {
                Node* tmp = this->head;
                this->head = this->head->next;
                this->destroy_node(tmp);
            }
        }

        // Resizes the list to contain count elements, filling with value if necessary.
        void resize(size_t count, const Type& value = Type()) {
            size_t current_size = 0;
            for (Node* current = this->head; current; current = current->next) {
                ++current_size;
            }
            if (count < current_size) {
                while (count < current_size) {
                    pop_front();
                    --current_size;
                }
            } else if (count > current_size) {
                while (count > current_size) {
                    push_front(value);
                    ++current_size;
                }
            }
        }

        // Swaps the contents of this list with other.
        void swap(forward_list& other) noexcept {
            std::swap(this->head, other.head);
            std::swap(this->alloc, other.alloc);
        }

        // Merges other list into this one, assuming both are sorted.
        void merge(forward_list& other) {
            Node** pos = &this->head;
            while (*pos && other.head) {
                if ((*pos)->value < other.head->value) {
                    pos = &(*pos)->next;
                } else {
                    Node* temp = other.head;
                    other.head = other.head->next;
                    temp->next = *pos;
                    *pos = temp;
                }
            }
            if (other.head) {
                *pos = other.head;
                other.head = nullptr;
            }
        }

        // Splices elements from other list into this list after the position pos.
        void splice_after(Iterator pos, forward_list& other) {
            if (!other.head) return;
            Node* last = other.head;
            while (last->next) last = last->next;
            last->next = pos.node->next;
            pos.node->next = other.head;
            other.head = nullptr;
        }

        // Removes all elements equal to value
        void remove(const Type& value) {
            Node** pos = &this->head;
            while (*pos) {
                if ((*pos)->value == value) {
                    Node* temp = *pos;
                    *pos = (*pos)->next;
                    this->destroy_node(temp);
                } else {
                    pos = &(*pos)->next;
                }
            }
        }

        // Removes all elements that satisfy the predicate pred.
        template<typename Predicate>
        void remove_if(Predicate pred) {
            Node** pos = &this->head;
            while (*pos) {
                if (pred((*pos)->value)) {
                    Node* temp = *pos;
                    *pos = (*pos)->next;
                    this->destroy_node(temp);
                } else {
                    pos = &(*pos)->next;
                }
            }
        }
        
        // Reverses the order of elements in the list.
        void reverse() {
            Node* prev = nullptr;
            Node* current = this->head;
            while (current) {
                Node* next = current->next;
                current->next = prev;
                prev = current;
                current = next;
            }
            this->head = prev;
        }

        // Removes consecutive duplicate elements from the list.
        void unique() {
            Node* current = this->head;
            while (current && current->next) {
                if (current->value == current->next->value) {
                    Node* temp = current->next;
                    current->next = current->next->next;
                    this->destroy_node(temp);
                } else {
                    current = current->next;
                }
            }
        }

        // Returns an iterator to the beginning of the list.
        Iterator begin() noexcept {
            return Iterator(this->head);
        }

        // Returns an iterator to the end of the list.
        Iterator end() noexcept {
            return Iterator(nullptr);
        }
        
        // Returns a constant iterator to the beginning of the list.
        ConstIterator cbegin() const noexcept {
            return ConstIterator(this->head);
        }

        // Returns a constant iterator to the end of the list.
        ConstIterator cend() const noexcept {
            return ConstIterator(nullptr);
        }
    };

} // namespace atl

#endif // FORWARD_LIST_H

