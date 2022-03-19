#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

/* 20% of the pages will have 80% of the references */
#define HIGH 20
#define FREQ 80
/* No. of Datapoints */
#define SAMPLES 20

typedef struct pte {
	int id;
	int present;
	int referenced;
	struct pte *next;
} pte;

void init(int *sequence, int refs, int pages){
	int high = (int)(pages*((float)HIGH/100));

	for(int i = 0; i < refs; i++) {
		if(rand()%100 < FREQ) {
			sequence[i] = rand() % high;
		} else {
			sequence[i] = high + rand()%(pages - high);
		}
	}
}

void clear_page_table(pte *page_table, int pages) {
	for(int i = 0; i < pages; i++) {
		page_table[i].present = 0;
	}
}

int simulate(int *seq, pte *table, int refs, int frames, int pgs) {

	int hits = 0;
	int allocated = 0;
	pte *last = NULL;

	for(int i = 0; i < refs; i++) {
		int next = seq[i];
		pte *entry = &table[next];

		if(entry->present == 1) {
			entry->referenced = 1;
			hits++;
		} else {
			if(allocated < frames) {
				allocated++;
				entry->present = 1;
				/* place the entry last in the list */
				if(last != NULL) {
					entry->next = last->next;
					last->next = entry;
				}
				if(last == NULL) {
					entry->next = entry;
				}
				last = entry;
			} else {
				pte *cand = last->next; 

				while(cand->referenced != 0) {
					cand->referenced = 0;
					last = cand;
					cand = cand->next;
				}
				cand->present = 0;
				cand->referenced = 0;

				entry->present = 1;
				entry->referenced = 0;
				/* place the entry last in the list */
				entry->next = cand->next;
				last->next = entry;
				last = entry;

			}
		}
	}
	return hits;
}

int main(int argc, char *argv[]){
	/* could be command line arguments */
	int refs=100000;
	int pages=100;
	
	int *sequence=(int*)malloc(refs*sizeof(int));
	
	pte *table = (pte *)malloc(pages*sizeof(pte));
	
	init(sequence,refs,pages);
	
	/* a small experiment to show that it works */
	/*
	for(int i=1; i<refs; i++){
		printf(" , %d", sequence[i]);
	}
	printf("\n");
	*/
	printf("# This is a benchmark of random replacement\n");
	printf("# %d page references\n", refs);
	printf("# %d pages \n", pages);
	printf("#\n#\n#frames\tratio\n");

	int frames;

	int incr = pages/SAMPLES;

	for(frames = incr; frames <= pages; frames += incr) {

		clear_page_table(table, pages);

		int hits = simulate(sequence, table, refs, frames, pages);

		float ratio = (float)hits/refs;

		printf("%d\t%.2f\n", frames, ratio);

	} 

	printf("\n");
	
	return 0;
}
