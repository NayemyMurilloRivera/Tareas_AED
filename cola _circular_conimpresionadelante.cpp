#include <iostream>
using namespace std;

template<class t, int n = 10>
struct cola {
    int num;
    t* starr;
    t* head;
    t* pot;

    cola();
    ~cola();
    bool lleno();
    bool vacio();
    bool push(t val);
    bool pop(t& val);
    void filita();
};

template<class t, int n>
cola<t, n>::cola() {
    num = n;
    starr = new t[num];
    head = nullptr;
    pot = nullptr;
}

template<class t, int n>
cola<t, n>::~cola() {
    delete[] starr;
}

template<class t, int n>
bool cola<t, n>::vacio() {
    return head == nullptr;
}

template<class t, int n>
bool cola<t, n>::lleno() {
    if (vacio()) return false;

    t* next;
    if (pot == starr + num - 1)
        next = starr;
    else
        next = pot + 1;

    if (next == head)
        return true;
    else
        return false;
}

template<class t, int n>
bool cola<t, n>::push(t val) {
    if (lleno()) return false;

    if (vacio()) {
        head = starr;
        pot = starr;
    }
    else {
        if (pot == starr + num - 1)
            pot = starr;
        else
            pot = pot + 1;
    }

    *pot = val;
    return true;
}

template<class t, int n>
bool cola<t, n>::pop(t& val) {
    if (vacio()) return false;

    val = *head;

    if (head == pot) {
        head = nullptr;
        pot = nullptr;
    }
    else {
        if (head == starr + num - 1)
            head = starr;
        else
            head = head + 1;
    }

    return true;
}

template<class t, int n>
void cola<t, n>::filita() {
    if (vacio()) {
        cout << "\nCola vacia\n";
        return;
    }
    int cnt = 0;
    t* aux = head;
    while (true) {
        cnt++;
        if (aux == pot)
            break;
        if (aux == starr + num - 1)
            aux = starr;
        else
            aux = aux + 1;
    }

    t* imprimir = pot;
    for (int i = 0; i < cnt; i++) {
        cout << *imprimir;
        if (i < cnt - 1)
            cout << "-";

        if (imprimir == starr)
            imprimir = starr + num - 1;
        else
            imprimir = imprimir - 1;
    }
    cout << endl;
}

int main() {
    cola<int, 10> colita;

    for (int i = 1; i <= 10; i++)
        colita.push(i);
    colita.filita(); 

    int x;
 
    colita.pop(x);
    colita.pop(x);
    colita.pop(x);

   
    colita.push(0);
    colita.push(-1);
    colita.push(-2);
    colita.filita(); 

    return 0;
}
