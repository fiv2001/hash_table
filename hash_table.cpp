#include <vector>
#include <stdexcept>
template<class KeyType, class ValueType> class HashMapIter;
template<class KeyType, class ValueType> class HashMapCIter;

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> > class HashMap {
friend class HashMapIter<KeyType, ValueType>;
typedef HashMapIter<KeyType, ValueType> iterator;
typedef HashMapCIter<KeyType, ValueType> const_iterator;
private:
	size_t sz = 1;
	size_t n = 0;
	size_t L = 2, R = 3;
	Hash hasher;
	std::vector <std::vector <std::pair <KeyType, ValueType>> > table;
public:
	HashMap(Hash h = std::hash<KeyType>()) : hasher(h){
		table.resize(sz);
		// hasher = h;
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
			// check();
		}
	}
	template <typename Iter> HashMap(Iter begin, Iter end, Hash h = std::hash<KeyType>()) : hasher(h) {
		// hasher = h;
		table.resize(sz);
		while (begin != end) {
			insert(*begin);
			begin++;
		}
		// check();
	}
	HashMap(std::initializer_list<std::pair <KeyType, ValueType> > l, Hash h = std::hash<KeyType>()) : hasher(h) {
		// hasher = h;
		table.resize(sz);
		std::vector<std::pair <KeyType, ValueType>> v(l);
		for (size_t i = 0; i < v.size(); i++) {
			insert(v[i]);
		}
		// check();
	}
	void erase(std::pair <KeyType, ValueType> p) {
		size_t index = hasher(p.first) % sz;
		for (size_t i = 0; i < table[index].size(); i++) {
			if (table[index][i] == p) {
				n--;
				swap(table[index][i], table[index].back());
				table[index].pop_back();
			}
		}
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
		while (table[i].empty() && i != sz) {
			i++;
		}
		return iterator(*this, i, j); 
	}
	const_iterator begin() const {
		size_t i = 0, j = 0;
		while (table[i].empty() && i != sz) {
			i++;
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
			if (key == table[index][j]) {
				return iterator(*this, index, j);
			}
		}
		return end();
	}
	const_iterator find(KeyType &key) const {
		size_t index = hasher(key) % sz;
		for (size_t j = 0; j < table[index].size(); j++) {
			if (key == table[index][j]) {
				return const_iterator(*this, index, j);
			}
		}
		return end();
	}
	ValueType & operator[](KeyType &key) {
		size_t index = hasher(key) % sz;
		for (size_t j = 0; j < table[index].size(); j++) {
			if (key == table[index][j]) {
				return &table[index][j].second;
			}
		}
		// insult(jopa)
		insert(key, ValueType());
		return *this[key];
	}
	const ValueType & at(KeyType &key) const {
		size_t index = hasher(key) % sz;
		for (size_t j = 0; j < table[index].size(); j++) {
			if (key == table[index][j]) {
				return &table[index][j].second;
			}
		}
		throw std::out_of_range("gotcha");
	}
	void clear() {
		for (size_t index = 0; index < sz; index++) {
			table[index].clear();
		}
	}
};

template<class KeyType, class ValueType> class HashMapIter {
private:
	size_t i, j;
	HashMap<KeyType, ValueType> &object;
public:
	HashMapIter(HashMap<KeyType, ValueType> &hm, size_t i1, size_t j1) : object(hm), i(i1), j(j1) {
	}
	HashMapIter & operator++() {
		if (object[i].size() == j + 1) {
			size_t index = i + 1;
			while (object.table[index].empty() && index != object.sz) {
				index++;
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
	bool operator==(HashMapIter<KeyType, ValueType> other) {
		return i == other.i && j == other.j;
	}
	bool operator!=(HashMapIter<KeyType, ValueType> other) {
		return !(*this == other);
	}
	std::pair<const KeyType, ValueType>& operator*() {
		return object.table[i][j];
	}
	std::pair<const KeyType, ValueType>* operator->() {
		return &object.table[i][j];
	}
};

template<class KeyType, class ValueType> class HashMapCIter {
private:
	size_t i, j;
	HashMap<KeyType, ValueType> &object;
public:
	HashMapCIter(HashMap<KeyType, ValueType> &hm, size_t i1, size_t j1) : object(hm), i(i1), j(j1) {
	}
	HashMapCIter & operator++() {
		if (object[i].size() == j + 1) {
			size_t index = i + 1;
			while (object.table[index].empty() && index != object.sz) {
				index++;
			}
			i = index;
			j = 0;
		} else {
			j++;
		}
		return *this;
	}
	HashMapCIter operator++(int) {
		HashMapIter<KeyType, ValueType> temp = *this;
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
		return object.table[i][j];
	}
	const std::pair<const KeyType, ValueType>* operator->() {
		return &object.table[i][j];
	}
};
