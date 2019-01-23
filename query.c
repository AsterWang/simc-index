// query.c ... query scan functions
// part of SIMC signature files
// Manage creating and using Query objects
// Written by John Shepherd, September 2018

#include "defs.h"
#include "query.h"
#include "reln.h"
#include "tuple.h"
#include "bits.h"
#include "tsig.h"
#include "psig.h"
#include "bsig.h"

// check whether a query is valid for a relation
// e.g. same number of attributes

int checkQuery(Reln r, char *q)
{
	if (*q == '\0') return 0;
	char *c;
	int nattr = 1;
	for (c = q; *c != '\0'; c++)
		if (*c == ',') nattr++;
	return (nattr == nAttrs(r));
}

// take a query string (e.g. "1234,?,abc,?")
// set up a QueryRep object for the scan

Query startQuery(Reln r, char *q, char sigs)
{
	Query new = malloc(sizeof(QueryRep));
	assert(new != NULL);
	if (!checkQuery(r,q)) return NULL;
	new->rel = r;
	new->qstring = q;
	new->nsigs = new->nsigpages = 0;
	new->ntuples = new->ntuppages = new->nfalse = 0;
	new->pages = newBits(nPages(r));
	switch (sigs) {
	case 't': findPagesUsingTupSigs(new); break;
	case 'p': findPagesUsingPageSigs(new); break;
	case 'b': findPagesUsingBitSlices(new); break;
	default:  setAllBits(new->pages); break; //set all bits to 1
	}
	new->curpage = 0;
	return new;
}

// scan through selected pages (q->pages)
// search for matching tuples and show each
// accumulate query stats

void scanAndDisplayMatchingTuples(Query q)
{
    printf("scanning....\n");
	assert(q != NULL);
	int page_id;
    q->ntuppages = 0;
    q->ntuples = 0;
	for (page_id = 0; page_id < q->rel->params.npages; page_id ++){
		if (bitIsSet(q->pages, page_id) == FALSE)
			continue;
		Page p = getPage(q->rel->dataf, page_id);
        q->ntuppages ++;
		int item_id;
		for (item_id = 0; item_id < pageNitems(p); item_id ++){
            q->ntuples ++;
			Byte* item = addrInPage(p, item_id, q->rel->params.tupsize);
			Byte* t;
			t = malloc(sizeof(Byte) * tupSize(q->rel) + 1);
			memcpy(t,item,tupSize(q->rel));
            t[tupSize(q->rel)] = '\0';
			Bool result = tupleMatch(q->rel, q->qstring, (Tuple)t);
			if (result == TRUE){
				showTuple(q->rel, (Tuple)t);
            } else {
                q->nfalse ++;
            }
		}
	}
}


// print statistics on query

void queryStats(Query q)
{
	printf("# signatures read:   %d\n", q->nsigs);
	printf("# sig pages read:    %d\n", q->nsigpages);
	printf("# tuples examined:   %d\n", q->ntuples);
	printf("# data pages read:   %d\n", q->ntuppages);
	printf("# false match pages: %d\n", q->nfalse);
}

// clean up a QueryRep object and associated data

void closeQuery(Query q)
{
	free(q->pages);
	free(q);
}

