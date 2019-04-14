#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "graph.hpp"
using namespace std;

int Node::total = 0;

int main() {

	// cout << "It just works.\n";

	// Cria 34 nos
	Graph kids;
	for(int i=1; i<35; i++) kids.add_node();

	// Le o arquivo e insere as adjacencias
	ifstream fs("karate.gml");
	if(!fs.is_open()) {
		cout << "Erro ao abrir o arquivo!";
		return 0;
	}
	string line;
	int src, tgt;
	while(getline(fs, line)) {
		// apenas se houver uma definicao de source ou target estaremos interessados nesta linha
		if(line.find("source") != -1) {
			// isola o valor dado por source (encontrado apos o caracter 11)
			stringstream(line.substr(11)) >> src;
			// na proxima linha se encontra o destino
			getline(fs, line);
			stringstream(line.substr(11)) >> tgt;
			// agora que ja sabemos quais os nos a ligar, so chamar a funcao
			kids.add_edge(src, tgt);
		}
	}
	fs.close();
	
	// imprime as informacoes do grado em tela
	kids.print_nodes();

	// encontra os cliques maximais, por meio de chamadas recursivas e filtragem de conteudo
	MaximalClique maximal(kids);
	// imprime em tela os cliques maximais encontrados
	maximal.print();
	// libera o espaco ocupado pelas listas de grafos maximais
	maximal.free();
	// libera o espaco ocupado pelo grafo
	kids.clear();

	// cout << "There will be NO microtransactions.\n";

	return 0;
}

void Graph::add_edge(int a, int b) {
	node[a-1]->add_neighbor(*node[b-1]);
	node[b-1]->add_neighbor(*node[a-1]);
}

void Node::add_neighbor(Node &nd) {
	neighbor.push_back(&nd);
	degree++;
}

void Graph::print_nodes() {
	cout << "Listagem de todos os vertices, seus respectivos graus e coeficientes de aglomeracao:\n";
	cout.precision(2);
	for(Node* i : node) 
		cout << "	Vertice " << i->get_id() << ": grau " << i->get_degree() << ", coef. " << showpoint << i->set_clustering_coefficient() << endl;
	cout << "\nO coeficiente medio de aglomeracao do grafo e ";
	double acc = 0;
	for(auto nd : node) acc += nd->get_clustering_coefficient();
	cout << acc/node.size() << "\n";
}

void Graph::clear() {
	for(auto nd : node) {
		delete nd;
	}
	node.clear();
}

MaximalClique::MaximalClique(Graph &graph) {
	maximas = 1;
	size_record = 0;
	marked_node.clear();
	maximal_clique.clear();

	int i_test = 0;

	for(Node* current_node : graph.node) {
		// declara um ponteiro para uma lista de ponteiros de no, que armazenara o clique maximal a ser descoberto
		vector<Node*>* current_clique = new vector<Node*> {current_node};

		// encontra os cliques maximais que envolvem este no
		vector< vector<Node*>* > resulting_clique
			= search_through(*current_node, current_clique);

			// cout << "Fim da iteracao do no " << ++i_test << endl;
			// for(vector<Node*>* test : resulting_clique) {
			// 	cout << "	Clique: ";
			// 	for(Node* node_test : *test) {
			// 		cout << node_test->get_id() << " ";
			// 	}
			// 	cout << "\n";
			// }

		// verifica se o maior destes cliques e maior que os ja registrados
		int i, current_size;
		// reserva o tamanho do maior clique ja registrado para comparar com o recorde dos novos cliques
		// tambem garante que nenhum dos cliques recebidos seja subclique da lista existente
		int registered_record;
		if(maximal_clique.empty()) registered_record = 0;
		else {
			for(auto clique_ptr = resulting_clique.begin();
				clique_ptr < resulting_clique.end();
				clique_ptr++
				) {
				if(subclique(**clique_ptr, maximal_clique)) {
					delete *clique_ptr;
					clique_ptr = resulting_clique.erase(clique_ptr) - 1;
				}
			}
			registered_record = maximal_clique[0]->size();
		}
		// verifica e resolve permutacoes equivalentes de cliques na lista
		erase_permutations(resulting_clique);
		// cout << "	registered_record: " << registered_record << endl;
		// cout << "	size_record: " << size_record << endl;

		// armazena os cliques retornados no vetor principal, tomando cuidado para manter os maximos em primeiro
		for(i=0;
			(i<resulting_clique.size())&&((current_size = resulting_clique[i]->size()) >= registered_record);
			i++
			) {
				if(current_size==registered_record) {
					maximas++;
					maximal_clique.insert(maximal_clique.begin(), resulting_clique[i]);
				}
				else {
					maximas = 1;
					registered_record = current_size;
					maximal_clique.insert(maximal_clique.begin(), resulting_clique[i]);
				}
		}
		for(
			vector< vector<Node*>* >::iterator it = resulting_clique.begin() + i;
			it < resulting_clique.end();
			it++
		) maximal_clique.push_back(*it);

		// armazena o ponteiro do no raiz da atual iteracao, a fim de evitar repeticoes desnecessarias
		marked_node.push_back(current_node);
	}
}

vector< vector<Node*>* > MaximalClique::search_through(Node &nd, vector<Node*>* current_clique) {
	// bandeira que indica se houve uma chamada recursiva na atual recursao
	// se houver, sem duvida o clique atual nao sera maximal
	// pois somente havera uma chamada caso seja encontrado
	// um vizinho que torne o clique atual ainda maior
	bool flag_recursion = false;
	vector< vector<Node*>* > resulting_clique;

	// itera pelos vizinhos para verificar se eles fazem parte do clique
	for(Node *candidate : nd.get_neighbors()) {
		// verifica se este no e vizinho de todos os nos do clique atual
		if(candidate->is_neighbor(*current_clique)) {
			bool flag_marked = false;

			// verifica se este vizinho e um dos nos que ja foram exauridos
			for(Node *marked : marked_node)
				if(*candidate==*marked) flag_marked = true;

			if(!flag_marked) {
				// cria um novo clique para adicionar o novo no
				vector<Node*>* parent_clique = new vector<Node*>;
				*parent_clique = *current_clique;
				parent_clique->push_back(candidate);

				// faz a chamada recursiva, recebe uma nova lista e concatena seu resultado na sua propria
				vector< vector<Node*>* > received_clique = search_through(*candidate, parent_clique);
				if(resulting_clique.empty())
					for(vector<Node*>* received_node : received_clique)
						resulting_clique.push_back(received_node);
				else {
					if(resulting_clique[0]->size() < size_record)
						// se o recorde de tamanho tiver mudado, certamente um clique maximo
						// esta presente no clique recebido. Podemos simplesmente colocar ele antes
						// na lista resultado
						for(vector< vector<Node*>* >::reverse_iterator rit = received_clique.rbegin();
							rit != received_clique.rend();
							rit++
							) resulting_clique.insert(resulting_clique.begin(), *rit);
					else {
						vector< vector<Node*>* >::iterator it;
						for(it = received_clique.begin();
							(it < received_clique.end())&&((*it)->size()==size_record);
							it++
							) resulting_clique.insert(resulting_clique.begin(), *it);
						for(;it < received_clique.end(); it++)
							resulting_clique.push_back(*it);
					}
				}
				// indica que houve a chamada recursiva
				flag_recursion = true;
			}
		}
	}
	// se nenhum dos vizinhos complementa o clique, o clique atual e maximal
	if(!flag_recursion) {
		// armazena o ponteiro para o clique maximal recem descoberto
		// se o tamanho do clique for superior ao recorde, ele vira maximo
		// os cliques maximos serao os primeiros
		if(current_clique->size()<size_record)
			resulting_clique.push_back(current_clique);
		// clique de mesmo tamanho do maior
		else if(current_clique->size()==size_record)
			resulting_clique.insert(resulting_clique.begin(), current_clique);
		// clique maior que os anteriores
		else {
			size_record = current_clique->size();
			resulting_clique.insert(resulting_clique.begin(), current_clique);
		}
	}
	// caso ele tenh feito chamadas recursivas, liberamos a memoria deste
	// clique, pois ele nao e maximal
	else delete current_clique;
	return resulting_clique;
}

bool MaximalClique::subclique(vector<Node*> &clique, vector< vector<Node*>* > &list) {
	for(auto maximal : list) {

		bool flag_found_same_clique = true;
		for(auto node_param : clique) {

			bool flag_found_same_node = false;
			for(auto node_maximal : *maximal) {

				if(*node_param==*node_maximal) {
					flag_found_same_node = true;
					break;
				}
			}
			if(!flag_found_same_node) {
				flag_found_same_clique = false;
				break;
			}
		}
		if(flag_found_same_clique) return true;
	}
	return false;
}

void MaximalClique::erase_permutations(vector< vector<Node*>* > &clique) {
	vector< vector<Node*>* >::iterator pivot_clique_ptr;
	vector< vector<Node*>* >::iterator clique_ptr;

	// iteram pelas listas
	for(pivot_clique_ptr = clique.begin();
		pivot_clique_ptr < clique.end() - 1;
		pivot_clique_ptr++
		) {
		for(clique_ptr = pivot_clique_ptr + 1;
			clique_ptr < clique.end();
			clique_ptr++
			) {

			// compara as duas listas selecionadas
			bool flag_found_same_clique = true;
			for(auto pivot_node : **pivot_clique_ptr) {
				bool flag_found_same_node = false;
				for(auto other_node : **clique_ptr) {
					if(pivot_node==other_node) {
						flag_found_same_node = true;
						break;
					}
				}
				if(!flag_found_same_node) {
					flag_found_same_clique = false;
					break;
				}
			}
			if(flag_found_same_clique) {
				delete *clique_ptr;
				clique_ptr = clique.erase(clique_ptr) - 1;
			}

		}
	}
}

void MaximalClique::print() {
	cout << endl << "Listagem de todos os cliques maximos e maximais:\n";
	int i;
	vector<Node*>::iterator ptr;

	for(i=0; i<maximas; i++) {
		cout << "	Clique maximo, " << maximal_clique[i]->size() << " elementos: ";
		for(ptr=(*maximal_clique[i]).begin(); ptr < (*maximal_clique[i]).end(); ptr++)
			cout << (**ptr).get_id() << " ";
		cout << "\n";
	}

	for(; i<maximal_clique.size(); i++) {
		cout << "	Clique maximal, " << maximal_clique[i]->size() << " elementos: ";
		for(ptr=(*maximal_clique[i]).begin(); ptr < (*maximal_clique[i]).end(); ptr++)
			cout << (**ptr).get_id() << " ";
		cout << "\n";
	}
}
void MaximalClique::free() {
	for(vector<Node*>* target : maximal_clique)
		delete target;
	marked_node.clear();
	maximas = 0;
}
// testado
bool Node::is_neighbor(Node &nd) {
	for(Node* candidate : neighbor)
		if(nd==*candidate) return true;
	return false;
}
// testado
bool Node::is_neighbor(vector<Node*> &node) {
	for(Node* current_node : node)
		if(!current_node->is_neighbor(*this)) return false;
	return true;
}

double Node::set_clustering_coefficient() {
	int total_links;
	// se nao houver grau, nao ha vizinhos
	if(!(total_links = degree)) return 1.0;

	// checa cada vizinho para verificar quantos nos ha entre eles
	auto check_list = neighbor;
	do {
		// pega o ultimo no do vetor
		auto base_node = check_list[check_list.size()-1];
		// retira ele do vetor
		check_list.pop_back();
		// verifica a ligacao entre os demais nos do vetor com este
		for(auto examined_node : check_list)
			if(base_node->is_neighbor(*examined_node))
				total_links++;
	} while(check_list.size()>1);

	double possible_links = degree + 1;
	possible_links = possible_links*(possible_links-1)/2;
	if(!possible_links) return clustering_coefficient = 1.0;
	return clustering_coefficient = total_links/possible_links;
}