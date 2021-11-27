/* Solution to comp20003 Assignment 2, 2020 semester 1.

   Authorship Declaration:

   (1) I certify that the program contained in this submission is completely
   my own individual work, except where explicitly noted by comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students,
   or by non-students as a result of request, solicitation, or payment,
   may not be submitted for assessment in this subject.  I understand that
   submitting for assessment work developed by or in collaboration with
   other students or non-students constitutes Academic Misconduct, and
   may be penalized by mark deductions, or by other penalties determined
   via the University of Melbourne Academic Honesty Policy, as described
   at https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will
   I do so until after the marks are released. I understand that providing
   my work to other students, regardless of my intention or any undertakings
   made to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring
   service, or drawing the attention of others to such services and code
   that may have been made available via such a service, may be regarded
   as Student General Misconduct (interfering with the teaching activities
   of the University and/or inciting others to commit Academic Misconduct).
   I understand that an allegation of Student General Misconduct may arise
   regardless of whether or not I personally make use of such solutions
   or sought benefit from such actions.

   Signed by: Goh Xing Yang 1001969
   Dated: 12/09/2020
*/

/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "ass2func.h"

/* Main binding function for map2 radius search
*/
int main(int argc, char **argv) {  

    int i;
    /* Initialising read and write files */
    FILE *readFile, *writeFile;
    readFile = fopen(argv[CSVFILE], "r");
    if (readFile == NULL) {
        perror("File open error.");
        exit(EXIT_FAILURE);
    }
    writeFile = fopen(argv[OUTPUTFILE], "w");
    if (writeFile == NULL) {

        perror("File open error.");
        exit(EXIT_FAILURE);
    }

    /* Initial data processing and reading coordinates from stdinput */
    int inputLines = 0;
    initial_data_processing(readFile);
    radcoords_t *inputCoordinates = read_input_coordinates_radius(&inputLines);
    kdt_t *dataKDT = initialise_KDT();
    
    /* Reading the csv file into the K-D Tree */
    while (read_single_data_line(dataKDT, readFile) == 1) {
        /* Continue to read csv data lines and adding
        them into the data linked list */
    }

    /* Search and print out coordinate matches */
    kdnode_t *closestNode;
    data_t *closestNodeData;
    int searchCount = 0, printCount = 0;
    for (i = 0; i < inputLines; i++) {
        radius_search(dataKDT->root, inputCoordinates[i], ROOTDEPTH,
                   inputCoordinates[i].r, &searchCount, &printCount, writeFile);
        printf("%.12g %.12g %.12g --> %d\n", inputCoordinates[i].x,
               inputCoordinates[i].y, inputCoordinates[i].r, searchCount);
        if (printCount == 0) {
            fprintf(writeFile, "%.12g %.12g %.12g --> NOTFOUND\n",
                    inputCoordinates[i].x, inputCoordinates[i].y,
                    inputCoordinates[i].r);
        }
        printCount=0;
        searchCount=0;
        fprintf(writeFile, "\n");
    }

   /* Free everything and close all the files */
   kdt_free_tree(dataKDT);
   free(inputCoordinates);
   fclose(readFile);
   fclose(writeFile);
   return 0;
}