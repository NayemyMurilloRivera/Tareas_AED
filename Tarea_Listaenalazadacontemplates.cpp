1 ) Para cada nodo: Libro : Titulo , autor , precio , calificacion (1-5) con menu interactivo , este libro se debe arregalar en la lista enlazada segun el orden ya sea ascen o descen (sin repeticiones)
2) Cambiar en templates // TAREA 1: primera tarea asc , des (foto de la pizarra)
3) TAREA 2: Cambiar nodos en libros , llene la lista enlazada segun el tipo , por titulo , precio , autor , asi por cada uno. ( este es de pensamiento libre : fuerza bruta (implementacion de functores) , mover segun el return))
web scraping // revision de paginas https://toscrape.com/ para los datos
#include <iostream>
#include <string>
using namespace std;
struct Libro {
    string titulo{};
    string autor{};
    double precio{ 0.0 };
    int calificacion{ 0 };
};

template <typename T>
struct CNode {
    T value;
    CNode<T>* next;

    CNode(T v) : value(v), next(nullptr) {}
};

struct ComparadorLibro {
    int criterio; 
    bool asc;

    ComparadorLibro(int c, bool a) : criterio(c), asc(a) {}

    bool operator()(const Libro& a, const Libro& b) {
        switch (criterio) {
        case 1: return asc ? a.titulo < b.titulo : a.titulo > b.titulo;
        case 2: return asc ? a.autor < b.autor : a.autor > b.autor;
        case 3: return asc ? a.precio < b.precio : a.precio > b.precio;
        case 4: return asc ? a.calificacion < b.calificacion : a.calificacion > b.calificacion;
        }
        return false;
    }
};

template <typename T, typename Compare>
class CSortedList {
public:
    CSortedList(Compare c) : head(nullptr), comp(c) {}

    bool find(T x, CNode<T>**& p) {
        for (p = &head; *p && comp((*p)->value, x); p = &((*p)->next));

        return *p &&
            !comp(x, (*p)->value) &&
            !comp((*p)->value, x);
    }

    bool ins(T x) {
        CNode<T>** p;

        if (find(x, p)) return false;

        CNode<T>* t = new CNode<T>(x);
        t->next = *p;
        *p = t;

        return true;
    }

    void print() {
        for (CNode<T>* t = head; t; t = t->next) {
            cout << t->value.titulo << " | "
                << t->value.autor << " | "
                << t->value.precio << " | "
                << t->value.calificacion << endl;
        }
    }

    void buscarTitulo(const string& titulo) {
        for (CNode<T>* t = head; t; t = t->next) {
            if (t->value.titulo == titulo) {
                cout << t->value.titulo << " | "
                    << t->value.autor << " | "
                    << t->value.precio << " | "
                    << t->value.calificacion << endl;
            }
        }
    }

    void buscarAutor(const string& autor) {
        for (CNode<T>* t = head; t; t = t->next) {
            if (t->value.autor == autor) {
                cout << t->value.titulo << " | "
                    << t->value.autor << " | "
                    << t->value.precio << " | "
                    << t->value.calificacion << endl;
            }
        }
    }

private:
    CNode<T>* head;
    Compare comp;
};

int main() {
    int criterio, orden =1;

    cout << "\n1. Titulo\n2. Autor\n3. Precio\n4. Calificacion\n";
    cin >> criterio;
    bool asc = true;

    if (criterio == 3 || criterio == 4) {
        int orden;
        cout << "Orden:\n1. Ascendente\n2. Descendente\n";
        cin >> orden;
        asc = (orden == 1);
    }


    ComparadorLibro comp(criterio, orden == 1);
    CSortedList<Libro, ComparadorLibro> lista(comp);

    lista.ins({ "1984", "Orwell", 40, 5 });
    lista.ins({ "ElQuijote", "Cervantes", 30, 4 });
    lista.ins({ "CienAnosDeSoledad", "GarciaMarquez", 45, 5 });
    lista.ins({ "LaMetamorfosis", "Kafka", 25, 5 });
    lista.ins({ "ElPrincipito", "SaintExupery", 20, 4 });
    lista.ins({ "It", "King", 50, 5 });
    lista.ins({ "Dracula", "Stoker", 35, 4 });
    lista.ins({ "Hamlet", "Shakespeare", 28, 5 });
    lista.ins({ "Odisea", "Homero", 32, 4 });
    lista.ins({ "Fahrenheit451", "Bradbury", 38, 5 });
    lista.ins({ "Dune", "Herbert", 55, 5 });
    lista.ins({ "Fundacion", "Asimov", 48, 5 });
    lista.ins({ "ElAlquimista", "Coelho", 22, 3 });
    lista.ins({ "Inferno", "Dante", 33, 5 });
    lista.ins({ "OrgulloYPrejuicio", "Austen", 27, 4 });

    int op;
    do {
        cout << "\n1. Insertar";
        cout << "\n2. Mostrar todos";
        cout << "\n3. Buscar por titulo";
        cout << "\n4. Buscar por autor";
        cout << "\n0. Salir\n";

        cin >> op;

        if (op == 1) {
            Libro l;
            cin >> l.titulo >> l.autor >> l.precio >> l.calificacion;
            lista.ins(l);
        }
        else if (op == 2) {
            lista.print();
        }
        else if (op == 3) {
            string t;
            cin >> t;
            lista.buscarTitulo(t);
        }
        else if (op == 4) {
            string a;
            cin >> a;
            lista.buscarAutor(a);
        }

    } while (op != 0);

    return 0;
}
