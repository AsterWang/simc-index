// tuple.c ... functions on tuples
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include "defs.h"
#include "tuple.h"
#include "reln.h"
#include "hash.h"
#include "bits.h"

// reads/parses next tuple in input

Tuple readTuple(Reln r, FILE *in)
{
	char line[MAXTUPLEN];
	if (fgets(line, MAXTUPLEN-1, in) == NULL)
		return NULL;
	line[strlen(line)-1] = '\0';
	// count fields
	// cheap'n'nasty parsing
	char *c; int nf = 1;
	for (c = line; *c != '\0'; c++)
		if (*c == ',') nf++;
	// invalid tuple
	if (nf != nAttrs(r)) return NULL;
	return strdup(line); // needs to be free'd sometime
}

// extract values into an array of strings

char **tupleVals(Reln r, Tuple t)
{
	char **vals = malloc(nAttrs(r)*sizeof(char *));
	char *c = t, *c0 = t;
	int i = 0;
	for (;;) {
		while (*c != ',' && *c != '\0') c++;
		if (*c == '\0') {
			// end of tuple; add last field to vals
			vals[i++] = strdup(c0);
			break;
		}
		else {
			assert(*c == ',');
			// end of next field; add to vals
			*c = '\0';
			vals[i++] = strdup(c0);
			*c = ',';
			c++; c0 = c;
		}
	}
	return vals;
}

// release memory used for attribute values

void freeVals(char **vals, int nattrs)
{
	int i;
	for (i = 0; i < nattrs; i++) free(vals[i]);
}

// compare two tuples (allowing for "unknown" values)

Bool tupleMatch(Reln r, Tuple t1, Tuple t2)
{
	assert(r != NULL && t1 != NULL && t2 != NULL);
	char **v1 = tupleVals(r, t1);
	char **v2 = tupleVals(r, t2);
	Bool match = TRUE;
	int i; int n = nAttrs(r);
	for (i = 0; i < n; i++) {
		if (v1[i][0] == '?' || v2[i][0] == '?') continue;
		if (strcmp(v1[i],v2[i]) == 0) continue;
		match = FALSE;
	}
	freeVals(v1,n); freeVals(v2,n);
	return match;
}

// insert a tuple into a page
// returns OK status if successful
// returns NOT_OK if not enough room
Status addTupleToPage(Reln r, Page p, Tuple t)
{
	if (pageNitems(p) == maxTupsPP(r)) return NOT_OK;
	int size = tupSize(r);
	Byte *addr = addrInPage(p, pageNitems(p), size);
	memcpy(addr, t, size);
	addOneItem(p);
	return OK;
}

Status addTupSigToPage(Reln r, Page p, Byte* tSig){
    if (pageNitems(p) == maxTsigsPP(r)) return NOT_OK;
    int size = tsigSize(r);
    Byte *addr = addrInPage(p, pageNitems(p), size);
    memcpy(addr, tSig, sizeof(Byte) * size);
    addOneItem(p);
    return OK;
}
Status mergePageSigToPage(Reln r, Page p, Byte* pSig){
    if (pageNitems(p) == maxPsigsPP(r)) return NOT_OK;
    int size = psigSize(r);
    //if the number of data pages is larger than #page signatures, we need to use a new page signature
    if (r->params.npages == r->params.npsigs + 1){
        r->params.npsigs ++;
        addOneItem(p);
        printf("add one item, current page item number is %d\n", pageNitems(p));
    }
    Byte* old_pSig = malloc(sizeof(Byte) * (size + 1));
//    printf("pageNitems(p) - 1 = %d\n", pageNitems(p) - 1);
    Byte* addr = addrInPage(p, pageNitems(p)-1, size);
    memcpy(old_pSig, addr, sizeof(Byte) * size);
    old_pSig[size] = '\0';
    int i;
//    int c = 0;
    for (i = 0; i < size; i ++){
        old_pSig[i] = pSig[i] | old_pSig[i];
//        if (old_pSig[i] != '\0'){
//            printf("pSig[%d] = %d ",i, pSig[i]);
//            printf("old_pSig[%d] = %d\n", i,old_pSig[i]);
//        }
//        if (old_pSig[i] != '\0')
//            c ++;
    }
//    printf("not null number is %d\n", c);
    memcpy(addr, old_pSig, sizeof(Byte) * size);
    return OK;
}


// get data for i'th tuple in Page

Tuple getTupleFromPage(Reln r, Page p, int i)
{
	assert(r != NULL && p != NULL);
	assert(i <= pageNitems(p));
	int size = tupSize(r);
	Byte *addr = addrInPage(p, i, size);
	Tuple tup = malloc(size+1);
	memcpy(tup, addr, size);
	tup[size] = '\0';
	return tup;
}

// display printable version of tuple on stdout

void showTuple(Reln r, Tuple t)
{
	printf("%s\n",t);
}
