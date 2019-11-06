
#ifndef nested_array_sort
#define nested_array_sort

struct elementContainer; struct element;


//Return the maximum amount of element moves allowed for the insertion of an element
int maximumElementMoves();

//Insert an element into the elementContainer
void insertElement(int value,elementContainer* const destination,const int &remainingUnsortedElements,elementContainer* const allContainers, element* const allElements);

//Extract the elements from their nested elementContainers and put them in the correct, sorted order into a standard array
void placeElementsInArray(elementContainer *source, int *array);

//Perform nestedArraySort with internal allocation/deallocation of memory
void nestedArraySort(int *array, int arrayLength);
//Perform nestedArraySort with external allocation/deallocation of memory
void nestedArraySort(int *array, int arrayLength, elementContainer* const allContainers, element* const allElements);

//Allocates memory for allContainers and allElements based on the array length
void allocateMemory(int arrayLength, elementContainer*& allContainers, element*& allElements);
//Deallocates memory for allContainers and allElements
void deallocateMemory(elementContainer*& allContainers, element*& allElements);

#endif