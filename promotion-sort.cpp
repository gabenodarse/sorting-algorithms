//Nested array sort with promotion (promotion sort)

#include <cstring>	//memmove
#include <cmath>	


/****************************************************************
PROMOTION SORT************************************************
*****************************************************************

EXPLANATION
-Promotion sort is originally based on nested array sort. The same simple idea holds: keep move operations to a minimum by making small arrays where each
	element points to a child array that holds the elements between it and the next element in the parent.
	Example:
	{2} - First element
	{23} - Second element 3 is greater than all elements in the array, put it at the end
	{023} - Third element 0 is less than all elements in the array, put it at the beginning
	{0!23} 0:{1} -Fourth element 1 is placed in a nested array associated with the smaller of the two elements it falls between
-Promotion sort seeks to solve the problem of nested array sort where randomness caused elements to have nested arrays of vastly varying sizes. This meant
	that an element in the parent nest with few/no elements might have to be moved multiple times when it could have been an element in a child array
	where it would have been hardly moved. Elements in the parent, ideally, should have larger nested arrays than elements in further nest levels, 
	and should be evenly sized.
-Promotion solves this by constructing the parent array from children that get "promoted". When an element gets promoted, it gets placed into the parent
	array, and is assigned a nested array that holds half the elements from the array it came from. In other words, when a nested array gets too large
	it is split in half, half of it remaining where it is and half of it being assigned to a promoted element. The promoted element, therefor, is the median
	of the previously full array.

USEFULNESS
-Promotion is a somewhat expensive operation that pays dividends in the long run. 
	It happens relatively infrequently (N / array-size times, so at worst O(N) time), but ensures that each placement happens in estimated logarithmic time.
	This results in a very fast, consistently fast sorting method with a reasonable use of memory. My very rough measurements were 2-3 times slower than
	quick sort on a list of 20000 numbers. This does not account for memory allocation/deallocation, because if sorting more than just once the same 
	memory chunk can be reused. If memory allocation/deallocation is included the run time is a little bit slower
-The type of data structure that is created seems very useful, and though I haven't learned about it I wouldn't be surprised if a data structure similar to
	it is used. It allows for quick insertions, deletions and searches. It is like a linked list that is accessible anywhere along the list.
-Improvements can be made on this prototype, which I have done and explained in binary-promotion-sort

*****************************************************************/


//TODO:
//Find a way to calculate the optimal container sizes and max nest level for a given array length
//Move binary search to its own function
//Trigger promotion of an element when a container hits its end or 0 index, rather than when a container hits a certain capacity
	//This would allow the removal of acceptableFirstIndex variable
//Allow a max container size that isn't even
//Create a new header to use in place of nested-array-sort.h, and change name of function from nestedArraySort

#include "nested-array-sort.h"

class Benchmarks;
elementContainer* promote(elementContainer* parentContainer, const int& valuePosition, const int& insertedValue, const bool& isHigherThanMedian, 
							Benchmarks* bm, elementContainer*& containerAssigner, element*& elementAssigner);
							


struct elementContainer{
	element *array;
	int firstElementIndex;
	int lastElementIndex;
	int nestLevel;
	int acceptableFirstIndex;
};


struct element{
	int val;
	elementContainer *nestedContainer;
};


class Benchmarks {
	public:
	int maxElements;	//Maximum amount of elements allowed in an element container
	int halfMaxElements;
	int dblMaxElements;
	int maxNestLevel;
	Benchmarks(int arrayLength){
		halfMaxElements = log2(arrayLength);
		maxElements = 2 * halfMaxElements;	//Must be even
		dblMaxElements = 4 * halfMaxElements;
		maxNestLevel = log2(arrayLength) / floor(log2(halfMaxElements));
	}
};


//Insert an element into the elementContainer
void insertElement(int value,elementContainer* destination, Benchmarks* const bm, elementContainer*& containerAssigner, element*& elementAssigner){
	
	bool isHigherThanMedian;
	int valuePosition; //The index of the greatest value less than or equal to the value to be inserted
	int low;
	int high;
	int mid;
	
	//Loop until the lowest nest level is reached
	while(true){
		low = destination->firstElementIndex;
		high = destination->lastElementIndex;
		mid = (low+high) / 2;
		
		//Check to see if the value to insert is higher or lower than the median, while updating low / high value for binary search
		if(value >= destination->array[mid].val){
			isHigherThanMedian = true;
			low = mid + 1;
		}
		else{
			isHigherThanMedian = false;
			high = mid - 1;
		}
		
		//binary search
		while(low < high){
			mid = (low + high) / 2;
			
			if(value >= destination->array[mid].val)
				low = mid + 1;
			else
				high = mid - 1;
		}
		
		//use high to compare and assign, 
			//(either low or high would work if used in all 3 appearances, but high is used to make the first element insertion trick work)
		if(value >= destination->array[high].val)
			valuePosition = high;
		else
			valuePosition = high - 1;
		
		
		if(destination->nestLevel < bm->maxNestLevel){
			//if the nested container is at max capacity, promote
			if((destination->array[valuePosition].nestedContainer->lastElementIndex - destination->array[valuePosition].nestedContainer->firstElementIndex + 1) 
				== bm->maxElements){
					destination = promote(destination, valuePosition, value, isHigherThanMedian, bm, containerAssigner, elementAssigner);
			}
			else{
				destination = destination->array[valuePosition].nestedContainer;
			}
			continue;
		}
		else break;
	
	}//End while loop
	
	
	//if the value to be inserted is higher than the median then inserting it would move all elements greater than it 1 index higher.
	//The opposite is done if the value is lower than the median (all elements less than or equal to the value to be inserted are moved an index lower)
	if(isHigherThanMedian){
		//if a move is necessary, move elements in the array over
		if(destination->lastElementIndex - valuePosition > 0)
		{
			memmove(destination->array + valuePosition + 2,destination->array + valuePosition + 1,
				sizeof(element) * (destination->lastElementIndex - valuePosition));
		}
		
		destination->array[valuePosition + 1].val = value;
		++destination->lastElementIndex;
		
		return;
	}
	else{
		//move the elements in the array over, insert the new value, and return
		memmove(destination->array + destination->firstElementIndex - 1,destination->array + destination->firstElementIndex,
			sizeof(element) * (valuePosition - destination->firstElementIndex + 1));
		destination->array[valuePosition].val = value;
		--destination->firstElementIndex;
		return;
	}
}

//Split a nested container in half by promoting its middle element to the parent container and associating it with the second half of the nested array
elementContainer* promote(elementContainer* parentContainer, const int& valuePosition, const int& insertedValue, const bool& isHigherThanMedian, 
							Benchmarks* bm, elementContainer*& containerAssigner, element*& elementAssigner){

	element *parentElement = parentContainer->array + valuePosition;
	elementContainer* fullContainer = parentElement->nestedContainer;
	element promotedElement = fullContainer->array[fullContainer->firstElementIndex + bm->halfMaxElements];
	
	
	//if the values are in indexes too low:
		//assign the parent element's old container to the promoted element's nested container 
		//set parent element to have a new nested container, and copy the first half of elements to that new container
	//if the values are in indexes too high:
		//assign the promoted element a new nested container. Copy the second half of the elements and assign it to the new container
	//in both cases, after both the old parent element and the promoted element point to the correct nested container::
		//Update the indexes of the old nested container to discard the elements that were copied over
		//insert the new element into the parent container
	if(fullContainer->firstElementIndex <= fullContainer->acceptableFirstIndex){
		promotedElement.nestedContainer = fullContainer;
		
		//Assign parentElement a new container
		parentElement->nestedContainer = containerAssigner++;
		parentElement->nestedContainer->array = elementAssigner;
		elementAssigner += bm->dblMaxElements;
		parentElement->nestedContainer->firstElementIndex = bm->maxElements;
		parentElement->nestedContainer->lastElementIndex = bm->maxElements + bm->halfMaxElements - 1;
		parentElement->nestedContainer->acceptableFirstIndex = bm->halfMaxElements;
		parentElement->nestedContainer->nestLevel = fullContainer->nestLevel;
		
		memcpy(parentElement->nestedContainer->array + bm->maxElements,
				fullContainer->array + fullContainer->firstElementIndex,
				sizeof(element) * bm->halfMaxElements);
		
		//Update fullContainer->firstElementIndex to discard the elements that were copied to the new container
		fullContainer->firstElementIndex += bm->halfMaxElements;	
	}
	else{
		//Assign promotedElement a new container
		promotedElement.nestedContainer = containerAssigner++;
		promotedElement.nestedContainer->array = elementAssigner;
		elementAssigner += bm->dblMaxElements;
		promotedElement.nestedContainer->firstElementIndex = bm->maxElements;
		promotedElement.nestedContainer->lastElementIndex = bm->maxElements + bm->halfMaxElements - 1;
		promotedElement.nestedContainer->acceptableFirstIndex = bm->halfMaxElements;
		promotedElement.nestedContainer->nestLevel = fullContainer->nestLevel;
		
		memcpy(promotedElement.nestedContainer->array + bm->maxElements,
				fullContainer->array + fullContainer->firstElementIndex + bm->halfMaxElements,
				sizeof(element) * bm->halfMaxElements);
		
		//Update fullContainer->lastElementIndex to discard the elements that were copied to the new container
		fullContainer->lastElementIndex -= bm->halfMaxElements;
	}
	
	
	//Insert promotedElement into the non-full parent container
	if(isHigherThanMedian){
		//move the elements in the array over and insert the new value
		memmove(parentElement + 2,
			parentElement + 1,
			sizeof(element) * (parentContainer->lastElementIndex - valuePosition));
		
		parentContainer->array[valuePosition + 1] = promotedElement;
		++parentContainer->lastElementIndex;
	}
	else{
		//move the elements in the array over and insert the new value
		memmove(parentContainer->array + parentContainer->firstElementIndex - 1,
			parentContainer->array + parentContainer->firstElementIndex,
			sizeof(element) * (valuePosition - parentContainer->firstElementIndex + 1));
		parentContainer->array[valuePosition] = promotedElement;
		--parentContainer->firstElementIndex;
		--parentElement;	//The parent element was shifted, so decrement to point to the correct element
	}
	
	
	return (insertedValue >= promotedElement.val) ? 
		promotedElement.nestedContainer : 
		parentElement->nestedContainer;
}


//Extract the elements from their nested elementContainers and put them in the correct, sorted order into a standard array
void placeElementsInArray(elementContainer *source, int *array, const int& maxNestLevel, int& index){
	for(int i = source -> firstElementIndex; i <= source -> lastElementIndex; ++i){
		if(source->nestLevel == maxNestLevel){
			array[index] = source->array[i].val;
			++index;
		}
		if(source->nestLevel != maxNestLevel){
			placeElementsInArray(source->array[i].nestedContainer, array, maxNestLevel, index);
		}
	}
}


//Allocates memory for allContainers and allElements based on the array length
void allocateMemory(int arrayLength, elementContainer*& allContainers, element*& allElements){
	allContainers = new elementContainer[arrayLength];
	allElements = new element[arrayLength * 8];
}
//Deallocates memory for allContainers and allElements
void deallocateMemory(elementContainer*& allContainers, element*& allElements){
	delete[] allContainers;
	delete[] allElements;
	allContainers = nullptr;
	allElements = nullptr;
}


void nestedArraySort(int* array, int arrayLength){
	elementContainer* allContainers = nullptr;
	element* allElements = nullptr;
	allocateMemory(arrayLength,allContainers,allElements);
	nestedArraySort(array, arrayLength, allContainers, allElements);
	deallocateMemory(allContainers,allElements);
}


void nestedArraySort(int *array, int arrayLength, elementContainer* containerAssigner, element* elementAssigner){
	if(arrayLength <= 1)
		return;
	
	Benchmarks bm(arrayLength);
	
	//Assign parent elementContainer
	elementContainer* parent = containerAssigner++;
	
	//arrays of elements are sized at 2 times the array length, because the starting element is in the middle, and the length of the array can span
	//n elements to the end (each element higher than the previous) or n elements to the beginning (each element lower than the previous)
	parent->array = elementAssigner; 
	elementAssigner += bm.dblMaxElements;	//Subsequent arrays will be assigned at elementAssigner so that two arrays never overlap
	parent->array[bm.maxElements].val = -99999;//Add the initial element
	
	//Assign container values
	parent->firstElementIndex = bm.maxElements;	//halfway between the beginning and end of parent->array
	parent->lastElementIndex = bm.maxElements;
	parent->acceptableFirstIndex = bm.halfMaxElements;
	parent->nestLevel = 0;
	
	
	//Assign the initial element containers
	elementContainer* initAssigner = parent;
	for(int i = 1;i <= bm.maxNestLevel;++i){
		
		//Create nested container
		initAssigner->array[bm.maxElements].nestedContainer = containerAssigner++;
				
		//Move down to the nested container
		initAssigner = initAssigner->array[bm.maxElements].nestedContainer;
		initAssigner->array = elementAssigner;
		elementAssigner += bm.dblMaxElements;
		initAssigner->array[bm.maxElements].val = -99999;
		
		//Assign remaining container values
		initAssigner->firstElementIndex = bm.maxElements;
		initAssigner->lastElementIndex = bm.maxElements;
		initAssigner->acceptableFirstIndex = bm.halfMaxElements;
		initAssigner->nestLevel = i;
	}
	
	//Trick to remove minimum value from the bottom nest level. 
	//When the first value gets inserted, it gets inserted at lastElementIndex + 1, the firstElementIndex, and lastElementIndex gets incremented and matches
	--initAssigner->lastElementIndex;
	initAssigner->array[initAssigner->lastElementIndex].val = -99999;
	
	
	for(int i = 0;i < arrayLength;++i){
		insertElement(array[i],parent,&bm,containerAssigner,elementAssigner);
	}
	
	int index = 0;
	placeElementsInArray(parent, array, bm.maxNestLevel, index);
}

