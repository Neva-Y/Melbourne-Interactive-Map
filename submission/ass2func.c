#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "ass2func.h"

/************************************************************************/

/* Function to print an error message and exit the program 
*/
void invalid_data_error(char *msg, int line) {
    printf("Error on line %3d: %s\n", line, msg);
    printf("The program will now terminate\n");
    exit(EXIT_FAILURE);
    return;
}
/************************************************************************/

/* Checking initial file format and removing the header line
*/
void initial_data_processing(FILE *readFile) {

    char ch;
    double val;

    /* Checking if the data file begins with a number, terminating if so */
    if (fscanf(readFile, "%lf", &val) == 1) {
        invalid_data_error("Data formatting error", __LINE__);
    }

    /* Taking the header line from the csv file and throwing it away */
    fscanf(readFile, "%c", &ch);
    while (ch != '\n') {
        fscanf(readFile, "%c", &ch);
   }
   return;
}

/************************************************************************/

/* Reading a single data line into the structs and putting it at the end of the 
linked list node
*/
int read_single_data_line(kdt_t *kdt, FILE *readFile) {

    int i;
    /* Allocate and check for the buffer to read in one line of the csv data */
    char **dataField = (char **)malloc(sizeof(char *)*NUMFIELDS);
    assert(dataField != NULL);
    for(i = 0; i < NUMFIELDS; i++) {
        dataField[i] = (char *)malloc(sizeof(char)*MAXCHAR);
        assert(dataField[i] != NULL);
    }

    /* Setting up counters and the struct to store the csv line data */
    int countChar = 0, countField = 0, countQuote = 0;
    data_t *lineData = (data_t *)malloc(sizeof(data_t));
    assert(lineData != NULL);

    while (fscanf(readFile, "%c", &(dataField[countField][countChar])) == 1) {
        
        /* End of the data line, ignore the '\n' character in the string by
        terminating with a '\0'. This is checked twice as I found '\n'
        to interact differently to the compiler's fscanf */
        if (dataField[countField][countChar] == '\n') {
            countChar--;
            dataField[countField][countChar] = '\0';

            /* Allocate the buffer to the struct, add the struct to the 
            linked list and free the buffer */
            allocate_struct(lineData, dataField);
            kdt->root = insert_KDnode(kdt->root, lineData, ROOTDEPTH);
            for(i = 0; i < NUMFIELDS; i++) {
                free(dataField[i]);
            }
            free(dataField);
            kdt->numElements++;
            return 1;
        }

        /* Handling all scenarios of a " mark in the csv file */ 
        if (dataField[countField][countChar] == '"') {
            countQuote++;
       
            /* Do not need to store the quotation so replace with next char */
            if(fscanf(readFile, "%c", &(dataField[countField][countChar]))==1) {

                /* End of the data line, ignore the '\n' character in the string 
                by terminating with a '\0'. This is the second check in case of 
                compiler differences */
                if (dataField[countField][countChar] == '\n') {
                    dataField[countField][countChar] = '\0';
    
                    /* Allocate the buffer to the struct, add the struct to the 
                    linked list and free the buffer */
                    allocate_struct(lineData, dataField);
                    kdt->root = insert_KDnode(kdt->root, lineData, ROOTDEPTH);
                    for(i = 0; i < NUMFIELDS; i++) {
                        free(dataField[i]);
                    }
                    free(dataField);
                    kdt->numElements++;
                    return 1;
                }
       
                /* Two consecutive quotation marks means that the quotation mark
                occurs in the data field */ 
                if (dataField[countField][countChar] == '"') {
                    countQuote++;
                    countChar++;
                }
       
                /* Field has ended, increment field counter and reset others */
                else if (dataField[countField][countChar] == ',' && 
                   (countQuote%EVENQUOTE) == 0 && countQuote != INVALIDQUOTES) {

                    dataField[countField][countChar] = '\0';
                    countChar = 0;
                    countQuote = 0;
                    countField++;
                }
       
                /* Standard reading scenario where the " denotes the start of 
                the field */
                else {
                    countChar++;
                }
            }
            
            /* Nothing to read, check if data ended before a \n character */
            else if (countChar != 0) {
                dataField[countField][countChar] = '\0';

                /* Allocate the buffer to the struct, add the struct to the 
                linked list and free the buffer */
                allocate_struct(lineData, dataField);
                kdt->root = insert_KDnode(kdt->root, lineData, ROOTDEPTH);
                for(i = 0; i < NUMFIELDS; i++) {
                    free(dataField[i]);
                }
                free(dataField);
                kdt->numElements++;
                return 0;
            }
        }
      
        /* Handling a regular ',' in the csv file that indicates the end of
        the field (does not occur in the field), increment the field counter */
        else if (dataField[countField][countChar] == ',' && countQuote == 0 ) {
            dataField[countField][countChar] = '\0';
            countChar = 0;
            countField++;
        }

        /* Handling ',' that occurs in the field (between quotation marks) */
        else if (dataField[countField][countChar] == ',' && 
            (countQuote%EVENQUOTE) != 0) {
            countChar++;
        }

        /* Regular reading scenario where character is not a newline, comma or
        quotation mark */
        else {
            countChar++;
        }
    }
    
    /* No more characters to read, end of the csv file, free all buffers and 
    allocated struct for the line data */
    for(i = 0; i < NUMFIELDS; i++) {
        free(dataField[i]);
    }
    free(dataField);
    free(lineData);
    return 0;
}
/************************************************************************/

/* Function to dynamically allocate memory to the corresponding struct fields 
and fill it with the characters that have been read into the buffer
*/
void allocate_struct(data_t *lineData, char **dataField) {

    lineData->year = (char *)malloc(sizeof(char)*
        (strlen(dataField[YEAR])+1));
    assert(lineData->year != NULL);
    strcpy(lineData->year, dataField[YEAR]);
 
    lineData->blockID = (char *)malloc(sizeof(char)*
        (strlen(dataField[BLOCKID])+1));
    assert(lineData->blockID != NULL);
    strcpy(lineData->blockID, dataField[BLOCKID]);
 
    lineData->propertyID = (char *)malloc(sizeof(char)*
        (strlen(dataField[PROPERTYID])+1));
    assert(lineData->propertyID != NULL);
    strcpy(lineData->propertyID, dataField[PROPERTYID]);
 
    lineData->baseID = (char *)malloc(sizeof(char)*
        (strlen(dataField[BASEID])+1));
    assert(lineData->baseID != NULL);
    strcpy(lineData->baseID, dataField[BASEID]);
 
    lineData->smallArea = (char *)malloc(sizeof(char)*
        (strlen(dataField[SMALLAREA])+1));
    assert(lineData->smallArea != NULL);
    strcpy(lineData->smallArea, dataField[SMALLAREA]);
 
    lineData->name = (char *)malloc(sizeof(char)*
        (strlen(dataField[NAME])+1));
    assert(lineData->name != NULL);
    strcpy(lineData->name, dataField[NAME]);
 
    lineData->industryCode = (char *)malloc(sizeof(char)*
        (strlen(dataField[INDUSTRYCODE])+1));
    assert(lineData->industryCode != NULL);
    strcpy(lineData->industryCode, dataField[INDUSTRYCODE]);
 
    lineData->industryDesc = (char *)malloc(sizeof(char)*
        (strlen(dataField[INDUSTRYDESC])+1));
    assert(lineData->industryDesc != NULL);
    strcpy(lineData->industryDesc, dataField[INDUSTRYDESC]);
 
    lineData->xCoords = (char *)malloc(sizeof(char)*
        (strlen(dataField[XCOORDS])+1));
    assert(lineData->xCoords != NULL);
    strcpy(lineData->xCoords, dataField[XCOORDS]);
 
    lineData->yCoords = (char *)malloc(sizeof(char)*
        (strlen(dataField[YCOORDS])+1));
    assert(lineData->yCoords != NULL);
    strcpy(lineData->yCoords, dataField[YCOORDS]);
 
    lineData->location = (char *)malloc(sizeof(char)*
        (strlen(dataField[LOCATION])+1));
    assert(lineData->location != NULL);
    strcpy(lineData->location, dataField[LOCATION]);
 
    return;
}
/************************************************************************/

/* Create a new node for the K-D tree
*/
kdnode_t *new_KDNode(data_t *d) {

    kdnode_t *tmpNode = (kdnode_t *)malloc(sizeof(kdnode_t));
    assert(tmpNode != NULL);
    tmpNode->head = (llnode_t *)malloc(sizeof(llnode_t));
    assert(tmpNode->head != NULL);
    tmpNode->head->data = d;
    tmpNode->head->next = NULL;
    tmpNode->left = NULL;
    tmpNode->right = NULL;

    return tmpNode;
}
/************************************************************************/

/* Insert identical coordinates to the front of the linked list
*/
void insert_duplicate(kdnode_t *kdNode, data_t *d) {

    assert(kdNode != NULL);
    llnode_t *linkedNode = (llnode_t *)malloc(sizeof(llnode_t));
    assert(linkedNode != NULL);
    linkedNode->data = d;
    linkedNode->next = kdNode->head;
    kdNode->head = linkedNode;
    return;
}
/************************************************************************/

/* Recursively insert a node into the KD tree and return root node
*/
kdnode_t *insert_KDnode(kdnode_t *rootNode, data_t *d, int depth) {

    /* Check if it is at a leaf */
    if (rootNode == NULL) {
        return new_KDNode(d);
    }
    
    /* Typecast void pointer to pointer to data_t */ 
    data_t *rootData = (data_t *)rootNode->head->data;

    /* Check if the coordinates are identical to current node */
    if ((atof(d->xCoords) == atof(rootData->xCoords)) &&
        (atof(d->yCoords) == atof(rootData->yCoords))) {
        insert_duplicate(rootNode, d);
        return rootNode;
    }

    /* Finding the current dimension for comparisons */
    int currDim = depth % KDDIM;

    /* Comparison of x coordinate */
    if (currDim == 1) {
        if (atof(d->xCoords) < atof(rootData->xCoords)) {
            rootNode->left = insert_KDnode(rootNode->left, d, depth+1);
        }

        /* Right Node contains larger or equal to values */
        else {
            rootNode->right = insert_KDnode(rootNode->right, d, depth+1);
        }
    }

    /* Comparison of y coordinate */
    else {
        if (atof(d->yCoords) < atof(rootData->yCoords)) {
            rootNode->left = insert_KDnode(rootNode->left, d, depth+1);
        }

        /* Right Node contains larger or equal to values */
        else {
            rootNode->right = insert_KDnode(rootNode->right, d, depth+1);
        }
    }
    return rootNode;
}
/************************************************************************/

/* Function to compare two nodes to return the node with the closer distance 
to specified coordinate
*/
kdnode_t *closer_distance(kdnode_t *nextNode, kdnode_t *currNode,
                          coords_t coords){
    data_t *nextNodeData = (data_t *)nextNode->head->data;
    data_t *currNodeData = (data_t *)currNode->head->data;

    double nextNodeDist = sqrt(pow((atof(nextNodeData->xCoords)-coords.x),2)+
                               pow((atof(nextNodeData->yCoords)-coords.y),2));
    double currNodeDist = sqrt(pow((atof(currNodeData->xCoords)-coords.x),2)+
                               pow((atof(currNodeData->yCoords)-coords.y),2));

    if (currNodeDist < nextNodeDist) {
        return currNode;
    }
    return nextNode;
}
/************************************************************************/

/* Function to search KD tree recursively, searches all possible nodes and
unwinds each function call by returning the node closest to the given coordinate
*/
kdnode_t *search_KDtree(kdnode_t *prevNode, kdnode_t *currNode, coords_t coords,
                        int depth, double prevDist, int *numSearch) {

    /* Pointer counter for number of searches performed */
    *(numSearch) = *(numSearch) + 1;

    /* Reached a leaf node, return the previous node */
    if (currNode == NULL){
        return prevNode;
    }
    data_t *nodeData = (data_t *)currNode->head->data;

    /* Calculate current node's distance to the coordinate and compare with
    previous closest, update the closest distance if closer */
    double champDist = sqrt(pow((atof(nodeData->xCoords)-coords.x),2)+
                            pow((atof(nodeData->yCoords)-coords.y),2));
    if (prevDist < champDist) {
        champDist = prevDist;
    }

    /* Find the current dimension of the kD tree (x or y) */
    int currDim = depth % KDDIM;

    if (currDim == XDIM){

        /* The x coordinate is within the radius of previous closest node,
        compare the previous best and the node's two children and return the
        closest node */
        if (fabs(atof(nodeData->xCoords)-coords.x) <= champDist) {
            return closer_distance(closer_distance(search_KDtree(currNode,
             currNode->left, coords, depth+1, champDist, numSearch),
              currNode, coords), closer_distance(search_KDtree(currNode,
               currNode->right, coords, depth+1, champDist, numSearch),
                currNode, coords), coords);
        }
        /* The x coordinate is out of the radius of the previous closest node,
        compare with right child if the node's x coordinate is less than the 
        desired coordinate or the left child if it's greater */
        else {
            if (atof(nodeData->xCoords) < coords.x) {
                return closer_distance(search_KDtree(currNode,
                 currNode->right, coords, depth+1, champDist, numSearch),
                  currNode, coords);
            }
            else {
                return closer_distance(search_KDtree(currNode, currNode->left,
                 coords, depth+1, champDist, numSearch), currNode, coords);
            }
        }
    }

    /* Same process for the y dimension of the KD tree */
    else {
        if (fabs(atof(nodeData->yCoords)-coords.y) <= champDist) {
            return closer_distance(closer_distance(search_KDtree(currNode,
             currNode->left, coords, depth+1, champDist, numSearch),
              currNode, coords), closer_distance(search_KDtree(currNode,
               currNode->right, coords, depth+1, champDist, numSearch),
                currNode, coords), coords);
        }
        else {
            if (atof(nodeData->yCoords) < coords.y) {
                return closer_distance(search_KDtree(currNode, currNode->right,
                 coords, depth+1, champDist, numSearch), currNode, coords);
            }
            else {
                return closer_distance(search_KDtree(currNode, currNode->left,
                 coords, depth+1, champDist, numSearch), currNode, coords);
            }
        }
    }
}
/************************************************************************/

/* Function to print closest node from the KD Tree
*/
void print_node(kdnode_t *kdNode, coords_t coords, FILE *writeFile) {

    assert(kdNode != NULL);
    assert(kdNode->head != NULL);
    llnode_t *linkedNode = kdNode->head;
    data_t *data = (data_t *)linkedNode->data;

    /* Write data from the head node into the file */
    fprintf(writeFile, "%.12g %.12g-> ", coords.x, coords.y);
    fprintf(writeFile, "Census year: %s || ", data->year);
    fprintf(writeFile, "Block ID: %s || ", data->blockID);
    fprintf(writeFile, "Property ID: %s || ", data->propertyID);
    fprintf(writeFile, "Base property ID: %s || ", data->baseID);
    fprintf(writeFile, "CLUE small area: %s || ", data->smallArea);
    fprintf(writeFile, "Trading Name: %s || ", data->name);
    fprintf(writeFile, "Industry (ANZSIC4) code: %s || ",
     data->industryCode);
    fprintf(writeFile, "Industry (ANZSIC4) description: %s || ",
     data->industryDesc);
    fprintf(writeFile, "x coordinate: %s || ", data->xCoords);
    fprintf(writeFile, "y coordinate: %s || ", data->yCoords);
    fprintf(writeFile, "Location: %s || ", data->location);
    fprintf(writeFile, "\n");

    /* Recursively write formatted lines to the writeFile until the end of the 
    linked list */
    while (linkedNode->next != NULL) {
        linkedNode = linkedNode->next;
        data = (data_t *)linkedNode->data;
        fprintf(writeFile, "%.12g %.12g --> ", coords.x, coords.y);
        fprintf(writeFile, "Census year: %s || ", data->year);
        fprintf(writeFile, "Block ID: %s || ", data->blockID);
        fprintf(writeFile, "Property ID: %s || ", data->propertyID);
        fprintf(writeFile, "Base property ID: %s || ", data->baseID);
        fprintf(writeFile, "CLUE small area: %s || ", data->smallArea);
        fprintf(writeFile, "Trading Name: %s || ", data->name);
        fprintf(writeFile, "Industry (ANZSIC4) code: %s || ",
         data->industryCode);
        fprintf(writeFile, "Industry (ANZSIC4) description: %s || ",
         data->industryDesc);
        fprintf(writeFile, "x coordinate: %s || ", data->xCoords);
        fprintf(writeFile, "y coordinate: %s || ", data->yCoords);
        fprintf(writeFile, "Location: %s || ", data->location);
        fprintf(writeFile, "\n");
    }
    fprintf(writeFile, "\n");
    return;
}
/************************************************************************/

/* Function to read in coordinates from std input and storing them into a
dynamically allocated array
*/
coords_t *read_input_coordinates(int *lines) {
    int buffer = COORDBUFFER, read = 0;
    coords_t *inputCoordinates = (coords_t *)malloc(sizeof(coords_t)*buffer);
    assert(inputCoordinates != NULL);

    /* Read in coordinates from input, doubling the buffer when required */
    while (scanf("%lf %lf\n", &(inputCoordinates[read].x),
                              &(inputCoordinates[read].y))==2) {
        if (read + 1 == buffer) {
            buffer *= 2;
            inputCoordinates = (coords_t *)realloc(inputCoordinates,
                                sizeof(coords_t)*buffer);
            assert(inputCoordinates != NULL);
        }
        read++;
    }

    /* Reallocate space for the dynamic array to the number of input coords */
    inputCoordinates = (coords_t *)realloc(inputCoordinates,
                        sizeof(coords_t)*read);
    assert(inputCoordinates != NULL);

    /* Updating pointer on how many coordinates were read then return the
    dynamic array*/
    *lines = read;
    return inputCoordinates;
}
/************************************************************************/

/* Function to recursively free the subtree in LRN (post order)
*/
void kdt_free_subtree(kdnode_t *node){
    
    if (node != NULL) {
        kdt_free_subtree(node->left);
        kdt_free_subtree(node->right);

        /* Free the linked list in the kd tree */
        llnode_t *linkedNode = node->head;
        data_t *linkedData;
        while (linkedNode != NULL) {
            linkedNode = node->head;
            linkedData = (data_t *)linkedNode->data;
            node->head = linkedNode->next;
            free(linkedData->year);
            free(linkedData->blockID);
            free(linkedData->propertyID);
            free(linkedData->baseID);
            free(linkedData->smallArea);
            free(linkedData->industryCode);
            free(linkedData->industryDesc);
            free(linkedData->xCoords);
            free(linkedData->yCoords);
            free(linkedData->location);
            free(linkedData->name);
            free(linkedData);
            free(linkedNode);
            linkedNode = node->head;
        }
        free(linkedNode);
        free(node);
    }
    return;
}
/************************************************************************/

/* Function to free KD Tree
*/
void kdt_free_tree(kdt_t *kdt) {
    assert(kdt != NULL);
    kdt_free_subtree(kdt->root);
    free(kdt);
}
/************************************************************************/

/* Function to create new KD Tree structure
*/
kdt_t* initialise_KDT(void) {
    kdt_t* kdt;
    kdt = (kdt_t *) malloc(sizeof(kdt_t));
    assert(kdt != NULL);

    /* Initialise the new KD Tree structure */
    kdt->root = NULL;
    kdt->numElements = 0;
    return kdt;
}
/************************************************************************/

/* Function to read in coordinates from std input and storing them into a
dynamically allocated array
*/
radcoords_t *read_input_coordinates_radius(int *lines) {
    int buffer = COORDBUFFER, read = 0;
    radcoords_t *inputCoordinates = (radcoords_t *)malloc(sizeof(radcoords_t)*
                                                          buffer);
    assert(inputCoordinates != NULL);

    /* Read in coordinates and radius from input, doubling the buffer
    when required */
    while (scanf("%lf %lf %lf\n", &(inputCoordinates[read].x),
                                  &(inputCoordinates[read].y),
                                  &(inputCoordinates[read].r))==3) {
        if (read + 1 == buffer) {
            buffer *= 2;
            inputCoordinates = (radcoords_t *)realloc(inputCoordinates,
                                sizeof(radcoords_t)*buffer);
            assert(inputCoordinates != NULL);
        }
        read++;
    }

    /* Reallocate space for the dynamic array to the number of input coords */
    inputCoordinates = (radcoords_t *)realloc(inputCoordinates,
                        sizeof(radcoords_t)*read);
    assert(inputCoordinates != NULL);

    /* Updating pointer on how many coordinates were read then return the
    dynamic array*/
    *lines = read;
    return inputCoordinates;
}
/************************************************************************/

/* Recursively search for points in the KD Tree that satisfy radius 
requirents and print them out 
*/
void radius_search(kdnode_t *currNode, radcoords_t coords, int depth,
                double radius, int *numSearch, int *numPrint, FILE *writeFile) {

    /* Pointer counter for number of searches performed */
    *(numSearch) = *(numSearch) + 1;
    if (currNode == NULL){
        return;
    }
    data_t *nodeData = (data_t *)currNode->head->data;

    /* Calculate current node's distance to the coordinate and compare with
    previous closest, update the closest distance if closer */
    double centreDistance = sqrt(pow((atof(nodeData->xCoords)-coords.x),2)+
                                 pow((atof(nodeData->yCoords)-coords.y),2));

    /* If the centreDistance is within the radius specified, print the node */
    if (centreDistance <= radius) {
        *(numPrint) = *(numPrint) + 1;
        print_node_radius(currNode, coords, writeFile);
    }

    /* Find the current dimension of the kD tree (x or y) */
    int currDim = depth % KDDIM;

    if (currDim == XDIM) {

        /* The x coordinate is within the radius, check both children of the 
        node and print if they fall within the radius */
        if (fabs(atof(nodeData->xCoords)-coords.x) <= radius) {
            radius_search(currNode->left, coords, depth+1, radius,
                          numSearch, numPrint, writeFile);
            return radius_search(currNode->right, coords, depth+1, radius,
                                 numSearch, numPrint, writeFile);
        }
        else {
            /* The x coordinate of the node is out of the specified radius,
            check right child if the node's x coordinate is less than the 
            desired coordinate or the left child if it's greater */
            if (atof(nodeData->xCoords) < coords.x) {
                return radius_search(currNode->right, coords, depth+1, radius,
                                     numSearch, numPrint, writeFile);
            }
            else {
                return radius_search(currNode->left, coords, depth+1, radius,
                                     numSearch, numPrint, writeFile);
            }
        }
    }

    /* Same process for the y dimension of the KD tree */
    else {
        if (fabs(atof(nodeData->yCoords)-coords.y) <= radius) {
            radius_search(currNode->left, coords, depth+1, radius,
                          numSearch, numPrint, writeFile);
            return radius_search(currNode->right, coords, depth+1, radius,
                                 numSearch, numPrint, writeFile);
        }
        else {
            if (atof(nodeData->yCoords) < coords.y) {
                return radius_search(currNode->right, coords, depth+1, radius,
                                     numSearch, numPrint, writeFile);
            }
            else {
                return radius_search(currNode->left, coords, depth+1, radius,
                                     numSearch, numPrint, writeFile);
            }
        }
    }
}
/************************************************************************/

/* Function to print node in the radius from the KD Tree (new function since
the radius is printed after the coordinates)
*/
void print_node_radius(kdnode_t *kdNode, radcoords_t coords, FILE *writeFile) {

    assert(kdNode != NULL);
    assert(kdNode->head != NULL);
    llnode_t *linkedNode = kdNode->head;
    data_t *data = (data_t *)linkedNode->data;

    /* Write data from the head node into the file */
    fprintf(writeFile, "%.12g %.12g %.12g ", coords.x, coords.y, coords.r);
    fprintf(writeFile, "Census year: %s || ", data->year);
    fprintf(writeFile, "Block ID: %s || ", data->blockID);
    fprintf(writeFile, "Property ID: %s || ", data->propertyID);
    fprintf(writeFile, "Base property ID: %s || ", data->baseID);
    fprintf(writeFile, "CLUE small area: %s || ", data->smallArea);
    fprintf(writeFile, "Trading Name: %s || ", data->name);
    fprintf(writeFile, "Industry (ANZSIC4) code: %s || ",
     data->industryCode);
    fprintf(writeFile, "Industry (ANZSIC4) description: %s || ",
     data->industryDesc);
    fprintf(writeFile, "x coordinate: %s || ", data->xCoords);
    fprintf(writeFile, "y coordinate: %s || ", data->yCoords);
    fprintf(writeFile, "Location: %s || ", data->location);
    fprintf(writeFile, "\n");

    /* Recursively write formatted lines to the writeFile until the end of the 
    linked list */
    while (linkedNode->next != NULL) {
        linkedNode = linkedNode->next;
        data = (data_t *)linkedNode->data;
        fprintf(writeFile, "%.12g %.12g %.12g ", coords.x, coords.y, coords.r);
        fprintf(writeFile, "Census year: %s || ", data->year);
        fprintf(writeFile, "Block ID: %s || ", data->blockID);
        fprintf(writeFile, "Property ID: %s || ", data->propertyID);
        fprintf(writeFile, "Base property ID: %s || ", data->baseID);
        fprintf(writeFile, "CLUE small area: %s || ", data->smallArea);
        fprintf(writeFile, "Trading Name: %s || ", data->name);
        fprintf(writeFile, "Industry (ANZSIC4) code: %s || ",
         data->industryCode);
        fprintf(writeFile, "Industry (ANZSIC4) description: %s || ",
         data->industryDesc);
        fprintf(writeFile, "x coordinate: %s || ", data->xCoords);
        fprintf(writeFile, "y coordinate: %s || ", data->yCoords);
        fprintf(writeFile, "Location: %s || ", data->location);
        fprintf(writeFile, "\n");
    }
    return;
}

