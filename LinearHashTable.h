/* Modifications TODO:
For the equals case (0.25), downsize
First remove the element, then check if it needs to be downsized


*/
#ifndef LINEAR_HASH_TABLE_H
#define LINEAR_HASH_TABLE_H

/*****************************************
 * UW User ID:  uwuserid
 * Submitted for ECE 250
 * Semester of Submission:  (Winter|Spring|Fall) 20NN
 *
 * By submitting this file, I affirm that
 * I am the author of all modifications to
 * the provided code.
 *****************************************/

#include "Exception.h"
#include "ece250.h"
#include <iostream>
#include <math.h>

using namespace std;


enum state {EMPTY, OCCUPIED, DELETED};

template<typename T>
class LinearHashTable {
	private:
		
		int original_cap;
		int count; // The number of elements currently in the hash table
		int power; // This	is	associated with the capacity of the hash table (array_size = 2^power)
		int array_size; // The capacity of the hash table
		T *array; //This array will contain the values placed in the hash table
		state *occupied; // An array of objects of type “state” – to store the status of the bin. The state
						 // of a bin is one of three possible values: EMPTY, OCCUPIED, or DELETED
		int mod(int, int) const;
		int hash( T const &, int) const;
		void doubleCapacity();
		void halfCapacity();

	public:
		LinearHashTable( int = 5 );
		~LinearHashTable();
		int size() const;
		int capacity() const;
		double load_factor() const;
		bool empty() const;
		bool member( T const & ) const;
		T bin( int ) const;

		void insert( T const & );
		bool remove( T const & );
		void clear();
		void print() const;
};


/*
The constructor takes an argument m and creates a hash table with 2^m bins, indexed from 0 to 2^m - 1. The
default value of m is 5. Notice that you need to allocate and initialize two arrays, one that stores the values
in the hash table, and the other that stores the status of the bins.
*/
template<typename T >
LinearHashTable<T >::LinearHashTable( int m ){
	if (m < 0){
		power = 0;
	}else{
		power = m;
	}
	
	array_size = pow(2, power);
	array = new T[array_size];
	occupied = new state[array_size];
	count = 0;
	original_cap = array_size;
	
}


/*
The destructor deletes the memory allocated for the hash table. Notice that the hash table has been
represented using two arrays, and they both need to be deleted.
*/
template<typename T >
LinearHashTable<T >::~LinearHashTable() {
	delete [] array;
	delete [] occupied;
	
}


/*
Return the number of elements currently stored in the hash table. (O(1))
*/
template<typename T >
int LinearHashTable<T >::size() const {
	return count;
}


/*
Return the number of bins in the hash table. (O(1))
*/
template<typename T >
int LinearHashTable<T >::capacity() const {
    return array_size;
}


/*
Return the load factor of the hash table. (O(1))
*/
template<typename T >
double LinearHashTable<T >::load_factor() const {
    double output = ((double) size() ) / ( (double) capacity());
	return output;
}


/*
Return true if the hash table is empty, false otherwise. (O(1))
*/
template<typename T >
bool LinearHashTable<T >::empty() const {
    if (count == 0){
    	return true;
    }else{
    	return false;
    }
}


/*
Return true if object obj is in the hash table and false otherwise. (O(1))
*/
template<typename T >
bool LinearHashTable<T >::member( T const &obj ) const {
	// implement the hashing and probing functions first
	
	int i = 0;
	int index = hash(obj, i);
	if (occupied[index] == EMPTY){
		return false; // there's no way it can be in the array...
	}else if (occupied[index] == OCCUPIED && array[index] == obj){
		//cout << "Found the member object, at h(k, 0) " << endl;
		return true;
	}else if (occupied[index] != EMPTY){ // occupied or deleted
		i++;
		index = hash(obj, i);
	// probe for it
		while (occupied[index] != EMPTY){
			if (occupied[index] == OCCUPIED && array[index] == obj){
				//cout << "Found a match at index " << index << endl; 
				return true;
			}
			i++;
			index = hash(obj, i);
		}
	
		// couldn't find it
    	return false;
	}
}


/*
Return the entry in bin n. The behaviour of this function is undefined if the bin
is not filled. It will only be used to test the class with the expected locations. (O(1))
*/
template<typename T >
T LinearHashTable<T >::bin( int n ) const {
    if (n < 0 || n >= array_size){
    	throw ArrayBoundsException();
    }
    return array[n];	
}


/*
The % operation gives negative numbers
*/
template<typename T >
int LinearHashTable<T >::mod(int n, int m) const{
	int index = n % m;
	
	if (index < 0){
		index = index + m;
	}
	
	return index;
}


/*

*/
template<typename T >
int LinearHashTable<T >::hash( T const &obj, int i) const{
	// convert the double to an int...
	
	int hk = mod((int) obj, capacity());
	int index = mod((hk + i), capacity());
	
	return index;
}

/*
double the size of hash table, rehash all elements, and then
insert the new element (O(n)).
*/
template<typename T >
void LinearHashTable<T >::doubleCapacity() {
	
	power++;
	array_size = array_size*2;
	
	T * arrayCopy = array;
	state * occupiedCopy = occupied;
	
	array = new T[array_size];
	occupied = new state[array_size];
	
	// rehash everything
	for (int i = 0; i < array_size/2; i++){
		if (occupiedCopy[i] == OCCUPIED){
			insert(arrayCopy[i]);
			count--; // remember that insert automatically updates the counter, so maintain it
		}
	}
	
	
	delete [] arrayCopy;
	delete [] occupiedCopy;
}
	
/*
 to be done in O(n)? 
*/
template<typename T >
void LinearHashTable<T >::halfCapacity() {
	
	// The array_size must be updated first because this function calls insert(), which must be done based
	// on the new array_size
	
	power--;
	
	int count_copy = count;
	
	array_size = array_size/2;
	
	T * arrayCopy = array;
	state * occupiedCopy = occupied;
	
	array = new T[array_size];
	occupied = new state[array_size];
		
	for (int i = 0; i < 2*array_size; i++){
		if (occupiedCopy[i] == OCCUPIED){
			insert(arrayCopy[i]);
			//count--; // remember that insert automatically updates the counter, so maintain it
		}
	}
	
	count = count_copy;
	delete [] arrayCopy;
	delete [] occupiedCopy;
	
}

/*
If load factor is less than or equal to 0.75, you have to insert the new
object into the hash table in the appropriate bin as determined by the hash function and the rules
of linear probing (O(1)). Otherwise, double the size of hash table, rehash all elements, and then
insert the new element (O(n)).
*/
template<typename T >
void LinearHashTable<T >::insert( T const &obj ) {
	if (load_factor() > 0.75){
		doubleCapacity();
	}

	int i = 0;
	int index = hash(obj, i);
	while (occupied[index] == OCCUPIED){
		// no break condition required
		i++;
		index = hash(obj, i);
	}
	
	//cout << "Insert at index " << index << endl;
	
	array[index] = obj;
	occupied[index] = OCCUPIED;
	count++;
		
}



/*
First check if the element to be removed even exits.
If it does not exist, then do nothing
If it does exist, then remove it.
Now check the load factor.
If the load factor is greater or EQUAL than 0.25, and the current capacity of hash table is not equal to the initial capacity (that was assigned in
the constructor), you have to shrink the capacity of the hash table to the half of its current size,
rehash the elements, and remove the input from the table (O(n)). 
*/
template<typename T >
bool LinearHashTable<T >::remove( T const &obj ) {
	// enter your implemetation here
	// delete normally
	if (!member(obj)){
		//cout << "The object isn't even in the table" << endl;
		return false;
	}
	
	int i = 0;
	int index = hash(obj, i);
	
	// we can just loop through the whole thing lol
	while (array[index] != obj){ // intil I find a match lol
		if (i == count){
			// we've probed the cluster and it's not there 
			return false;	
		}
	 	i++;
		index = hash(obj, i);
	}
	// array[index] = NULL; // not actually neccesary
	occupied[index] = DELETED;
	count--;
	
	
	if (load_factor() <= 0.25 && capacity() != original_cap){
		halfCapacity();
	}
	
	return true;
}


/*
Remove all the elements in the hash table. (O(m))
*/
template<typename T >
void LinearHashTable<T >::clear() {
	count = 0;
	delete [] array;
	delete [] occupied;
	
	array = new T[array_size];
	occupied = new state[array_size];
}

template<typename T >
void LinearHashTable<T >::print() const{

	for (int i  = 0; i < array_size; i++){
		if (occupied[i] == OCCUPIED){
			cout << array[i] << ", ";
		}
	}
	cout << "\n" << endl;
}

#endif

