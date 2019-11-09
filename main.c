
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Darwin.h"

#define BUFFER_SIZE 1024  /* Maximum length of a line in input file */

/* Uncomment the following line to enable debugging prints 
 * or comment to disable it */
#define DEBUG

#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define DPRINT(...)
#endif /* DEBUG */


/**
 * @brief Optional function to initialize data structures that 
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize (void)
{
	return 1;
}


/**
 * @brief insert new species in Species' list with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_species (int sid)
{
    struct Species* new_node=(struct Species*)malloc(sizeof(struct Species));
    struct Species* curr;
    new_node->sid=sid;
    //if list empty
    if (Species_head == NULL)
    {
        Species_head = new_node;
    }
        // if the node is to be inserted at the beginning
        // of the doubly linked list
    else if ((Species_head)->sid >= new_node->sid) {
        new_node->next = Species_head;
        new_node->next->prev = new_node;
        Species_head = new_node;
    }
    else
    {
        /* Locate the node before the point of insertion */
        curr = Species_head;
        // locate the node after which the new node
        // is to be inserted
        while (curr->next != NULL &&  curr->next->sid < new_node->sid)
            curr = curr->next;

        /* Make the appropriate links */
        new_node->next = curr->next;

        // if the new node is not inserted
        // at the end of the list
        if (curr->next != NULL)
            new_node->next->prev = new_node;

        curr->next = new_node;
        new_node->prev = curr;
    }
    curr = Species_head;
    while (curr->next != NULL){
        curr=curr->next;
    }
    Species_tail=curr;
    print_populations();
    return 1;
}

/**
 * @brief insert new population with ID <gid> in Species' with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_population(int gid, int sid, int cid){
    struct Population * new_node=(struct Population*)malloc(sizeof(struct Population));
    struct Population * curr;
    struct Species *speciesHead=Species_head;
    new_node->gid=gid;
    new_node->sid=sid;
    new_node->cid=cid;
    while (speciesHead->sid!=sid) {
        speciesHead = speciesHead->next;
    }
    //if list is empty or it goes at head
    if(speciesHead->Population_head==NULL || speciesHead->Population_head->gid >= new_node->gid){
        new_node->next=speciesHead->Population_head;
        speciesHead->Population_head=new_node;
    }
    else{
        curr=speciesHead->Population_head;
        while (curr->next!=NULL && curr->next->gid < new_node->gid){
            curr=curr->next;
        }
        new_node->next=curr->next;
        curr->next=new_node;
    }
    curr=speciesHead->Population_head;
    while (curr->next!=NULL){
        curr=curr->next;
    }
    speciesHead->Population_tail=curr;
    print_populations();
	return 1;
}
/**
 * @brief delete species with ID <sid> and its populations
 *
 * @return 1 on success
 *         0 on failure
 */
int delete_species(int sid){
    struct Species* curr=Species_tail;
    while (curr->sid!=sid){
        curr=curr->prev;
    }
    if(curr->next!=NULL)
        curr->next->prev = curr->prev;
    if(curr->prev!=NULL)
        curr->prev->next = curr->next;
    free(curr);
    printf("SPECIES\n  ");
    print_populations();
    printf("CONTINENTS\n  ");
    print_continents();
    return 1;
}

/**
 * @brief merge species with IDs <sid1> and <sid2> into new species with ID <sid3>
 *
 * @return 1 on success
 *         0 on failure
 */
int merge_species(int sid1, int sid2, int sid3){
    struct Species* s1=Species_head;
    struct Species* s2=Species_head;
    insert_species(sid3);
    while (s1->sid!=sid1){
        s1=s1->next;
    }
    while (s2->sid!=sid2){
        s2=s2->next;
    }
    struct Population* ps1=s1->Population_head;
    struct Population* ps2=s2->Population_head;
    while (ps1!=NULL){
        insert_population(ps1->gid,sid3,ps1->cid);
        ps1=ps1->next;
    }
    while (ps2!=NULL){
        insert_population(ps2->gid,sid3,ps2->cid);
        ps2=ps2->next;
    }
    delete_species(sid1);
    delete_species(sid2);
    print_populations();
    return 1;
}

/**
 * @brief Distribute species' population in continents array
 *
 * @return 1 on success
 *         0 on failure
 */
int distribute(){
    for (int i = 0; i <= 4 ; ++i) {
        continents[i]=(struct ContinentList*)malloc(sizeof(struct ContinentList));
        continents[i]->sentinel=(struct ContinentPopulation*)malloc(sizeof(struct ContinentPopulation));
        continents[i]->sentinel->gid=-1;
    }
    struct Species* species=Species_head;
    while (species!=NULL){
        struct Population* population=species->Population_head;
        while (population!=NULL){
            struct ContinentPopulation* new_population=(struct ContinentPopulation*)malloc(sizeof(struct ContinentPopulation));
            new_population->gid=population->gid;
            new_population->next = continents[population->cid]->populationList;
            continents[population->cid]->populationList=new_population;
            population=population->next;
        }
        species=species->next;
    }
    print_continents();
	return 1;
}

/**
 * @brief delete population with ID <gid> from species with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int delete_population(int gid, int sid){
    struct Species* species=Species_head;
    while (species->sid!=sid){
        species=species->next;
    }
    struct Population * population=species->Population_head;
    struct Population *prevP=population;
    if (population != NULL && population->gid == gid)
    {
        species->Population_head = population->next;   // Changed head
    }
    while (population != NULL && population->gid != gid)
    {
        prevP = population;
        population = population->next;
    }
    if(continents[population->cid]!=NULL) {
        struct ContinentPopulation *continentPopulation = continents[population->cid]->populationList;
        struct ContinentPopulation *prevC = continentPopulation;
        if (continentPopulation != NULL && continentPopulation->gid == gid) {
            continents[population->cid]->populationList = continentPopulation->next;   // Changed head
        }
        while (continentPopulation != NULL && continentPopulation->gid != gid) {
            prevC = continentPopulation;
            continentPopulation = continentPopulation->next;
        }
        prevC->next=continentPopulation->next;
        free(continentPopulation);
    }
    prevP->next = population->next;
    int savecid=population->cid;
    free(population);
    struct Population* pP=species->Population_head;
    printf("SPECIES\n  ");
    printf("<S:%d>",species->sid);
    while (pP!=NULL){
        printf("<G:%d>,",pP->gid);
        pP=pP->next;
    }
    printf("CONTINENTS\n  ");
    printf("Continent %d: ",savecid);
    struct ContinentPopulation* curr=continents[savecid]->populationList;
    while (curr!=NULL){
        printf("<%d>,",curr->gid);
        curr=curr->next;
    }
    printf("\n");
    return 1;
}

/**
 * @brief Remaining species evolve into homo sapiens.
 *
 * @return 1 on success
 *         0 on failure
 */
int evolution(){
    struct Species* species=Species_head;
    struct Population* populationHead=species->Population_head;
    struct Population* populationTail=species->Population_tail;
    struct Population* curr=populationHead;
    while (species!=NULL){
        if(species->next!=NULL) {
            populationHead->next_species=species->next->Population_head;
            populationTail->next=species->next->Population_head;
            populationHead=populationHead->next_species;
            populationTail=populationTail->next;
        }
        species=species->next;
    }
    hs_L=curr;
    print_homo_sapiens();
	return 1;
}

/**
 * @brief Gather statistics on species from homo_sapiens list
 *
 * @return 1 on success
 *         0 on failure
 */
int species_statistics(){
    int count=0;
    struct Population* hs=hs_L;
    while (hs!=NULL){
        count++;
        hs=hs->next_species;
    }
    printf("Homo Sapiens Species: %d\n",count);
    return 1;
}

/**
 * @brief Gather statistics on population belonging to species with ID <sid> from homo_sapiens list
 *
 * @return 1 on success
 *         0 on failure
 */
int population_statistics(int sid){
    struct Population* hs =hs_L;
    int count=0;
    while (hs->next_species!=NULL && hs->sid!=sid){
        hs=hs->next_species;
    }
    while (hs!=NULL && hs->sid==sid){
        count++;
        hs=hs->next;
    }
    printf("Homo Sapiens populations: %d\n",count);
	return 1;
}

/**
 * @brief Print species' list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_species(){
    struct Species* curr=Species_head;
    while (curr!= NULL) {
        printf(" <S:%d>", curr->sid);
        curr = curr->next;
    }
    printf("\n");
    return 1;
}

/**
 * @brief Print species' list with population lists
 *
 * @return 1 on success
 *         0 on failure
 */
int print_populations(){
    struct Species* curr=Species_head;
    while (curr!= NULL) {
        struct Population* pp=curr->Population_head;
        printf(" <S:%d>\n    ", curr->sid);
        while (pp != NULL) {
        printf("<G:%d, C:%d>", pp->gid,pp->cid);
        pp=pp->next;
        }
        printf("\n");
        curr = curr->next;
    }
    return 1;
}

/**
 * @brief Print continents array with each continent's population list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_continents(){
    for(int i=0;i<4;i++){
        struct ContinentPopulation* curr=continents[i]->populationList;
        printf("Continent %d: ",i);
        while (curr!=NULL){
            printf("%d ",curr->gid);
            curr=curr->next;
        }
        printf("\n");
    }
	return 1;
}

/**
 * @brief Print homo_sapiens list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_homo_sapiens(){
    struct Population* hs=hs_L;
    printf("Homo Sapiens: ");
    while (hs!=NULL){
        printf("<%d,%d>",hs->gid,hs->sid);
        if(hs->next!=NULL && hs->next->sid!=hs->sid)
            printf("|");
        hs=hs->next;
    }
    printf("\n");
	return 1;
}

/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */
int free_all(void)
{

	return 1;
}


/**
 * @brief The main function
 *
 * @param argc Number of arguments
 * @param argv Argument vector
 *
 * @return 0 on success
 *         1 on failure
 */
int main(int argc, char** argv)
{
	FILE *fin = NULL;
	char buff[BUFFER_SIZE], event;

	/* Check command buff arguments */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_file> \n", argv[0]);
		return EXIT_FAILURE;
	}

	/* Open input file */
	if (( fin = fopen(argv[1], "r") ) == NULL ) {
		fprintf(stderr, "\n Could not open file: %s\n", argv[1]);
		perror("Opening test file\n");
		return EXIT_FAILURE;
	}

	/* Initializations */
	initialize();

	/* Read input file buff-by-buff and handle the events */
	while (fgets(buff, BUFFER_SIZE, fin)) {

		DPRINT("\n>>> Event: %s", buff);

		switch(buff[0]) {

			/* Comment */
			case '#':
				break;

				/* Insert Species
				 * S <sid> */
			case 'S':
				{
					int sid;

					sscanf(buff, "%c %d", &event, &sid);
					DPRINT("%c %d\n", event, sid);

					if (insert_species(sid)) {
						DPRINT("%c %d succeeded\n", event, sid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, sid);
					}

					break;
				}

				/* Insert population
				 * G <gid> <sid> <cid> */
			case 'G':
				{
					int gid, sid, cid;

					sscanf(buff, "%c %d %d %d", &event, &gid, &sid, &cid);
					DPRINT("%c %d %d %d\n", event, gid, sid, cid);

					if (insert_population(gid, sid, cid)) {
						DPRINT("%c %d %d %d succeeded\n", event, gid, sid, cid);
					} else {
						fprintf(stderr, "%c %d %d %d failed\n", event, gid, sid, cid);
					}

					break;
				}

				/* Merge species
				 * M <sid1> <sid2> <sid3> */
			case 'M':
				{
					int sid1, sid2, sid3;

					sscanf(buff, "%c %d %d %d", &event, &sid1, &sid2, &sid3);
					DPRINT("%c %d %d %d\n", event, sid1, sid2, sid3);

					if (merge_species(sid1, sid2, sid3)) {
						DPRINT("%c %d %d %d succeeded\n", event, sid1, sid2, sid3);
					} else {
						fprintf(stderr, "%c %d %d %d failed\n", event, sid1, sid2, sid3);
					}

					break;
				}

				/* Distribute species
				 * D */
			case 'D':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (distribute()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Delete population
				 * K <gid> <sid> */
			case 'K':
				{
					int gid, sid;

					sscanf(buff, "%c %d %d", &event, &gid, &sid);
					DPRINT("%c %d %d\n", event, gid, sid);

					if (delete_population(gid, sid)) {
						DPRINT("%c %d %d succeeded\n", event, gid, sid);
					} else {
						fprintf(stderr, "%c %d %d failed\n", event, gid, sid);
					}

					break;
				}

				/* Delete species
				 * F <sid> */
			case 'F':
				{
					int sid;

					sscanf(buff, "%c %d", &event, &sid);
					DPRINT("%c %d\n", event, sid);

					if (delete_species(sid)) {
						DPRINT("%c %d succeeded\n", event, sid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, sid);
					}

					break;
				}

				/* Evolution to homo sapiens
				 * E */
			case 'E':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (evolution()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Species' statistics
				 * N */
			case 'N':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (species_statistics()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Population statistics
				 * J <sid> */
			case 'J':
				{
					int sid;

					sscanf(buff, "%c %d", &event, &sid);
					DPRINT("%c %d\n", event, sid);

					if (population_statistics(sid)) {
						DPRINT("%c %d succeeded\n", event, sid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, sid);
					}

					break;
				}

				/* Print species
				 * P */
			case 'P':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_species()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print populations
				 * X */
			case 'X':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_populations()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print continents
				 * C */
			case 'C':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_continents()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print homo sapiens
				 * W */
			case 'W':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_homo_sapiens()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Empty line */
			case '\n':
				break;

				/* Ignore everything else */
			default:
				DPRINT("Ignoring buff: %s \n", buff);

				break;
		}
	}

	free_all();
	return (EXIT_SUCCESS);
}
