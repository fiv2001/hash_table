// Copyright[2020] <fiv2001@gmail.com>

/* This is an implementation of unordered map, which uses the separate chaining method
as a way to resolve collisions of hashes. You can read more about it here: 
"https://en.wikipedia.org/wiki/Hash_table#Separate_chaining"; This method provides unbelievable
speed and very low memory usage both for big and small structures. This implementation was 
verified(will be verified(maybe)) by the most experienced data structures authorities.
Do your projects need to speed up a bit or maybe they consume too much memory? You can 
easily solve these problems for a fair price of $9.99 per copy. Contact me here: fiv2001@gmail.com
*/

#include <vector>
#include <stdexcept>
#include <utility>
#include <algorithm>

template<class KeyType, class ValueType,
    class Hash = std::hash<KeyType> > class HashMapIter;
template<class KeyType, class ValueType,
    class Hash = std::hash<KeyType> > class HashMapCIter;

template<class KeyType, class ValueType,
    class Hash = std::hash<KeyType> > class HashMap {

friend class HashMapIter<KeyType, ValueType, Hash>;
friend class HashMapCIter<KeyType, ValueType, Hash>;

public:
    typedef HashMapIter<KeyType, ValueType, Hash> iterator;
    typedef HashMapCIter<KeyType, ValueType, Hash> const_iterator;

    HashMap(Hash h = Hash()) : Hasher(h) {
        Table.resize(Hashing_module);
    };

    // This function inserts one element for a total complexity of O(m),
    // where m is the number of elements in the structure with the same hash.
    // (with a good hashing function it is O(1) average).
    void insert(std::pair <KeyType, ValueType> p) {
        size_t index = Hasher(p.first) % Hashing_module;
        bool exists = 0;
        for (size_t i = 0; i < Table[index].size(); i++) {
            if (Table[index][i].first == p.first) {
                exists = 1;
            }
        }
        if (!exists) {
            Current_number_of_elements++;
            Table[index].push_back(p);
            check();
        }
    }

    template <typename Iter>
    HashMap(Iter begin, Iter end, Hash h = Hash()) : Hasher(h) {
        Table.resize(Hashing_module);
        while (begin != end) {
            insert(*begin);
            begin++;
        }
        check();
    }

    HashMap(std::initializer_list<std::pair <KeyType, ValueType> > l,
         Hash h = Hash()) : Hasher(h) {
        Table.resize(Hashing_module);
        std::vector<std::pair <KeyType, ValueType>> v(l);
        for (size_t i = 0; i < v.size(); i++) {
            insert(v[i]);
        }
        check();
    }

    // This function deletes one element from the structure for a total complexity of O(m)
    // where m is the number of elements in the structure with the same hash.
    void erase(KeyType key) {
        size_t index = Hasher(key) % Hashing_module;
        for (size_t i = 0; i < Table[index].size(); i++) {
            if (Table[index][i].first == key) {
                Current_number_of_elements--;
                swap(Table[index][i], Table[index].back());
                Table[index].pop_back();
            }
        }
        check();
    }

    bool empty() const {
        return Current_number_of_elements == 0;
    }

    size_t size() const {
        return Current_number_of_elements;
    }

    Hash hash_function() const {
        return Hasher;
    };

    // This function returns an iterator to the first element for O(1) amortized complexity.
    iterator begin() {
        size_t i = 0, j = 0;
        while (Table[i].empty()) {
            i++;
            if (i == Hashing_module) {
                break;
            }
        }
        return iterator(*this, i, j);
    }

    const_iterator begin() const {
        size_t i = 0, j = 0;
        while (Table[i].empty()) {
            i++;
            if (i == Hashing_module) {
                break;
            }
        }
        return const_iterator(*this, i, j);
    }

    // This function returns an iterator to the element, which is supposed to be placed after the last element
    // (also known as the end iterator).
    iterator end() {
        return iterator(*this, Hashing_module, 0);
    }

    const_iterator end() const {
        return const_iterator(*this, Hashing_module, 0);
    }

    // This function returns an iterator to the element with the given key
    // for a total complexity of O(m) where m is the number of elements in the
    // structure with the same hash. If there is no such element, it returns an iterator
    // to the element, which is supposed to be placed after the last element (also known
    // as the end iterator).
    iterator find(KeyType &key) {
        size_t index = Hasher(key) % Hashing_module;
        for (size_t j = 0; j < Table[index].size(); j++) {
            if (key == Table[index][j].first) {
                return iterator(*this, index, j);
            }
        }
        return end();
    }

    iterator find(KeyType &&key) {
        size_t index = Hasher(key) % Hashing_module;
        for (size_t j = 0; j < Table[index].size(); j++) {
            if (key == Table[index][j].first) {
                return iterator(*this, index, j);
            }
        }
        return end();
    }

    const_iterator find(KeyType &key) const {
        size_t index = Hasher(key) % Hashing_module;
        for (size_t j = 0; j < Table[index].size(); j++) {
            if (key == Table[index][j].first) {
                return const_iterator(*this, index, j);
            }
        }
        return end();
    }

    const_iterator find(KeyType &&key) const {
        size_t index = Hasher(key) % Hashing_module;
        for (size_t j = 0; j < Table[index].size(); j++) {
            if (key == Table[index][j].first) {
                return const_iterator(*this, index, j);
            }
        }
        return end();
    }

    // This function provides a convenient way to access the element with the given key.
    // If there is no such element, it inserts an element with such key and returns it.
    ValueType & operator[](KeyType &key) {
        size_t index = Hasher(key) % Hashing_module;
        for (size_t j = 0; j < Table[index].size(); j++) {
            if (key == Table[index][j].first) {
                return Table[index][j].second;
            }
        }
        insert({key, ValueType()});
        return (*this)[key];
    }

    ValueType & operator[](KeyType &&key) {
        size_t index = Hasher(key) % Hashing_module;
        for (size_t j = 0; j < Table[index].size(); j++) {
            if (key == Table[index][j].first) {
                return Table[index][j].second;
            }
        }
        insert({key, ValueType()});
        return (*this)[key];
    }

    // This function is a bit less convenient way to access the elements by key, but in
    // case of absence of elements with the given key it throws an exception.
    const ValueType & at(KeyType &key) const {
        size_t index = Hasher(key) % Hashing_module;
        for (size_t j = 0; j < Table[index].size(); j++) {
            if (key == Table[index][j]) {
                return &Table[index][j].second;
            }
        }
        throw std::out_of_range("");
    }

    const ValueType & at(KeyType &&key) const {
        size_t index = Hasher(key) % Hashing_module;
        for (size_t j = 0; j < Table[index].size(); j++) {
            if (key == Table[index][j].first) {
                return Table[index][j].second;
            }
        }
        throw std::out_of_range("");
    }

    void clear() {
        for (size_t index = 0; index < Hashing_module; index++) {
            Table[index].clear();
        }
        Current_number_of_elements = 0;
        check();
    }

// If you are just an average user, you don't really need to see the following code. It's not
// necessary to be able to use this data structure. But if you wish to find out how it works,
// feel free to take a look!
private:
    // This is a hashing module and the current number of cells in table.
    size_t Hashing_module = 1;
    size_t Current_number_of_elements = 0;

    // These variables are required to maintain the good ratio between the number of cells
    // in table and the number of inserted elements. It is always between Left_border and Right_border.
    static constexpr size_t Left_border = 1, Right_border = 3;
    Hash Hasher;
    std::vector <std::vector <std::pair <KeyType, ValueType>> > Table;

    // This cute little function is the main source of all the incredible speed of this data structure:
    // it maintains the ratio between the number of cells in table and the number of inserted elements 
    void rebuild() {
        std::vector <std::pair<KeyType, ValueType>> all;
        for (size_t i = 0; i < Hashing_module; i++) {
            for (size_t j = 0; j < Table[i].size(); j++) {
                all.push_back(Table[i][j]);
            }
        }
        Hashing_module = std::max((size_t)2,
             Current_number_of_elements * (Left_border + Right_border) / 2);
        Table.clear();
        Table.resize(Hashing_module);
        for (size_t i = 0; i < all.size(); i++) {
            size_t index = Hasher(all[i].first) % Hashing_module;
            Table[index].push_back(all[i]);
        }
    }

    inline void check() {
        if (Hashing_module < Left_border * Current_number_of_elements) {
            rebuild();
        }
        if (Hashing_module > Right_border * Current_number_of_elements) {
            rebuild();
        }
    }
};

// This is a non-const iterator to the elements inserted into the HashMap
template<class KeyType, class ValueType, class Hash> class HashMapIter {
  private:
    size_t i, j;
    HashMap<KeyType, ValueType, Hash> *object = nullptr;

  public:
    HashMapIter() : object(nullptr) {
    }
    HashMapIter& operator=(HashMapIter other) {
        i = other.i;
        j = other.j;
        return *this;
    }
    HashMapIter(HashMap<KeyType, ValueType, Hash> &hm, size_t i1, size_t j1)
     : i(i1), j(j1), object(&hm) {
    }
    HashMapIter & operator++() {
        if (i == object-> Hashing_module) {
            return *this;
        }
        if (object->Table[i].size() <= j + 1) {
            size_t index = i + 1;
            if (index < object->Hashing_module) {
                while (object->Table[index].empty() &&
                        index != object->Hashing_module) {
                    index++;
                    if (index == object->Hashing_module) {
                        break;
                    }
                }
            }
            i = index;
            j = 0;
        } else {
            j++;
        }
        return *this;
    }
    HashMapIter operator++(int) {
        HashMapIter<KeyType, ValueType> temp = *this;
        ++(*this);
        return temp;
    }
    bool operator==(HashMapIter<KeyType, ValueType, Hash> other) {
        return i == other.i && j == other.j;
    }
    bool operator!=(HashMapIter<KeyType, ValueType, Hash> other) {
        return !(*this == other);
    }
    std::pair<const KeyType, ValueType>& operator*() {
        return reinterpret_cast<std::pair<const KeyType, ValueType>&>
            (object->Table[i][j]);
    }
    std::pair<const KeyType, ValueType>* operator->() {
        return reinterpret_cast<std::pair<const KeyType, ValueType>*>
            (&object->Table[i][j]);
    }
};

// This is a const iterator to the elements inserted into the HashMap.
template<class KeyType, class ValueType, class Hash> class HashMapCIter {
  private:
    size_t i, j;
    const HashMap<KeyType, ValueType, Hash> * object = nullptr;

  public:
    HashMapCIter() {
    }
    HashMapCIter& operator=(HashMapCIter other) {
        i = other.i;
        j = other.j;
        object = other.object;
        return *this;
    }
    HashMapCIter(const HashMap<KeyType, ValueType, Hash> &hm,
        size_t i1, size_t j1) : i(i1), j(j1),  object(&hm) {
    }
    HashMapCIter & operator++() {
        if (i == object->Hashing_module) {
            return *this;
        }
        if (object->Table[i].size() <= j + 1) {
            size_t index = i + 1;
            if (index < object->Hashing_module) {
                while (object->Table[index].empty() &&
                        index != object->Hashing_module) {
                    index++;
                    if (index == object->Hashing_module) {
                        break;
                    }
                }
            }
            i = index;
            j = 0;
        } else {
            j++;
        }
        return *this;
    }
    HashMapCIter operator++(int) {
        HashMapCIter<KeyType, ValueType> temp = *this;
        ++(*this);
        return temp;
    }
    bool operator==(HashMapCIter<KeyType, ValueType> other) {
        return i == other.i && j == other.j;
    }
    bool operator!=(HashMapCIter<KeyType, ValueType> other) {
        return !(*this == other);
    }
    const std::pair<const KeyType, ValueType>& operator*() {
        return reinterpret_cast<const std::pair<const KeyType, ValueType>&>
            (object->Table[i][j]);
    }
    const std::pair<const KeyType, ValueType>* operator->() {
        return reinterpret_cast<const std::pair<const KeyType, ValueType>*>
            (&object->Table[i][j]);
    }
};
