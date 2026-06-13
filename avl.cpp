#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
using namespace std;

// ═══════════════════════════════════════════════════════
//  NODO DEL ÁRBOL
// ═══════════════════════════════════════════════════════
struct Nodo {
    int val;
    Nodo* izq = nullptr;
    Nodo* der = nullptr;
    Nodo(int v) : val(v) {}
};

// ═══════════════════════════════════════════════════════
//  AVL  (inserción BST + PODAR)
// ═══════════════════════════════════════════════════════
class AVL {
public:
    Nodo* raiz = nullptr;

    Nodo* _ins(Nodo* n, int v) {
        if (!n) return new Nodo(v);
        if (v < n->val) n->izq = _ins(n->izq, v);
        else             n->der = _ins(n->der, v);
        return n;
    }
    void insertar(int v) { raiz = _ins(raiz, v); }

    int altura(Nodo* n) {
        if (!n) return -1;
        return 1 + max(altura(n->izq), altura(n->der));
    }
    int fb(Nodo* n) {
        if (!n) return 0;
        return altura(n->izq) - altura(n->der);
    }
    bool todosCero(Nodo* n) {
        if (!n) return true;
        return fb(n) == 0 && todosCero(n->izq) && todosCero(n->der);
    }
    Nodo* podarPaso(Nodo* n) {
        if (!n) return nullptr;
        if (!n->izq && !n->der) { delete n; return nullptr; }
        n->izq = podarPaso(n->izq);
        n->der = podarPaso(n->der);
        return n;
    }
    void podar() {
        while (!todosCero(raiz))
            raiz = podarPaso(raiz);
    }

    // Recorridos
    void inorden(Nodo* n, vector<int>& r) {
        if (!n) return;
        inorden(n->izq, r); r.push_back(n->val); inorden(n->der, r);
    }
    void preorden(Nodo* n, vector<int>& r) {
        if (!n) return;
        r.push_back(n->val); preorden(n->izq, r); preorden(n->der, r);
    }
};

// ═══════════════════════════════════════════════════════
//  TABLA INORDEN × PREORDEN → posiciones en pantalla
//
//  La idea del profe:
//    columna = posición en inorden  → coordenada X
//    fila    = posición en preorden → coordenada Y (nivel BFS)
//
//  Pero fila-preorden no es lo mismo que nivel real del árbol,
//  así que usamos BFS para obtener el nivel real de cada nodo
//  y así la Y queda bien distribuida verticalmente.
//  La X sí viene directo del inorden (orden izquierda→derecha).
// ═══════════════════════════════════════════════════════
struct InfoNodo {
    sf::Vector2f pos;
    int fb;
    bool esHoja;
};

map<int, InfoNodo> calcularPosiciones(AVL& arbol,
                                       float anchoVentana,
                                       float altoVentana,
                                       float margen = 60.f)
{
    map<int, InfoNodo> info;
    if (!arbol.raiz) return info;

    // 1) Inorden → posición X (columna en la tabla)
    vector<int> io;
    arbol.inorden(arbol.raiz, io);
    map<int, int> colX;  // val → índice 0..N-1
    for (int i = 0; i < (int)io.size(); i++) colX[io[i]] = i;

    // 2) BFS → nivel real de cada nodo (posición Y)
    map<int, int> nivel;
    int maxNivel = 0;
    queue<pair<Nodo*, int>> q;
    q.push({arbol.raiz, 0});
    while (!q.empty()) {
        auto [n, lv] = q.front(); q.pop();
        nivel[n->val] = lv;
        maxNivel = max(maxNivel, lv);
        if (n->izq) q.push({n->izq, lv+1});
        if (n->der) q.push({n->der, lv+1});
    }

    // 3) Mapear índice → coordenada en píxeles
    int N = (int)io.size();
    float stepX = (anchoVentana - 2*margen) / max(N - 1, 1);
    float stepY = (altoVentana  - 2*margen) / max(maxNivel, 1);

    // Recorremos todos los nodos con preorden para mantener el orden del profe
    vector<int> po;
    arbol.preorden(arbol.raiz, po);

    for (int v : po) {
        float px = margen + colX[v] * stepX;
        float py = margen + nivel[v] * stepY;
        bool esHoja = true;
        // detectar hoja recorriendo el árbol
        queue<Nodo*> bq;
        bq.push(arbol.raiz);
        while (!bq.empty()) {
            Nodo* cur = bq.front(); bq.pop();
            if (cur->val == v) {
                esHoja = (!cur->izq && !cur->der);
                break;
            }
            if (cur->izq) bq.push(cur->izq);
            if (cur->der) bq.push(cur->der);
        }
        Nodo* nPtr = nullptr;
        queue<Nodo*> bq2; bq2.push(arbol.raiz);
        while (!bq2.empty()) {
            Nodo* c = bq2.front(); bq2.pop();
            if (c->val == v) { nPtr = c; break; }
            if (c->izq) bq2.push(c->izq);
            if (c->der) bq2.push(c->der);
        }
        info[v] = { {px, py}, arbol.fb(nPtr), esHoja };
    }
    return info;
}

// ═══════════════════════════════════════════════════════
//  DIBUJAR ARISTAS (recursivo, antes que los nodos)
// ═══════════════════════════════════════════════════════
void dibujarAristas(sf::RenderWindow& w, Nodo* n,
                    map<int, InfoNodo>& info)
{
    if (!n) return;
    sf::Color colorArista(100, 120, 150);
    if (n->izq) {
        sf::Vertex linea[2] = {
            sf::Vertex(info[n->val].pos,     colorArista),
            sf::Vertex(info[n->izq->val].pos, colorArista)
        };
        w.draw(linea, 2, sf::Lines);
        dibujarAristas(w, n->izq, info);
    }
    if (n->der) {
        sf::Vertex linea[2] = {
            sf::Vertex(info[n->val].pos,     colorArista),
            sf::Vertex(info[n->der->val].pos, colorArista)
        };
        w.draw(linea, 2, sf::Lines);
        dibujarAristas(w, n->der, info);
    }
}

// ═══════════════════════════════════════════════════════
//  DIBUJAR NODOS
// ═══════════════════════════════════════════════════════
void dibujarNodos(sf::RenderWindow& w, sf::Font& fuente,
                  map<int, InfoNodo>& info, bool podado)
{
    // Colores igual a tu código original
    sf::Color C_NORMAL  (180, 210, 245);   // azul claro  = balanceado (fb=0)
    sf::Color C_DESBAL  (239, 159,  39);   // naranja     = fb != 0
    sf::Color C_PODADO  (200,  80,  80);   // rojo        = árbol ya podado
    sf::Color C_BORDE_N ( 24,  95, 165);
    sf::Color C_BORDE_D (147,  60,   6);
    float R = 26.f;

    for (auto& [val, inf] : info) {
        sf::Color relleno, borde;
        if (podado) {
            relleno = C_PODADO;  borde = C_BORDE_D;
        } else if (inf.fb == 0) {
            relleno = C_NORMAL;  borde = C_BORDE_N;
        } else {
            relleno = C_DESBAL;  borde = C_BORDE_D;
        }

        // Círculo del nodo
        sf::CircleShape circ(R);
        circ.setOrigin(R, R);
        circ.setPosition(inf.pos);
        circ.setFillColor(relleno);
        circ.setOutlineColor(borde);
        circ.setOutlineThickness(2.5f);
        w.draw(circ);

        // Valor del nodo
        sf::Text tv;
        tv.setFont(fuente);
        tv.setString(to_string(val));
        tv.setCharacterSize(18);
        tv.setFillColor(sf::Color(20, 20, 30));
        tv.setStyle(sf::Text::Bold);
        auto b = tv.getLocalBounds();
        tv.setOrigin(b.left + b.width/2, b.top + b.height/2);
        tv.setPosition(inf.pos);
        w.draw(tv);

        // Factor de balanceo arriba del nodo
        string fbStr = (inf.fb > 0 ? "+" : "") + to_string(inf.fb);
        sf::Text tfb;
        tfb.setFont(fuente);
        tfb.setString(fbStr);
        tfb.setCharacterSize(13);
        tfb.setFillColor(inf.fb == 0
            ? sf::Color(80, 180, 80)    // verde si 0
            : sf::Color(230, 100, 30)); // naranja si != 0
        auto bf = tfb.getLocalBounds();
        tfb.setOrigin(bf.left + bf.width/2, bf.top + bf.height/2);
        tfb.setPosition(inf.pos.x, inf.pos.y - R - 12);
        w.draw(tfb);
    }
}

// ═══════════════════════════════════════════════════════
//  UI — instrucciones y estado
// ═══════════════════════════════════════════════════════
void dibujarUI(sf::RenderWindow& w, sf::Font& f,
               int idxArbol, int totalArboles,
               bool podado, bool balanceado,
               const vector<int>& arr)
{
    // Título e instrucciones
    string arrStr = "[";
    for (int i = 0; i < (int)arr.size(); i++)
        arrStr += to_string(arr[i]) + (i+1<(int)arr.size() ? "," : "]");

    sf::Text t;
    t.setFont(f);
    t.setCharacterSize(14);
    t.setFillColor(sf::Color(180, 180, 160));
    t.setPosition(14, 14);
    t.setString(
        "ARBOL AVL  " + to_string(idxArbol+1) + "/" + to_string(totalArboles)
        + "   " + arrStr + "\n\n"
        "[ESPACIO]  siguiente arbol\n"
        "[P]        PODAR()\n"
        "[R]        resetear arbol actual"
    );
    w.draw(t);

    // Estado abajo
    sf::Text est;
    est.setFont(f);
    est.setCharacterSize(15);
    est.setPosition(14, 570);
    if (podado && balanceado) {
        est.setFillColor(sf::Color(100, 200, 100));
        est.setString("Arbol podado — todos los FB = 0  ✓");
    } else if (podado) {
        est.setFillColor(sf::Color(200, 80, 80));
        est.setString("Podado pero aun no balanceado");
    } else {
        est.setFillColor(sf::Color(239, 159, 39));
        est.setString("Presiona [P] para PODAR()");
    }
    w.draw(est);

    // Leyenda
    auto leyenda = [&](float x, float y, sf::Color c, const string& lbl){
        sf::CircleShape dot(7); dot.setPosition(x,y); dot.setFillColor(c); w.draw(dot);
        sf::Text lt; lt.setFont(f); lt.setCharacterSize(13);
        lt.setFillColor(sf::Color(200,200,180));
        lt.setPosition(x+20, y-1); lt.setString(lbl); w.draw(lt);
    };
    leyenda(14, 610, sf::Color(180,210,245), "fb = 0");
    leyenda(14, 632, sf::Color(239,159, 39), "fb != 0");
    leyenda(14, 654, sf::Color(200, 80, 80), "podado");
}

// ═══════════════════════════════════════════════════════
//  MAIN
// ═══════════════════════════════════════════════════════
int main() {
    // ── Arrays de entrada (agrega los que te dé el profe) ──
    vector<vector<int>> arrays = {
        {12, 8, 18, 5, 11, 17},          // árbol 1 — pizarra
        {10, 5, 15, 3, 7, 12, 20},       // árbol 2
        {20, 10, 30, 5, 15, 25, 35, 1}   // árbol 3
    };

    sf::RenderWindow ventana(sf::VideoMode(820, 700), "AVL - PODAR");
    ventana.setFramerateLimit(60);

    sf::Font fuente;
    vector<string> rutas = {
        "C:/Windows/Fonts/arial.ttf",           // Windows
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", // Linux
        "/System/Library/Fonts/Helvetica.ttc"   // Mac
    };
    for (auto& r : rutas) if (fuente.loadFromFile(r)) break;

    int idx = 0;
    bool podado = false;

    // Construir árbol actual
    auto buildArbol = [&](int i) {
        AVL* a = new AVL();
        for (int x : arrays[i]) a->insertar(x);
        return a;
    };

    AVL* arbol = buildArbol(idx);

    sf::Color COLOR_FONDO(20, 20, 30);

    while (ventana.isOpen()) {
        sf::Event ev;
        while (ventana.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed)
                ventana.close();

            if (ev.type == sf::Event::KeyPressed) {
                // ESPACIO → siguiente árbol
                if (ev.key.code == sf::Keyboard::Space) {
                    delete arbol;
                    idx = (idx + 1) % (int)arrays.size();
                    arbol = buildArbol(idx);
                    podado = false;
                }
                // R → resetear árbol actual
                if (ev.key.code == sf::Keyboard::R) {
                    delete arbol;
                    arbol = buildArbol(idx);
                    podado = false;
                }
                // P → PODAR
                if (ev.key.code == sf::Keyboard::P) {
                    arbol->podar();
                    podado = true;
                }
            }
        }

        // Calcular posiciones desde tabla inorden×preorden
        auto info = calcularPosiciones(*arbol, 820, 520, 70);
        bool bal  = arbol->todosCero(arbol->raiz);

        ventana.clear(COLOR_FONDO);
        dibujarAristas(ventana, arbol->raiz, info);
        dibujarNodos(ventana, fuente, info, podado);
        dibujarUI(ventana, fuente, idx, (int)arrays.size(),
                  podado, bal, arrays[idx]);
        ventana.display();
    }

    delete arbol;
    return 0;
}
