#include <iostream>
using namespace std;
template <class T>
struct CNode
{
    T value;
    CNode<T>* next;

    CNode(T v)
    {
        value = v;
        next = nullptr;
    }
};
template <class T>
struct Asc {
    bool operator()(const T& a, const T& b) {
        return a < b;
    }
};

template <class T>
struct Desc {
    bool operator()(const T& a, const T& b) {
        return a > b;
    }
};

template <class T, class O>
class CSortedList
{
public:
    CSortedList(O c);
    ~CSortedList();
    bool find(T x, CNode<T>**& p);
    bool ins(T x);
    bool rem(T x);
    void print();
private:
    CNode<T>* head;
    O comp;
};

template<class T, class O>
CSortedList<T,O>::CSortedList(O c)
{
    head = nullptr;
    comp = c;
}

template<class T, class O>
CSortedList<T,O>::~CSortedList()
{
    while (head) {
        CNode<T>* t = head;
        head = head->next;
        delete t;
    }
}

template<class T, class O>
bool CSortedList<T,O>::find(T x, CNode<T>**& p)
{
    for (p = &head; *p && comp((*p)->value, x); p = &((*p)->next));

    return *p &&
           !comp(x, (*p)->value) &&
           !comp((*p)->value, x);
}

template<class T, class O>
bool CSortedList<T,O>::ins(T x)
{
    CNode<T>** p;

    if (find(x, p)) return false;

    CNode<T>* t = new CNode<T>(x);
    t->next = *p;
    *p = t;

    return true;
}

template<class T, class O>
bool CSortedList<T,O>::rem(T x)
{
    CNode<T>** p;

    if (!find(x, p)) return false;

    CNode<T>* t = *p;
    *p = t->next;
    delete t;

    return true;
}

template<class T, class O>
void CSortedList<T,O>::print()
{
    for (CNode<T>* t = head; t; t = t->next)
        cout << t->value << " ";
    cout << "\n";
}

int main()
{
    CSortedList<int, Asc<int>> l(Asc<int>()); // ascendente
    // CSortedList<int, Desc<int>> l(Desc<int>()); // descendente

    l.ins(3); l.ins(14); l.ins(7); l.ins(11);
    l.print();

    l.rem(11); l.print();
    l.rem(14); l.print();
    l.rem(3); l.print();
    l.rem(7); l.print();
}
