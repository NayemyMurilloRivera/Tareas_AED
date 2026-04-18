#include <iostream>
using namespace std;

template<class t, int n = 10>
struct cola_circular {
    int n_elementos;
    t* arrpr;
    t* head;
    t* tail;

    cola_circular();
    ~cola_circular();
    bool lleno();
    bool vacio();
    bool push(t val);
    bool pop(t& val);
    void print();
    void print_normal();
};

template<class t, int n>
cola_circular<t, n>::cola_circular() {
    n_elementos = n;
    arrpr = new t[n_elementos];
    head = nullptr;
    tail = nullptr;
}

template<class t, int n>
cola_circular<t, n>::~cola_circular() {
    delete[] arrpr;
}

template<class t, int n>
bool cola_circular<t, n>::vacio() {
    return head == nullptr;
}

template<class t, int n>
bool cola_circular<t, n>::lleno() {
    if (vacio()) return false;

    t* next;
    if (tail == arrpr + (n_elementos - 1))
        next = arrpr;
    else
        next = tail + 1;

    return next == head;
}

template<class t, int n>
bool cola_circular<t, n>::push(t val) {
    if (lleno()) return false;

    if (vacio()) {
        head = arrpr;
        tail = arrpr;
    }
    else {
        if (tail == arrpr + (n_elementos - 1))
            tail = arrpr;
        else
            ++tail;
    }

    *tail = val;
    return true;
}

template<class t, int n>
bool cola_circular<t, n>::pop(t& val) {
    if (vacio()) return false;

    val = *head;

    if (head == tail) {
        head = nullptr;
        tail = nullptr;
    }
    else {
        if (head == arrpr + (n_elementos - 1))
            head = arrpr;
        else
            ++head;
    }

    return true;

}

template<class T, int n>
void cola_circular<T, n>::print_normal() {
    if (vacio()) {
        cout << "Cola vacia\n";
        return;
    }
    cout << "Impresion normal:[";

    for (T* p = head;p !=tail ; p++) {
        if ( p == arrpr + (n_elementos)) {
            p = arrpr;
            
        }
        cout << *p;
    }
    cout << *tail;

    cout << "]";

}
template<class T, int n>
void cola_circular<T, n>::print() {
    if (vacio()) {
        cout << "Cola vacia\n";
        return;
    }

    cout << "\n";

    if (head == arrpr || head < tail) {
        T* aux = head;
        for (; aux <= arrpr + (n_elementos - 1); aux++) {
            cout << "[" << *aux << "]";
        }
    }

    else {
        T* aux = arrpr;
        for (; aux <= arrpr + (n_elementos - 1); aux++) {
            cout << "[" << *aux << "]";
        }
    }
    cout << "\nhead -> " << *head;
    cout << "\ntail -> " << *tail << "\n";
}
int main() {
    cola_circular<int, 5> p;

    p.push(1);
    p.push(2);
    p.push(3);
    p.push(4);
    p.push(5);

    p.print();
    p.print_normal();

    int x;
    p.pop(x);
    p.pop(x);

    p.print();


    p.push(6);
    p.push(7);
    p.pop(x);
    p.push(8);

    p.print();
    p.print_normal();
    

    return 0;
}
