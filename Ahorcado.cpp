#include <iostream>
#include <ctime>
using namespace std;

struct Estado {
    string progreso;
    int vidas;
    string usadas;
};

struct Juego {
    int vidas;
    string palabra;
    string progreso;
    string usadas;

    Estado historial[10];
    int idx;
    int movimientos;

    Juego();
    bool ingresarLetra(char c);
    bool repetida(char c);
    bool gano();
    void mostrar();
    void deshacer();
};

Juego::Juego() {
    vidas = 15;

    string palabras[10] = {
        "MUSCULO",
        "PROTEINA",
        "HIPERTROFIA",
        "ENTRENAMIENTO",
        "PESAS",
        "CARDIO",
        "CREATINA",
        "RUTINA",
        "FUERZA",
        "SENTADILLA"
    };

    srand(time(NULL));
    palabra = palabras[rand() % 10];

    progreso = "";
    for (int i = 0; i < palabra.length(); i++) {
        progreso += "_";
    }

    usadas = "";
    idx = 0;
    movimientos = 0;
}

bool Juego::repetida(char c) {
    for (int i = 0; i < usadas.length(); i++) {
        if (usadas[i] == c) return true;
    }
    return false;
}

bool Juego::ingresarLetra(char c) {

    // Guardar estado (para undo)
    historial[idx].progreso = progreso;
    historial[idx].vidas = vidas;
    historial[idx].usadas = usadas;
    idx = (idx + 1) % 10;
    movimientos++;

    usadas += c;

    bool acierto = false;

    for (int i = 0; i < palabra.length(); i++) {
        if (palabra[i] == c) {
            progreso[i] = c;
            acierto = true;
        }
    }

    if (!acierto) {
        vidas--;
        cout << "Fallaste (-1 vida)\n";
    }

    return acierto;
}

bool Juego::gano() {
    for (int i = 0; i < progreso.length(); i++) {
        if (progreso[i] == '_') return false;
    }
    return true;
}

void Juego::mostrar() {
    cout << "Palabra: " << progreso << endl;
}

void Juego::deshacer() {
    if (movimientos == 0) {
        cout << "Nada que deshacer\n";
        return;
    }

    int ant = (idx + 9) % 10;

    progreso = historial[ant].progreso;
    vidas = historial[ant].vidas;
    usadas = historial[ant].usadas;

    idx = ant;
    movimientos--;
}

int main() {

    Juego j;
    char c;
    int limiteUndo = 10;

    cout << "=== AHORCADO (GYM EDITION) ===\n";
    cout << "      __      \n";
    cout << "   .-\"  \"-.   \n";
    cout << "  /        \\  \n";
    cout << " |  O    O | \n";
    cout << " |   ____  | \n";
    cout << "  \\        /  \n";
    cout << "   '-.__.-'   \n";
    cout << "      ||      \n";
    cout << "   ___||___   \n";
    cout << "  /   ||   \\ \n";
    cout << " /    ||    \\\n";
    cout << "|     ||     |\n";
    cout << " \\__________/ \n";
    j.mostrar();

    while (j.vidas > 0 && !j.gano()) {

        cout << "\nVidas: " << j.vidas << endl;
        cout << "Usadas: " << j.usadas << endl;
        cout << "Letra (* = undo): ";
        cin >> c;

        if (c == '*') {
            if (limiteUndo > 0) {
                j.deshacer();
                limiteUndo--;
            } else {
                cout << "Sin deshacer disponibles\n";
            }
        }
        else if (c < 'A' || c > 'Z') {
            cout << "Solo mayusculas A-Z\n";
        }
        else if (j.repetida(c)) {
            cout << "Letra repetida\n";
        }
        else {
            j.ingresarLetra(c);
        }

        j.mostrar();
    }

    cout << "\n";
    if (j.gano()) {
        cout << "Ganaste! -> " << j.palabra << endl;
    } else {
        cout << "Perdiste! -> " << j.palabra << endl;
    }
}
