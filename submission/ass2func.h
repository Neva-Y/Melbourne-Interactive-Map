/* Header guards
*/
#ifndef ASS2FUNC
#define ASS2FUNC

/************************************************************************/

/* Hash defines for program functionality 
*/
#define MAXCHAR 128     /* Maximum number of characters in a single field */
#define NUMFIELDS 11    /* Number of data fields for a line */
#define EVENQUOTE 2     /* Number of quote marks for the final location field */
#define INVALIDQUOTES 4 /* Invalid number of quotation marks in a field */
#define CSVFILE 1       /* Argv access to name of the data file */
#define OUTPUTFILE 2    /* Argv access to name of the write file */
#define SEARCHRADIUS 3  /* Argv access to the radius search value */
#define KDDIM 2         /* Dimension of the K-D Tree */
#define ROOTDEPTH 1     /* Depth of the root of the K-D Tree */
#define COORDBUFFER 10  /* Initial buffer for the number of coordinates to read */
#define XDIM 1          /* x Dimension of the K-D Tree */

/* Hash defines for order of the struct for Melbourne CLUE
*/
#define YEAR 0
#define BLOCKID 1
#define PROPERTYID 2
#define BASEID 3
#define SMALLAREA 4
#define NAME 5
#define INDUSTRYCODE 6
#define INDUSTRYDESC 7
#define XCOORDS 8
#define YCOORDS 9
#define LOCATION 10


/************************************************************************/

/* Defining the data fields for each Melbourne CLUE
*/
typedef struct {
    char *year;
    char *blockID;
    char *propertyID;
    char *baseID;
    char *smallArea;
    char *name;
    char *industryCode;
    char *industryDesc;
    char *xCoords;
    char *yCoords;
    char *location;
} data_t;

/* Defining struct for coordinate pairs
*/
typedef struct {
    double x;
    double y;
} coords_t;

/* Defining struct for coordinate pairs and a radius
*/
typedef struct {
    double x;
    double y;
    double r;
} radcoords_t;

/* Defining the linked list node type 
*/
typedef struct llnode llnode_t;

struct llnode {
    data_t *data;
    llnode_t *next;
};


/* Defining the KD node type 
*/
typedef struct kdnode kdnode_t;

struct kdnode {
    llnode_t *head;
    kdnode_t *left;
    kdnode_t *right;
};

/* Defining the KD tree search type 
*/
typedef struct {
    int numElements;
    kdnode_t *root;
} kdt_t;

/************************************************************************/

/* Function prototypes 
*/
void invalid_data_error(char *msg, int line);
void initial_data_processing(FILE *readFile);
int read_single_data_line(kdt_t *kdt, FILE *readFile);
void allocate_struct(data_t *lineData, char **dataField);
kdnode_t *new_KDNode(data_t *d);
void insert_duplicate(kdnode_t *rootNode, data_t *d);
kdnode_t *insert_KDnode(kdnode_t *rootNode, data_t *d, int depth);
kdnode_t *search_KDtree(kdnode_t *prevNode, kdnode_t *currNode, coords_t coords,
                        int depth, double prevDist, int *numSearch);
kdnode_t *closer_distance(kdnode_t *nextNode, kdnode_t *currNode, 
                          coords_t coords);
coords_t *read_input_coordinates(int *linesRead);
void print_node(kdnode_t *kdNode, coords_t coords, FILE *writeFile);
void kdt_free_subtree(kdnode_t *node);
void kdt_free_tree(kdt_t *kdt);
kdt_t* initialise_KDT(void);
radcoords_t *read_input_coordinates_radius(int *lines);
void radius_search(kdnode_t *currNode, radcoords_t coords, int depth,
                 double radius, int *numSearch, int *numPrint, FILE *writeFile);
void print_node_radius(kdnode_t *kdNode, radcoords_t coords, FILE *writeFile);

/************************************************************************/
#endif