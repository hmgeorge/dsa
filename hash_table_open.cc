#include <list>
#include <iostream>
#include <map>
#include <string>
#include <cmath>
#include <vector>
#include <cstdlib>

using namespace std;

typedef int int32_t;
typedef unsigned int uint32_t;

//create hash_type template function
//this must be specialized for a type if it needs to be hashed.
//See BasicHashTable_test.cpp
template <typename T> uint32_t hash_type(const T & k) {
    return 0;
}

//template specializations for int and string
template <>
uint32_t hash_type<int32_t>(const int32_t &i) {
    return i;
}

//with const, even constant strings can be passed
template <> uint32_t hash_type<string>(const string &s) {
    uint32_t h = 0;
    string::const_iterator iter = s.begin();
    while (iter != s.end()) {
        h += (*iter - '0');
        iter++;
    }
    h += s.length();
    return h;
}

template <typename K, typename V>
class hash_table {
public:
    hash_table() {
        base = 1;
        buckets = NULL;
        count = 0;
        load_factor = .7;
    }

    void add(const K& k, const V& v);
    bool find(const K& k, V& v);
    bool remove(const K& k);
    void dump();
private:
    uint32_t base;
    int32_t count;
    float load_factor;

    struct entry {
        entry() { flags = FREE; }
        entry(const K& k, const V& v) {
            key = k;
            value = v;
            flags = FREE;
        }

        bool is_free() { return flags & FREE; }

        bool set_free() {
            if (is_free()) {
                cout << "entry was already free, FIXME\n";
                return false;
            }
            flags = FREE;
        }

        bool set(const K&k, const V& v) {
            if (!is_free()) {
                cout << "tried setting to an inuse entry, FIXME!\n";
                return false;
            }

            key = k;
            value = v;
            flags = PRESENT;
            return true;
        }

        enum {
            FREE = 0, //entry is free
            PRESENT = 1, //entry has valid k,v
        };

        K key;
        V value;
        uint32_t flags;
    };

    //In open addressing mode, each slot stores an entry
    vector<entry> * buckets;
    int32_t hash(const K& k) {
        const float A = (sqrt(5) - 1)/2; //CLRS
        int32_t h = hash_type<K>(k);
        return static_cast<int32_t>((1 << base)*(h * A - floor(h * A)));
    }

    int32_t hash2(const K& k) {
        //m = 1 << base
        //make result of hash2 an odd number so that
        //m and the result are co-primes
        int32_t h = hash_type<K>(k);
        return (h % ((1 << base) - 1) | 1);

        //another secondary hash is to set m to be a prime number
        //and design h2 so that it always returns a positive integer < m
        //1 + (k mod (m-1))
    }

    void update(bool add);
    void rehash();
    void add_l(const K& k, const V& v);
    void init_buckets();
};

template <typename K, typename V>
void hash_table<K, V>::init_buckets()
{
    buckets = new vector<entry>(1 << base);
    for (size_t i; i < buckets->size(); i++) {
        entry e;
        buckets->at(i) = e;
    }
}

template <typename K, typename V>
void hash_table<K, V>::add_l(const K& k, const V& v)
{
    uint32_t h, h1 = hash(k), h2 = hash2(k);
    uint32_t i = 0;
    uint32_t m = 1 << base;

    if (buckets == NULL) {
        init_buckets();
    }

    do {
        h = (h1 + i*h2) % m;
    } while (++i < m && !buckets->at(h).is_free());

    if (i == m) {
        cout << "no free slot available!? FIXME \n";
        exit(-1);
    }

    buckets->at(h).set(k, v);
}

template <typename K, typename V>
void hash_table<K, V>::add(const K& k, const V& v)
{
    add_l(k, v);
    update(true /*add*/);
}

template <typename K, typename V>
bool hash_table<K, V>::find(const K& k, V& v)
{
    uint32_t h, h1 = hash(k), h2 = hash2(k);
    uint32_t i = 0, m = 1 << base;
    
    do {
        h = (h1 + i*h2) % m;
        entry &e = buckets->at(h);
        if (!e.is_free()) {
            if (e.key == k) {
                v = e.value;
                return true;
            }
        }
    } while (++i < m);
     return false;
}

template <typename K, typename V>
bool hash_table<K, V>::remove(const K& k)
{
    uint32_t h, h1 = hash(k), h2 = hash2(k);
    uint32_t i = 0, m = 1 << base;
    
    do {
        h = (h1 + i*h2) % m;
        entry &e = buckets->at(h);
        if (!e.is_free()) {
            if (e.key == k) {
                return e.set_free();
            }
        }
    } while (++i < m);
    // update is not called for open addressing mode.
    return false;
}

template <typename K, typename V>
void hash_table<K, V>::dump()
{
    cout << "\n";

    if (!buckets) {
        cout << "empty\n";
    }

    for (size_t i = 0; i < buckets->size(); i++) {
        entry &e = buckets->at(i);
        cout << "(" << e.key << ", " << e.value << ", "
             << (e.is_free() ? "F" : "U") << ")\n";
    }
}

template <typename K, typename V>
void hash_table<K, V>::update(bool add)
{
    float load;

    if (!add) {
        return;
    }

    ++count;
    load = (count * 1.0 / (1 << base));
    if (load <= load_factor) {
        return;
    }
    rehash();
}

template <typename K, typename V>
void hash_table<K, V>::rehash()
{
    //buckets should not be NULL.
    vector<entry> * tmp_buckets = buckets;
    base += 1;
    buckets = NULL;
    init_buckets();

    while (tmp_buckets->empty()) {
        entry &e = *tmp_buckets->begin();
        add_l(e.key, e.value);
        tmp_buckets->erase(tmp_buckets->begin());
    }
    tmp_buckets->clear();
    delete tmp_buckets;
}

int main(int argc, char **argv) {
    hash_table<int32_t, int32_t> dict;
}
