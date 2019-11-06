//Nested array sort

#include <cstring>


//#define DIAGNOSTICS
#ifdef DIAGNOSTICS
#include <iostream>
#endif


/****************************************************************
-The idea behind nested array sort is to put elements in order with few move operations. This is accomplished by creating an array, which will be populated
with sorted elements, and only adding additional elements if there are few prior elements that will have to be moved. If too many prior elements would
have to be moved, the element will be placed into a nested array associated with its position in the parent array
-Elements can be moved higher or lower in a sorted array to create space for the new inserted element, so as long as the new element is close to the 
beginning or end of the sorted array, it can be inserted.
-When an additional element is added but does not fit the criteria for including it in the array, it falls down to a nested array. This nested array is 
associated with the smaller of the two elements in the parent array it falls between.

Example:
{2} - First element
{23} - Second element 3 is greater than all elements in the array, put it at the end
{023} - Third element 0 is less than all elements in the array, put it at the beginning
{0!23} 0:{1} - Inserting fourth element 1 would require move operations, so put it in a nested array associated with the smaller of the two elements it falls 
between

When all elements have been sorted, the full, sorted array can be constructed by starting at the first element of the parent array and copying the elements to 
an array. After copying each element, check to see if the element has a nested array associated with it, as that nested array will contain values between it 
and the next element in the parent

This sorting method can get very memory intensive in trade for speed (O(N^2))
*****************************************************************/


//TODO:
//Determine optimized formulas for determining whether to insert an element or send it into a nested array. 
//	Square root of the amount of elements to be sorted seems good. Can also make it a function of how many elements are left to sort 
//	(if there are a lot of elements that remain to be sorted, moving multiple elements may pay dividends in the long run because subsequent searches
//	will be searching through more elements in higher parent array rather than searching through many small nested arrays.)
//Assign memory outside the program and have it passed in as a variable, so successive sorts won't have to constantly assign memory. This should
//	significantly help speed, as memory allocation seems to take a significant amount of the sorting algorithms time.
//Try to consider/formulate ways to cut down on memory cost. Memory use becomes highly fragmented due to the worst-case-scenario memory allocation
//Move binary search to its own function


int numberOfElements = 0;//Keep track of how much memory is used
int numberOfArrays = 0;//Keep track of the total amount of arrays

	
struct elementContainer; struct element;
//elementContainer data struct contains values of beginning index and ending index, and an array for all the elements it contains
struct elementContainer{
	element *array;
	int firstElementIndex;
	int lastElementIndex;
};


//Element data struct contains the value of the element and the nested array that may be associated with it (nullptr if none is)
struct element{
	int val;
	elementContainer *nestedArray;
};


//Returns the maximum number of elements to move when inserting an element. If the number of moves exceeds this, drop the element into a nested array.
int maximumElementMoves(){
	return 100;
}


//Insert an element into the array, using remainingUnsortedElements to determine whether it's worth it to move elements or to create a nested array
void insertElement(int value,elementContainer* destination,const int &remainingUnsortedElements,elementContainer* const allContainers, element* const allElements){
	
	bool isHigherThanMedian;
	int valuePosition; //The index of the greatest value less than or equal to the value to be inserted
	int low;
	int high;
	int mid;
	
	
	//while the element has yet to be inserted, compare it with the elements in the destination elementContainer
	while(true){
	
	
	low = destination->firstElementIndex;
	high = destination->lastElementIndex;
	mid = (low+high) / 2;
	
	//Check to see if the value to insert is higher or lower than the median, while updating low / high value for binary search
	mid = (low + high) / 2;
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
	
	
	//high == low at this point. This index is either the valuePosition or 1 higher than the valuePosition
	if(value >= destination->array[high].val)
		valuePosition = high;
	else
		valuePosition = high - 1;
	
	//if the value to be inserted is higher than the median then inserting it would move all elements greater than it 1 index higher.
	//The opposite is done if the value is lower than the median (all elements less than or equal to the value to be inserted are moved an index lower)
	if(isHigherThanMedian){
		
		//if the value is close enough to the edge of the array, move the elements in the array to insert the element.
		//else insert the element into a nested array
		if(destination->lastElementIndex - valuePosition <= maximumElementMoves()){
			//move the elements in the array over, insert the new value, and return
			memmove(destination->array + valuePosition + 2,destination->array + valuePosition + 1,
				sizeof(element) * (destination->lastElementIndex - valuePosition));
			destination->array[valuePosition + 1].val = value;
			destination->array[valuePosition + 1].nestedArray = nullptr;
			++destination->lastElementIndex;
			return;
		}
		else{
			//if a nested array already exists
			if(destination->array[valuePosition].nestedArray != nullptr){
				destination = destination->array[valuePosition].nestedArray;
				continue;
			}
			//else create a nested array and assign it memory
			else{
				destination->array[valuePosition].nestedArray = allContainers + numberOfArrays;
				++numberOfArrays;
				
				destination->array[valuePosition].nestedArray->array = allElements + numberOfElements;
				numberOfElements += 2*remainingUnsortedElements;
				destination->array[valuePosition].nestedArray->firstElementIndex = remainingUnsortedElements - 1;
				destination->array[valuePosition].nestedArray->lastElementIndex = destination->array[valuePosition].nestedArray->firstElementIndex;
				destination->array[valuePosition].nestedArray->array[destination->array[valuePosition].nestedArray->firstElementIndex].val = value;
				destination->array[valuePosition].nestedArray->array[destination->array[valuePosition].nestedArray->firstElementIndex].nestedArray = nullptr;
				return;
			}
		}
		
	}
	else{
		
		//if the value is close enough to the edge of the array, move the elements in the array to insert the element.
		//else insert the element into a nested elementContainer
		if(valuePosition - destination->firstElementIndex <= maximumElementMoves()){
			//move the elements in the array over, insert the new value, and return
			memmove(destination->array + destination->firstElementIndex - 1,destination->array + destination->firstElementIndex,
				sizeof(element) * (valuePosition - destination->firstElementIndex + 1));
			destination->array[valuePosition].val = value;
			destination->array[valuePosition].nestedArray = nullptr;
			--destination->firstElementIndex;
			return;
		}
		else{
			//if a nested elementContainer already exists
			if(destination->array[valuePosition].nestedArray != nullptr){
				destination = destination->array[valuePosition].nestedArray;
				continue;
			}
			//else create a nested elementContainer and assign it memory
			else{
				destination->array[valuePosition].nestedArray = allContainers + numberOfArrays;
				++numberOfArrays;
				
				destination->array[valuePosition].nestedArray->array = allElements + numberOfElements;
				numberOfElements += 2*remainingUnsortedElements;
				destination->array[valuePosition].nestedArray->firstElementIndex = remainingUnsortedElements - 1;
				destination->array[valuePosition].nestedArray->lastElementIndex = destination->array[valuePosition].nestedArray->firstElementIndex;
				destination->array[valuePosition].nestedArray->array[destination->array[valuePosition].nestedArray->firstElementIndex].val = value;
				destination->array[valuePosition].nestedArray->array[destination->array[valuePosition].nestedArray->firstElementIndex].nestedArray = nullptr;
				return;
			}
		}
	}
	
	
	}//End while the element has yet to be inserted
}


//Extract the elements from their nested elementContainers and put them in the correct, sorted order into a standard array
void placeElementsInArray(elementContainer *source, int *array){
	static int index = 0;
	for(int i = source -> firstElementIndex; i <= source -> lastElementIndex; ++i){
		array[index] = source->array[i].val;
		++index;
		if(source->array[i].nestedArray != nullptr){
			placeElementsInArray(source->array[i].nestedArray, array);
		}
	}
}


void nestedArraySort(int *array, int arrayLength){
	if(arrayLength == 0)
		return;
	
	//Allocate memory for all elementContainers and elements that may be required
	elementContainer *allContainers = new elementContainer[5000];
	element *allElements = new element[70'000'000];
	
	//Assign parent elementContainer
	elementContainer *parent = allContainers;
	++numberOfArrays;
	
	//Assign the parent elementContainer's array
	parent->array = allElements; //2 times the array length, because the starting element is in the middle, and hypothetically the length of the array can span n elements to the end (each element higher than the previous) or n elements to the beginning (each element lower than the previous)
	numberOfElements += 2*arrayLength;	//Subsequent arrays will be assigned at allElements[numberOfElements] so that two arrays never overlap
	//Add the initial element
	parent->firstElementIndex = arrayLength - 1;	//halfway between the beginning and end of parent->array
	parent->lastElementIndex = parent->firstElementIndex;
	parent->array[parent->firstElementIndex].val = array[0];
	parent->array[parent->firstElementIndex].nestedArray = nullptr;
	
	//For each element
	int remainingUnsortedElements = arrayLength - 1;	//Number of elements minus the initial element
	for(int i = 1;i < arrayLength;++i){
		insertElement(array[i],parent,remainingUnsortedElements,allContainers,allElements);
		--remainingUnsortedElements;
	}
	
	placeElementsInArray(parent, array);
	
	#ifdef DIAGNOSTICS
	std::cout << "Number of elements in memory: " << numberOfElements << "\n";
	std::cout << "Number of elementContainers in memory: " << numberOfArrays << "\n";
	std::cout << "Size of elementContainer: " << sizeof(elementContainer) << ", size of element: " << sizeof(element) << "\nFor a total memory usage of: " << 
	sizeof(elementContainer) * numberOfArrays + sizeof(element) * numberOfElements << "\n";
	#endif
	
	delete[] allContainers;
	delete[] allElements;
}