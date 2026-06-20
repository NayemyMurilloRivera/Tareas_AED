#include <iostream>
#include <windows.h>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cmath>
using namespace std;

struct nodito {
    int valor;
    int columna;
    int fil;
    nodito* next;
    nodito(int v, int row, int col)
        : valor(v), fil(row), columna(col), next(nullptr) {
    }
};

class sparsa {
public:
    int col, fil;
    nodito* head;

    sparsa(int coll, int fill) : head(nullptr), col(coll), fil(fill) {}

    bool findx(int x, int y, nodito*& h) {
        nodito* aux = head;
        for (; aux != nullptr && !(aux->fil == x && aux->columna == y); aux = aux->next);
        if (aux != nullptr) { h = aux; return true; }
        return false;
    }

    int get(int x, int y) {
        nodito* h = nullptr;
        if (findx(x, y, h)) return h->valor;
        return 0;
    }

    void set(int x, int y, int v) {
        nodito* h = nullptr;
        if (findx(x, y, h)) h->valor = v;
        else insert(x, y, v);
    }

    void insert(int x, int y, int v) {
        if (v == 0) return;
        nodito* h = nullptr;
        if (findx(x, y, h)) return;
        nodito* nuevo = new nodito(v, x, y);
        nuevo->next = head;
        head = nuevo;
    }

    void remove(int x, int y) {
        nodito* prev = nullptr;
        nodito* aux = head;
        for (; aux != nullptr && !(aux->fil == x && aux->columna == y);
            prev = aux, aux = aux->next);
        if (aux == nullptr) return;
        if (prev == nullptr) head = aux->next;
        else prev->next = aux->next;
        delete aux;
    }

    ~sparsa() { while (head) { nodito* t = head->next; delete head; head = t; } }
};

struct nodoD {
    double valor;
    int fil, col;
    nodoD* next;
    nodoD(double v, int r, int c) : valor(v), fil(r), col(c), next(nullptr) {}
};

class MatrizSparsa {
    int filas, cols;
    nodoD* head;

    bool find(int r, int c, nodoD*& h) const {
        nodoD* aux = head;
        for (; aux && !(aux->fil == r && aux->col == c); aux = aux->next);
        if (aux) { h = aux; return true; }
        return false;
    }

    string fmt(double v) const {
        int r = (int)round(v);
        if (fabs(v - r) < 1e-9) return to_string(r);
        ostringstream ss;
        ss << fixed << setprecision(2) << v;
        return ss.str();
    }

public:
    MatrizSparsa(int f, int c) : filas(f), cols(c), head(nullptr) {}

    double get(int r, int c) const {
        nodoD* h = nullptr;
        if (const_cast<MatrizSparsa*>(this)->find(r, c, h)) return h->valor;
        return 0.0;
    }

    void set(int r, int c, double v) {
        nodoD* h = nullptr;
        if (find(r, c, h)) {
            if (fabs(v) < 1e-12) {
                nodoD* prev = nullptr, * aux = head;
                for (; aux && !(aux->fil == r && aux->col == c); prev = aux, aux = aux->next);
                if (!aux) return;
                if (!prev) head = aux->next;
                else prev->next = aux->next;
                delete aux;
            }
            else {
                h->valor = v;
            }
        }
        else if (fabs(v) > 1e-12) {
            nodoD* n = new nodoD(v, r, c);
            n->next = head;
            head = n;
        }
    }

    void printGrid() const {
        int w = 7;
        auto sep = [&](string l, string m, string r, string h) {
            cout << l;
            for (int j = 0; j < cols; j++) {
                for (int k = 0; k < w; k++) cout << h;
                cout << (j < cols - 1 ? m : r);
            }
            cout << "\n";
            };
        sep("┌", "┬", "┐", "─");
        for (int i = 0; i < filas; i++) {
            cout << "│";
            for (int j = 0; j < cols; j++) {
                string s = fmt(get(i, j));
                int pad = w - (int)s.size();
                int lp = pad / 2, rp = pad - lp;
                cout << string(lp, ' ') << s << string(rp, ' ') << "│";
            }
            cout << "\n";
            if (i < filas - 1) sep("├", "┼", "┤", "─");
        }
        sep("└", "┴", "┘", "─");
    }

    void printSparsa() const {
        cout << "Posiciones validas:\n";
        for (nodoD* aux = head; aux != nullptr; aux = aux->next)
            cout << "  (" << aux->fil << "," << aux->col << ") = " << fmt(aux->valor) << "\n";
    }

    void swapFilas(int a, int b) {
        for (int j = 0; j < cols; j++) {
            double va = get(a, j), vb = get(b, j);
            set(a, j, vb);
            set(b, j, va);
        }
    }

    int getFilas() const { return filas; }
    int getCols()  const { return cols; }

    ~MatrizSparsa() { while (head) { nodoD* t = head->next; delete head; head = t; } }
};

void gaussiana(MatrizSparsa& M, int n) {
    for (int col = 0; col < n; col++) {
        int filaPivot = col;
        double maxVal = fabs(M.get(col, col));
        for (int i = col + 1; i < n; i++) {
            double v = fabs(M.get(i, col));
            if (v > maxVal) { maxVal = v; filaPivot = i; }
        }
        if (filaPivot != col) {
            cout << "Intercambio fila " << col << " con fila " << filaPivot << "\n";
            M.swapFilas(col, filaPivot);
        }

        double pivot = M.get(col, col);
        if (fabs(pivot) < 1e-12) {
            cout << "Pivote cero en columna " << col << "\n";
            continue;
        }

        for (int fila = col + 1; fila < n; fila++) {
            double factor = M.get(fila, col) / pivot;
            if (fabs(factor) < 1e-12) continue;
            for (int j = col; j <= n; j++)
                M.set(fila, j, M.get(fila, j) - factor * M.get(col, j));
        }

        cout << "Paso " << col + 1 << ":\n";
        M.printGrid();
    }

    vector<double> x(n, 0.0);
    for (int i = n - 1; i >= 0; i--) {
        double suma = M.get(i, n);
        for (int j = i + 1; j < n; j++)
            suma -= M.get(i, j) * x[j];
        x[i] = suma / M.get(i, i);
    }

    cout << "\nRespuesta:\n";
    for (int i = 0; i < n; i++) {
        int r = (int)round(x[i]);
        if (fabs(x[i] - r) < 1e-9)
            cout << "  x" << i << " = " << r << "\n";
        else
            cout << "  x" << i << " = " << fixed << setprecision(2) << x[i] << "\n";
    }

    cout << "\n";
    M.printSparsa();
}

int main() {
    SetConsoleOutputCP(65001);
    int n;
    cout << "Variables n: ";
    cin >> n;

    MatrizSparsa M(n, n + 1);

    for (int i = 0; i < n; i++) {
        cout << "Fila " << i << ": ";
        for (int j = 0; j <= n; j++) {
            double v; cin >> v;
            M.set(i, j, v);
        }
    }

    cout << "\nMatriz inicial:\n";
    M.printGrid();
    cout << "\nEliminacion:\n";
    gaussiana(M, n);

    return 0;
}
