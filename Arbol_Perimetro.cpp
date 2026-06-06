/*

TAREA

SFML
Install SFML

La idea es dibujar los algoritmos usando SFML
De hora en adelante la tarea será dibujada.

La tarea de hoy, es el ejercicio del arbol del parcial.

Dibuja el arbol y pinta los nodos del contorno de ese codigo.
Ya no imprimir, sino pintar.

Segun la cantidad de numeros que le profe pide.
Actualmente es un arbol binario.

Haz un arbol binario y pintas el contorno.

ESTO REVISARA LA PROXIMA SEMANA, NO MAS.

Los numeros:
VISUALGO, LA PAGINA RECOMENDADA POR GUSTAMBO PARA LOS NUEVOS ALGORITMOS.

RECOMIENDA QUE HAGA QUE LAS CLASES TENGAS LOS DIBUJSO YA HECHAS, PARA QUE NO DEPENDA DEL AERBOl SI NO QUE CADA CLASE DE DIBUJO VIVA PRO SU CUENTA. Y YA, LLAMAS A LA CLASE Y GENIAL!




*/
#include <SFML/Graphics.hpp>
#include <iostream>
#include <initializer_list>
#include <stack>
#include <vector>
#include <unordered_set>
#include <sstream>

//////////////////////////////////////////////////////////////////
// ===== Version Alternativa =====
//////////////////////////////////////////////////////////////////

struct TNode
{
    int data;

    TNode* child[2];

    float px, py;

    bool border;

    TNode(int v)
    {
        data = v;

        child[0] = child[1] = nullptr;

        px = py = 0;

        border = false;
    }

    // ===== Agregado : dibujar nodo =====

    void render(sf::RenderWindow& win, sf::Font& font)
    {
        // ===== Agregado : nodo =====

        sf::CircleShape node(23);

        node.setPosition(px, py);

        // ===== Agregado : colores diferentes =====

        if (border)
            node.setFillColor(sf::Color(220, 70, 70));
        else
            node.setFillColor(sf::Color(70, 140, 255));

        node.setOutlineThickness(3);

        node.setOutlineColor(sf::Color(180, 180, 180));

        win.draw(node);

        // ===== Agregado : texto =====

        sf::Text txt;

        txt.setFont(font);

        txt.setString(std::to_string(data));

        txt.setCharacterSize(16);

        txt.setFillColor(sf::Color::Black);

        sf::FloatRect b = txt.getLocalBounds();

        txt.setOrigin(
            b.left + b.width / 2.0f,
            b.top + b.height / 2.0f
        );

        txt.setPosition(
            px + 23,
            py + 22
        );

        win.draw(txt);
    }
};

//////////////////////////////////////////////////////////////////

class BinTree
{
public:

    BinTree();

    bool find(int x, TNode**& p);

    bool insert(int x);

    void insert(std::initializer_list<int> vals);

    void build_perimeter();

    void draw(sf::RenderWindow& win,
        sf::Font& font);

private:

    TNode* root;

    // ===== Agregado : perimeter =====

    std::vector<int> order;

    std::unordered_set<int> used;

    // ===== Agregado : helpers =====

    void left_side(TNode* p);

    void visible_nodes(TNode* p);

    void right_side(TNode* p);

    // ===== Agregado : dibujo =====

    void locate(
        TNode* p,
        int level,
        float a,
        float b
    );

    void draw_lines(
        TNode* p,
        sf::RenderWindow& win
    );

    void draw_tree(
        TNode* p,
        sf::RenderWindow& win,
        sf::Font& font
    );
};

//////////////////////////////////////////////////////////////////

BinTree::BinTree()
{
    root = nullptr;
}

//////////////////////////////////////////////////////////////////

bool BinTree::find(int x, TNode**& p)
{
    for (
        p = &root;
        *p && (*p)->data != x;
        p = &((*p)->child[x > (*p)->data])
        );

    return *p;
}

//////////////////////////////////////////////////////////////////

bool BinTree::insert(int x)
{
    TNode** p;

    if (find(x, p))
        return false;

    *p = new TNode(x);

    return true;
}

//////////////////////////////////////////////////////////////////

void BinTree::insert(std::initializer_list<int> vals)
{
    for (int x : vals)
        insert(x);
}

//////////////////////////////////////////////////////////////////
// ===== Agregado : lado izquierdo =====
//////////////////////////////////////////////////////////////////

void BinTree::left_side(TNode* p)
{
    while (p)
    {
        if (p->child[0] || p->child[1])
        {
            order.push_back(p->data);

            used.insert(p->data);
        }

        if (p->child[0])
            p = p->child[0];
        else
            p = p->child[1];
    }
}

//////////////////////////////////////////////////////////////////
// ===== Agregado : visibles =====
//////////////////////////////////////////////////////////////////

void BinTree::visible_nodes(TNode* p)
{
    if (!p)
        return;

    visible_nodes(p->child[0]);

    bool visible =
        (!p->child[0] || !p->child[1]);

    if (visible)
    {
        if (!used.count(p->data))
        {
            order.push_back(p->data);

            used.insert(p->data);
        }
    }

    visible_nodes(p->child[1]);
}

//////////////////////////////////////////////////////////////////
// ===== Agregado : lado derecho =====
//////////////////////////////////////////////////////////////////

void BinTree::right_side(TNode* p)
{
    std::stack<int> aux;

    while (p)
    {
        if (p->child[0] || p->child[1])
            aux.push(p->data);

        if (p->child[1])
            p = p->child[1];
        else
            p = p->child[0];
    }

    while (!aux.empty())
    {
        order.push_back(aux.top());

        used.insert(aux.top());

        aux.pop();
    }
}

//////////////////////////////////////////////////////////////////

void BinTree::build_perimeter()
{
    // ===== Agregado : limpiar =====

    order.clear();

    used.clear();

    if (!root)
        return;

    // ===== Agregado : raiz =====

    order.push_back(root->data);

    used.insert(root->data);

    // ===== Agregado : recorrido =====

    left_side(root->child[0]);

    visible_nodes(root->child[0]);

    visible_nodes(root->child[1]);

    right_side(root->child[1]);
}

//////////////////////////////////////////////////////////////////
// ===== Agregado : posiciones =====
//////////////////////////////////////////////////////////////////

void BinTree::locate(
    TNode* p,
    int level,
    float a,
    float b)
{
    if (!p)
        return;

    float middle = (a + b) / 2.0f;

    p->px = middle;

    p->py = 60 + level * 105;

    locate(
        p->child[0],
        level + 1,
        a,
        middle
    );

    locate(
        p->child[1],
        level + 1,
        middle,
        b
    );
}

//////////////////////////////////////////////////////////////////
// ===== Agregado : lineas =====
//////////////////////////////////////////////////////////////////

void BinTree::draw_lines(
    TNode* p,
    sf::RenderWindow& win)
{
    if (!p)
        return;

    for (int i = 0; i < 2; i++)
    {
        if (p->child[i])
        {
            sf::Vertex edge[] =
            {
                sf::Vertex(
                    sf::Vector2f(
                        p->px + 23,
                        p->py + 23
                    ),
                    sf::Color(180,180,180)
                ),

                sf::Vertex(
                    sf::Vector2f(
                        p->child[i]->px + 23,
                        p->child[i]->py + 23
                    ),
                    sf::Color(180,180,180)
                )
            };

            win.draw(edge, 2, sf::Lines);
        }
    }

    draw_lines(p->child[0], win);

    draw_lines(p->child[1], win);
}

//////////////////////////////////////////////////////////////////
// ===== Agregado : dibujar =====
//////////////////////////////////////////////////////////////////

void BinTree::draw_tree(
    TNode* p,
    sf::RenderWindow& win,
    sf::Font& font)
{
    if (!p)
        return;

    p->border = used.count(p->data);

    p->render(win, font);

    draw_tree(p->child[0], win, font);

    draw_tree(p->child[1], win, font);
}

//////////////////////////////////////////////////////////////////

void BinTree::draw(
    sf::RenderWindow& win,
    sf::Font& font)
{
    // ===== Agregado : posiciones =====

    locate(root, 0, 40, 1360);

    // ===== Agregado : lineas =====

    draw_lines(root, win);

    // ===== Agregado : nodos =====

    draw_tree(root, win, font);

    // ===== Agregado : cuadro perimeter =====

    sf::RectangleShape box;

    box.setSize(sf::Vector2f(1320, 70));

    box.setPosition(30, 700);

    box.setFillColor(sf::Color(25, 25, 25));

    box.setOutlineThickness(2);

    box.setOutlineColor(sf::Color(120, 120, 120));

    win.draw(box);

    // ===== Agregado : perimeter =====

    std::stringstream ss;

    ss << "OUTSIDE BORDER : ";

    for (int x : order)
        ss << x << " ";

    sf::Text txt;

    txt.setFont(font);

    txt.setCharacterSize(24);

    txt.setFillColor(sf::Color(230, 230, 230));

    txt.setString(ss.str());

    txt.setPosition(45, 720);

    win.draw(txt);
}

//////////////////////////////////////////////////////////////////

int main()
{
    // ===== Agregado : ventana =====

    sf::RenderWindow app(
        sf::VideoMode(1400, 800),
        "Binary Tree Border"
    );

    app.setFramerateLimit(60);

    // ===== Agregado : fuente =====

    sf::Font font;

    if (!font.loadFromFile(
        "C:/Windows/Fonts/arial.ttf"))
    {
        std::cout << "Error font\n";
    }

    // ===== Agregado : arbol =====

    BinTree bt;

    bt.insert({
        50,
        30,70,
        20,40,60,80,
        15,25,35,45,55,65,75,90,
        12,22,28,38,42,48,52,62,68,78,85,95
        });

    // ===== Agregado : perimeter =====

    bt.build_perimeter();

    //////////////////////////////////////////////////////////////

    while (app.isOpen())
    {
        sf::Event e;

        while (app.pollEvent(e))
        {
            if (e.type ==
                sf::Event::Closed)
            {
                app.close();
            }
        }

        // ===== Agregado : fondo distinto =====

        app.clear(sf::Color(15, 15, 20));

        bt.draw(app, font);

        app.display();
    }

    return 0;
}
