//Pyramid sort

#include <cmath>

//#define DIAGNOSTICS
#ifdef DIAGNOSTICS

#include <string>
std::string debug = "";
#include <map>
std::map<std::string,int> g_measurements;
#include <iostream>

#endif





/****************************************************************
PYRAMID SORT*****************************************************
*****************************************************************

*****************************************************************/


//TODO:
//Move binary search to its own function

#include "nested-array-sort.h"

#ifdef DIAGNOSTICS
void printStructure(elementContainer* source, std::string containerIdentifier = "P");
#endif

elementContainer* promote(elementContainer* parent, int promotedValue, elementContainer*& superParent, int level,
							elementContainer** containerAssigner);	//>:<is & needed?

elementContainer* superParentPromote(int promotedValue, elementContainer*& superParent, int level, 
							elementContainer** containerAssigner);
							
void insertValue(int value, elementContainer* destination, elementContainer*& parent, elementContainer** containerAssigner);


//Element data struct contains the value of the element and the nested array that may be associated with it (nullptr if none is)
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


//Find the location where an element should be inserted
void findInsertLoc(int value, elementContainer*& superParent, elementContainer** containerAssigner){
	
	#ifdef DIAGNOSTICS
	++g_measurements["inside findInsertLoc"];
	#endif
	
	elementContainer* destination = superParent;
	
	//Loop until the lowest nest level is reached
	#ifdef DIAGNOSTICS
	while(++g_measurements["checking for bottom nest level"] && destination->nestedContainer != nullptr)
	#else
	while(destination->nestedContainer != nullptr)
	#endif
	{
		#ifdef DIAGNOSTICS
		if(destination->nestedContainer->parentContainer != destination){
			printStructure(destination);
			exit(0);
		}
		if(destination->element0.nestedContainer->parentContainer != destination){
			printStructure(destination);
			exit(0);
		}
		if(destination->hasTwoElements && destination->element1.nestedContainer->parentContainer != destination){
			printStructure(destination);
			exit(0);
		}
		#endif
		
		#ifdef DIAGNOSTICS
		++g_measurements["if value >= destination->elements[0].val"];
		#endif
		if(value >= destination->element0.val){
			#ifdef DIAGNOSTICS
			++g_measurements["if !hasTwoElements ||"];
			#endif
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
	
	insertValue(value, destination, superParent, containerAssigner);	//>:>Separate from find location function
	
}

//				>:<Explain why assignment happens here instead of inside promote
void insertValue(int value, elementContainer* destination, elementContainer*& superParent, elementContainer** containerAssigner){
	#ifdef DIAGNOSTICS
	++g_measurements["insert into lowest level"];

	if(destination->parentContainer == nullptr){
		std::cout << "trying to insert value when parent container == nullptr";
		exit(0);
	}
	if(destination->nestedContainer != nullptr){
		std::cout << "trying to insert value when there is a nested container";
		exit(0);
	}
	#endif
	
	//if the destination is full, promote
	if(destination->hasTwoElements){
		destination->hasTwoElements = false;	//an element will be promoted
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


//Promote a value to its parent container, and assign the newly promoted value a nested container.
//Return that nested container to the calling function to assign values to its own element and nested containers.
elementContainer* promote(elementContainer* parent, int promotedValue, elementContainer*& superParent, int level,
							elementContainer** containerAssigner){
	
	#ifdef DIAGNOSTICS
	++g_measurements["inside promote"];
	#endif
	
	if(parent->hasTwoElements){
		parent->hasTwoElements = false;	//An element will be promoted
		elementContainer* newContainer;	//The new container of the value that needs to be promoted (not to be confused with the 
												//container this function will create and return) >:< better description
		
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

//
elementContainer* superParentPromote(int promotedValue, elementContainer*& superParent, int level,
							elementContainer** containerAssigner){
	
	#ifdef DIAGNOSTICS
	++g_measurements["superparent promotion"];
	#endif
		
	//Create the new super parent
	elementContainer* newSuperParent = containerAssigner[level+1]++;
	newSuperParent->hasTwoElements = false;
	newSuperParent->parentContainer = nullptr;
	newSuperParent->nestedContainer = superParent;
	
	superParent->parentContainer = newSuperParent;
	superParent = newSuperParent;	//>:<moveable (to immediately before the return)
	
	newSuperParent->element0.val = promotedValue;
	
	//create the new container that will be returned, and assign it the correct parent
	newSuperParent->element0.nestedContainer = containerAssigner[level]++;
	newSuperParent->element0.nestedContainer->parentContainer = newSuperParent;
	
	return newSuperParent->element0.nestedContainer;
}



//>:>https://secweb.cs.odu.edu/~zeil/cs361/web/website/Lectures/recursionConversion/page/recursionConversion.html
//Extract the elements from their nested elementContainers and put them in the correct, sorted order into a standard array
void placeElementsInArray(elementContainer* source, int *array){
	static int index = 0;//>:> more elegant method
	
	#ifdef DIAGNOSTICS
	if(source->hasTwoElements){
		++g_measurements["2 elements"];
	}
	#endif
	
	if(source->nestedContainer != nullptr){
		placeElementsInArray(source->nestedContainer, array);
		
		array[index] = source->element0.val;
		++index;
		placeElementsInArray(source->element0.nestedContainer, array);
		
		if(source->hasTwoElements){
			array[index] = source->element1.val;
			++index;
			placeElementsInArray(source->element1.nestedContainer, array);
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


#ifdef DIAGNOSTICS
void printStructure(elementContainer* source, std::string containerIdentifier){
	std::cout << "CONTAINER \"" << containerIdentifier << "\": ()";
	std::cout << ", " << source->element0.val;
	if(source->hasTwoElements){
		std::cout << ", " << source->element1.val;
	}
	std::cout << "\n";
	if(source->nestedContainer != nullptr){
		printStructure(source->nestedContainer, containerIdentifier + "()");
		printStructure(source->element0.nestedContainer, containerIdentifier + "0");
		if(source->hasTwoElements){
			printStructure(source->element1.nestedContainer, containerIdentifier + "1");
		}
	}
}
#endif

void nestedArraySort(int *array, int arrayLength, elementContainer* containerAssigner){
	if(arrayLength <= 1)
		return;
	
	elementContainer** contAssigner = new elementContainer*[(int)(std::log2(arrayLength + 1))];
	int position = arrayLength;
	for(int i = 0; i < std::log2(arrayLength + 1);++i){
		position /= 2;
		contAssigner[i] = (containerAssigner + position);
	}
	
	#ifdef DIAGNOSTICS
	g_measurements["inside findInsertLoc"] = 0;
	g_measurements["checking for bottom nest level"] = 0;
	g_measurements["if value >= destination->elements[0].val"] = 0;	//(equivalent to the number of descents)
	g_measurements["if !hasTwoElements ||"] = 0;
	g_measurements["insert into lowest level"] = 0;
	g_measurements["inside promote"] = 0;
	g_measurements["superparent promotion"] = 0;
	g_measurements["2 elements"] = 0;
	#endif

	//assign parent elementContainer
	elementContainer* superParent = contAssigner[1]++;
	
	//initialize the first three containers (superParent and its 2 children)
	superParent->hasTwoElements = false;
	superParent->parentContainer = nullptr;
	superParent->nestedContainer = contAssigner[0]++;
	superParent->element0.nestedContainer = contAssigner[0]++;
	
	superParent->nestedContainer->parentContainer = superParent;
	superParent->nestedContainer->hasTwoElements = false;
	//superParent->nestedContainer->element0.nestedContainer does not have to be set, it is never checked
	superParent->nestedContainer->nestedContainer = nullptr;
	
	superParent->element0.nestedContainer->parentContainer = superParent;
	superParent->element0.nestedContainer->hasTwoElements = false;
	//superParent->element0.nestedContainer->element0.nestedContainer does not have to be set, it is never checked
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
		findInsertLoc(array[i],superParent,contAssigner);
	}
	
	
	placeElementsInArray(superParent, array);
	
	#ifdef DIAGNOSTICS
	for (std::map<std::string,int>::iterator it=g_measurements.begin(); it!=g_measurements.end(); ++it)
		std::cout << it->first << " => " << it->second << '\n';
	#endif
}


void nestedArraySort(int* array, int arrayLength){
	elementContainer* allContainers = nullptr;
	allocateMemory(arrayLength,allContainers);
	nestedArraySort(array, arrayLength, allContainers);
	deallocateMemory(allContainers);
}

