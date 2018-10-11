/**
 * @file Heaper.cpp - use threads to implement a two-pass parallel algorithm to
 * compute the prefix sum of an array
 * @author Lolita Lam
 *
 * Design:
 * 1. Use heap and allocate 8 threads to implement recursive pairwise sums.
 * 2. Again allocate 8 threads to compute the prefix sum in parallel. 
 */

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <future>
using namespace std;

const int N = 100000000;  		/** const integer to determine size of array */
typedef vector<int> Data; 		/** defined type to hold integers */


/**
 * @class Heaper - array-implemented tree made of two arrays (interior, data)
 */
class Heaper {
public:
	/**
	 * Constructor
	 * Will pad the array if the size of the array is not a power of two
	 * @param Data 		Data object
	 */
	Heaper(Data *data) : n(data->size()), data(data) {
		unsigned result = nextPowOfTwo(n);
		if (data->size() == result)
			interior = new Data(n-1, 0);
		else {
			int padding = result - n;
			padData(data, padding);
			interior = new Data((n-1) + padding, 0);
			n = data->size();
		}
	}
	/**
	 * Destructor
	 */
	virtual ~Heaper() {
		delete interior;
	}
protected:
	int n; 						/** the size of Data */
	const Data *data;			/** pointer to array containing leaf nodes */
	Data *interior;				/** pointer to array containing interior nodes */

	/**
	 * Returns the size of heap
	 * @returns the size of the heap (interior and data array)
	 */
	virtual int size() {
		return (n-1) + n;
	}
	/**
	 * Returns the value of an item in the heap
	 *
	 * @param i  		index in array
	 * @returns the value of an indexed item in an array
	 */
	virtual int value(int i) {
		if (i < n-1)
			return interior->at(i);
		else
			return data->at(i - (n-1));
	}
	/**
	 * Returns the parent of an index
	 *
	 * @param i 		index in array
	 * @returns the index of the parent
	 */
	virtual int parent(int i) {
		return (i-1)/2;
	}
	/**
	 * Returns a boolean describing the position of a node
	 * 
	 * @param i 		index in array
	 * @returns true if index is in data array, false otherwise
	 */
	virtual bool isLeaf(int i) {
		if (i == n-1 || i > n-1)
			return true;
		return false;
	}
	/**
	 * Returns the left child of an item
	 * 
	 * @param i 		index in array
	 * @returns the index of the left child
	 */
	virtual int left(int i) {
		return 2*i+1;
	}
	/**
	 * Returns the right child of an item
	 * 
	 * @param i 		index in array
	 * @returns the index of the right child
	 */
	virtual int right(int i) {
		return 2*i+2;
	}
	/**
	 * Returns a boolean for whether a number is a power of two
	 * 
	 * @param i 		an integer
	 * @returns true if power of two, otherwise false
	 */
	bool isPowOfTwo(int i) {
		return i > 0 && !(i & (i-1));
	}
	/**
	 * Returns the next power of two of an integer, if it isn't one already
	 * 
	 * @param i 		an integer
	 * @returns an integer that is a power of two
	 */
	int nextPowOfTwo(int i) {
		int count = 0;
		if (i && !(i &(i-1)))
			return i;
		while (i != 0) {
			i >>= 1;
			count += 1; 
		}
		return 1 << count;
	}
	/**
	 * Pads the data if the size is not a power of 2. 
	 *
	 * @param d 		Data object
	 * @param i 		size of padding 
	 */
	void padData(Data *d, int i) {
		for (int j = 0; j < i; j++)
				d->push_back(0);
	}
};



/**
 * @class SumHeap - class which inherits from Heaper
 */
class SumHeap : public Heaper {
public: 
	/**
	 * Constructor
	 * 
	 * @param data 		Data object
	 */
	SumHeap(Data *data) : Heaper(data) {
		calcSum(0, 1);
	}
	/**
	 * Prefix sum pass of algorithm. Pads array if it isn't a power of two.
	 *
	 * @param d 		Data object
	 */
	void prefixSums(Data *d) { 
		int size = d->size();
		if (size != nextPowOfTwo(size)) {
			int i = data->size() - size; //how much needs to be padded
			padData(d, i);
		}
		sumPrefixes(d, 0, 0, 1);
	}
	/**
	 * Prints the items in the passed-in Data object.
	 *
	 * @param data 		Data object
	 */
	void printHeap(Data *data) {
		for(size_t i=0; i<data->size(); ++i)
  			cout << data->at(i) << ' ';
		cout << endl;
	}
private:
	/**
	 * Calculates sum in pairs at each level recursively by computing work on 
	 * the left side and forking a thread on the right side. 
	 * 
	 * @param i 		index of array
	 * @param level		current level of tree
	 */
	void calcSum(int i, int level) {
		if (isLeaf(i))
			return;
		if (level < 4) {
			auto handle = async(launch::async, &SumHeap::calcSum, this, right(i), level+1);
			calcSum(left(i), level+1);
			handle.wait();
		}
		else {
			calcSum(left(i), level+1);
			calcSum(right(i), level+1);
		}
		interior->at(i) = value(left(i)) + value(right(i));
	}
	/**
	 * Calculates prefix sum at each level recursively by computing work on 
	 * the left side and forking a thread on the right side. 
	 * 
	 * @param data 		Data object
	 * @param sumPrior	value from parent
	 * @param level		current level of tree
	 */
	void sumPrefixes(Data *data, int i, int sumPrior, int level) {
		if (isLeaf(i)) {
			data->at(i - (data->size() - 1)) = sumPrior + value(i);
			return;
		}
		if (level < 4) {
			auto handle = async(launch::async, &SumHeap::sumPrefixes, this, data, right(i), sumPrior + value(left(i)), level+1);
			sumPrefixes(data, left(i), sumPrior, level+1);
			handle.wait();
		}
		else {
			sumPrefixes(data, left(i), sumPrior, level+1);
			sumPrefixes(data, right(i), sumPrior + value(left(i)), level+1);
		}
	}
};



/**
 * Test for prefix sum.
 * 
 * @returns 0 (success)
 */
int main() {
	// create arrays of N elements with value 1 
    Data data(N, 1);
    Data prefix(N, 1);

    // start timer
    auto start = chrono::steady_clock::now();

    SumHeap heap(&data);
    heap.prefixSums(&prefix);

    // stop timer
    auto end = chrono::steady_clock::now();
    auto elapsed = chrono::duration<double,milli>(end-start).count();
    
    cout << "Elapsed time is: " << elapsed << "ms" << endl;
    cout << "N is " << N << " but size is padded to " << prefix.size() << endl;
    cout << "prefix[0] is " << prefix[0] << endl;
    cout << "prefix[N-1] is " << prefix[N-1] << endl;
    cout << "prefix[" << prefix.size() << "] is " << prefix.back() << endl;
    
    return 0;
}
