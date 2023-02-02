#include <iostream>
#include <string>
#include "list.h"
using std::cout;


List::List() {
	first = nullptr;	
}

List::~List() {
	if (first != nullptr){
		int sz = size();
		for (int i = 0; i < sz-1; i++){
			remove(first->next->value, List::DeleteFlag::EQUAL);
		}
	}
	delete first;
}

bool List::exists(int d) const {
	Node* temp = first;
	while(temp != nullptr){
		if (temp -> value == d){
			return true;
		}
		temp = temp->next;
	}
	return false;
}

int List::size() const {
	int counter(0);
	Node* temp = first;
	while(temp != nullptr){
		counter++;
		temp = temp->next;
	}
	return counter;
}

bool List::empty() const {
	return first==nullptr;
}

void List::insertFirst(int d) {
	first = new Node(d, first);
}

void List::remove(int d, DeleteFlag df) {
	if (empty()){
		return;
	}
	Node* temp = first;
	Node* previous = first;
	while(temp != nullptr){
		if (df == List::DeleteFlag::EQUAL){
			if ((temp -> value) == d){
				if(previous == temp){
					first = temp->next;
					delete temp;
					return;
				}
				previous->next = temp->next;
				delete temp;
				return;
			}
		}
		if (df == List::DeleteFlag::LESS){
			if ((temp -> value) < d){
				if(previous == temp){
					first = temp->next;
					delete temp;
					return;
				}
				previous->next = temp->next;
				delete temp;
				return;
			}
		}
		if (df == List::DeleteFlag::GREATER){
			if ((temp -> value) > d){
				if(previous == temp){
					first = temp->next;
					delete temp;
					return;
				}
				previous->next = temp->next;
				delete temp;
				return;
			}
			
		}
		previous = temp;
		temp = temp->next;
	}
}

void List::print() const {
	Node* temp = first;
	int counter = 0;
	cout << "Printing List: \n";
	if (empty()){
		cout << "Empty!" << std::endl;
		return;
	}
	while(temp != nullptr){
		cout << "Node " << counter << "="<<(temp->value) << "\n";
		counter++;
		temp = temp->next;
	}
	cout << std::endl;

}
