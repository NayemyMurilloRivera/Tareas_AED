
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
		if (precedencia(c) == 4) { // "(" NO HACE NI M
			pela.push(c);
		}
		if (precedencia(c) == 0) { // numeros
			resultado.push_back(c);
		}

		if (precedencia(c) == 2) { // debe seguir la regla de mayor e igual 
			for (; !pela.empty() && precedencia(pela.top()) >=2; pela.pop()) {
				resultado.push_back(pela.top());
			}
			pela.push(c);

		}
		if (precedencia(c) == 3) { // debe seguir la regla de mayor e igual * /
			for (; !pela.empty() && precedencia(pela.top()) > 3; pela.pop()) {
				resultado.push_back(pela.top());
			}
			pela.push(c);

		}

		if (precedencia(c) == 1) {
			for (; !pela.empty() && precedencia(pela.top()) >= 1; pela.pop()) {
				resultado.push_back(pela.top());
			}
			pela.push(c);

		}


		if (precedencia(c) == 5) {
			for ( ; !pela.empty() && precedencia(pela.top())!=4; pela.pop()){
				resultado.push_back(pela.top()); 
			}
			if (!pela.empty()) pela.pop();

		}

	for (; !pela.empty(); pela.pop()) {
		resultado.push_back(pela.top());

	}
		
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
	string a;
	cout << "Ingresa tu a:" << endl; cin >> a;
	polar webadita;
	webadita.forma_pila(a);
	webadita.impresion();


	
	
}
