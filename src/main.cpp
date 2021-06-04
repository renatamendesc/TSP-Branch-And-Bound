#include "data.h"
#include "hungarian.h"
#include <iostream>
#include <vector>
#include <climits>

using namespace std;

double ** matrizReal;
double ** matrizModificada;
int dimension;

typedef struct {

	vector <pair <int, int>> arcosProibidos; // Lista de arcos que foram proibidos
	vector <vector <int>> subtours; // Lista de subtours gerados pela solução do algoritmo húngaro

	double lowerBound; // Resultado da solução gerada pelo algoritmo húngaro
	int escolhido; // Subtour escolhido que terá arcos proibidos
	bool podar; // Indica se o nó deve gerar filhos

} Node;

void proibeArcos(Node &node){

	// Matriz modificada recebe matriz original
	for(int i = 0; i < dimension; i++){
        for(int j = 0; j < dimension; j++){
			matrizModificada[i][j] = matrizReal[i][j];
        }
    }

	// Percorre arcos a serem proibidos
	for(int i = 0; i < node.arcosProibidos.size(); i++){

		// Adiciona restrição referente aos arcos proibidos
		matrizModificada[node.arcosProibidos[i].first-1][node.arcosProibidos[i].second-1] = INFINITE;
	}
}

void calcularSolucao(hungarian_problem_t *p, Node &node){
	vector <int> listaCandidatos;
	vector <int> subtour;

	// Forma lista de candidatos
	for(int i = 0; i < dimension; i++){
		listaCandidatos.push_back(i+1);
	}

	// Forma solução de subtours
	while(!listaCandidatos.empty()){
		int i = listaCandidatos[0]-1; 
		int inicio = i;
		subtour.push_back(inicio + 1);

		while(true){
			// Apaga nó em questão da lista de candidatos
			for(int j = 0; j < listaCandidatos.size(); j++){
				if(listaCandidatos[j] == i+1){
					listaCandidatos.erase(listaCandidatos.begin() + j);
				}
			}

			// Verifica existência do arco
			for(int j = 0; j < dimension; j++){
				if(p->assignment[i][j]){
					subtour.push_back(j+1);
					i = j;
					break;
				}
			}

			// Encerra percurso do subtour
			if(inicio == i){
				node.subtours.push_back(subtour);
				subtour.clear();

				break;
			}
		}
	}

	if(node.subtours.size() == 1){
		node.podar = true;
	}else{
		node.podar = false;
	}

	int menorSubtour = INT_MAX;

	// Verifica subtour a ser escolhido
	for(int i = 0; i < node.subtours.size(); i++){
		if(node.subtours[i].size() < menorSubtour){
			menorSubtour = node.subtours[i].size();
			node.escolhido = i;
		}
	}
}

void branchAndBound(){
	vector <double> upperBounds;
	vector <Node> arvore;
	Node raiz;

	matrizModificada = new double *[dimension];

	for(int i = 0; i < dimension; i++){
		matrizModificada[i] = new double [dimension];

        for(int j = 0; j < dimension; j++){
			matrizModificada[i][j] = matrizReal[i][j];
        }
    }

	int mode = HUNGARIAN_MODE_MINIMIZE_COST;
	hungarian_problem_t p;
	hungarian_init(&p, matrizModificada, dimension, dimension, mode); // Carregando o problema

	raiz.lowerBound = hungarian_solve(&p);
	calcularSolucao(&p, raiz); // Calcula subtours da solução

	arvore.push_back(raiz); // Adiciona raiz

	hungarian_free(&p);

	while(!arvore.empty()){
		int tam = arvore.size();

		// Percorre nós da árvore	
		for(int i = 0; i < tam; i++){
			Node node = arvore[0];

			// Verifica se o nó em questão tem solução viável
			if(!node.podar){

				// Percorre arcos a serem proibidos
				for(int j = 0; j < node.subtours[node.escolhido].size()-1; j++){

					Node newNode;
					bool flag = true;
					newNode.arcosProibidos = node.arcosProibidos; // Novo nó herda arcos proibidos 

					// Define o arco proibido em questão
					pair <int, int> arco;

					arco.first = node.subtours[node.escolhido][j];
					arco.second = node.subtours[node.escolhido][j+1];

					// Adiciona novo arco proibido
					newNode.arcosProibidos.push_back(arco);

					proibeArcos(newNode);

					hungarian_problem_t p;
					hungarian_init(&p, matrizModificada, dimension, dimension, mode); // Carregando o problema
					newNode.lowerBound = hungarian_solve(&p);
					calcularSolucao(&p, newNode);

					/*
					cout << "Filhos de " << node.lowerBound << ": " << endl;
					cout << "Lower bound: " << newNode.lowerBound << endl;

					cout << "Subtours: " << endl;
					for(int k = 0; k < newNode.subtours.size(); k++){
						for(int l = 0; l < newNode.subtours[k].size(); l++){
							cout << newNode.subtours[k][l] << " ";
						}
						cout << endl;
					}
					*/

					for(int k = 0; k < upperBounds.size(); k++){
						if(newNode.lowerBound > upperBounds[k]){
							flag = false;
						}
					}

					if(flag){
						arvore.push_back(newNode); // Adiciona na árvore
					}
					
					hungarian_free(&p);
				}

			} else {
				/*
				cout << "Lower bound: " << node.lowerBound << endl;

				cout << "Subtours: " << endl;
				for(int k = 0; k < node.subtours.size(); k++){
					for(int l = 0; l < node.subtours[k].size(); l++){
						cout << node.subtours[k][l] << " ";
					}
					cout << endl;
				}
				*/

				upperBounds.push_back(node.lowerBound);

				// cout << "podar!" << endl;

			}

			arvore.erase(arvore.begin());

		}
	}

	double custo = __DBL_MAX__;
	for(int i = 0; i < upperBounds.size(); i++){
		//cout << "upper bound " << i+1 << ": " << upperBounds[i] << endl;
		if(upperBounds[i] < custo){
			custo = upperBounds[i];
		}
	}

	for (int i = 0; i < dimension; i++){
        delete[] matrizModificada[i];
    }

	cout << "Custo: " << custo << endl;
}

int main(int argc, char** argv) {

	Data * data = new Data(argc, argv[1]);
	data->readData();

	double ** cost = new double*[data->getDimension()];
	for(int i = 0; i < data->getDimension(); i++){
		cost[i] = new double[data->getDimension()];
		for (int j = 0; j < data->getDimension(); j++){
			cost[i][j] = data->getDistance(i,j);
		}
	}

	dimension = data->getDimension();
	matrizReal = cost;

	branchAndBound();

	for (int i = 0; i < data->getDimension(); i++) delete [] cost[i];
	delete [] cost;
	delete data;

	return 0;
}
