
#include <iostream>
#include <stack>
#include <vector>
#include <cctype>
using namespace std;

class polar {
private:
    vector<string> resultado;
    stack<string> pela;

public:
    void forma_pila(string a);
    int precedencia(string a);
    void impresion();
    vector<string> obtenerResultado();
};

int polar::precedencia(string a) {
    if (a == "+" || a == "-") return 1;
    if (a == "*" || a == "/" || a == "%") return 2;
    if (a == "^") return 3;
    if (a == "(") return 4;
    if (a == ")") return 5;
    return 0;
}

void polar::forma_pila(string a) {
    for (int i = 0; i < a.length(); i++) {
        string c(1, a[i]);
        int p = precedencia(c);

        if (p == 4) {
            pela.push(c);
        }
        else if (p == 0) {
            resultado.push_back(c);
        }
        else if (p == 2) {
            while (!pela.empty() && precedencia(pela.top()) != 4 && precedencia(pela.top()) >= 2) {
                resultado.push_back(pela.top());
                pela.pop();
            }
            pela.push(c);
        }
        else if (p == 3) {
            while (!pela.empty() && precedencia(pela.top()) != 4 && precedencia(pela.top()) > 3) {
                resultado.push_back(pela.top());
                pela.pop();
            }
            pela.push(c);
        }
        else if (p == 1) {
            while (!pela.empty() && precedencia(pela.top()) != 4 && precedencia(pela.top()) >= 1) {
                resultado.push_back(pela.top());
                pela.pop();
            }
            pela.push(c);
        }
        else if (p == 5) {
            while (!pela.empty() && precedencia(pela.top()) != 4) {
                resultado.push_back(pela.top());
                pela.pop();
            }
            if (!pela.empty()) pela.pop();
        }
    }

    while (!pela.empty()) {
        resultado.push_back(pela.top());
        pela.pop();
    }
}

void polar::impresion() {
    for (string c : resultado) {
        cout << c;
    }
}

vector<string> polar::obtenerResultado() {
    return resultado;
}

bool esNumero(string c) {
    return isdigit(c[0]);
}

// 🔥 EVALUADOR COMPLETO Y SEGURO
bool evaluar_postfijo(vector<string> resultado, int &res) {
    stack<int> s;

    for (string c : resultado) {
        if (esNumero(c)) {
            s.push(stoi(c));
        } else {
            if (s.size() < 2) {
                cout << "\nError: expresion invalida\n";
                return false;
            }

            int b = s.top(); s.pop();
            int a = s.top(); s.pop();

            if (c == "+") s.push(a + b);
            else if (c == "-") s.push(a - b);
            else if (c == "*") s.push(a * b);
            else if (c == "/") {
                if (b == 0) {
                    cout << "\nError: division entre cero\n";
                    return false;
                }
                s.push(a / b);
            }
            else if (c == "%") {
                if (b == 0) {
                    cout << "\nError: modulo por cero\n";
                    return false;
                }
                s.push(a % b);
            }
            else if (c == "^") {
                int potencia = 1;
                for (int i = 0; i < b; i++) potencia *= a;
                s.push(potencia);
            }
        }
    }

    if (s.size() != 1) {
        cout << "\nError: expresion invalida\n";
        return false;
    }

    res = s.top();
    return true;
}

int main() {
    string a;
    cout << "Ingresa expresion (ej: (8-2)/(1-1)): ";
    cin >> a;

    polar obj;
    obj.forma_pila(a);

    cout << "Postfijo: ";
    obj.impresion();

    int resultado_final;
    if (evaluar_postfijo(obj.obtenerResultado(), resultado_final)) {
        cout << "\nResultado: " << resultado_final;
    } else {
        cout << "\nNo se pudo calcular el resultado.";
    }

    return 0;
}
