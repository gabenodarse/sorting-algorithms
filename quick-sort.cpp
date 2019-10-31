//Quick sort implementation modified from https://www.geeksforgeeks.org/quick-sort/


/*	This function takes the last element as a pivot, places the pivot element at its correct position in sorted array,
	and places all smaller elements to the left of the pivot and all greater elements to right 
*/
int partition (int arr[], int low, int high) 
{ 
    int pivot = arr[high];    // pivot 
    int i = (low - 1);  // Index of smaller element 
  
    for (int j = low; j <= high- 1; j++) 
    { 
        // If current element is smaller than or 
        // equal to pivot 
        if (arr[j] <= pivot) 
        { 
            i++;    // increment index of smaller element 
			int swapperVariable = arr[i];
			arr[i] = arr[j];
			arr[j] = swapperVariable;
        } 
    } 
    int swapperVariable = arr[i + 1];
	arr[i + 1] = arr[high];
	arr[high] = swapperVariable;
    return (i + 1); 
} 
  
/* The main function that implements QuickSort 
 arr[] --> Array to be sorted, 
  low  --> Starting index, 
  high  --> Ending index */
void quickSort(int arr[], int low, int high) 
{ 
    if (low < high) 
    { 
        /* pi is partitioning index, arr[p] is now 
           at right place */
        int pi = partition(arr, low, high); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, low, pi - 1); 
        quickSort(arr, pi + 1, high); 
    } 
}