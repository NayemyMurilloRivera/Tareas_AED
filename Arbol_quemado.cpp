#include <SFML/Graphics.hpp>
#include <queue>
#include <map>
#include <vector>
#include <set>
#include <string>

// ── ESTRUCTURA DEL ARBOL ─────────────────────────────────────────────────────
// Nodo con posicion en pantalla
struct Nodo {
    int valor;
    float x, y;
};

// Adyacencia (grafo no dirigido, igual que el arbol del profe)
std::map<int, std::vector<int>> adj;

// Posiciones de cada nodo en pantalla
std::map<int, sf::Vector2f> posiciones;

// ── ESTADO DEL FUEGO ─────────────────────────────────────────────────────────
std::map<int, int> vuelta_quemado;  // nodo -> en que vuelta se quemo (-1 = no quemado)
int vuelta_actual = 0;
bool simulacion_terminada = false;

// Cola BFS
std::queue<int> cola;
std::set<int> visitados;

// ── COLORES ──────────────────────────────────────────────────────────────────
sf::Color COLOR_NORMAL    = sf::Color(180, 210, 245);   // azul claro
sf::Color COLOR_QUEMANDO  = sf::Color(239, 159,  39);   // naranja (vuelta actual)
sf::Color COLOR_QUEMADO   = sf::Color(200,  80,  80);   // rojo oscuro
sf::Color COLOR_BORDE_N   = sf::Color( 24,  95, 165);
sf::Color COLOR_BORDE_Q   = sf::Color(147,  60,   6);
sf::Color COLOR_FONDO     = sf::Color( 20,  20,  30);
sf::Color COLOR_ARISTA    = sf::Color(100, 100, 120);

void construirArbol() {
    // Arbol 1 del pizarron:
    //         12
    //        /  \
    //      13    10
    //           /  \
    //          14   15
    //         / \ / \
    //        21 24 22 23

    adj[12] = {13, 10};
    adj[13] = {12};
    adj[10] = {12, 14, 15};
    adj[14] = {10, 21, 24};
    adj[15] = {10, 22, 23};
    adj[21] = {14};
    adj[24] = {14};
    adj[22] = {15};
    adj[23] = {15};

    // Posiciones en pantalla (x, y)
    posiciones[12] = {400, 80};
    posiciones[13] = {220, 200};
    posiciones[10] = {560, 200};
    posiciones[14] = {460, 330};
    posiciones[15] = {660, 330};
    posiciones[21] = {360, 460};
    posiciones[24] = {500, 460};
    posiciones[22] = {600, 460};
    posiciones[23] = {740, 460};

    // Inicializar todos como no quemados
    for (auto& [nodo, _] : adj)
        vuelta_quemado[nodo] = -1;
}

// Avanza UNA vuelta del BFS
void avanzarVuelta() {
    if (cola.empty() || simulacion_terminada) {
        simulacion_terminada = true;
        return;
    }

    vuelta_actual++;

    // Todos los nodos en la cola AHORA son la "frontera" de esta vuelta
    int tam_frontera = cola.size();
    std::queue<int> siguiente;

    for (int i = 0; i < tam_frontera; i++) {
        int nodo = cola.front();
        cola.pop();

        // Marcar vecinos no visitados -> siguiente vuelta
        for (int vecino : adj[nodo]) {
            if (visitados.find(vecino) == visitados.end()) {
                visitados.insert(vecino);
                vuelta_quemado[vecino] = vuelta_actual;
                siguiente.push(vecino);
            }
        }
    }

    // Si no hay mas nodos que quemar, terminamos
    if (siguiente.empty())
        simulacion_terminada = true;
    else
        cola = siguiente;
}

void iniciarFuego(int objetivo) {
    // Resetear todo
    while (!cola.empty()) cola.pop();
    visitados.clear();
    vuelta_actual = 0;
    simulacion_terminada = false;
    for (auto& [n, v] : vuelta_quemado) v = -1;

    // Empezar BFS desde el objetivo
    visitados.insert(objetivo);
    vuelta_quemado[objetivo] = 0;   // vuelta 0 = el objetivo mismo
    cola.push(objetivo);
}

void dibujarArbol(sf::RenderWindow& ventana, sf::Font& fuente) {
    // ── Dibujar aristas ───────────────────────────────────────────────────────
    std::set<std::pair<int,int>> aristasYaDibujadas;
    for (auto& [nodo, vecinos] : adj) {
        for (int vecino : vecinos) {
            auto arista = std::make_pair(std::min(nodo, vecino), std::max(nodo, vecino));
            if (aristasYaDibujadas.count(arista)) continue;
            aristasYaDibujadas.insert(arista);

            sf::Vertex linea[2];
            linea[0].position = posiciones[nodo];
            linea[1].position = posiciones[vecino];
            linea[0].color = COLOR_ARISTA;
            linea[1].color = COLOR_ARISTA;
            ventana.draw(linea, 2, sf::Lines);
        }
    }

    // ── Dibujar nodos ─────────────────────────────────────────────────────────
    for (auto& [nodo, pos] : posiciones) {
        int v = vuelta_quemado[nodo];

        sf::Color relleno, borde;
        if (v == -1) {
            relleno = COLOR_NORMAL;
            borde   = COLOR_BORDE_N;
        } else if (!cola.empty()) {
            // Si todavia hay cola, los de la vuelta_actual son "en llamas"
            // Los anteriores ya estan quemados
            // (usamos si esta en la cola actual o no)
            relleno = (v == vuelta_actual) ? COLOR_QUEMANDO : COLOR_QUEMADO;
            borde   = COLOR_BORDE_Q;
        } else {
            relleno = (v == 0) ? COLOR_QUEMANDO : COLOR_QUEMADO;
            borde   = COLOR_BORDE_Q;
        }

        // Circulo
        sf::CircleShape circulo(30);
        circulo.setOrigin(30, 30);
        circulo.setPosition(pos);
        circulo.setFillColor(relleno);
        circulo.setOutlineColor(borde);
        circulo.setOutlineThickness(2.5f);
        ventana.draw(circulo);

        // Numero del nodo
        sf::Text texto;
        texto.setFont(fuente);
        texto.setString(std::to_string(nodo));
        texto.setCharacterSize(18);
        texto.setFillColor(sf::Color(30, 20, 10));
        texto.setStyle(sf::Text::Bold);
        sf::FloatRect bounds = texto.getLocalBounds();
        texto.setOrigin(bounds.left + bounds.width/2, bounds.top + bounds.height/2);
        texto.setPosition(pos);
        ventana.draw(texto);

        // Etiqueta de vuelta debajo del nodo
        if (v >= 0) {
            sf::Text etiq;
            etiq.setFont(fuente);
            etiq.setString("v" + std::to_string(v == 0 ? 1 : v + 1));
            etiq.setCharacterSize(13);
            etiq.setFillColor(sf::Color(200, 200, 180));
            sf::FloatRect eb = etiq.getLocalBounds();
            etiq.setOrigin(eb.left + eb.width/2, eb.top + eb.height/2);
            etiq.setPosition(pos.x, pos.y + 44);
            ventana.draw(etiq);
        }
    }
}

void dibujarUI(sf::RenderWindow& ventana, sf::Font& fuente, int objetivo) {
    // Instrucciones
    sf::Text instruc;
    instruc.setFont(fuente);
    instruc.setCharacterSize(15);
    instruc.setFillColor(sf::Color(180, 180, 160));
    instruc.setPosition(20, 20);
    instruc.setString(
        "ARBOL QUEMADO (BFS)\n"
        "Objetivo: " + std::to_string(objetivo) + "\n\n"
        "[ESPACIO] -> siguiente vuelta\n"
        "[R]       -> reiniciar\n"
        "[1-9]     -> cambiar objetivo\n"
    );
    ventana.draw(instruc);

    // Estado actual
    sf::Text estado;
    estado.setFont(fuente);
    estado.setCharacterSize(16);
    estado.setFillColor(simulacion_terminada ? sf::Color(100, 200, 100) : sf::Color(239, 159, 39));
    estado.setPosition(20, 560);
    if (simulacion_terminada)
        estado.setString("Arbol completamente quemado!");
    else
        estado.setString("Vuelta actual: " + std::to_string(vuelta_actual == 0 ? 1 : vuelta_actual + 1));
    ventana.draw(estado);

    // Leyenda
    auto dibLeyenda = [&](float x, float y, sf::Color c, const std::string& label) {
        sf::CircleShape dot(8);
        dot.setPosition(x, y);
        dot.setFillColor(c);
        ventana.draw(dot);
        sf::Text t;
        t.setFont(fuente);
        t.setCharacterSize(13);
        t.setFillColor(sf::Color(200, 200, 180));
        t.setPosition(x + 22, y - 2);
        t.setString(label);
        ventana.draw(t);
    };
    dibLeyenda(20, 610, COLOR_NORMAL,   "sin quemar");
    dibLeyenda(20, 632, COLOR_QUEMANDO, "en llamas");
    dibLeyenda(20, 654, COLOR_QUEMADO,  "ya quemado");
}

int main() {
    sf::RenderWindow ventana(sf::VideoMode(820, 700), "Arbol Quemado - BFS");
    ventana.setFramerateLimit(60);
    
    sf::Font fuente;
    bool fuente_cargada = false;
    std::vector<std::string> rutas_fuentes = {
        // Windows
        "C:/Windows/Fonts/arial.ttf",
        ...
    };
    for (const auto& ruta : rutas_fuentes) {
        if (fuente.loadFromFile(ruta)) {
            fuente_cargada = true;
            break;
        }
    }
    // Si no encontro ninguna el texto no se ve pero el arbol igual funciona

    construirArbol();

    int objetivo = 14;   // <-- cambia esto o usa las teclas
    iniciarFuego(objetivo);

    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed)
                ventana.close();

            if (evento.type == sf::Event::KeyPressed) {
                // Espacio -> avanzar vuelta
                if (evento.key.code == sf::Keyboard::Space)
                    avanzarVuelta();

                // R -> reiniciar
                if (evento.key.code == sf::Keyboard::R)
                    iniciarFuego(objetivo);

                // Numeros 1-9 para cambiar objetivo
                // Nodos disponibles: 10,12,13,14,15,21,22,23,24
                int nodos[] = {10,12,13,14,15,21,22,23,24};
                for (int i = 0; i < 9; i++) {
                    if (evento.key.code == sf::Keyboard::Num1 + i) {
                        objetivo = nodos[i];
                        iniciarFuego(objetivo);
                    }
                }
            }
        }

        ventana.clear(COLOR_FONDO);
        dibujarArbol(ventana, fuente);
        dibujarUI(ventana, fuente, objetivo);
        ventana.display();
    }

    return 0;
}
