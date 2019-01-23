// psig.c ... functions on page signatures (psig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
#include "tsig.h"
#include "hash.h"

Bits makePageSig(Reln r, Tuple t)
{
    assert(r != NULL && t != NULL);
    Count m = psigBits(r);
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

void findPagesUsingPageSigs(Query q)
{
	assert(q != NULL);
    Bits querySig = makePageSig(q->rel, q->qstring);
	unsetAllBits(q->pages);
    Page p;
    Bits item_bits;
    Count psigSize = psigSize(q->rel);
    Count psigBits = psigBits(q->rel);
    int i, j;
    int page_id = 0;
    for (i = 0; i < q->rel->params.psigNpages; i ++){
        p = getPage(q->rel->psigf, i);
        q->nsigpages ++;
        for (j = 0; j < pageNitems(p); j ++){
            item_bits = newBits(psigBits);
            Byte* rest_items = addrInPage(p, j,psigSize);
            Byte* item = malloc(sizeof(Byte) * psigSize + 1);
            memcpy(item, rest_items, psigSize);
            setBitstring(item, item_bits);
            if (isSubset(querySig,item_bits) == TRUE){
                page_id = q->nsigs;
                if(bitIsSet(q->pages, page_id) == FALSE){
                    q->ntuppages ++;
                    setBit(q->pages, page_id);
                }
            }
            q->nsigs ++;
        }
    }
}

