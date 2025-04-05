# CPS-125-projects
Summary: 
Objectives: make calculations and conclusions based on the real data collected by the NOAA. (national oceanic and atmospheric administration) on daily lake average surface water temperatures. 
Ontario
Erie 
Huron 
Michigan
St. Clair 
Collected between Jan and December from 1995 to 2025 in degree Celsius. 
Six files: 1 data/lake. 
In format: all_year_glsea_avg_s_C.csv
Days: 1-365.  1- Jan 1, 365- Dec 365. 


Headers used: 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_YEARS 30
#define NUM_DAYS 365
#define NUM_LAKES 6

Functions used: 
// Function to swap two values
void swap(double *a, double *b) {
    double temp = *a;
    *a = *b;
    *b = temp;
}

// Bubble Sort function
void bubbleSort(double arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
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
    bubbleSort(arr, size);

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

// Function to detect outliers by year: 
void detectOutliersByYear(double arr[], int size, double Q1, double Q3) {
    double IQR = Q3 - Q1;
    double lower = Q1 - 1.5 * IQR;
    double upper = Q3 + 1.5 * IQR;
    
    for (int i = 0; i < size; i++) {
        if (arr[i] < lower || arr[i] > upper) {
            printf("Outlier detected: %.2f°C\n", arr[i]);
        }
    }
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
    while (fgets(line, sizeof(line), file)) {
        if (day >= NUM_DAYS) break;
        char *token = strtok(line, ",");
        int year = 0;
        while (token != NULL) {
            if (year > 0 && year <= NUM_YEARS) {
                data[day][year - 1] = atof(token);
            }
            token = strtok(NULL, ",");
            year++;
        }
        day++;
    }
    fclose(file);
    return 0;
}



// Main function (for processing each lake data files) 
int main() {
    // Data for each lake for 30 years
    double lakeData[NUM_LAKES][NUM_DAYS][NUM_YEARS];

    // File names for each lake
    const char *lakeFiles[] = {
        "all_year_glsea_avg_o_C.csv", // Lake Ontario
        "all_year_glsea_avg_e_C.csv", // Lake Erie
        "all_year_glsea_avg_h_C.csv", // Lake Huron
        "all_year_glsea_avg_m_C.csv", // Lake Michigan
        "all_year_glsea_avg_s_C.csv", // Lake Superior
        "all_year_glsea_avg_c_C.csv"  // Lake St. Clair
    };

    // Lake names corresponding to the file names
    const char *lakeNames[] = {
        "Lake Ontario", 
        "Lake Erie", 
        "Lake Huron", 
        "Lake Michigan", 
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

        // Calculate quartiles for the lake
        double Q1, Q2, Q3;
        printf("\nProcessing data for %s:\n", lakeNames[i]);
        
        // Flatten the data to consider daily temperatures across all years for each lake
        findQuartiles(lakeData[i][0], NUM_DAYS * NUM_YEARS, &Q1, &Q2, &Q3);
        printf("Q1: %.2f, Q2 (Median): %.2f, Q3: %.2f\n", Q1, Q2, Q3);

        // Detect outliers for the lake (Use the entire dataset of daily temperatures by year)
        detectOutliersByYear(lakeData[i], NUM_DAYS, Q1, Q3);

        // Calculate and display average temperature for each day of the year for the lake
        for (int j = 0; j < NUM_DAYS; j++) {
            double avgTemp = mean(lakeData[i][j], NUM_YEARS);
            int month, dayOfMonth;
            convertToMonthDay(j + 1, &month, &dayOfMonth);
            printf("Day %d (%02d-%02d): Average Temp: %.2f°C\n", j + 1, month, dayOfMonth, avgTemp);
        }

        // Calculate the summer average (day 172 to day 265) for the lake
        double summerAverage = 0;
        for (int day = 171; day < 265; day++) {
            summerAverage += mean(lakeData[i][day], NUM_YEARS);
        }
        summerAverage /= (265 - 171); // Calculate average temperature for the summer period
        printf("Summer Average Temp for %s (days 172 to 265): %.2f°C\n", lakeNames[i], summerAverage);
    }

    return 0;
}






