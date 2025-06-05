// Copyright 2025 Pozdnova Polina
#pragma once

#include <iostream>
#include <utility>
#include <random>
#include <chrono>
#include <initializer_list>
#include <stdexcept>

#define CAPACITY 15
#define DELETED_LIMIT 0.15

enum State { empty, busy, deleted };

template<class T>
class TVector {
    T* _data = nullptr;
    int _size = 0;
    int _capacity = CAPACITY;
    size_t _deleted = 0;
    State* _states = nullptr;

public:
    // Constructors //
    TVector();
    explicit TVector(int);
    TVector(int, const T*);
    explicit TVector(std::initializer_list<T>);
    TVector(const TVector<T>&);

    // Destructor //
    ~TVector();

    // Getters //
    T* data() const noexcept;
    int size() const noexcept;
    int capacity() const noexcept;
    //size_t deleted_count() const noexcept;
    int get_deleted() const noexcept;
    T& front() const;
    T& back() const;
    T* begin() const;
    T* end() const;

    // Setters //
    void set_size(int);
    void set_data(T*);
    
    // Functions //
    bool is_empty() const noexcept;
    bool is_full() const noexcept;
    T& at(int) const;
    void emplace(int, const T&);
    void assign(const TVector<T>&);
    void user_output() const noexcept;
    void real_output() const;

    // Insertion //
    void push_front(const T&);
    void push_back(const T&);
    void insert(int, const T&);

    // Deletion //
    void pop_front();
    void pop_back();
    void erase(int);

    // Memory //
    void reserve(int);
    void resize(int);
    void resize(int, const T&);
    void clear() noexcept;
    void shrink_to_fit();

    // Operators //
    TVector<T>& operator=(const TVector<T>& other);
    bool operator==(const TVector<T>&) const;
    bool operator!=(const TVector<T>&) const;
    T& operator[](int) const;

private:
    void effective_deletion();
    T& reverse_at(int) const;
};

// Constructors //
template<class T>
TVector<T>::TVector() {
    _data = new T[_capacity];
    _states = new State[_capacity];
    for (int i = 0; i < _capacity; i++) _states[i] = empty;
}

template<class T>
TVector<T>::TVector(int size) {
    set_size(size);

    _capacity = _size + CAPACITY;
    _data = new T[_capacity];
    _states = new State[_capacity];
    for (int i = 0; i < _size; i++) _states[i] = busy;
    for (int i = _size; i < _capacity; ++i) _states[i] = empty;
}

template<class T>
TVector<T>::TVector(int size, const T* data) {
    set_size(size);
    if (data == nullptr) throw std::logic_error("The data is empty!");
    
    _size = size;
    _capacity = _size + CAPACITY;
    _data = new T[_capacity];
    _states = new State[_capacity];
    for (int i = 0; i < _size; i++) {
        _data[i] = data[i];
        _states[i] = busy;
    }
    for (int i = _size; i < _capacity; ++i) _states[i] = empty;
}

template<class T>
TVector<T>::TVector(std::initializer_list<T> init) {
    set_size(init.size());
    _capacity = _size + CAPACITY;
    _data = new T[_capacity];
    _states = new State[_capacity];
    const T* list = init.begin();
    for (int i = 0; i < _size; i++) {
        _data[i] = list[i];
        _states[i] = busy;
    }
    for (int i = _size; i < _capacity; ++i) _states[i] = empty;
}

template<class T>
TVector<T>::TVector(const TVector<T>& other) {
    _size = other._size;
    _capacity = other._capacity;
    _deleted = other._deleted;
    _data = new T[_capacity];
    _states = new State[_capacity];
    for (int i = 0; i < _capacity; i++) {
        _data[i] = other._data[i];
        _states[i] = other._states[i];
    }
}

// Destructor //
template<class T>
TVector<T>::~TVector() {
    delete[] _data;
    delete[] _states;
}

// Getters //
template<class T>
T* TVector<T>::data() const noexcept { return _data; }

template<class T>
int TVector<T>::size() const noexcept { return _size - _deleted; }

template<class T>
int TVector<T>::capacity() const noexcept { return _capacity; }

//template<class T>
//size_t TVector<T>::deleted_count() const noexcept { return _deleted; }

template<class T>
int TVector<T>::get_deleted() const noexcept { return _deleted; }

template<class T>
T& TVector<T>::front() const { return at(0); }  // Пользовательский

template<class T>
T& TVector<T>::back() const { return reverse_at(size() - 1); }  // Пользовательский

template<class T> T* TVector<T>::begin() const {  // Фактический
    if (_data == nullptr) throw std::logic_error("Data does not exist!");
    return _data;
}

template<class T> T* TVector<T>::end() const {  // Фактический
    if (_data == nullptr) throw std::logic_error("Data does not exist!");
    return _data + _size;
}

// Setters //
template<class T>
void TVector<T>::set_size(int size) {
    if (size < 0) throw std::logic_error("The size is less than 0!");
    _size = size;
}

template<class T>
void TVector<T>::set_data(T* data) {
    if (data == nullptr) {
        throw std::logic_error("The data is empty!");
    }
    _data = data;
}


// Functions //
template<class T>
bool TVector<T>::is_empty() const noexcept { return size() == 0; }

template<class T>
bool TVector<T>::is_full() const noexcept { return _size >= _capacity; }

// --- //
template<class T>
T& TVector<T>::at(int index) const {
    if (index < 0 || index >= size()) throw std::out_of_range("Index out of range");

    int real_index = -1;
    for (int i = 0; i < _size; i++) {
        if (_states[i] == busy) real_index++;
        if (real_index == index) return _data[i];
    }
    throw std::out_of_range("Index not found among busy elements");
}

template<class T>
T& TVector<T>::reverse_at(int index) const {
    if (index < 0 || index >= size()) throw std::out_of_range("Index out of range");

    int real_index = size();
    for (int i = _size - 1; i >= 0; i--) {
        if (_states[i] == busy) real_index--;
        if (real_index == index) return _data[i];
    }
    throw std::out_of_range("Index not found among busy elements");
}

// --- //
template<class T>  // замена значения
void TVector<T>::emplace(int index, const T& value) {
    if (index >= size() || index < 0) { throw std::out_of_range("Index out of range"); }
    if (size() == 0) { throw std::logic_error("Placement of the value is impossible! The size is 0!"); }
    
    at(index) = value;
}

template<class T>
void TVector<T>::assign(const TVector<T>& other) {
    if (this == &other) return;
    delete[] _data;
    delete[] _states;
    _size = other._size;
    _capacity = other._capacity;
    _deleted = other._deleted;
    _data = new T[_capacity];
    _states = new State[_capacity];
    for (int i = 0; i < _capacity; ++i) {
        _data[i] = other._data[i];
        _states[i] = other._states[i];
    }
}

// --- //
template<class T>
void TVector<T>::user_output() const noexcept {
    std::cout << "{ ";
    int printed = 0;
    for (int i = 0; i < _size; ++i) {
        if (_states[i] == busy) {
            std::cout << _data[i];
            printed++;
            if (printed < size()) std::cout << ", ";
        }
    }
    std::cout << " }" << std::endl;
}

template<class T>
void TVector<T>::real_output() const {
    std::cout << "{ ";
    for (int i = 0; i < _size; ++i) {
        switch (_states[i]) {
        case busy: std::cout << _data[i] << " (busy) "; break;
        case deleted: std::cout << _data[i] << " (deleted) "; break;
        case empty: std::cout << _data[i] << " (empty) "; break;
        }
    }
    std::cout << "}" << std::endl;
}

// Insertion //
template<class T> void TVector<T>::push_front(const T& value) {
    if (_size == 0) {
        _size = 1;
        reserve(CAPACITY);
        _data[0] = value;
        _states[0] = busy;
        return;
    }
    else {
        int index_first_busy = -1;
        int index_first_deleted_or_empty = -1;
        for (int i = 0; i < _size; i++) {
            if (_states[i] == busy) { index_first_busy = i; break; }
        }

        if (index_first_busy == 0) {
            if (is_full()) reserve(_size + CAPACITY);

            for (int i = 1; i < _capacity; i++) {
                if (_states[i] == deleted || _states[i] == empty) { 
                    index_first_deleted_or_empty = i; 
                    if (_states[i] == deleted) _deleted--;
                    if (_states[i] == empty) _size++;
                    _states[i] = busy;
                    break;
                }
            }
            for (int i = index_first_deleted_or_empty; i >= index_first_busy; i--) {
                _data[i] = _data[i - 1];
            }
            _data[index_first_busy] = value;
        }
        else {
            _data[index_first_busy - 1] = value;
            if (_states[index_first_busy - 1] == deleted) _deleted--;
            _states[index_first_busy - 1] = busy;
        }
    }
}

template<class T>
void TVector<T>::push_back(const T& value) {
    if (is_full()) reserve(_size + CAPACITY);

    for (int i = _capacity; i >= 0; i--) {
        if (_states[i] = busy) {
            _data[i+1] = value;
            if (_states[i+1] == deleted) _deleted--;
            if (_states[i+1] == empty) _size++;
            _states[i + 1] = busy;
        }
        if (i == 0) {
            _data[i] = value;
            if (_states[i] == deleted) _deleted--;
            if (_states[i] == empty) _size++;
            _states[i] = busy;
        }
    }
}

template<class T> void TVector<T>::insert(int index, const T& value) {
    if (index > (size()) || index < 0) {
        throw std::out_of_range("Index out of range");
    }

    if (is_full()) reserve(_size + CAPACITY);

    if (index == 0) {
        push_front(value);
        return;
    }

    if (index == size()) {
        push_back(value);
        return;
    }

    int index_busy = 0;
    int real_index = 0;
    while (index_busy < index && real_index < _size) {
        if (_states[real_index] == busy) index_busy++;
        real_index++;
    }

    if (_states[real_index + 1] == deleted || _states[real_index + 1] == empty) {
        _data[real_index + 1] = value;
        if (_states[real_index + 1] == deleted) _deleted--;
        if (_states[real_index + 1] == empty) _size++;
        _states[real_index + 1] = busy;
    }
    else {
        for (int i = _size; i > real_index; i--) {
            _data[i] = _data[i - 1];
            _states[i] = _states[i - 1];
            _data[real_index] = value;
            _states[real_index] = busy;
            _size++;
        }
    }
}


// Deletion functions //
template<class T> void TVector<T>::pop_front() {
    if (size() == 0) throw std::logic_error("Deletion is not possible! The size is 0!");

    int index = -1;
    for (int i = 0; i < _size; i++) {
        if (_states[i] == busy) {
            index = i;
            break;
        }
    }

    if (index == _size - 1) {
        _states[index] = empty;
        _size--;
        while (_states[index - 1] == deleted) {
            _states[index - 1] = empty;
            _deleted--;
            index--;
        }
    }
    else {
        for (int i = 0; i < _size; i++) {
            if (_states[i] == busy) {
                _states[i] = deleted;
                _deleted++;
                break;
            }
        }
    }

    if (_deleted >= static_cast<int>(_size * DELETED_LIMIT)) {
        effective_deletion();
    }
}

template<class T> void TVector<T>::pop_back() {
    if (size() == 0) { throw std::logic_error("Deletion is not possible! The size is 0!"); }

    int index = _size - 1;

    if (index == _size - 1) {
        _states[index] = empty;
        _size--;
        while (_states[index - 1] == deleted) {
            _states[index - 1] = empty;
            _deleted--;
            index--;
        }
    }

    if (_deleted >= static_cast<size_t>(_size * DELETED_LIMIT)) {
        effective_deletion();
    }
}

template<class T> void TVector<T>::erase(int index) {
    if (index >= size() || index < 0) { throw std::out_of_range("Index out of range"); }
    if (size() == 0) { throw std::logic_error("Deletion is not possible! The size is 0!"); }

    if (index == size()) {
        pop_back();
        return;
    }

    if (index == 1) {
        pop_front();
        return;
    }

    int index_busy = 0;
    int real_index = 0;
    for (int i = 0; i < _size; i++) {
        if (_states[i] == busy) {
            index_busy++;
            if (index_busy == index) {
                real_index = i;
                break;
            }
        }
    }

    _states[real_index] = deleted;
    _deleted++;

    if (_deleted >= static_cast<int>(_size * DELETED_LIMIT)) {
        effective_deletion();
    }
}

// Memory //
    // Подготоваить память для new_capacity элементов (Цель: эффективность - выделить память заранее)
template<class T> void TVector<T>::reserve(int new_capacity) {
    if (new_capacity > _capacity) {
        T* new_data = new T[new_capacity];
        State* new_states = new State[new_capacity];

        for (int i = 0; i < _capacity; i++) {
            new_data[i] = _data[i];
            new_states[i] = _states[i];
        }
        for (int i = _capacity; i < new_capacity; i++) new_states[i] = empty;
        delete[] _data;
        delete[] _states;
        _data = new_data;
        _states = new_states;
        _capacity = new_capacity;
    }
}

    // Перезаписать без get_deleted (Изменить size, перевыделить capacity только в одном случае)
template<class T> void TVector<T>::resize(int new_size) {
    if (new_size < 0) throw std::invalid_argument("The size cannot be negative!");

    effective_deletion(); // Массив в начале без пробелов, дальше хвост с capacity
    if (new_size < _size) {  // capacity не перевыделяется
        for (int i = new_size; i < _size; i++) {
            _states[i] = empty;
        }
        _size = new_size;
    }
    else if (new_size == _size) {
        ;
    }
    else if (new_size < _capacity) {    // capacity не перевыделяется
        for (int i = _size; i < new_size; i++) {
            _data[i] = T{};
            _states[i] = busy;
        }
        _size = new_size;
    }
    else if (new_size > _capacity) {
        reserve(new_size + CAPACITY);

        for (int i = _size; i < new_size; i++) {
            _data[i] = T{};
            _states[i] = busy;
        }
        _size = new_size;
    }
}

template<class T> void TVector<T>::resize(int new_size, const T& value) {
    if (new_size < 0) throw std::invalid_argument("The size cannot be negative!");

    effective_deletion(); // Массив в начале без пробелов, дальше хвост с capacity
    if (new_size <= _size) resize(new_size);
    else if (new_size < _capacity) {    // capacity не перевыделяется
        for (int i = _size; i < new_size; i++) {
            _data[i] = value;
            _states[i] = busy;
        }
        _size = new_size;
    }
    else if (new_size > _capacity) {
        reserve(new_size + CAPACITY);

        for (int i = _size; i < new_size; i++) {
            _data[i] = value;
            _states[i] = busy;
        }
        _size = new_size;
    }
}

template<class T> void TVector<T>::clear() noexcept {
    delete[] _data;
    delete[] _states;
    _size = 0;
    _capacity = CAPACITY;
    _deleted = 0;
    _data = new T[_capacity];
    _states = new State[_capacity];
    for (int i = 0; i < _capacity; i++) _states[i] = empty;
}

    // Убирает empty
template<class T> void TVector<T>::shrink_to_fit() {
    effective_deletion();
    if (_size < _capacity) {
        T* new_data = new T[_size];
        State* new_states = new State[_size];
        for (int i = 0; i < _size; i++) {
            new_data[i] = _data[i];
            new_states[i] = _states[i];
        }
        delete[] _data;
        delete[] _states;
        _data = new_data;
        _states = new_states;
        _capacity = 0;
    }
}

// Operators overload //
template<class T> TVector<T>& TVector<T>::operator=(const TVector<T>& other) {
    if (this != &other) {
        assign(other);
    }
    return *this;
}

template <class T> bool TVector<T>::operator==(const TVector<T>& other) const {
    if (size() != other.size()) return false;
    if (is_empty() && other.is_empty()) return true;
    for (int i = 0; i < size(); i++) {
        if (at(i) != other.at(i)) return false;
    }
    return true;
}

template <class T> bool TVector<T>::operator!=(const TVector<T>& other) const {
    return !(*this == other);
}

template <class T> T& TVector<T>::operator[](int index) const {
    return at(index);
}

// Private functions
    // Убрать get_deleted, но не перевыделять capacity 
template<class T> void TVector<T>::effective_deletion() {
    if (_deleted == 0) return;

    int new_size = 0;
    for (int i = 0; i < _size; i++) {
        if (_states[i] == busy) {
            _data[new_size] = _data[i];
            _states[new_size] = busy;
            new_size++;
        }
    }


    for (int i = new_size; i < _size; i++) {
        _states[i] = empty;
        _capacity++;
    }

    _size = new_size;
    _deleted = 0;
}


// Friend functions

// Search functions
template <class T> int find_first(const TVector<T>& vector, const T& value) {
    if (vector.size() == 0) throw std::logic_error("The size is 0!");
    for (int i = 0; i < vector.size(); i++) {
        if (vector.at(i) == value) return i;
    }
    return INT_MIN;
}

template <class T> int find_last(const TVector<T>& vector, const T& value) {
    if (vector.size() == 0) throw std::logic_error("The size is 0!");
    for (int i = vector.size() - 1; i >= 0; i--) {
        if (vector.at(i) == value) return i;
    }
    return INT_MAX;
}

template <class T> int* find_all(const TVector<T>& vector, const T& value) {
    if (vector.size() == 0) throw std::logic_error("The size is 0!");
    int* result = nullptr, size_res = 0;
    for (int i = 0; i < vector.size(); i++) {
        if (vector.at(i) == value) size_res++;
    }

    if (size_res == 0) {
        result = new int[1];
        result[0] = INT_MIN;
    }
    else {
        result = new int[size_res];
        for (int i = 0, j = 0; j < size_res; i++) {
            if (vector.at(i) == value) {
                result[j] = i;
                j++;
            }
        }
    }
    return result;
}

// Sorting and shuffling
template <class T> void hoara_sort(TVector<T>& mass, int start, int end) {
    if (start >= end) return;
    int l = start, r = end;
    T base_elem = mass[(l + r) / 2];
    while (l <= r) {
        while (mass[l] < base_elem) l++;
        while (mass[r] > base_elem) r--;
        if (l <= r) {
            T temp = mass[l];
            mass[l] = mass[r];
            mass[r] = temp;
            l++;
            r--;
        }
    }
    hoara_sort(mass, start, r);
    hoara_sort(mass, l, end);
}

template <class T> void fisherYatesShuffle(TVector<T>& mass) {
    if (mass.size() <= 1) return;
    std::random_device rd;
    std::mt19937 gen(rd());
    for (int i = mass.size() - 1; i > 0; i--) {
        std::uniform_int_distribution<> distr(0, i);

        int j = distr(gen);
        std::swap(mass[i], mass[j]);
    }
}



