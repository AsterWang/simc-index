// tsig.c ... functions on Tuple Signatures (tsig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include <unistd.h>
#include <string.h>
#include "defs.h"
#include "tsig.h"
#include "reln.h"
#include "hash.h"
#include "bits.h"
#include "page.h"

// make a tuple signature

Bits makeTupleSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	Count m = tsigBits(r);
	Bits tsig = newBits(m);
	unsetAllBits(tsig);
	char **tupValues = tupleVals(r,t);
	int i;
	Count k = codeBits(r);
	for (i = 0; i < nAttrs(r); i ++){
        int nbits = 0;
		if (strcmp(tupValues[i],"?") != 0){
			Word r = hash_any(tupValues[i], (int)(strlen(tupValues[i])));
			srandom(r);
			while(nbits < k){
				int rand_number = random() % m;
				if (bitIsSet(tsig, rand_number) == FALSE){
					setBit(tsig, rand_number);
					nbits ++;
				}
			}
		}
	}
    freeVals(tupValues, nAttrs(r));
	return tsig;
}	

// find "matching" pages using tuple signatures

void findPagesUsingTupSigs(Query q)
{
    assert(q != NULL);
    //TODO
    int i, j;
    Bits querySig = makeTupleSig(q->rel, q->qstring);
    unsetAllBits(q->pages);
    Page p;
    Count tsigSize = tsigSize(q->rel);
    Count tsigBits = tsigBits(q->rel);
    Bits item_bits;
    Byte* item = malloc(sizeof(Byte) * tsigSize + 1);
    Byte* rest_items;
    int tPage_id = 0;
    for (i = 0; i < q->rel->params.tsigNpages; i ++){
        p = getPage(q->rel->tsigf, i);
        q->nsigpages ++;
        for (j = 0; j < pageNitems(p); j ++){
            q->nsigs ++;
            item_bits = newBits(tsigBits);
            rest_items = addrInPage(p, j,tsigSize);
            memcpy(item, rest_items, tsigSize);
            setBitstring(item, item_bits);
            if (isSubset(querySig,item_bits) == TRUE){
                tPage_id = iceil(i * q->rel->params.tsigPP + j + 1, 97) - 1;
                if(bitIsSet(q->pages, tPage_id) == FALSE){
                    q->ntuppages ++;
                    setBit(q->pages, tPage_id);
                }
            }
        }
    }
    free(item);
    q->ntuples = q->rel->params.ntups;
    showBits(q->pages); printf("\n");
}


int scanAndGetMatchingPageID(Query q, Bits tsig){
	assert(q != NULL);
	Count nbytes = getBitsBytes(tsig);
	File tsig_file = tsigFile(q->rel);
	int page_id = -1;
    Byte* t = malloc(sizeof(Byte) * nbytes + 1);
    Byte* item;
	for (page_id = 0; page_id < nBsigPages(q->rel); page_id ++){
		Page page = getPage(tsig_file, page_id);
		int item_id;
		for (item_id = 0; item_id < pageNitems(page); item_id ++){
			item = addrInPage(page, item_id, nbytes);
			memcpy(t, item, nbytes);
            int i;
            for (i = 0; i < nbytes; i ++){
                if (getBitstring(tsig, i) != t[i])
                    break;
            }
        }
	}
    free(t);
    return page_id;
}
