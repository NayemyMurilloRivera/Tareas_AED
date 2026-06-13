#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

// ═══════════════════════════════════════════════════════
//  NODO
// ═══════════════════════════════════════════════════════
struct Nodo {
    int val;
    Nodo* izq = nullptr;
    Nodo* der = nullptr;
    Nodo(int v) : val(v) {}
};

// ═══════════════════════════════════════════════════════
//  AVL
// ═══════════════════════════════════════════════════════
class AVL {
public:
    Nodo* raiz = nullptr;

    // ── Inserción ────────────────────────────────────
    Nodo* _ins(Nodo* n, int v) {
        if (!n) return new Nodo(v);
        if (v < n->val) n->izq = _ins(n->izq, v);
        else             n->der = _ins(n->der, v);
        return n;
    }
    void insertar(int v) { raiz = _ins(raiz, v); }

    // ── Altura y FB ──────────────────────────────────
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

    // ── PODAR: elimina UNA sola hoja por llamada ─────
    void podarUnaHoja(Nodo*& n, bool& eliminado) {
        if (!n || eliminado) return;

        // Recurrir primero hacia los hijos
        podarUnaHoja(n->izq, eliminado);
        if (!eliminado) podarUnaHoja(n->der, eliminado);

        if (eliminado) return;

        // Revisar si algún hijo es hoja y causa desbalanceo
        if (n->izq && !n->izq->izq && !n->izq->der && fb(n) != 0) {
            delete n->izq;
            n->izq = nullptr;
            eliminado = true;
            return;
        }
        if (n->der && !n->der->izq && !n->der->der && fb(n) != 0) {
            delete n->der;
            n->der = nullptr;
            eliminado = true;
        }
    }

    // Llama esto una vez por tecla — devuelve false si ya terminó
    bool podarPaso() {
        if (todosCero(raiz)) return false;
        bool eliminado = false;
        podarUnaHoja(raiz, eliminado);
        return !todosCero(raiz);
    }

    // ── Recorridos ───────────────────────────────────
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
//  POSICIONES: tabla inorden × BFS nivel → (x, y) pixels
// ═══════════════════════════════════════════════════════
struct InfoNodo { sf::Vector2f pos; int fbVal; };

map<int, InfoNodo> calcularPosiciones(AVL& arbol,
                                       float W, float H,
                                       float margen = 70.f)
{
    map<int, InfoNodo> info;
    if (!arbol.raiz) return info;

    // Inorden → columna X
    vector<int> io;
    arbol.inorden(arbol.raiz, io);
    map<int, int> colX;
    for (int i = 0; i < (int)io.size(); i++) colX[io[i]] = i;

    // BFS → nivel Y
    map<int, int> nivel;
    int maxNivel = 0;
    queue<pair<Nodo*, int>> q;
    q.push({arbol.raiz, 0});
    while (!q.empty()) {
        auto [n, lv] = q.front(); q.pop();
        nivel[n->val] = lv;
        maxNivel = max(maxNivel, lv);
        if (n->izq) q.push({n->izq, lv + 1});
        if (n->der) q.push({n->der, lv + 1});
    }

    int N = (int)io.size();
    float stepX = (W - 2 * margen) / max(N - 1, 1);
    float stepY = (H - 2 * margen) / max(maxNivel, 1);

    // Buscar nodo por valor (auxiliar)
    auto buscar = [&](int v) -> Nodo* {
        queue<Nodo*> bq;
        bq.push(arbol.raiz);
        while (!bq.empty()) {
            Nodo* c = bq.front(); bq.pop();
            if (c->val == v) return c;
            if (c->izq) bq.push(c->izq);
            if (c->der) bq.push(c->der);
        }
        return nullptr;
    };

    for (auto& [v, col] : colX) {
        float px = margen + col * stepX;
        float py = margen + nivel[v] * stepY;
        Nodo* ptr = buscar(v);
        info[v] = { {px, py}, arbol.fb(ptr) };
    }
    return info;
}

// ═══════════════════════════════════════════════════════
//  DIBUJO
// ═══════════════════════════════════════════════════════
void dibujarAristas(sf::RenderWindow& w, Nodo* n,
                    map<int, InfoNodo>& info)
{
    if (!n) return;
    sf::Color c(100, 120, 150);
    auto dibLinea = [&](int a, int b) {
        sf::Vertex l[2] = {
            sf::Vertex(info[a].pos, c),
            sf::Vertex(info[b].pos, c)
        };
        w.draw(l, 2, sf::Lines);
    };
    if (n->izq) { dibLinea(n->val, n->izq->val); dibujarAristas(w, n->izq, info); }
    if (n->der) { dibLinea(n->val, n->der->val); dibujarAristas(w, n->der, info); }
}

void dibujarNodos(sf::RenderWindow& w, sf::Font& fuente,
                  map<int, InfoNodo>& info)
{
    sf::Color C_NORMAL (180, 210, 245);
    sf::Color C_DESBAL (239, 159,  39);
    sf::Color C_BN     ( 24,  95, 165);
    sf::Color C_BD     (147,  60,   6);
    const float R = 26.f;

    for (auto& [val, inf] : info) {
        bool bal = (inf.fbVal == 0);
        sf::CircleShape circ(R);
        circ.setOrigin(R, R);
        circ.setPosition(inf.pos);
        circ.setFillColor   (bal ? C_NORMAL : C_DESBAL);
        circ.setOutlineColor(bal ? C_BN     : C_BD);
        circ.setOutlineThickness(2.5f);
        w.draw(circ);

        // Valor
        sf::Text tv;
        tv.setFont(fuente); tv.setString(to_string(val));
        tv.setCharacterSize(18); tv.setStyle(sf::Text::Bold);
        tv.setFillColor(sf::Color(20, 20, 30));
        auto b = tv.getLocalBounds();
        tv.setOrigin(b.left + b.width/2, b.top + b.height/2);
        tv.setPosition(inf.pos);
        w.draw(tv);

        // FB arriba del nodo
        string fbStr = (inf.fbVal > 0 ? "+" : "") + to_string(inf.fbVal);
        sf::Text tfb;
        tfb.setFont(fuente); tfb.setString(fbStr);
        tfb.setCharacterSize(13);
        tfb.setFillColor(bal ? sf::Color(80,180,80) : sf::Color(230,100,30));
        auto bf = tfb.getLocalBounds();
        tfb.setOrigin(bf.left + bf.width/2, bf.top + bf.height/2);
        tfb.setPosition(inf.pos.x, inf.pos.y - R - 12);
        w.draw(tfb);
    }
}

void dibujarUI(sf::RenderWindow& w, sf::Font& f,
               int idx, int total, const vector<int>& arr,
               bool podando, bool terminado, int hojasElim)
{
    // Encabezado
    string arrStr = "[";
    for (int i = 0; i < (int)arr.size(); i++)
        arrStr += to_string(arr[i]) + (i+1 < (int)arr.size() ? "," : "]");

    sf::Text th;
    th.setFont(f); th.setCharacterSize(14);
    th.setFillColor(sf::Color(180,180,160));
    th.setPosition(14, 14);
    th.setString("ARBOL AVL  " + to_string(idx+1) + "/" + to_string(total)
        + "   " + arrStr + "\n\n"
        "[P]        iniciar PODAR paso a paso\n"
        "[ESPACIO]  eliminar hoja  /  sig. arbol\n"
        "[R]        resetear");
    w.draw(th);

    // Estado
    sf::Text est;
    est.setFont(f); est.setCharacterSize(15);
    est.setPosition(14, 570);
    if (terminado) {
        est.setFillColor(sf::Color(100, 200, 100));
        est.setString("Balanceado! Se eliminaron " + to_string(hojasElim)
                      + " hojas.   [ESPACIO] = siguiente arbol");
    } else if (podando) {
        est.setFillColor(sf::Color(239, 159, 39));
        est.setString("[ESPACIO] = eliminar siguiente hoja   ("
                      + to_string(hojasElim) + " eliminadas)");
    } else {
        est.setFillColor(sf::Color(180, 180, 160));
        est.setString("[P] = iniciar PODAR paso a paso");
    }
    w.draw(est);

    // Leyenda
    auto dot = [&](float x, float y, sf::Color c, const string& lbl) {
        sf::CircleShape d(7); d.setPosition(x, y); d.setFillColor(c); w.draw(d);
        sf::Text lt; lt.setFont(f); lt.setCharacterSize(13);
        lt.setFillColor(sf::Color(200,200,180));
        lt.setPosition(x+20, y-1); lt.setString(lbl); w.draw(lt);
    };
    dot(14, 618, sf::Color(180,210,245), "fb = 0  (balanceado)");
    dot(14, 642, sf::Color(239,159, 39), "fb != 0 (desbalanceado)");
}

// ═══════════════════════════════════════════════════════
//  MAIN
// ═══════════════════════════════════════════════════════
int main() {
    vector<vector<int>> arrays = {
        {12, 8, 18, 5, 11, 17},
        {10, 5, 15, 3, 7, 12, 20},
        {20, 10, 30, 5, 15, 25, 35, 1}
    };

    sf::RenderWindow ventana(sf::VideoMode(820, 700), "AVL - PODAR");
    ventana.setFramerateLimit(60);

    sf::Font fuente;
    for (auto& r : vector<string>{
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/System/Library/Fonts/Helvetica.ttc"
    }) if (fuente.loadFromFile(r)) break;

    auto buildArbol = [&](int i) {
        AVL* a = new AVL();
        for (int x : arrays[i]) a->insertar(x);
        return a;
    };

    int  idx       = 0;
    bool podando   = false;
    bool terminado = false;
    int  hojasElim = 0;
    AVL* arbol     = buildArbol(idx);

    while (ventana.isOpen()) {
        sf::Event ev;
        while (ventana.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed)
                ventana.close();

            if (ev.type == sf::Event::KeyPressed) {

                // [P] → activar poda paso a paso
                if (ev.key.code == sf::Keyboard::P && !terminado)
                    podando = true;

                // [ESPACIO] → un paso de poda OR siguiente árbol
                if (ev.key.code == sf::Keyboard::Space) {
                    if (podando && !terminado) {
                        bool sigue = arbol->podarPaso();
                        hojasElim++;
                        if (!sigue) { terminado = true; podando = false; }
                    } else if (!podando) {
                        delete arbol;
                        idx = (idx + 1) % (int)arrays.size();
                        arbol     = buildArbol(idx);
                        podando   = false;
                        terminado = false;
                        hojasElim = 0;
                    }
                }

                // [R] → resetear árbol actual
                if (ev.key.code == sf::Keyboard::R) {
                    delete arbol;
                    arbol     = buildArbol(idx);
                    podando   = false;
                    terminado = false;
                    hojasElim = 0;
                }
            }
        }

        auto info = calcularPosiciones(*arbol, 820, 520, 70);

        ventana.clear(sf::Color(20, 20, 30));
        dibujarAristas(ventana, arbol->raiz, info);
        dibujarNodos(ventana, fuente, info);
        dibujarUI(ventana, fuente, idx, (int)arrays.size(),
                  arrays[idx], podando, terminado, hojasElim);
        ventana.display();
    }

    delete arbol;
    return 0;
}
