//Nested set sort

#include <cstring>


//#define DIAGNOSTICS
#ifdef DIAGNOSTICS
#include <iostream>
#endif


/****************************************************************
-The idea behind nested set sort is to put elements in order with few move operations. This is accomplished by creating a set, which will be populated
with sorted elements, and only adding additional elements if there are few prior elements that will have to be moved. If too many prior elements would
have to be moved, the element will be placed into a nested set associated with its position in the parent set
-Elements can be moved higher or lower in a sorted set to create space for the new inserted element, so as long as the new element is close to the 
beginning or end of the sorted set, it can be inserted.
-When an additional element is added but does not fit the criteria for including it in the set, it falls down to a nested set. This nested set is associated
with the smaller of the two elements in the parent set it falls between.

Example:
{2} - First element
{23} - Second element 3 is greater than all elements in the set, put it at the end
{023} - Third element 0 is less than all elements in the set, put it at the beginning
{0!23} 0:{1} - Inserting fourth element 1 would require move operations, so put it in a nested set associated with the smaller of the two elements it falls 
between

When all elements have been sorted, the full, sorted array can be constructed by starting at the first element of the parent set and copying the elements to 
an array. After copying each element, check to see if the element has a nested array associated with it, as that nested array will contain values between it 
and the next element in the parent

This sorting method can get very memory intensive in trade for speed
*****************************************************************/


//TODO:
//Determine optimized formulas for determining whether to insert an element or send it into a nested set. 
//	Square root of the amount of elements to be sorted seems good. Can also make it a function of how many elements are left to sort 
//	(if there are a lot of elements that remain to be sorted, moving multiple elements may pay dividends in the long run because subsequent searches
//	will be searching through more elements in higher parent sets rather than searching through many small nested lists.
//Assign memory outside the program and have it passed in as a variable, so successive sorts won't have to constantly assign memory. This should
//	significantly help speed, as memory allocation seems to take a significant amount of the sorting algorithms time.
//Try to consider/formulate ways to cut down on memory cost. Memory use becomes highly fragmented due to the worst-case-scenario memory allocation


int numberOfElements = 0;//Keep track of how much memory is used
int numberOfSets = 0;//Keep track of the total amount of sets

	
struct set; struct element;
//Set data struct contains values of beginning index and ending index, and an array to all the elements it contains
struct set{
	element *array;
	int firstElementIndex;
	int lastElementIndex;
};


//Element data struct contains the value of the element and the nested array that may be associated with it (nullptr if none is)
struct element{
	int val;
	set *nestedSet;
};


//Returns the maximum number of elements to move when inserting an element. If the number of moves exceeds this, drop the element into a nested set.
int maximumElementMoves(){
	return 20000;
}


//Insert an element into the set, using remainingUnsortedElements to determine whether it's worth it to move elements or to create a nested set
void insertElement(int value,set* const insertionSet,const int &remainingUnsortedElements,set* const allSets, element* const allElements){
	
	bool isHigherThanMedian;
	int valuePosition; //The index of the greatest value less than or equal to the value to be inserted
	int low = insertionSet->firstElementIndex;
	int high = insertionSet->lastElementIndex;
	int mid = (low+high) / 2;
	
	
	//Check to see if the value to insert is higher or lower than the median, while updating low / high value for binary search
	mid = (low + high) / 2;
	if(value >= insertionSet->array[mid].val){
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
		
		if(value >= insertionSet->array[mid].val)
			low = mid + 1;
		else
			high = mid - 1;
	}
	
	
	//high == low at this point. This index is either the valuePosition or 1 higher than the valuePosition
	if(value >= insertionSet->array[high].val)
		valuePosition = high;
	else
		valuePosition = high - 1;
	
	
	//if the value to be inserted is higher than the median then inserting it would move all elements greater than it 1 index higher.
	//The opposite is done if the value is lower than the median (all elements less than or equal to the value to be inserted are moved an index lower)
	if(isHigherThanMedian){
		
		//if the value is close enough to the edge of the array, move the elements in the array to insert the element.
		//else insert the element into a nested set
		if(insertionSet->lastElementIndex - valuePosition <= maximumElementMoves()){
			//move the elements in the array over, insert the new value, and return
			memmove(insertionSet->array + valuePosition + 2,insertionSet->array + valuePosition + 1,
				sizeof(element) * (insertionSet->lastElementIndex - valuePosition));
			insertionSet->array[valuePosition + 1].val = value;
			insertionSet->array[valuePosition + 1].nestedSet = nullptr;
			++insertionSet->lastElementIndex;
			return;
		}
		else{
			//if a nested set already exists
			if(insertionSet->array[valuePosition].nestedSet != nullptr){
				insertElement(value,insertionSet->array[valuePosition].nestedSet,remainingUnsortedElements,allSets,allElements);
			}
			//else create a nested set and assign it memory
			else{
				insertionSet->array[valuePosition].nestedSet = allSets + numberOfSets;
				++numberOfSets;
				
				insertionSet->array[valuePosition].nestedSet->array = allElements + numberOfElements;
				numberOfElements += 2*remainingUnsortedElements;
				insertionSet->array[valuePosition].nestedSet->firstElementIndex = remainingUnsortedElements - 1;
				insertionSet->array[valuePosition].nestedSet->lastElementIndex = insertionSet->array[valuePosition].nestedSet->firstElementIndex;
				insertionSet->array[valuePosition].nestedSet->array[insertionSet->array[valuePosition].nestedSet->firstElementIndex].val = value;
				insertionSet->array[valuePosition].nestedSet->array[insertionSet->array[valuePosition].nestedSet->firstElementIndex].nestedSet = nullptr;
			}
		}
		
	}
	else{
		
		//if the value is close enough to the edge of the array, move the elements in the array to insert the element.
		//else insert the element into a nested set
		if(valuePosition - insertionSet->firstElementIndex <= maximumElementMoves()){
			//move the elements in the array over, insert the new value, and return
			memmove(insertionSet->array + insertionSet->firstElementIndex - 1,insertionSet->array + insertionSet->firstElementIndex,
				sizeof(element) * (valuePosition - insertionSet->firstElementIndex + 1));
			insertionSet->array[valuePosition].val = value;
			insertionSet->array[valuePosition].nestedSet = nullptr;
			--insertionSet->firstElementIndex;
			return;
		}
		else{
			//if a nested set already exists
			if(insertionSet->array[valuePosition].nestedSet != nullptr){
				insertElement(value,insertionSet->array[valuePosition].nestedSet,remainingUnsortedElements,allSets,allElements);
			}
			//else create a nested set and assign it memory
			else{
				insertionSet->array[valuePosition].nestedSet = allSets + numberOfSets;
				++numberOfSets;
				
				insertionSet->array[valuePosition].nestedSet->array = allElements + numberOfElements;
				numberOfElements += 2*remainingUnsortedElements;
				insertionSet->array[valuePosition].nestedSet->firstElementIndex = remainingUnsortedElements - 1;
				insertionSet->array[valuePosition].nestedSet->lastElementIndex = insertionSet->array[valuePosition].nestedSet->firstElementIndex;
				insertionSet->array[valuePosition].nestedSet->array[insertionSet->array[valuePosition].nestedSet->firstElementIndex].val = value;
				insertionSet->array[valuePosition].nestedSet->array[insertionSet->array[valuePosition].nestedSet->firstElementIndex].nestedSet = nullptr;
			}
		}
	}
	
	
	//If the element has been assigned by now, the function has returned
}


//Extract the elements from their nested sets and put them in the correct, sorted order into the array
void placeElementsInArray(set *parentSet, int *array){
	static int index = 0;
	for(int i = parentSet -> firstElementIndex; i <= parentSet -> lastElementIndex; ++i){
		array[index] = parentSet->array[i].val;
		++index;
		if(parentSet->array[i].nestedSet != nullptr){
			placeElementsInArray(parentSet->array[i].nestedSet, array);
		}
	}
}




void nestedSetSort(int *array, int arrayLength){
	if(arrayLength == 0)
		return;
	
	//Allocate memory for all sets and elements that may be required
	set *allSets = new set[5000];
	element *allElements = new element[70'000'000];
	
	//Assign parent set
	set *parentSet = allSets;
	++numberOfSets;
	
	//Assign the parent set's array
	parentSet->array = allElements; //2 times the array length, because the starting element is in the middle, and hypothetically the length of the array can span n elements to the end (each element higher than the previous) or n elements to the beginning (each element lower than the previous)
	numberOfElements += 2*arrayLength;	//Subsequent arrays will be assigned at allElements[numberOfElements] so that two arrays never overlap
	//Add the initial element
	parentSet->firstElementIndex = arrayLength - 1;	//halfway between the beginning and end of parentSet->array
	parentSet->lastElementIndex = parentSet->firstElementIndex;
	parentSet->array[parentSet->firstElementIndex].val = array[0];
	parentSet->array[parentSet->firstElementIndex].nestedSet = nullptr;
	
	//For each element
	int remainingUnsortedElements = arrayLength - 1;	//Number of elements that remain to be sorted
	for(int i = 1;i < arrayLength;++i){
		insertElement(array[i],parentSet,remainingUnsortedElements,allSets,allElements);
		--remainingUnsortedElements;
	}
	
	placeElementsInArray(parentSet, array);
	
	#ifdef DIAGNOSTICS
	std::cout << "Number of elements in memory: " << numberOfElements << "\n";//>:<
	std::cout << "Number of sets in memory: " << numberOfSets << "\n";//>:<
	std::cout << "Size of set: " << sizeof(set) << ", size of element: " << sizeof(element) << "\nFor a total memory usage of: " << 
	sizeof(set) * numberOfSets + sizeof(element) * numberOfElements << "\n";
	#endif
	
	delete[] allSets;
	delete[] allElements;
}