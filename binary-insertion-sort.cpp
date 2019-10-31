
int binarySearch(int array[],int value,int arrayLength);

void binaryInsertionSort(int a[], int n) { 
    int i, loc, j, selected; 
  
    for (i = 1; i < n; ++i) 
    { 
        j = i - 1; 
        selected = a[i]; 
  
        // Search for where the selected value should be inserted
        loc = binarySearch(a, selected, i); 
  
        // Move all elements after location to create space 
        while (j >= loc) 
        { 
            a[j+1] = a[j]; 
            j--; 
        } 
        a[j+1] = selected; 
    } 
}