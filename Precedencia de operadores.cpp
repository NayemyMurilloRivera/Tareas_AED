
#include <iostream>
#include <stack>
#include <vector>
using namespace std;
class polar {
private:
	vector<string> resultado;
	stack<string> pela;
	

public:
	void forma_pila(string a);
	int precedencia(string a);
	void impresion();

	
};

int polar::precedencia(string a) {
	if (a == "+" || a == "-")return 1;
	if (a == "*" || a == "/" || a=="%") return 2;
	if (a == "^" || a == "$") return 3;
	if (a == "(")return 4;
	if (a == ")") return 5;
	return 0;
}

void polar::forma_pila(string a) {
	for (int i = 0; i < a.length(); i++) {
		string c(1, a[i]);
		cout << c << " -> " << precedencia(c) << endl;
		int p = precedencia(c);
		if (p == 4) {
			pela.push(c);
		}
		else if (p == 0) {
			resultado.push_back(c);
		}
		else if (p == 2) {
			for (; !pela.empty() && precedencia(pela.top()) != 4 && precedencia(pela.top()) >= 2; pela.pop()) {
				resultado.push_back(pela.top());
			}
			pela.push(c);
		}
		else if (p == 3) {
			for (; !pela.empty() && precedencia(pela.top()) != 4 && precedencia(pela.top()) > 3; pela.pop()) {
				resultado.push_back(pela.top());
			}
			pela.push(c);
		}
		else if (p == 1) {
			for (; !pela.empty() && precedencia(pela.top()) != 4 && precedencia(pela.top()) >= 1; pela.pop()) {
				resultado.push_back(pela.top());
			}
			pela.push(c);
		}
		else if (p == 5) {
			for (; !pela.empty() && precedencia(pela.top()) != 4; pela.pop()) {
				resultado.push_back(pela.top());
			}
			if (!pela.empty()) pela.pop();
		}

	}

	for (; !pela.empty(); pela.pop()) {
		resultado.push_back(pela.top());

	}
		
	
}
void polar::impresion()
{
	
	for (int i = 0; i < resultado.size(); i++) {
		cout << resultado[i];
	}

}
int main()
{
	cout << "FUNCIONA NUEVO CODIGO\n";
	string a;
	cout << "Ingresa tu a:" << endl; cin >> a;
	polar webadita;
	webadita.forma_pila(a);
	webadita.impresion();	
}
