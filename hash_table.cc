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

template <typename T>
struct comparer {
    bool operator()(const T& a, const T& b) = 0;
};

template <> struct comparer<string> {
    bool operator()(const string& a, const string& b) {
        cout << "operator() " << a << " " << b << "\n";
        return a == b;
    }
};

template <> struct comparer<int32_t> {
    bool operator()(const int32_t &a, const int32_t& b) {
        return a == b;
    }
};

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
        load_factor = 3.0;
    }

    ~hash_table() { clear(); }

    void add(const K& k, const V& v);
    bool find(const K& k, V& v);
    bool remove(const K& k);
    void dump();
private:

    uint32_t base;
    int32_t count;
    float load_factor;

    struct entry {
        entry() { }
        entry(const K& k, const V& v) {
            key = k;
            value = v;
        }
        entry (const entry& e) {
            key = e.key;
            value = e.value;
        }
        K key;
        V value;
    };

    //no clue why. read this
    //http://stackoverflow.com/questions/3311633/nested-templates-with-dependent-scope
    list<entry> free_list;
    typedef typename list<entry>::iterator entry_list_iterator;

    vector<list<entry> > * buckets;

    int32_t hash(const K& k) {
        const float A = (sqrt(5) - 1)/2; //CLRS
        int32_t h = hash_type<K>(k);
        return static_cast<int32_t>((1 << base)*(h * A - floor(h * A)));
    }

    void update(bool add);
    void rehash();
    void add_l(const K& k, const V& v);
    void clear();
};

template <typename K, typename V>
void hash_table<K, V>::add_l(const K& k, const V& v)
{
    entry_list_iterator it = free_list.begin();
    if (buckets == NULL) {
        buckets = new vector<list<entry> >(1 << base);
    }

    entry e;
    if (free_list.empty()) {
        e.key = k;
        e.value = v;
    } else {
        entry_list_iterator i = free_list.begin();
        e = *i;
        free_list.erase(i);
        e.key = k;
        e.value = v;
    }
    int32_t h = hash(k);
    buckets->at(h).push_back(e);
}

template <typename K, typename V>
void hash_table<K, V>::add(const K& k, const V& v)
{
    add_l(k,v);
    update(true /*add*/);
}

template <typename K, typename V>
bool hash_table<K, V>::find(const K& k, V& v)
{
    int32_t h = hash(k);
    list<entry> &l = buckets->at(h);
    entry_list_iterator iter = l.begin();
    while (iter != l.end()) {
        if ((*iter).key == k) {
            v = (*iter).value;
            return true;
        }
        iter++;
    }
    return false;
}

template <typename K, typename V>
bool hash_table<K, V>::remove(const K& k)
{
    int32_t h = hash(k);
    //remember we just need the reference. without &, the list is copied over!
    list<entry> &l = buckets->at(h);
    entry_list_iterator iter = l.begin();
    while (iter != l.end()) {
        if (k == (*iter).key) {
            // this shouldn't call the destructor if pointers are stored.
            // hqowever if the object were stored, that would be deleted.
            free_list.push_back(*iter);
            l.erase(iter);
            update(false /*add*/);
            return true;
        }
        iter++;
    }
    return false;
}

template <typename K, typename V>
void hash_table<K, V>::dump()
{
    cout << "\n";

    if (!buckets) {
        cout << "empty\n";
        return;
    }

    for (size_t i = 0; i < buckets->size(); i++) {
        list<entry> &l = buckets->at(i);
        entry_list_iterator it = l.begin();
        cout << "slot " << i << " - ";
        while (it != l.end()) {
            entry &e = *it;
            cout << "(" << e.key << ", " << e.value << ") ";
            it++;
        }
        cout << "\n";
    }
    cout << "\n";
}

template <typename K, typename V>
void hash_table<K, V>::update(bool add)
{
    float load;
    if (add) {
        ++count;
        load = (count * 1.0 / (1 << base));
        if (load <= load_factor) {
            return;
        }
        base += 1;
        rehash();
    } else {
        --count;
        load = (count * 1.0 / (1 << base));
        if (load >= (load_factor / 4)) {
            return;
        }
        base -= 1;
        if (base == 0) {
            base = 1;
        }
        rehash();
    }
    dump();
}

template <typename K, typename V>
void hash_table<K, V>::rehash()
{
    vector<list<entry> > *tmp_buckets = buckets;
    buckets = NULL;
    while (!tmp_buckets->empty()) {
        list<entry> &l = *tmp_buckets->begin();
        while (!l.empty()) {
            entry &e = *l.begin();
            add_l(e.key, e.value);
            l.erase(l.begin());
        }
        tmp_buckets->erase(tmp_buckets->begin());
    }
    tmp_buckets->clear();
    delete tmp_buckets;
}

template <typename K, typename V>
void hash_table<K, V>::clear()
{
    count = 0;
    if (!buckets) {
        return;
    }

    vector<list<entry> > *tmp_buckets = buckets;
    buckets = NULL;
    while (!tmp_buckets->empty()) {
        list<entry> &l = *tmp_buckets->begin();
        l.clear();
        tmp_buckets->erase(tmp_buckets->begin());
    }
    tmp_buckets->clear();
    delete tmp_buckets;
}


void test1()
{
    hash_table<string, string> dict;
    dict.add("firstname", "haynes");
    dict.add("lastname", "george");
    dict.dump();
    string v;
    cout << dict.find("1", v) << "\n";
    dict.remove("firstname");
    dict.dump();
}

void test2()
{
    hash_table<int32_t, int32_t> dict;
    vector<int32_t> keys;

    for (size_t i = 0; i < 20; i++) {
        int k = rand() % 20;
        keys.push_back(k);
        dict.add(k, rand() % 100);
    }

    dict.dump();
    
    for (size_t i = 0; i < keys.size(); i++) {
        dict.remove(keys.at(i));
    }

    dict.dump();
}

int main(int argc, char **argv) {
    test2();
    return 0;
}
