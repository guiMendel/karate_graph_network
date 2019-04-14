#ifndef _GRAPH_
#define _GRAPH_
#include <iterator>
#include <vector>
#include <string>
using namespace std;

class Node {
	int id;	// ID GML do no
	int degree;	// grau do no
	double clustering_coefficient; // armazena o resultado da funcao de mesmo nome
	string label;	// rotulo do no
	vector<Node*> neighbor;	// uma lista de vizinhos do no

public:
	Node() : id(++total), degree(0) {}
	~Node() {total--;}
	bool operator == (Node &nd) {return this->id == nd.get_id();}
	static int total;	// total de nos
	int get_degree() {return degree;}
	int get_id() {return id;}
	const vector<Node*>& get_neighbors() {return neighbor;}
	void add_neighbor(Node &nd);
	// garante se o no especificado e vizinho do atual
	bool is_neighbor(Node &nd);
	// garante se todos os nos da lista sao vizinhos do atual
	bool is_neighbor(vector<Node*> &node);
	// calcula o coeficiente de aglomeracao local
	double set_clustering_coefficient();
	double get_clustering_coefficient() {return clustering_coefficient;}
};

class Graph {
public:
	vector<Node*> node;	// a famosa lista de nos

	void add_node() {node.push_back(new Node);}	// adiciona um no
	void add_edge(int a, int b);	// adiciona uma aresta
	void print_nodes();	// imprime na tela diversas informacoes sobre o grafo
	void print_maximal_cliques();	// imprime na tela todos os cliques maximos e maximais
	// libera a memoria e apaga o grafo
	void clear();
};

class MaximalClique {
	// essa lista vai conter todos os vertices que ja tiveram suas opcoes exauridas
	vector<Node*> marked_node;
	// essa lista vai conter todos os cliques maximais encontrados
	vector< vector<Node*>* > maximal_clique;
	// indica quantos cliques maximos existem
	int maximas;
	// armazena o tamanho do maior clique
	int size_record;
	// funcao recursiva que serve de base para o construtor
	// procura em todos os adjacentes de um vertice dado
	// por um vertice que faca parte do clique fornecido
	// retorna uma lista de cliques maximais encontrados 
	// a paritr da raiz fornecida (maximos na frente)
	vector< vector<Node*>* > search_through(Node &nd, vector<Node*>* current_clique);
	// verifica se um dado clique ja esta presente na lista
	// de cliques maximais
	bool subclique(vector<Node*> &clique, vector< vector<Node*>* > &list);
	// apaga todas as ocorrencias de permutacao de um mesmo clique na lista especificada
	void erase_permutations(vector< vector<Node*>* > &clique);

public:
	// itera vertice por vertice, tratando cada um como raiz,
	// e encontra todas as possibilidades de clique maximal
	// que envolvam este vertice, os devolve numa lista
	MaximalClique(Graph &graph);
	// imprime no terminal os cliques encontrados
	void print();
	// libera a memoria das listas dinamicas de cliques
	void free();
};

#endif