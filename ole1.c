/*
 * This application is written as a response to the One Billion Records challenge (1BRC).
 * 
 * Ref: https://www.morling.dev/blog/one-billion-row-challenge/
 *	
 * Developed by Ole Myhra, July 2024
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DATA_BUFFER_SIZE 255
#define MAX_STATIONS 100000
#define STATION_NAME_SIZE 100

struct station {
	char name[STATION_NAME_SIZE];
	double min;
	double mean;
	double max;
	double sum;
	double last_value;
	int number_of_measurements;
};

inline unsigned int hash(const char *s);
struct station *search(char *key, struct station *hash_table[]);
int comparator(const void *p1, const void *p2);

int main(int argc, char **argv)
{
	/* Variable declarations */
	FILE *fp = NULL;
	char data_buffer[DATA_BUFFER_SIZE];
	struct station *results[MAX_STATIONS];
	struct station *lost_pointers[MAX_STATIONS];
	struct station *sorted_stations = NULL;
	struct station tmp_station = {	.name = "",
	       				.min=0.0, 
					.max=0.0, 
					.mean=0.0, 
					.sum=0.0, 
					.last_value=0.0, 
					.number_of_measurements=0};
	int return_code = 0;
	unsigned int tmp_hash = 0;
	int station_count = 0;
	int sorted_stations_index = 0;
	int lost_stations_counter = 0;

	time_t start_time, stop_time;
	time(&start_time);

	/* Buffer initializations */
	memset(results, 0, MAX_STATIONS * sizeof(struct station *));
	memset(data_buffer, 0, DATA_BUFFER_SIZE * sizeof(char));
	memset(lost_pointers, 0, MAX_STATIONS * sizeof(struct station *));

	/* Checking for the correct number of user supplied arguments */
	if(argc != 2) {
		printf("usage: %s measurement_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	/* Opening the file containing the stations measurements */
	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		fprintf(stderr, "Unable to open file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}


	/* Loop until the end of file, read each line and split the values into a 
	 * temporary station structure. Then create a hash and chech of the station
	 * exists. If it does then update the measurement values and if not then
	 * create a new entry in the hash table pointing to the new structure */

	while((fgets(data_buffer, DATA_BUFFER_SIZE, fp)) != NULL) {
		return_code = sscanf(data_buffer, "%35[^;];%25lf", tmp_station.name, 
				&(tmp_station.last_value));

		if(return_code == EOF) {
			fprintf(stderr, "Error during data parsing!");
			exit(EXIT_FAILURE);
		}

		tmp_hash = hash(tmp_station.name);

		struct station *pTest = search(tmp_station.name, results);
		if (pTest != NULL) {

			if(tmp_station.last_value < pTest->min)
				pTest->min = tmp_station.last_value;

			if(tmp_station.last_value > pTest->max)
				pTest->max = tmp_station.last_value;

			pTest->number_of_measurements++;

			pTest->sum += tmp_station.last_value;
			pTest->mean = pTest->sum / (double) pTest->number_of_measurements;


		} else {
			results[tmp_hash] = (struct station * ) malloc(sizeof(struct station));
			lost_pointers[lost_stations_counter++] = results[tmp_hash];

			if (results[tmp_hash] == NULL) {
				fprintf(stderr, "Allocation of memory failed!\n");
				exit(EXIT_FAILURE);
			}
			memset(&(results[tmp_hash]->name), 0, STATION_NAME_SIZE); 
			results[tmp_hash]->min = 0.0;
			results[tmp_hash]->max = 0.0;
			results[tmp_hash]->mean = 0.0;
			results[tmp_hash]->number_of_measurements = 0;
			results[tmp_hash]->sum = 0.0;

			strcpy(results[tmp_hash]->name, tmp_station.name);
			results[tmp_hash]->min = tmp_station.last_value;
			results[tmp_hash]->max = tmp_station.last_value;
			results[tmp_hash]->mean = tmp_station.last_value;
			results[tmp_hash]->number_of_measurements++;
			results[tmp_hash]->sum += tmp_station.last_value;

		}

	}
	
	fclose(fp);

	/* Count the stations in the hash table */
	for(int i=0;i<MAX_STATIONS;i++)
		if(results[i] != NULL) { station_count++; }

	/* Create an array of structs in the same size as the number of stations
	 * in the hash table; results */
	sorted_stations = (struct station *) malloc(station_count * sizeof(struct station));
	
	if(sorted_stations == NULL) {
		fprintf(stderr, "Unable to allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	memset(sorted_stations, 0, (station_count * sizeof(struct station)));
	
	/* Copy the stations in the hash table into the sorted table */
	for (int i=0;i<MAX_STATIONS;i++) {
		if(results[i] != NULL)
			sorted_stations[sorted_stations_index++] = *results[i];
	}
	
	/* Sort the sorted array using quick sort */
	qsort(sorted_stations, station_count, sizeof(struct station), comparator);
	
	/*Print out the result */
	for(int i=0;i<station_count;i++) {
		printf("%s;%.2lf;%.2lf;%.2lf\n", sorted_stations[i].name, sorted_stations[i].min, sorted_stations[i].mean, sorted_stations[i].max);
	}
	
	/* Free memory from the results hash table */
	for(int i=0;i<MAX_STATIONS;i++)
		free(lost_pointers[i]);

	free(sorted_stations);
	
	/*Calculate the time to perform the task and print the result */
	time(&stop_time);
	printf("\nExecution time %.2f seconds\n", difftime(stop_time, start_time));

	return EXIT_SUCCESS;
}


/*
 * Creates a hash code based upon the station name
 */
inline unsigned int hash(const char *s)
{
	unsigned int hashval = 0;
	for(; *s != '\0'; s++)
		hashval = *s + 31 * hashval;
	return hashval % MAX_STATIONS;
}

/*
 * Searches the hash table for a key, if found it returns the pointer
 * to the struct containing the key. If not found it will search through
 * the next entries in the hash table.
 */
struct station *search(char *key, struct station *hash_table[])
{
	unsigned int hashval = hash(key);
	while(hash_table[hashval] !=  NULL) {
		if((strcmp(hash_table[hashval]->name, key) == 0)) {
			return hash_table[hashval];
		}

		++hashval;

		hashval %= MAX_STATIONS;
	}
	return NULL;
}

/*
 * Compares the struct station names and using strcmp.
 * This function is used by qsort()
 */
int comparator(const void *p1, const void *p2)	
{
	return strcmp(((struct station *)p1)->name, ((struct station *)p2)->name);
}

