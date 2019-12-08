//Pyramid sort

#include <cmath>


/****************************************************************
PYRAMID SORT*****************************************************
*****************************************************************
Pyramid sort is based on promotion sort: Build a tree of nested arrays, where the nested arrays hold all elements that fall between its parent element
	and the next element in the parent.
	Example:
	{03} - An array with no nested arrays
	{0!3} 0:{1} - An array with a 1 nested in an array associated with the element 0 - more generally, nested arrays are associated with the greatest element 
	less than to all elements contained in the array.

There is a small conceptual difference between pyramid sort and promotion sort and a few optimization differences that have been implemented here that
	have yet to be applied to promotion sort. 
	-The conceptual difference is that the containers in pyramid sort always hold either 1 or 2 elements (thus forming
	a tight, evenly distributed tree, or, as I am tentatively calling it, a pyramid). The reason for this is that upon evaluating promotion sort I found
	out that the binary search evaluated on each nested array was a bottleneck. By limiting the amount of elements to 1 or 2, no binary search is required:
	only a single comparison to the first element is required, and in cases where the comparison is >=, a comparison to the second element if it exists.
	-In pyramid sort, each container has a nested array of all elements that fall before the first element. Promotion sort uses a worse solution to handling
	elements that fall before any values in the parent that involves absolute minimums and duplicating elements.
	-Checks for promotion happen once the bottom nest level is reached rather than at each nest level while searching down. This means that a promotion
	can cause successive promotions, but less checks for promotions have to happen in total.
	-In pyramid sort, memory is allocated in a more localized way to prevent cache misses, whereas in promotion sort memory location depends on when 
	the allocation happens
	
Further possible optimizations:
	-Eliminating binary search and checks for promotions on each nest level effectively reduced the amount of linearithmic scaling components to a minimum.
	The only other thing that may be possible to remove would be the while loop inside findInsertLoc(). This would be possible if I knew a way to generate
	functions during runtime, because the while loop executes a set amount of times based on how many levels up the superParent is. So instead of checking
	if the bottom nest level is reached every time, a new function could be created every time superParentPromote() is called that executed the contents
	of the while loop 1 extra time.
	-Using std::move rather than standard copy assignment on the operations in promote() and insertValue() tht happen O(n) times
	-For small arrays (probably under 10000 elements or so), the high cost of promotion would not be worth doing as often as happens in pyramid sort. It 
	would be beneficial in those cases if each container held 2-4 elements instead of 1-2.
	-For data entries that aren't presumed to be random / of indeterminite order, promotion sort with the optimizations incorporated in this file would
	probably be a faster data structure. Inserting many contiguous elements at once would be faster if array sizes were larger. An example I can think of
	would be a text editor adding elements somewhere in the middle of tens of thousands of chararacters of text: creating a few new nested arrays at the
	correct location to hold an insertion of 100s of characters would be much more efficient than inserting them 1 at a time in the pyramid. Deletions 
	on multiple contiguous elements should also be faster with larger nested arrays.

*****************************************************************/


struct element; struct elementContainer;

//Element data struct contains the value of the element and the nested array that is associated with it
struct element{
	int val;
	elementContainer *nestedContainer;
};


//elementContainer contains up to two elements, and the associated parent and nested containers
struct elementContainer{
	elementContainer* nestedContainer;
	elementContainer* parentContainer;
	element element0;
	element element1;
	bool hasTwoElements;
};


elementContainer* promote(elementContainer* parent, int promotedValue, elementContainer*& superParent, int level,
							elementContainer** containerAssigner);

elementContainer* superParentPromote(int promotedValue, elementContainer*& superParent, int level, 
							elementContainer** containerAssigner);
							
void insertValue(int value, elementContainer* destination, elementContainer*& parent, elementContainer** containerAssigner);


//Find the location where an element should be inserted
elementContainer* findInsertLoc(int value, elementContainer* destination){
	
	//Loop until the lowest nest level is reached
	while(destination->nestedContainer != nullptr){
		
		if(value >= destination->element0.val){
			if(!destination->hasTwoElements || value < destination->element1.val){
				//destination->elements[0].nestedContainer
				destination = destination->element0.nestedContainer;
				continue;
			}
			else{
				//destination->elements[1].nestedContainer
				destination = destination->element1.nestedContainer;
				continue;
			}
		}
		else{
			//destination->nestedContainer
			destination = destination->nestedContainer;
			continue;
		}
	
	}//End while loop

	return destination;
}

//Insert a value into the bottom level
void insertValue(int value, elementContainer* destination, elementContainer*& superParent, elementContainer** containerAssigner){
	
	//if the destination is full, promote
	if(destination->hasTwoElements){
		destination->hasTwoElements = false;
		elementContainer* newContainer;	//The container of the promoted element
		
		//promote the second highest value, assign the highest value as the element in the new container
		if(value >= destination->element0.val){
			if(value >= destination->element1.val){
				newContainer = promote(destination->parentContainer, destination->element1.val, superParent, 0, containerAssigner);
				
				//assign values to the new container's members, excluding parentContainer
				newContainer->hasTwoElements = false;
				
				newContainer->nestedContainer = nullptr;	//unnecessary to also set newContainer->elements[0].nestedContainer to nullptr
				newContainer->element0.val = value;
			}
			else{
				newContainer = promote(destination->parentContainer, value, superParent, 0, containerAssigner);
				
				//assign values to the new container's members, excluding parentContainer
				newContainer->hasTwoElements = false;
				
				newContainer->nestedContainer = nullptr;	//unnecessary to also set newContainer->elements[0].nestedContainer to nullptr
				newContainer->element0.val = destination->element1.val;
			}
		}
		else{
			newContainer = promote(destination->parentContainer, destination->element0.val, superParent, 0, containerAssigner);
			
			//assign values to the new container's members, excluding parentContainer
			newContainer->hasTwoElements = false;
			
			newContainer->nestedContainer = nullptr;	//unnecessary to also set newContainer->elements[0].nestedContainer to nullptr
			newContainer->element0.val = destination->element1.val;
			
			destination->element0.val = value;	//The old element at index 0 was promoted
		}
	}
	
	//else no promotion, just insert
	else{
		if(value >= destination->element0.val){
			destination->element1.val = value;
		}
		else{
			destination->element1 = destination->element0;
			destination->element0.val = value;
		}
		destination->hasTwoElements = true;
	}
}


//Promote a value to its parent container, and assign the promoted value a new nested container.
//Return that nested container to the calling function to assign values to its own element and nested containers.
//All values to the new container are assigned by the calling function, to avoid the need to check for the presence of nested arrays 
	//(when insertValue calls a promotion it doesn't have nested containers which would otherwise be accessed). Only the parent has to be assigned here.
elementContainer* promote(elementContainer* parent, int promotedValue, elementContainer*& superParent, int level,
							elementContainer** containerAssigner){
	
	//Trigger another promotion
	if(parent->hasTwoElements){
		parent->hasTwoElements = false;
		elementContainer* newContainer;	//The new container of the extra triggered promotion 
											//(not to be confused with the container this function will create and return)
		
		//when there is no parent, super parent promote
		if(parent->parentContainer == nullptr){
			
			//promote the second highest value to the new superparent, assign the highest value as the element in the new container
			if(promotedValue >= parent->element0.val){
				if(promotedValue >= parent->element1.val){
					newContainer = superParentPromote(parent->element1.val, superParent, level + 1, containerAssigner);
					
					//assign values to the new container's members, excluding parentContainer
					newContainer->hasTwoElements = false;
					
					newContainer->nestedContainer = parent->element1.nestedContainer;	//the promoted element's old container
					newContainer->nestedContainer->parentContainer = newContainer;
					newContainer->element0.val = promotedValue;
					
					//create the new container that will be returned, and assign it the correct parent
					newContainer->element0.nestedContainer = containerAssigner[level]++;
					newContainer->element0.nestedContainer->parentContainer = newContainer;
					
					return newContainer->element0.nestedContainer;
				}
				else{
					newContainer = superParentPromote(promotedValue, superParent, level + 1, containerAssigner);
					
					//assign values to the new container's members, excluding parentContainer
					newContainer->hasTwoElements = false;
					
					newContainer->element0 = parent->element1;
					newContainer->element0.nestedContainer->parentContainer = newContainer;
					
					//create the new container that will be returned, and assign it the correct parent
					newContainer->nestedContainer = containerAssigner[level]++;
					newContainer->nestedContainer->parentContainer = newContainer;
					
					return newContainer->nestedContainer;
				}
			}
			else{
				newContainer = superParentPromote(parent->element0.val, superParent, level + 1, containerAssigner);
				
				//assign values to the new container's members, excluding parentContainer
				newContainer->hasTwoElements = false;
				
				newContainer->nestedContainer = parent->element0.nestedContainer;	//the promoted element's old container
				newContainer->nestedContainer->parentContainer = newContainer;
				newContainer->element0 = parent->element1;
				newContainer->element0.nestedContainer->parentContainer = newContainer;
				
				parent->element0.val = promotedValue;	//The old element at index 0 was promoted
				
				//create the new container that will be returned, and assign it the correct parent
				parent->element0.nestedContainer = containerAssigner[level]++;
				parent->element0.nestedContainer->parentContainer = parent;
				
				return parent->element0.nestedContainer;
			}
		}
	
		else{
			
			//promote the second highest value, assign the highest value as the element in the new container
			if(promotedValue >= parent->element0.val){
				if(promotedValue >= parent->element1.val){
					newContainer = promote(parent->parentContainer, parent->element1.val, superParent, level + 1, containerAssigner);
					
					//assign values to the new container's members, excluding parentContainer
					newContainer->hasTwoElements = false;
					
					newContainer->nestedContainer = parent->element1.nestedContainer;	//the promoted element's old container
					newContainer->nestedContainer->parentContainer = newContainer;
					newContainer->element0.val = promotedValue;	//assign values to the original value that was being promoted 
					
					//create the new container that will be returned, and assign it the correct parent
					newContainer->element0.nestedContainer = containerAssigner[level]++;
					newContainer->element0.nestedContainer->parentContainer = newContainer;
					
					return newContainer->element0.nestedContainer;
				}
				else{
					newContainer = promote(parent->parentContainer, promotedValue, superParent, level + 1, containerAssigner);
					
					//assign values to the new container's members, excluding parentContainer
					newContainer->hasTwoElements = false;
				
					newContainer->element0 = parent->element1;
					newContainer->element0.nestedContainer->parentContainer = newContainer;
					
					//create the new container that will be returned, and assign it the correct parent
					newContainer->nestedContainer = containerAssigner[level]++;
					newContainer->nestedContainer->parentContainer = newContainer;
					
					return newContainer->nestedContainer;
				}
			}
			else{
				newContainer = promote(parent->parentContainer, parent->element0.val, superParent, level + 1, containerAssigner);
				
				//assign values to the new container's members, excluding parentContainer
				newContainer->hasTwoElements = false;
				
				newContainer->nestedContainer = parent->element0.nestedContainer;	//the promoted element's old container
				newContainer->nestedContainer->parentContainer = newContainer;
				newContainer->element0 = parent->element1;
				newContainer->element0.nestedContainer->parentContainer = newContainer;
					
				//assign promoted element to the parent and assign it a new nested container
				parent->element0.val = promotedValue;	//The old element at index 0 was promoted
				
				//create the new container that will be returned, and assign it the correct parent
				parent->element0.nestedContainer = containerAssigner[level]++;
				parent->element0.nestedContainer->parentContainer = parent;
				
				return parent->element0.nestedContainer;
			}
		}
	}
	
	//else no extra triggered promotions, simply assign and return
	else{
		parent->hasTwoElements = true;	//number of elements in parent will go up by 1
		
		if(promotedValue >= parent->element0.val){
			parent->element1.val = promotedValue;
			
			//create the new container that will be returned, and assign it the correct parent
			parent->element1.nestedContainer = containerAssigner[level]++;	
			parent->element1.nestedContainer->parentContainer = parent;
			
			return parent->element1.nestedContainer;
		}
		else{
			parent->element1 = parent->element0;	//move the existing element over
			parent->element0.val = promotedValue;
			
			//create the new container that will be returned, and assign it the correct parent
			parent->element0.nestedContainer = containerAssigner[level]++;
			parent->element0.nestedContainer->parentContainer = parent;
			
			return parent->element0.nestedContainer;
		}
	}
	
}


//Promote an element from the super parent, creating a new super parent on top of regular promotion
elementContainer* superParentPromote(int promotedValue, elementContainer*& superParent, int level,
							elementContainer** containerAssigner){
	
	//Create the new super parent
	elementContainer* newSuperParent = containerAssigner[level+1]++;
	newSuperParent->hasTwoElements = false;
	newSuperParent->parentContainer = nullptr;
	newSuperParent->nestedContainer = superParent;
	
	superParent->parentContainer = newSuperParent;
	
	newSuperParent->element0.val = promotedValue;
	
	//create the new container that will be returned, and assign it the correct parent
	newSuperParent->element0.nestedContainer = containerAssigner[level]++;
	newSuperParent->element0.nestedContainer->parentContainer = newSuperParent;
	
	superParent = newSuperParent;
	return newSuperParent->element0.nestedContainer;
}


//Extract the elements from their nested elementContainers and put them in the correct, sorted order into a standard array
void placeElementsInArray(elementContainer* source, int* array, int& index){
	
	if(source->nestedContainer != nullptr){
		placeElementsInArray(source->nestedContainer, array, index);
		
		array[index] = source->element0.val;
		++index;
		placeElementsInArray(source->element0.nestedContainer, array, index);
		
		if(source->hasTwoElements){
			array[index] = source->element1.val;
			++index;
			placeElementsInArray(source->element1.nestedContainer, array, index);
		}
	}
	else{
		array[index] = source->element0.val;
		++index;
		
		if(source->hasTwoElements){
			array[index] = source->element1.val;
			++index;
		}
	}
	
}


//Allocates memory for allContainers and allElements based on the array length
void allocateMemory(int arrayLength, elementContainer*& allContainers){
	allContainers = new elementContainer[arrayLength];
}
//Deallocates memory for allContainers and allElements
void deallocateMemory(elementContainer*& allContainers){
	delete[] allContainers;
	allContainers = nullptr;
}


//Sort an array
void pyramidSort(int *array, int arrayLength, elementContainer* containerMemory){
	if(arrayLength <= 1)
		return;
	
	elementContainer** containerAssigner = new elementContainer*[(int)(std::log2(arrayLength + 1))];
	int position = arrayLength;
	for(int i = 0; i < std::log2(arrayLength + 1);++i){
		position /= 2;
		containerAssigner[i] = (containerMemory + position);
	}

	//assign parent elementContainer
	elementContainer* superParent = containerAssigner[1]++;
	
	//initialize the first three containers (superParent and its 2 children)
	superParent->hasTwoElements = false;
	superParent->parentContainer = nullptr;
	superParent->nestedContainer = containerAssigner[0]++;
	superParent->element0.nestedContainer = containerAssigner[0]++;
	
	superParent->nestedContainer->parentContainer = superParent;
	superParent->nestedContainer->hasTwoElements = false;
	
	superParent->element0.nestedContainer->parentContainer = superParent;
	superParent->element0.nestedContainer->hasTwoElements = false;
	
	//Set the nested containers of both bottom containers to nullptr, as that is what is checked to determine if the bottom has been reached
	superParent->nestedContainer->nestedContainer = nullptr;
	superParent->element0.nestedContainer->nestedContainer = nullptr;
	
	
	//Assign values to the correct locations in the initial containers
	if(array[0] >= array[1]){
		if(array[0] >= array[2]){
			superParent->element0.nestedContainer->element0.val = array[0];
			if(array[1] >= array[2]){
				superParent->element0.val = array[1];
				superParent->nestedContainer->element0.val = array[2];
			}
			else{
				superParent->element0.val = array[2];
				superParent->nestedContainer->element0.val = array[1];
			}
		}
		else{
			superParent->element0.nestedContainer->element0.val = array[2];
			superParent->element0.val = array[0];
			superParent->nestedContainer->element0.val = array[1];
		}
	}
	else{
		if(array[1] >= array[2]){
			superParent->element0.nestedContainer->element0.val = array[1];
			if(array[0] >= array[2]){
				superParent->element0.val = array[0];
				superParent->nestedContainer->element0.val = array[2];
			}
			else{
				superParent->element0.val = array[2];
				superParent->nestedContainer->element0.val = array[0];
			}
		}
		else{
			superParent->element0.nestedContainer->element0.val = array[2];
			superParent->element0.val = array[1];
			superParent->nestedContainer->element0.val = array[0];
		}
	}
	
	
	//Place the rest of the elements
	for(int i = 3;i < arrayLength;++i){
		elementContainer* destination = findInsertLoc(array[i],superParent);
		insertValue(array[i], destination, superParent, containerAssigner);
	}
	
	int index = 0;
	placeElementsInArray(superParent, array, index);
}

//sort an array
void pyramidSort(int* array, int arrayLength){
	elementContainer* allContainers = nullptr;
	allocateMemory(arrayLength,allContainers);
	pyramidSort(array, arrayLength, allContainers);
	deallocateMemory(allContainers);
}

