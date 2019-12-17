//Trail sort


/****************************************************************
TRAIL SORT*****************************************************
*****************************************************************

Trail sort is a sorting technique arising from a different way to look at binary trees. A binary tree can be looked at as not a parent
	and two children, but as a trail of elements with each element containing its own trail. The "trail" is a sequence of elements all higher
	or all lower than the last. An element within this trail can have an offshooting trail of the opposite direction.
	Example:
	    5
	   / \
	  2   8        -Binary tree
	     / \
		7   10
	   / \
	  6   9
	This can instead be thought of as:
	5--8--10
	|  |
	2  7--9
	   |
	   6
	where 5-8-10 is a trail, 2 is the trail associated with 5, and 7-6 is the trail associated with 8, and 9 is the trail associated with 7

Trail sort works by shifting the tree whenever the length of an element's trail diverges from its "height" in the trail its contained in
	(height is how many more elements are in the containing trail before the end is reached. In the example above, 8 has a height of 1
	and a trail length of 2)
	When a shift occurs, the child from the longer of trail and height becomes the new parent.
	Example:
		5				     6
		 \				    / \
		  6		------->   5   7
		   \
		    7

In a more complicated case, where the shifting elements have children, the trail of the element that is becoming the parent changes places
	Example, when inserting 11 into the example above:
		5							     8
	   / \							   /   \
	  2   8        ----------->       5     10
	     / \						 / \      \
		7   10					    2   7      11
	   / \	  \                        / \
	  6   9    11                     6   9 
	What happened: 
	8 was becoming the new parent of 5, so the elements branching off its smaller child had to change places. These elements moved to
	branch off of 5 instead
	
A superParent is created above the peak of to ensure consistency in the trail/height terminology

By doing these swaps, in practically all cases the tree grows nest levels logarithmically. In my test with 100,000 random elements, 
the max nest level was 23. This results in a linearithmically scaling (on average) algorithm

Challenges*************
This current implementation is much too complicated. Although the idea for sorting this way came from the thought of laying out a binary 
	tree with a height and a trailLength, it could likely be simplified by just referring to them as lower trail length and higher trail
	length. If creating a binary tree structure with minimal data, storing only three variables, trailLength, trailPtr, and isParentLesser 
	(which is eqivalent to the trail direction) for each element would be efficient. But for sorting and shifting the tree, more information
	is needed, and the trailLength/height become hard to distinguish when the direction of each is changing.
The performance gains of such a complicated method are nothing special, either. Although the average growth of this algorithm is 
	linearithmic, it is with a rather high coefficient compared to other linearithmic scaling algorithms. Not only that, in heavily
	zig-zagging trees the performance can degrade to O(n^2), similar to how quick sort degrades on sorted / reverse sorted arrays.
*****************************************************************/

struct element;


void updateTree(element* comparisonElement, bool isTrailLesser);


//contains the value of the element and necessary tracked variables
struct element{
	int val;
	element* parent;
	element* lesserChild;
	element* greaterChild;
	int trailLength;
	int height;
	bool isParentLesser;
};


//Insert an element into the binary tree
void insertVal(int value, element* peak, element*& elementAssigner){
	
	element*& comparisonElement = peak;
	
	//Loop until the lowest nest level is reached
	while(true)
	{
		
		if(value >= comparisonElement->val){
			if(comparisonElement->greaterChild != nullptr){
				comparisonElement = comparisonElement->greaterChild;
				continue;
			}
			else{
				//assign the new element to the greaterChild
				comparisonElement->greaterChild = elementAssigner++;
				comparisonElement->greaterChild->val = value;
				comparisonElement->greaterChild->parent = comparisonElement;
				comparisonElement->greaterChild->lesserChild = nullptr;
				comparisonElement->greaterChild->greaterChild = nullptr;
				comparisonElement->greaterChild->trailLength = 0;
				comparisonElement->greaterChild->height = 0;
				comparisonElement->greaterChild->isParentLesser = true;
				updateTree(comparisonElement, true);
				break;
			}
		}
		else{
			if(comparisonElement->lesserChild != nullptr){
				comparisonElement = comparisonElement->lesserChild;
				continue;
			}
			else{
				//assign the new element to the lesserChild
				comparisonElement->lesserChild = elementAssigner++;
				comparisonElement->lesserChild->val = value;
				comparisonElement->lesserChild->parent = comparisonElement;
				comparisonElement->lesserChild->lesserChild = nullptr;
				comparisonElement->lesserChild->greaterChild = nullptr;
				comparisonElement->lesserChild->trailLength = 0;
				comparisonElement->lesserChild->height = 0;
				comparisonElement->lesserChild->isParentLesser = false;
				
				updateTree(comparisonElement, false);
				break;
			}
		}
	}//End while loop
	
}


//Checks parent elements for any triggered shifts. 
void updateTree(element* comparisonElement, bool isTrailLesser){
	
	
	element tmp;	//for swapping values in case a shift occurs
	while(comparisonElement->isParentLesser == isTrailLesser){
		//if the disparity between the height and the trailLength won't become 2
		if(comparisonElement->height - comparisonElement->trailLength != 1){
			++comparisonElement->height;
			comparisonElement = comparisonElement->parent;
			continue;
		}
		//else shift trail and return
		else if(isTrailLesser){
			tmp = *(comparisonElement->greaterChild);
			
			comparisonElement->parent->greaterChild = comparisonElement->greaterChild;
			
			comparisonElement->greaterChild->lesserChild = comparisonElement;
			comparisonElement->greaterChild->parent = comparisonElement->parent;
			comparisonElement->greaterChild->trailLength = comparisonElement->trailLength + 1;
			
			comparisonElement->parent = comparisonElement->greaterChild;
			comparisonElement->greaterChild = tmp.lesserChild;
			comparisonElement->height = comparisonElement->trailLength;
			comparisonElement->isParentLesser = !isTrailLesser;
			
			if(comparisonElement->greaterChild != nullptr){
				int tmp2 = comparisonElement->greaterChild->trailLength;
				comparisonElement->greaterChild->trailLength = comparisonElement->greaterChild->height;
				comparisonElement->greaterChild->height = tmp2;
				comparisonElement->trailLength = tmp2 + 1;
				comparisonElement->greaterChild->isParentLesser = true;
				comparisonElement->greaterChild->parent = comparisonElement;
			}
			else{
				comparisonElement->trailLength = 0;
			}
			return;
		}
		else{
			tmp = *(comparisonElement->lesserChild);
			
			comparisonElement->parent->lesserChild = comparisonElement->lesserChild;
			
			comparisonElement->lesserChild->greaterChild = comparisonElement;
			comparisonElement->lesserChild->parent = comparisonElement->parent;
			comparisonElement->lesserChild->trailLength = comparisonElement->trailLength + 1;
			
			comparisonElement->parent = comparisonElement->lesserChild;
			comparisonElement->lesserChild = tmp.greaterChild;
			comparisonElement->height = comparisonElement->trailLength;
			comparisonElement->isParentLesser = !isTrailLesser;
			
			if(comparisonElement->lesserChild != nullptr){
				int tmp2 = comparisonElement->lesserChild->trailLength;
				comparisonElement->lesserChild->trailLength = comparisonElement->lesserChild->height;
				comparisonElement->lesserChild->height = tmp2;
				comparisonElement->trailLength = tmp2 + 1;
				comparisonElement->lesserChild->isParentLesser = false;
				comparisonElement->lesserChild->parent = comparisonElement;
			}
			else{
				comparisonElement->trailLength = 0;
			}
			return;
		}
	}
	
	
	//none of the trail had to shift, so check for a shift at the top of the trail
	if(comparisonElement->trailLength - comparisonElement->height < 1){
		//no shift is needed, update trail length and return
		++comparisonElement->trailLength;
		return;
	}
	//else shift parent
	else if(isTrailLesser){
		tmp = *(comparisonElement->greaterChild);
		
		comparisonElement->parent->lesserChild = comparisonElement->greaterChild;
		
		comparisonElement->greaterChild->lesserChild = comparisonElement;
		comparisonElement->greaterChild->parent = comparisonElement->parent;
		comparisonElement->greaterChild->trailLength = comparisonElement->trailLength;
		comparisonElement->greaterChild->height = comparisonElement->height + 1;
		comparisonElement->greaterChild->isParentLesser = false;
		
		comparisonElement->parent = comparisonElement->greaterChild;
		comparisonElement->greaterChild = tmp.lesserChild;
		
		if(comparisonElement->greaterChild != nullptr){
			int tmp2 = comparisonElement->greaterChild->trailLength;
			comparisonElement->greaterChild->trailLength = comparisonElement->greaterChild->height;
			comparisonElement->greaterChild->height = tmp2;
			comparisonElement->trailLength = tmp2 + 1;
			comparisonElement->greaterChild->isParentLesser = true;
			comparisonElement->greaterChild->parent = comparisonElement;
		}
		else{
			comparisonElement->trailLength = 0;
		}
		
		updateTree(comparisonElement->parent->parent, !isTrailLesser);	//check further up the tree for recursive shifts
		return;
	}
	else{
		tmp = *(comparisonElement->lesserChild);
			
		comparisonElement->parent->greaterChild = comparisonElement->lesserChild;
		
		comparisonElement->lesserChild->greaterChild = comparisonElement;
		comparisonElement->lesserChild->parent = comparisonElement->parent;
		comparisonElement->lesserChild->trailLength = comparisonElement->trailLength;
		comparisonElement->lesserChild->height = comparisonElement->height + 1;
		comparisonElement->lesserChild->isParentLesser = true;
		
		comparisonElement->parent = comparisonElement->lesserChild;
		comparisonElement->lesserChild = tmp.greaterChild;
		
		if(comparisonElement->lesserChild != nullptr){
			int tmp2 = comparisonElement->lesserChild->trailLength;
			comparisonElement->lesserChild->trailLength = comparisonElement->lesserChild->height;
			comparisonElement->lesserChild->height = tmp2;
			comparisonElement->trailLength = tmp2 + 1;
			comparisonElement->lesserChild->isParentLesser = false;
			comparisonElement->lesserChild->parent = comparisonElement;
		}
		else{
			comparisonElement->trailLength = 0;
		}
		
		updateTree(comparisonElement->parent->parent, !isTrailLesser);	//check further up the tree for recursive shifts
	}
}


//Transfer the values from the binary tree to the array
void placeElementsInArray(element* source, int* array, int& index){
	if(source->lesserChild != nullptr){
		placeElementsInArray(source->lesserChild, array, index);
	}
	array[index] = source->val;
	index += 1;
	if(source->greaterChild != nullptr){
		placeElementsInArray(source->greaterChild, array, index);
	}
}


//Allocates memory for elements based on the array length !!!return pointer to the memory instead of using an out parameter
void allocateMemory(int arrayLength, element*& elementMemory){
	elementMemory = new element[arrayLength + 1];
}
//Deallocates memory for elements
void deallocateMemory(element*& elementMemory){
	delete[] elementMemory;
	elementMemory = nullptr;
}


element* initSuperParent(element*& elementAssigner, int val){
	element* superParent = elementAssigner++;
	superParent->trailLength = 1;
	superParent->isParentLesser = false;	//so there is a change in direction at the top
	superParent->height = 999999;	//!!! int max to prevent shifts
	
	superParent->greaterChild = elementAssigner++;;
	superParent->greaterChild->val = val;
	superParent->greaterChild->lesserChild = nullptr;
	superParent->greaterChild->greaterChild = nullptr;
	superParent->greaterChild->parent = superParent;
	superParent->greaterChild->trailLength = 0;
	superParent->greaterChild->height = 0;
	superParent->greaterChild->isParentLesser = true;
	
	return superParent;
}


void trailSort(int *array, int arrayLength, element* elementAssigner){
	if(arrayLength <= 1)
		return;
	
	element* superParent = initSuperParent(elementAssigner, array[0]);
	
	//Place the rest of the elements
	for(int i = 1;i < arrayLength;++i){
		insertVal(array[i],superParent->greaterChild,elementAssigner);
	}
	
	int index = 0;
	placeElementsInArray(superParent->greaterChild, array, index);
}


void trailSort(int* array, int arrayLength){
	element* elementMemory;
	allocateMemory(arrayLength,elementMemory);
	trailSort(array, arrayLength, elementMemory);
	deallocateMemory(elementMemory);
}

