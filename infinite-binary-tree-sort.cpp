struct RelatedNumber{
	int val;
	RelatedNumber* lowerChild;
	RelatedNumber* higherChild;
};

void infiniteBinaryTreeSort(int *array, int arrayLength);
void transferRelatedNumbersToArray(RelatedNumber* parent,int* intArray,int& index);


/*************************************
Infinite binary tree sort works by creating a binary tree that can extend infinitely. Each element can be linked to two other elements, an element higher
and an element lower than itself. When a new element is inserted, it is compared first with the highest element in the tree. Then, depending on if the element
was higher or lower than the parent element (even is regarded the same as being higher), the new element is compared to the lower child or the higher child.
If the lower child / higher child does not exist, the new element is inserted in that position and becomes the parent element's child.
*************************************/


//TODO:
//Might be faster if memory allocation is assigned all at once outside this module and the chunk is passed in. Might not be.

void infiniteBinaryTreeSort(int *array, int arrayLength){
	RelatedNumber* root = new RelatedNumber;
	root->lowerChild = nullptr;
	root->higherChild = nullptr;
	root->val = array[0];
	
	RelatedNumber* comparisonValue;	//variable to keep track of the RelatedNumber being compared to the to-be-inserted value
	//Outer loop, for each element in the array compare the element to the other elements and insert it
	for(int i = 1;i < arrayLength;++i){
		comparisonValue = root;
		
		//Compare the to-be-inserted value to elements in the tree until reaching a position where no element is yet placed
		while(true){
			//if the array value is greater than or equal to the comparison value, work with the higher child. Else work with the lower child.
			if(array[i] >= comparisonValue->val){
				if(comparisonValue->higherChild == nullptr){
					comparisonValue->higherChild = new RelatedNumber;
					comparisonValue->higherChild->higherChild = nullptr;
					comparisonValue->higherChild->lowerChild = nullptr;
					comparisonValue->higherChild->val = array[i];
					break;
				}
				else{
					comparisonValue = comparisonValue->higherChild;
					continue;
				}
			}
			else{
				if(comparisonValue->lowerChild == nullptr){
					comparisonValue->lowerChild = new RelatedNumber;
					comparisonValue->lowerChild->higherChild = nullptr;
					comparisonValue->lowerChild->lowerChild = nullptr;
					comparisonValue->lowerChild->val = array[i];
					break;
				}
				else{
					comparisonValue = comparisonValue->lowerChild;
					continue;
				}
			}
		}
	}
	
	//Replace the values in the array with the values sorted in the infinite binary tree
	int index = 0;
	transferRelatedNumbersToArray(root,array,index);
}

//Function to transfer the tree of RelatedNumbers to an array
//Parameters are the RelatedNumber to insert, the array to deposit the exported ints, 
	//and an int by reference to track the index to insert at (presumably starting at 0)
void transferRelatedNumbersToArray(RelatedNumber* parent,int* intArray,int &index){
	//If there's a lower child transfer that branch of the tree first
	if(parent->lowerChild != nullptr)
		transferRelatedNumbersToArray(parent->lowerChild,intArray,index);
	
	//When there's no lower child, assign this value to the array and increment the index
	intArray[index] = parent->val;
	++index;
	
	//If there's a higher child, transfer that branch of the tree to the array
	if(parent->higherChild != nullptr)
		transferRelatedNumbersToArray(parent->higherChild,intArray,index);
}