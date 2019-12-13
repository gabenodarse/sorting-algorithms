//Fixed tree sort

/********************************************************************************************************
//Fixed tree sort creates a binary search tree of a fixed size. The tree is big enough to hold all elements of the array, but
//	this would never realistically happen. Elements are placed in the binary tree on order of appearance, searching for an empty
//	location where it can be inserted. Elements that overflow past the fixed size of the binary tree are put into overflow arrays
//	corresponding to the location at which they overflowed. These overflow arrays are then sorted on their own. This results in a fast
//	sort, except in some edge cases (largely sorted / reverse sorted arrays, the same edge cases where quick sort would break down).

//The tree is layed out so the super parent element is at index (treeSize / 2), and its children are at indexes (treeSize / 4) and
//	(3 * treeSize / 4). This patterncontinues down the tree until the bottom level. All bottom level elements are in odd indexes, and the
//	non-bottom level elements fill in the evens. Elements that overflow fall into overflow arrays with identifiers of either 1 less than
//	the position of overflow (if it was less than that element) or the position of overflow (if it was greter than or equal to. For
//	example, if an element overflowed at bottom level index 3, it would be placed in overflow array 2 or 3

//In this implementation the overflow arrays are indiscriminately sorted using insertion sort. Although the overflow arrays can
//	be up to n - log(n) in size, realistically they are usually pretty small and insertion sort is efficient

//This implementation uses -1 as a placeholder for indexes in the tree yet to be filled, so any array that may contain the value -1
//	would need to modify the function to work correctly
********************************************************************************************************/

//TODO:
//Separate memory allocation / deallocation from the sort


struct overflowPlaceholder{
	int val;
	int loc;
};
struct overflowContainer{
	int count;
	int index;
	int* array;
};

void treeSort(int *array, int arrayLength);
void insertionSort(int arr[], int n);
void placeInArray(int* array, int* tree, int treeSize, overflowContainer* overflowContainers, const int invalidInt);


//Returns the first integer exponent of 2 that is greater than arrayLength
int getTreeSize(int arrayLength){
	if(arrayLength <= 0)
		return -1;
	
	//Set all bits from the most significant bit to the end of the int to 1
	arrayLength |= arrayLength >> 1; 
    arrayLength |= arrayLength >> 2; 
    arrayLength |= arrayLength >> 4; 
    arrayLength |= arrayLength >> 8; 
    arrayLength |= arrayLength >> 16;
	
	//Return the value of the int with only the most significant bit + 1 set
	return arrayLength + 1;
}


void treeSort(int *array, int arrayLength){
	if(arrayLength <= 1)
		return;
	
	const int invalidInt = -1;	//An int outside the domain of the array
	int treeSize = getTreeSize(arrayLength);
	int* tree = new int[treeSize];
	overflowContainer* overflowContainers = new overflowContainer[treeSize];	//the containers for all possible overflows
	int* overflowMemory = new int[treeSize];	//will hold the memory of all overflow arrays
	
	//an array of all elements that overflowed and where they overflowed
	overflowPlaceholder* overflowValues = new overflowPlaceholder[treeSize];
	int numOverflows = 0;
	
	//Initialize
	for(int i = 0;i < treeSize/2;++i){
		overflowContainers[i].count = 0;
	}
	for(int i = 0;i < treeSize;++i){
		tree[i] = invalidInt;
	}
	
	//Place the elements in the tree
	for(int i = 0;i < arrayLength;++i){
		int comparisonIndex = treeSize / 2;
		int divider = treeSize / 4;	//Because tree size is an exponent of 2, dividing by 2 will not round until 1/2 == 0
		
		while(true){
			if(tree[comparisonIndex] == invalidInt){
				tree[comparisonIndex] = array[i];
				break;
			}
			
			if(divider > 0){
				comparisonIndex = (array[i] >= tree[comparisonIndex]) ? comparisonIndex + divider : comparisonIndex - divider;
				divider /= 2;
				continue;
			}
			//else there is an overflow
			else{
				int overflowIndex = (array[i] >= tree[comparisonIndex]) ? comparisonIndex : comparisonIndex - 1;
				
				++overflowContainers[overflowIndex].count;
				overflowValues[numOverflows].val = array[i];
				overflowValues[numOverflows].loc = overflowIndex;
				++numOverflows;
				break;
			}
		}
	}
	
	
	int* memoryAssigner = overflowMemory;
	
	//assign memory locations for all overflow arrays
	for(int i = 0; i < treeSize; ++i){
		if(overflowContainers[i].count > 0){
			overflowContainers[i].array = memoryAssigner;
			memoryAssigner += overflowContainers[i].count;
			overflowContainers[i].index = 0;
		}
	}
	
	//transfer overflowing elements to their overflow arrays
	for(int i = 0; i < numOverflows; ++i){
		const int& loc = overflowValues[i].loc;
		overflowContainers[loc].array[overflowContainers[loc].index] = overflowValues[i].val;
		++overflowContainers[loc].index;
	}

	placeInArray(array, tree, treeSize, overflowContainers, invalidInt);
	
	delete[] tree;
	delete[] overflowContainers;
	delete[] overflowMemory;
	delete[] overflowValues;
}


void placeInArray(int* array, int* tree, int treeSize, overflowContainer* overflowContainers, const int invalidInt){
	int index = 0;
	
	for(int i = 0;i < treeSize;++i){
		if(tree[i] != invalidInt){
			array[index] = tree[i];
			++index;
		}
		if(overflowContainers[i].count > 0){
			insertionSort(overflowContainers[i].array, overflowContainers[i].count);	//Sort indiscriminately with insertion sort
			for(int j = 0;j < overflowContainers[i].count;++j){
				array[index] = overflowContainers[i].array[j];
				++index;
			}
		}
	}
}