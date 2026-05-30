#include <SFML/Graphics.hpp>
#include <iostream>
#include <initializer_list>
#include <stack>
#include <vector>
#include <unordered_set>
#include <sstream>
// descarga el sfml 2019

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

    void render(sf::RenderWindow& win, sf::Font& font)
    {

        sf::CircleShape node(23);

        node.setPosition(px, py);

        if (border)
            node.setFillColor(sf::Color(220, 70, 70));
        else
            node.setFillColor(sf::Color(70, 140, 255));

        node.setOutlineThickness(3);

        node.setOutlineColor(sf::Color(180, 180, 180));

        win.draw(node);

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

    std::vector<int> order;

    std::unordered_set<int> used;


    void left_side(TNode* p);

    void visible_nodes(TNode* p);

    void right_side(TNode* p);

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

BinTree::BinTree()
{
    root = nullptr;
}

bool BinTree::find(int x, TNode**& p)
{
    for (
        p = &root;
        *p && (*p)->data != x;
        p = &((*p)->child[x > (*p)->data])
        );

    return *p;
}

bool BinTree::insert(int x)
{
    TNode** p;

    if (find(x, p))
        return false;

    *p = new TNode(x);

    return true;
}

void BinTree::insert(std::initializer_list<int> vals)
{
    for (int x : vals)
        insert(x);
}

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

void BinTree::build_perimeter()
{
 

    order.clear();

    used.clear();

    if (!root)
        return;
    order.push_back(root->data);

    used.insert(root->data);


    left_side(root->child[0]);

    visible_nodes(root->child[0]);

    visible_nodes(root->child[1]);

    right_side(root->child[1]);
}

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

void BinTree::draw(
    sf::RenderWindow& win,
    sf::Font& font)
{


    locate(root, 0, 40, 1360);

    draw_lines(root, win);


    draw_tree(root, win, font);

    sf::RectangleShape box;

    box.setSize(sf::Vector2f(1320, 70));

    box.setPosition(30, 700);

    box.setFillColor(sf::Color(25, 25, 25));

    box.setOutlineThickness(2);

    box.setOutlineColor(sf::Color(120, 120, 120));

    win.draw(box);

    std::stringstream ss;

    ss << "Orden de bordes : ";

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

int main()
{

    sf::RenderWindow app(
        sf::VideoMode(1400, 800),
        "Binary Tree Border"
    );

    app.setFramerateLimit(60);


    sf::Font font;

    if (!font.loadFromFile(
        "C:/Windows/Fonts/arial.ttf"))
    {
        std::cout << "Error font\n";
    }

    BinTree bt;

    bt.insert({
        50,
        30,70,
        20,40,60,80,
        15,25,35,45,55,65,75,90,
        12,22,28,38,42,48,52,62,68,78,85,95
        });

    bt.build_perimeter();
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
        app.clear(sf::Color(15, 15, 20));

        bt.draw(app, font);

        app.display();
    }

    return 0;
}
