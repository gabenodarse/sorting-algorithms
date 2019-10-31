
int binarySearch(int array[],int value,int arrayLength){
	int lowerBound = -1;
	int& upperBound = arrayLength;
	int comparisonIndex = (lowerBound + upperBound) / 2;
	
	while(upperBound - lowerBound > 1){
		if(value < array[comparisonIndex]){
			upperBound = comparisonIndex;
		}
		else{
			lowerBound = comparisonIndex;
		}
		comparisonIndex = (lowerBound + upperBound) / 2;
	}
	
	return upperBound;
}