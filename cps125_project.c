#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_YEARS 30
#define NUM_DAYS 365
#define NUM_LAKES 6

typedef struct {
	double temp;
	int index;
}temper;

// Function to swap two values
void swap(double *a, double *b) {

	double temp = *a;
	*a = *b;
	*b = temp;
}

// Bubble Sort function
void bubbleSort(double arr[], int size) {
    int needPass = 1; // Flag if another round of bubble sort is needed
    while (needPass) {
        needPass = 0; // Assume we can stop after this loop
        for (int i = 0; i < size - 1; i++) {
            if (arr[i] > arr[i + 1]) {
                // Swap and require another pass since the array wasn't sorted
                swap(&arr[i], &arr[i + 1]);
                needPass = 1;
            }
        }
    }
}

// Function to calculate the median
double findMedian(double arr[], int start, int end) {
	int n = end - start + 1;
	if (n % 2 == 0) {
		return (arr[start + n / 2 - 1] + arr[start + n / 2]) / 2.0;
	} else {
		return arr[start + n / 2];
	}
}

// Function to find quartiles
void findQuartiles(double arr[], int size, double *Q1, double *Q2, double *Q3) {
	
	bubbleSort(arr, size-1);

	*Q2 = findMedian(arr, 0, size - 1);

	int mid = size / 2;
	
	*Q1 = findMedian(arr, 0, mid - 1);
	
	*Q3 = findMedian(arr, mid + (size % 2 == 0 ? 0 : 1), size - 1);

}

// Function to calculate the mean
double mean(double arr[], int size) {
	double sum = 0;
	for (int i = 0; i < size; i++) {
		sum += arr[i];
	}
	return sum / size;
}

// Function to detect outliers based on IQR
void detectOutliers(double arr[], int size, double Q1, double Q3) {
	double IQR = Q3 - Q1;
	double lowerBound = Q1 - 1.5 * IQR;
	double upperBound = Q3 + 1.5 * IQR;

	for (int j = 0; j < NUM_YEARS; ++j){
	    if (arr[j] < lowerBound || arr[j] > upperBound) {
		    printf("Outlier: %.2lf, ", arr[j]);
		    printf("%d ", j+1995);
	    }
	}
	
    
    /*printf("\n%lf\n", lowerBound);
    printf("%lf\n", upperBound);
    
    printf("%lf\n", Q3);
    printf("%lf\n", Q1);*/
}


// Function to convert day number to month-day format
void convertToMonthDay(int day, int *month, int *dayOfMonth) {
	int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	*month = 0;
	*dayOfMonth = day;
	while (*dayOfMonth > daysInMonth[*month]) {
		*dayOfMonth -= daysInMonth[*month];
		(*month)++;
	}
	(*month)++;  // For 1-based month
}


// Return a token and update the string to the next token start
char* tokenize(char **str, char delim, char terminator) {
    // If there is an empty string, no tokens
    if (**str == '\0')
       return NULL;
    
    int off = 0;
    char* ptr = *str;
    // Loop while we havent reached the terminator
    while (ptr[off] != terminator || ptr[off] == '\0') {
       // End string at deliminator point
       if (ptr[off] == delim) {
          ptr[off] = '\0';
          break;
       }
       off++;
    }
    // End string at terminator
    if (ptr[off] == terminator) {
       ptr[off] = '\0';
    }

    // Update string pointer to next token including terminator
    char* token = ptr;
    *str = ptr + off + (ptr[off] == terminator ? 0 : 1);
    return token;
}

// Function to read lake data from a CSV file

int readLakeData(const char *filename, double data[NUM_DAYS][NUM_YEARS]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
       printf("Error opening file %s.\n", filename);
       return -1;
    }


    char line[1024];
    int day = 0;
    fgets(line, sizeof(line), file);
    while (fgets(line, sizeof(line), file)) {
       if (day >= NUM_DAYS) break;
       char* linePtr = line;

       char* dayToken = tokenize(&linePtr, ',', '\n');
       char* token = tokenize(&linePtr, ',', '\n');
       int year = 0;
       while (*linePtr != '\0') {
          if (year < NUM_YEARS) {
             if (strlen(token) != 0)
                data[day][year] = atof(token);
             else
                data[day][year] = 0.0;
          }
          token = tokenize(&linePtr, ',', '\n');
          year++;
       }
       day++;
    }
    fclose(file);
    return 0;
}

int main() {
	// Data for each lake for 30 years
	double lakeData[NUM_LAKES][NUM_DAYS][NUM_YEARS];
	double sortlakeData[NUM_LAKES][NUM_DAYS][NUM_YEARS];
	double tempList[NUM_LAKES];

	    // File names for each lake
    const char *lakeFiles[] = {
        "all_year_glsea_avg_h_C.txt", // Lake Huron
        "all_year_glsea_avg_o_C.txt", // Lake Ontario
        "all_year_glsea_avg_m_C.txt", // Lake Michigan
        "all_year_glsea_avg_e_C.txt", // Lake Erie
        "all_year_glsea_avg_s_C.txt", // Lake Superior
        "all_year_glsea_avg_c_C.txt"  // Lake St. Clair
    };

    // Lake names corresponding to the file names
    const char *lakeNames[] = {
        "Lake Huron",
        "Lake Ontario", 
        "Lake Michigan", 
        "Lake Erie", 
        "Lake Superior", 
        "Lake St. Clair"
    };

	// Process each lake's data
	for (int i = 0; i < NUM_LAKES; i++) {
		// Read data for each lake (data is stored in lakeData[i])
		if (readLakeData(lakeFiles[i], lakeData[i]) != 0) {
			printf("Error reading data for %s\n", lakeFiles[i]);
			return 1;
		}
		memcpy(sortlakeData, lakeData, sizeof(sortlakeData));
		
        
        printf("\nProcessing data for %s:\n", lakeNames[i]);

        // Loop through each day of the year
		for (int j = 0; j < NUM_DAYS; j++) {
		    
			double avgTemp = mean(lakeData[i][j], NUM_YEARS);
			
			int month, dayOfMonth;
			
			convertToMonthDay(j + 1, &month, &dayOfMonth);

			
			printf("\n\nDay %d (%02d-%02d): Average Temp: %.2fB0C\n", j + 1, month, dayOfMonth, avgTemp);

            // Calculate quartiles for the lake
            
    		double Q1, Q2, Q3;
    		// Flatten the data to consider daily temperatures across all years for each lake
    		findQuartiles(sortlakeData[i][j], NUM_YEARS, &Q1, &Q2, &Q3);
    		
    		printf("Q1: %.2f, Q2 (Median): %.2f, Q3: %.2f\n", Q1, Q2, Q3);
    
    		// Detect outliers for the lake (Use the entire dataset of daily temperatures by year)
    		detectOutliers(lakeData[i][j], NUM_DAYS, Q1, Q3);
		}

		// Calculate Average Warmest and Coldest Day of Each Year

		temper warmest = {mean(lakeData[i][0], NUM_YEARS),0}, coldest = {mean(lakeData[i][0], NUM_YEARS),0};

		for (int j = 0; j < NUM_DAYS; j++) {

			double avgTemp = mean(lakeData[i][j], NUM_YEARS);
            if (j == NUM_DAYS-1){
                continue;
            }
			if (avgTemp > warmest.temp) {
				warmest.temp = avgTemp;
				warmest.index = j;
			}
			if (avgTemp < coldest.temp) {
				coldest.temp = avgTemp;
				coldest.index = j;
			}
		}

		int month, dayOfMonth;

		convertToMonthDay(coldest.index + 1, &month, &dayOfMonth);
		printf("Coldest Average: Day %d (%02d-%02d): Average Temp: %.2lf °C\n", coldest.index + 1, month, dayOfMonth, coldest.temp);

		convertToMonthDay(warmest.index, &month, &dayOfMonth);
		printf("Warmest Average: Day %d (%02d-%02d): Average Temp: %.2lf °C\n", warmest.index + 1, month, dayOfMonth, warmest.temp);

		// Calculate the summer average (day 172 to day 265) for the lake
		double summerAverage = 0;
		for (int day = 171; day < 265; day++) {
			summerAverage += mean(lakeData[i][day], NUM_YEARS);
		}
		
		summerAverage /= (265 - 171); // Calculate average temperature for the summer period
		printf("Summer Average Temp for %s (days 172 to 265): %.2f°C\n", lakeNames[i], summerAverage);
		tempList[i] = summerAverage;
	}
	
	printf("\nCoolest to Warmest Lakes in the Summer:\n");
	
	double sortTempList[NUM_LAKES];
	
	memcpy(sortTempList, tempList, sizeof(sortTempList));

	bubbleSort(sortTempList, NUM_LAKES);
	
	for (int x=0; x<NUM_LAKES;++x){
	    for (int z=0; z<NUM_LAKES;++z){
	        if (sortTempList[x] == tempList[z])
	            printf("%s\n", lakeNames[z]);
	    }
	    
	}
	
	
	return 0;
}
