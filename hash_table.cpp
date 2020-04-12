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
  private:
    size_t sz = 1;
    size_t n = 0;
    size_t L = 1, R = 3;
    Hash hasher;
    std::vector <std::vector <std::pair <KeyType, ValueType>> > table;
    void rebuild() {
        std::vector <std::pair<KeyType, ValueType>> all;
        for (size_t i = 0; i < sz; i++) {
            for (size_t j = 0; j < table[i].size(); j++) {
                all.push_back(table[i][j]);
            }
        }
        sz = std::max((size_t)2, n * (L + R) / 2);
        table.clear();
        table.resize(sz);
        for (size_t i = 0; i < all.size(); i++) {
            size_t index = hasher(all[i].first) % sz;
            table[index].push_back(all[i]);
        }
    }
    inline void check() {
        if (sz < L * n) {
            rebuild();
        }
        if (sz > R * n) {
            rebuild();
        }
    }

 public:
    HashMap(Hash h = Hash()) : hasher(h) {
        table.resize(sz);
    };
    void insert(std::pair <KeyType, ValueType> p) {
        size_t index = hasher(p.first) % sz;
        bool exists = 0;
        for (size_t i = 0; i < table[index].size(); i++) {
            if (table[index][i].first == p.first) {
                exists = 1;
            }
        }
        if (!exists) {
            n++;
            table[index].push_back(p);
            check();
        }
    }
    template <typename Iter>
    HashMap(Iter begin, Iter end, Hash h = Hash()) : hasher(h) {
        table.resize(sz);
        while (begin != end) {
            insert(*begin);
            begin++;
        }
        check();
    }
    HashMap(std::initializer_list<std::pair <KeyType, ValueType> > l,
         Hash h = Hash()) : hasher(h) {
        table.resize(sz);
        std::vector<std::pair <KeyType, ValueType>> v(l);
        for (size_t i = 0; i < v.size(); i++) {
            insert(v[i]);
        }
        check();
    }
    void erase(KeyType key) {
        size_t index = hasher(key) % sz;
        for (size_t i = 0; i < table[index].size(); i++) {
            if (table[index][i].first == key) {
                n--;
                swap(table[index][i], table[index].back());
                table[index].pop_back();
            }
        }
        check();
    }
    bool empty() const {
        return n == 0;
    }
    size_t size() const {
        return n;
    }
    Hash hash_function() const {
        return hasher;
    };
    iterator begin() {
        size_t i = 0, j = 0;
        while (table[i].empty()) {
            i++;
            if (i == sz) {
                break;
            }
        }
        return iterator(*this, i, j);
    }
    const_iterator begin() const {
        size_t i = 0, j = 0;
        while (table[i].empty()) {
            i++;
            if (i == sz) {
                break;
            }
        }
        return const_iterator(*this, i, j);
    }
    iterator end() {
        return iterator(*this, sz, 0);
    }
    const_iterator end() const {
        return const_iterator(*this, sz, 0);
    }
    iterator find(KeyType &key) {
        size_t index = hasher(key) % sz;
        for (size_t j = 0; j < table[index].size(); j++) {
            if (key == table[index][j].first) {
                return iterator(*this, index, j);
            }
        }
        return end();
    }
    iterator find(KeyType &&key) {
        size_t index = hasher(key) % sz;
        for (size_t j = 0; j < table[index].size(); j++) {
            if (key == table[index][j].first) {
                return iterator(*this, index, j);
            }
        }
        return end();
    }
    const_iterator find(KeyType &key) const {
        size_t index = hasher(key) % sz;
        for (size_t j = 0; j < table[index].size(); j++) {
            if (key == table[index][j].first) {
                return const_iterator(*this, index, j);
            }
        }
        return end();
    }
    const_iterator find(KeyType &&key) const {
        size_t index = hasher(key) % sz;
        for (size_t j = 0; j < table[index].size(); j++) {
            if (key == table[index][j].first) {
                return const_iterator(*this, index, j);
            }
        }
        return end();
    }
    ValueType & operator[](KeyType &key) {
        size_t index = hasher(key) % sz;
        for (size_t j = 0; j < table[index].size(); j++) {
            if (key == table[index][j].first) {
                return table[index][j].second;
            }
        }
        insert({key, ValueType()});
        return (*this)[key];
    }
    ValueType & operator[](KeyType &&key) {
        size_t index = hasher(key) % sz;
        for (size_t j = 0; j < table[index].size(); j++) {
            if (key == table[index][j].first) {
                return table[index][j].second;
            }
        }
        insert({key, ValueType()});
        return (*this)[key];
    }
    const ValueType & at(KeyType &key) const {
        size_t index = hasher(key) % sz;
        for (size_t j = 0; j < table[index].size(); j++) {
            if (key == table[index][j]) {
                return &table[index][j].second;
            }
        }
        throw std::out_of_range("");
    }
    const ValueType & at(KeyType &&key) const {
        size_t index = hasher(key) % sz;
        for (size_t j = 0; j < table[index].size(); j++) {
            if (key == table[index][j].first) {
                return table[index][j].second;
            }
        }
        throw std::out_of_range("");
    }
    void clear() {
        for (size_t index = 0; index < sz; index++) {
            table[index].clear();
        }
        n = 0;
        check();
    }
};

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
        if (i == object-> sz) {
            return *this;
        }
        if (object->table[i].size() <= j + 1) {
            size_t index = i + 1;
            if (index < object->sz) {
                while (object->table[index].empty() && index != object->sz) {
                    index++;
                    if (index == object->sz) {
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
            (object->table[i][j]);
    }
    std::pair<const KeyType, ValueType>* operator->() {
        return reinterpret_cast<std::pair<const KeyType, ValueType>*>
            (&object->table[i][j]);
    }
};

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
        if (i == object->sz) {
            return *this;
        }
        if (object->table[i].size() <= j + 1) {
            size_t index = i + 1;
            if (index < object->sz) {
                while (object->table[index].empty() && index != object->sz) {
                    index++;
                    if (index == object->sz) {
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
            (object->table[i][j]);
    }
    const std::pair<const KeyType, ValueType>* operator->() {
        return reinterpret_cast<const std::pair<const KeyType, ValueType>*>
            (&object->table[i][j]);
    }
};
