#pragma once

/*
 * Growable array
 */

template <typename T, size_t InitialSize=0>
struct Array
{
    Array() {
        data = InitialSize ? new T[InitialSize] : NULL;
        size = 0;
        capacity = InitialSize;
    }
    ~Array() {
        delete[] data;
    }
    void Clear() {
        delete[] data;
        size = 0;
        capacity = InitialSize;
        data = new T[capacity];
    }        
    void Add(const T& newdatum) {
        if (size + 1 > capacity) {
            capacity = (capacity == 0) ? 1 : capacity*2;
            T* newdata = new T[capacity];
            memcpy(newdata, data, sizeof(T)*size);
            data = newdata;
        }
        data[size++] = newdatum;
    }
    T& operator[](size_t i) {
        assert(i <= size);
        return data[i];
    }
    T*     data;
    size_t size;
    size_t capacity;
};
