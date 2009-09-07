/* deflate.h -- internal compression state
 * Copyright (C) 1995-1998 Jean-loup Gailly
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

#ifndef _DEFLATE_H
#define _DEFLATE_H

#define INBUFLEN    0x80000
#define OUTBUFLEN   0x40000

#define Z_BUFSIZE	16384

#define LENGTH_CODES 29
#define LITERALS  256
#define L_CODES (LITERALS+1+LENGTH_CODES)
#define D_CODES   30
#define BL_CODES  19
#define HEAP_SIZE (2*L_CODES+1)
#define MAX_BITS 15

#define INIT_STATE    42
#define BUSY_STATE   113
#define FINISH_STATE 666
/* Stream status */

/* Maximum value for memLevel in deflateInit2 */
#define MAX_MEM_LEVEL 9
#define MAX_WBITS   15 /* 32K LZ77 window */

#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1 /* will be removed, use Z_SYNC_FLUSH instead */
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4
/* Allowed fluxh values; see deflate() below for details */

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)

#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)

#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_DEFAULT_STRATEGY    0


#define Z_DEFLATED   8
#define Z_NULL		0  /* for initializing zalloc, zfree, opaque */

        /* common constants */
#  define DEF_MEM_LEVEL 9

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */


                     /* Type declarations */
typedef unsigned char  Byte;  /* 8 bits */
typedef unsigned int   uInt;  /* 16 bits or more */
typedef unsigned long  uLong; /* 32 bits or more */
typedef unsigned short ush;

typedef Byte* (*alloc_func) (Byte* opaque, uInt items, uInt size);
typedef void   (*free_func)  (Byte* opaque, Byte* address);

typedef struct z_stream_s {
    Byte    *next_in;  /* next input byte */
    uInt     avail_in;  /* number of bytes available at next_in */
    uLong    total_in;  /* total nb of input bytes read so far */

    Byte    *next_out; /* next output byte should be put there */
    uInt     avail_out; /* remaining free space at next_out */
    uLong    total_out; /* total nb of bytes output so far */

    struct internal_state  *state; /* not visible by applications */

    alloc_func zalloc;  /* used to allocate the internal state */
    free_func  zfree;   /* used to free the internal state */
    Byte*     opaque;  /* private data object passed to zalloc and zfree */

    uLong   adler;      /* adler32 value of the uncompressed data */
    uLong   reserved;   /* reserved for future use */
} z_stream;


/* Diagnostic functions */

#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)

typedef uLong ( *check_func) (uLong check, const Byte *buf,uInt len);
//Byte* zcalloc (Byte* opaque, unsigned items, unsigned size);
//void   zcfree  (Byte* opaque, Byte* ptr);

#define ZALLOC(strm, items, size) (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (Byte*)(addr))
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}

/* Data structure describing a single value and its code string. */
typedef struct ct_data_s {
    union {
        ush  freq;       /* frequency count */
        ush  code;       /* bit string */
    } fc;
    union {
        ush  dad;        /* father node in Huffman tree */
        ush  len;        /* length of bit string */
    } dl;
}  ct_data;

#define Freq fc.freq
#define Code fc.code
#define Dad  dl.dad
#define Len  dl.len

typedef struct static_tree_desc_s  static_tree_desc;

typedef struct tree_desc_s {
    ct_data *dyn_tree;           /* the dynamic tree */
    int     max_code;            /* largest code with non zero frequency */
    static_tree_desc *stat_desc; /* the corresponding static tree */
}  tree_desc;

typedef struct internal_state {
    z_stream* strm;      /* pointer back to this zlib stream */
    int   status;        /* as the name implies */
    Byte *pending_buf;  /* output still pending */
    uLong   pending_buf_size; /* size of pending_buf */
    Byte *pending_out;  /* next pending byte to output to the stream */
    int   pending;       /* nb of bytes in the pending buffer */
    int   noheader;      /* suppress zlib header and adler32 */
    Byte  method;        /* STORED (for zip only) or DEFLATED */
    int   last_fluxh;    /* value of fluxh param for previous deflate call */

    /* used by deflate.c: */
    uInt  w_size;        /* LZ77 window size (32K by default) */
    uInt  w_bits;        /* log2(w_size)  (8..16) */
    uInt  w_mask;        /* w_size - 1 */

    Byte *window;
    uLong window_size;
    ush *prev;
    ush *head; /* Heads of the hash chains or NIL. */

    uInt  ins_h;          /* hash index of string to be inserted */
    uInt  hash_size;      /* number of elements in hash table */
    uInt  hash_bits;      /* log2(hash_size) */
    uInt  hash_mask;      /* hash_size-1 */

    uInt  hash_shift;

    long block_start;
    /* Window position at the beginning of the current output block. Gets
     * negative when the window is moved backwards.
     */

    uInt match_length;           /* length of best match */
    uInt prev_match;             /* previous match */
    int match_available;         /* set if previous match exists */
    uInt strstart;               /* start of string to insert */
    uInt match_start;            /* start of matching string */
    uInt lookahead;              /* number of valid bytes ahead in window */

    uInt prev_length;
    uInt max_chain_length;

    uInt max_lazy_match;

#define max_insert_length  max_lazy_match
    /* Insert new strings in the hash table only if the match length is not
     * greater than this length. This saves time but degrades compression.
     * max_insert_length is used only for compression levels <= 3.
     */

    int level;    /* compression level (1..9) */
    int strategy; /* favor or force Huffman coding*/

    uInt good_match;
    /* Use a faster search when the previous match is longer than this */

    int nice_match; /* Stop searching when current match exceeds this */

                /* used by trees.c: */
    /* Didn't use ct_data typedef below to supress compiler warning */
    struct ct_data_s dyn_ltree[HEAP_SIZE];   /* literal and length tree */
    struct ct_data_s dyn_dtree[2*D_CODES+1]; /* distance tree */
    struct ct_data_s bl_tree[2*BL_CODES+1];  /* Huffman tree for bit lengths */

    struct tree_desc_s l_desc;               /* desc. for literal tree */
    struct tree_desc_s d_desc;               /* desc. for distance tree */
    struct tree_desc_s bl_desc;              /* desc. for bit length tree */

    ush bl_count[MAX_BITS+1];
    /* number of codes at each bit length for an optimal tree */

    int heap[2*L_CODES+1];      /* heap used to build the Huffman trees */
    int heap_len;               /* number of elements in the heap */
    int heap_max;               /* element of largest frequency */
    /* The sons of heap[n] are heap[2*n] and heap[2*n+1]. heap[0] is not used.
     * The same heap array is used to build all trees.
     */

    Byte depth[2*L_CODES+1];
    /* Depth of each subtree used as tie breaker for trees of equal frequency
     */

    Byte *l_buf;          /* buffer for literals or lengths */

    uInt  lit_bufsize;
    uInt last_lit;      /* running index in l_buf */

    ush *d_buf;
    /* Buffer for distances. To simplify the code, d_buf and l_buf have
     * the same number of elements. To use different lengths, an extra flag
     * array would be necessary.
     */

    uLong opt_len;        /* bit length of current block with optimal trees */
    uLong static_len;     /* bit length of current block with static trees */
    uInt matches;       /* number of string matches in current block */
    int last_eob_len;   /* bit length of EOB code for last block */

    ush bi_buf;
    /* Output buffer. bits are inserted starting at the bottom (least
     * significant bits).
     */
    int bi_valid;
    /* Number of valid bits in bi_buf.  All bits above the last valid bit
     * are always zero.
     */

}  deflate_state;

/* Output a byte on the stream.
 * IN assertion: there is enough room in pending_buf.
 */
#define put_byte(s, c) {s->pending_buf[s->pending++] = (c);}

#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1. */

#define MAX_BL_BITS 7
#define END_BLOCK 256
#define REP_3_6      16
#define REPZ_3_10    17
#define REPZ_11_138  18

 const int extra_lbits[LENGTH_CODES] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
 const int extra_dbits[D_CODES] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
 const int extra_blbits[BL_CODES]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,7};
 const Byte bl_order[BL_CODES]   = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};

#define Buf_size (8 * 2*sizeof(char))
/* Number of bits used within bi_buf. (bi_buf might be implemented on
 * more than 16 bits on some systems.)
 */

#define ALLOC(size) malloc(size)
#define TRYFREE(p) {if (p) free(p);}

//#define ALLOC(size) GlobalLock(GlobalAlloc(GHND,size));
//#define TRYFREE(p) {if (p) GlobalFree(p);}


/* ===========================================================================
 * Local data. These are initialized only once.
 */
#define DIST_CODE_LEN  512 /* see definition of array dist_code below */

/* The static literal tree. Since the bit lengths are imposed, there is no
 * need for the L_CODES extra codes used during heap construction. However
 * The codes 286 and 287 are needed to build a canonical tree (see _tr_init
 * below).
 */
const ct_data static_ltree[L_CODES+2] = {
{{ 12},{  8}}, {{140},{  8}}, {{ 76},{  8}}, {{204},{  8}}, {{ 44},{  8}},
{{172},{  8}}, {{108},{  8}}, {{236},{  8}}, {{ 28},{  8}}, {{156},{  8}},
{{ 92},{  8}}, {{220},{  8}}, {{ 60},{  8}}, {{188},{  8}}, {{124},{  8}},
{{252},{  8}}, {{  2},{  8}}, {{130},{  8}}, {{ 66},{  8}}, {{194},{  8}},
{{ 34},{  8}}, {{162},{  8}}, {{ 98},{  8}}, {{226},{  8}}, {{ 18},{  8}},
{{146},{  8}}, {{ 82},{  8}}, {{210},{  8}}, {{ 50},{  8}}, {{178},{  8}},
{{114},{  8}}, {{242},{  8}}, {{ 10},{  8}}, {{138},{  8}}, {{ 74},{  8}},
{{202},{  8}}, {{ 42},{  8}}, {{170},{  8}}, {{106},{  8}}, {{234},{  8}},
{{ 26},{  8}}, {{154},{  8}}, {{ 90},{  8}}, {{218},{  8}}, {{ 58},{  8}},
{{186},{  8}}, {{122},{  8}}, {{250},{  8}}, {{  6},{  8}}, {{134},{  8}},
{{ 70},{  8}}, {{198},{  8}}, {{ 38},{  8}}, {{166},{  8}}, {{102},{  8}},
{{230},{  8}}, {{ 22},{  8}}, {{150},{  8}}, {{ 86},{  8}}, {{214},{  8}},
{{ 54},{  8}}, {{182},{  8}}, {{118},{  8}}, {{246},{  8}}, {{ 14},{  8}},
{{142},{  8}}, {{ 78},{  8}}, {{206},{  8}}, {{ 46},{  8}}, {{174},{  8}},
{{110},{  8}}, {{238},{  8}}, {{ 30},{  8}}, {{158},{  8}}, {{ 94},{  8}},
{{222},{  8}}, {{ 62},{  8}}, {{190},{  8}}, {{126},{  8}}, {{254},{  8}},
{{  1},{  8}}, {{129},{  8}}, {{ 65},{  8}}, {{193},{  8}}, {{ 33},{  8}},
{{161},{  8}}, {{ 97},{  8}}, {{225},{  8}}, {{ 17},{  8}}, {{145},{  8}},
{{ 81},{  8}}, {{209},{  8}}, {{ 49},{  8}}, {{177},{  8}}, {{113},{  8}},
{{241},{  8}}, {{  9},{  8}}, {{137},{  8}}, {{ 73},{  8}}, {{201},{  8}},
{{ 41},{  8}}, {{169},{  8}}, {{105},{  8}}, {{233},{  8}}, {{ 25},{  8}},
{{153},{  8}}, {{ 89},{  8}}, {{217},{  8}}, {{ 57},{  8}}, {{185},{  8}},
{{121},{  8}}, {{249},{  8}}, {{  5},{  8}}, {{133},{  8}}, {{ 69},{  8}},
{{197},{  8}}, {{ 37},{  8}}, {{165},{  8}}, {{101},{  8}}, {{229},{  8}},
{{ 21},{  8}}, {{149},{  8}}, {{ 85},{  8}}, {{213},{  8}}, {{ 53},{  8}},
{{181},{  8}}, {{117},{  8}}, {{245},{  8}}, {{ 13},{  8}}, {{141},{  8}},
{{ 77},{  8}}, {{205},{  8}}, {{ 45},{  8}}, {{173},{  8}}, {{109},{  8}},
{{237},{  8}}, {{ 29},{  8}}, {{157},{  8}}, {{ 93},{  8}}, {{221},{  8}},
{{ 61},{  8}}, {{189},{  8}}, {{125},{  8}}, {{253},{  8}}, {{ 19},{  9}},
{{275},{  9}}, {{147},{  9}}, {{403},{  9}}, {{ 83},{  9}}, {{339},{  9}},
{{211},{  9}}, {{467},{  9}}, {{ 51},{  9}}, {{307},{  9}}, {{179},{  9}},
{{435},{  9}}, {{115},{  9}}, {{371},{  9}}, {{243},{  9}}, {{499},{  9}},
{{ 11},{  9}}, {{267},{  9}}, {{139},{  9}}, {{395},{  9}}, {{ 75},{  9}},
{{331},{  9}}, {{203},{  9}}, {{459},{  9}}, {{ 43},{  9}}, {{299},{  9}},
{{171},{  9}}, {{427},{  9}}, {{107},{  9}}, {{363},{  9}}, {{235},{  9}},
{{491},{  9}}, {{ 27},{  9}}, {{283},{  9}}, {{155},{  9}}, {{411},{  9}},
{{ 91},{  9}}, {{347},{  9}}, {{219},{  9}}, {{475},{  9}}, {{ 59},{  9}},
{{315},{  9}}, {{187},{  9}}, {{443},{  9}}, {{123},{  9}}, {{379},{  9}},
{{251},{  9}}, {{507},{  9}}, {{  7},{  9}}, {{263},{  9}}, {{135},{  9}},
{{391},{  9}}, {{ 71},{  9}}, {{327},{  9}}, {{199},{  9}}, {{455},{  9}},
{{ 39},{  9}}, {{295},{  9}}, {{167},{  9}}, {{423},{  9}}, {{103},{  9}},
{{359},{  9}}, {{231},{  9}}, {{487},{  9}}, {{ 23},{  9}}, {{279},{  9}},
{{151},{  9}}, {{407},{  9}}, {{ 87},{  9}}, {{343},{  9}}, {{215},{  9}},
{{471},{  9}}, {{ 55},{  9}}, {{311},{  9}}, {{183},{  9}}, {{439},{  9}},
{{119},{  9}}, {{375},{  9}}, {{247},{  9}}, {{503},{  9}}, {{ 15},{  9}},
{{271},{  9}}, {{143},{  9}}, {{399},{  9}}, {{ 79},{  9}}, {{335},{  9}},
{{207},{  9}}, {{463},{  9}}, {{ 47},{  9}}, {{303},{  9}}, {{175},{  9}},
{{431},{  9}}, {{111},{  9}}, {{367},{  9}}, {{239},{  9}}, {{495},{  9}},
{{ 31},{  9}}, {{287},{  9}}, {{159},{  9}}, {{415},{  9}}, {{ 95},{  9}},
{{351},{  9}}, {{223},{  9}}, {{479},{  9}}, {{ 63},{  9}}, {{319},{  9}},
{{191},{  9}}, {{447},{  9}}, {{127},{  9}}, {{383},{  9}}, {{255},{  9}},
{{511},{  9}}, {{  0},{  7}}, {{ 64},{  7}}, {{ 32},{  7}}, {{ 96},{  7}},
{{ 16},{  7}}, {{ 80},{  7}}, {{ 48},{  7}}, {{112},{  7}}, {{  8},{  7}},
{{ 72},{  7}}, {{ 40},{  7}}, {{104},{  7}}, {{ 24},{  7}}, {{ 88},{  7}},
{{ 56},{  7}}, {{120},{  7}}, {{  4},{  7}}, {{ 68},{  7}}, {{ 36},{  7}},
{{100},{  7}}, {{ 20},{  7}}, {{ 84},{  7}}, {{ 52},{  7}}, {{116},{  7}},
{{  3},{  8}}, {{131},{  8}}, {{ 67},{  8}}, {{195},{  8}}, {{ 35},{  8}},
{{163},{  8}}, {{ 99},{  8}}, {{227},{  8}}
};


/* The static distance tree. (Actually a trivial tree since all codes use
 * 5 bits.)
 */
 const ct_data static_dtree[D_CODES] = {
{{ 0},{ 5}}, {{16},{ 5}}, {{ 8},{ 5}}, {{24},{ 5}}, {{ 4},{ 5}},
{{20},{ 5}}, {{12},{ 5}}, {{28},{ 5}}, {{ 2},{ 5}}, {{18},{ 5}},
{{10},{ 5}}, {{26},{ 5}}, {{ 6},{ 5}}, {{22},{ 5}}, {{14},{ 5}},
{{30},{ 5}}, {{ 1},{ 5}}, {{17},{ 5}}, {{ 9},{ 5}}, {{25},{ 5}},
{{ 5},{ 5}}, {{21},{ 5}}, {{13},{ 5}}, {{29},{ 5}}, {{ 3},{ 5}},
{{19},{ 5}}, {{11},{ 5}}, {{27},{ 5}}, {{ 7},{ 5}}, {{23},{ 5}}
};

/* Distance codes. The first 256 values correspond to the distances
 * 3 .. 258, the last 256 values correspond to the top 8 bits of
 * the 15 bit distances.
 */
const Byte _dist_code[DIST_CODE_LEN] = {
 0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,
 8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10,
10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,  0, 16, 17,
18, 18, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22,
23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27,
27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
};

/* length code for each normalized match length (0 == MIN_MATCH) */
const Byte _length_code[MAX_MATCH-MIN_MATCH+1]= {
 0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 12, 12,
13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19,
19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26,
26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28
};

/* First normalized length for each code (0 = MIN_MATCH) */
 const int base_length[LENGTH_CODES] = {
0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, 56,
64, 80, 96, 112, 128, 160, 192, 224, 0};


/* First normalized distance for each code (0 = distance of 1) */
 const int base_dist[D_CODES] = {
    0,     1,     2,     3,     4,     6,     8,    12,    16,    24,
   32,    48,    64,    96,   128,   192,   256,   384,   512,   768,
 1024,  1536,  2048,  3072,  4096,  6144,  8192, 12288, 16384, 24576};


struct static_tree_desc_s {
    const ct_data *static_tree;  /* static tree or NULL */
    const int *extra_bits;      /* extra bits for each code or NULL */
    int     extra_base;          /* base index for extra_bits */
    int     elems;               /* max number of elements in the tree */
    int     max_length;          /* max bit length for the codes */
};

static_tree_desc  static_l_desc	= {static_ltree, extra_lbits, LITERALS+1, L_CODES, MAX_BITS};
static_tree_desc  static_d_desc	= {static_dtree, extra_dbits, 0,D_CODES, MAX_BITS};
static_tree_desc  static_bl_desc	= {(const ct_data *)0, extra_blbits, 0,   BL_CODES, MAX_BL_BITS};

/* ===========================================================================
 * Local (static) routines in this file.
 */

#  define send_code(s, c, tree) { send_bits(s, tree[c].Code, tree[c].Len); }

/* ===========================================================================
 * Output a short LSB first on the stream.
 * IN assertion: there is enough room in pendingBuf.
 */
#define put_short(s, w) { \
    put_byte(s, (Byte)((w) & 0xff)); \
    put_byte(s, (Byte)((ush)(w) >> 8)); \
}

#define send_bits(s, value, length) \
{ int len = length;\
  if (s->bi_valid > (int)Buf_size - len) {\
    int val = value;\
    s->bi_buf |= (val << s->bi_valid);\
    put_short(s, s->bi_buf);\
    s->bi_buf = (ush)val >> (Buf_size - s->bi_valid);\
    s->bi_valid += len - Buf_size;\
  } else {\
    s->bi_buf |= (value) << s->bi_valid;\
    s->bi_valid += len;\
  }\
}

#define MAX(a,b) (a >= b ? a : b)

# define _tr_tally_lit(s, c, fluxh) \
  { Byte cc = (c); \
    s->d_buf[s->last_lit] = 0; \
    s->l_buf[s->last_lit++] = cc; \
    s->dyn_ltree[cc].Freq++; \
    fluxh = (s->last_lit == s->lit_bufsize-1); \
   }

typedef enum {
    need_more,      /* block not completed, need more input or more output */
    block_done,     /* block fluxh performed */
    finish_started, /* finish started, need only more output at next deflate */
    finish_done     /* finish done, accept no more input or output */
} block_state;

typedef block_state (*compress_func) (deflate_state *s, int fluxh);
/* Compression function. Returns the block state after the call. */

block_state deflate_slow   (deflate_state *s, int fluxh);
void lm_init        (deflate_state *s);
void _tr_init(deflate_state *s);
void _tr_fluxh_block(deflate_state *s,char *buf, uLong stored_len,int eof);

/* ===========================================================================
 * Local data
 */
typedef struct config_s {
   ush good_length; /* reduce lazy search above this match length */
   ush max_lazy;    /* do not perform lazy search above this match length */
   ush nice_length; /* quit search above this match length */
   ush max_chain;
   compress_func func;
} config;

 const config configuration_table[10] = {
/*      good lazy nice chain */
/* 0 */ {0,    0,  0,    0, deflate_slow},  /* store only */
/* 1 */ {0,    0,  0,    0, deflate_slow}, /* maximum speed, no lazy matches */
/* 2 */ {0,    0,  0,    0, deflate_slow},
/* 3 */ {0,    0,  0,    0, deflate_slow},

/* 4 */ {4,    4, 16,   16, deflate_slow},  /* lazy matches */
/* 5 */ {8,   16, 32,   32, deflate_slow},
/* 6 */ {8,   16, 128, 128, deflate_slow},
/* 7 */ {8,   32, 128, 256, deflate_slow},
/* 8 */ {32, 128, 258, 1024, deflate_slow},
/* 9 */ {32, 258, 258, 4096, deflate_slow}}; /* maximum compression */

const uLong crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

/* ========================================================================= */
#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);
/* ========================================================================= */
 uLong crc32(uLong crc,const Byte *buf,uInt len)
 {
	 if (buf == Z_NULL) return 0L;
	 crc = crc ^ 0xffffffffL;
	 while (len >= 8)
	 {
		 DO8(buf);
		 len -= 8;
	 }
	 if (len) 
	 {
		 do {
			 DO1(buf);
		 } while (--len);
	 }
	 return crc ^ 0xffffffffL;
 }

void zmemcpy(Byte* dest,const Byte* source,uInt len)
{
    if (len == 0) return;
    do {
        *dest++ = *source++; /* ??? to be unrolled */
    } while (--len != 0);
}

int zmemcmp(const Byte* s1,const Byte* s2,uInt  len)
{
    uInt j;

    for (j = 0; j < len; j++) {
        if (s1[j] != s2[j]) 
			return 2*(s1[j] > s2[j])-1;
    }
    return 0;
}

void zmemzero(Byte* dest,uInt  len)
{
    if (len == 0) return;
    do {
        *dest++ = 0;  /* ??? to be unrolled */
    } while (--len != 0);
}

Byte* zcalloc (Byte* opaque,unsigned items,unsigned size)
{
    if (opaque) 
		items += size - size; /* make compiler happy */
    return (Byte*)calloc(items, size);
}

void  zcfree(Byte* opaque,Byte* ptr)
{
    free(ptr);
    if (opaque) return; /* make compiler happy */
}


/* ========================================================================= */
int  deflateReset (z_stream* strm)
{
    deflate_state *s;
    
    if (strm == Z_NULL || strm->state == Z_NULL ||
        strm->zalloc == Z_NULL || strm->zfree == Z_NULL) return Z_STREAM_ERROR;

    strm->total_in = strm->total_out = 0;

    s = (deflate_state *)strm->state;
    s->pending = 0;
    s->pending_out = s->pending_buf;

    if (s->noheader < 0) {
        s->noheader = 0; /* was set to -1 by deflate(..., Z_FINISH); */
    }
    s->status = s->noheader ? BUSY_STATE : INIT_STATE;
    strm->adler = 1;
    s->last_fluxh = Z_NO_FLUSH;

    _tr_init(s);
    lm_init(s);

    return Z_OK;
}


/* ========================================================================= */
int deflateEnd(z_stream* strm)
{
    int status;

    if (strm == Z_NULL || strm->state == Z_NULL) return Z_STREAM_ERROR;

    status = strm->state->status;
    if (status != INIT_STATE && status != BUSY_STATE &&	status != FINISH_STATE) {
      return Z_STREAM_ERROR;
    }

    /* Deallocate in reverse order of allocations: */
    TRY_FREE(strm, strm->state->pending_buf);
    TRY_FREE(strm, strm->state->head);
    TRY_FREE(strm, strm->state->prev);
    TRY_FREE(strm, strm->state->window);

    ZFREE(strm, strm->state);
    strm->state = Z_NULL;

    return status == BUSY_STATE ? Z_DATA_ERROR : Z_OK;
}

// ===========================================================================
int deflateInit2(z_stream* strm,int level,int method,int windowBits,int memLevel,int strategy)
{
    deflate_state *s;
    int noheader = 0;

    ush *overlay;
    if (strm == Z_NULL) return Z_STREAM_ERROR;

    if (strm->zalloc == Z_NULL) {
		strm->zalloc = zcalloc;
		strm->opaque = (Byte*)0;
    }
    if (strm->zfree == Z_NULL) strm->zfree = zcfree;

    if (level == Z_DEFAULT_COMPRESSION) level = 6;


    if (windowBits < 0) { /* undocumented feature: suppress zlib header */
        noheader = 1;
        windowBits = -windowBits;
    }
    if (memLevel < 1 || memLevel > MAX_MEM_LEVEL || method != Z_DEFLATED ||
        windowBits < 8 || windowBits > 15 || level < 0 || level > 9 ||
	strategy < 0 || strategy > Z_HUFFMAN_ONLY) {
        return Z_STREAM_ERROR;
    }
    s = (deflate_state *) ZALLOC(strm, 1, sizeof(deflate_state));
    if (s == Z_NULL) return Z_MEM_ERROR;
    strm->state = (struct internal_state  *)s;
    s->strm = strm;

    s->noheader = noheader;
    s->w_bits = windowBits;
    s->w_size = 1 << s->w_bits;
    s->w_mask = s->w_size - 1;

    s->hash_bits = memLevel + 7;
    s->hash_size = 1 << s->hash_bits;
    s->hash_mask = s->hash_size - 1;
    s->hash_shift =  ((s->hash_bits+MIN_MATCH-1)/MIN_MATCH);

    s->window = (Byte *) ZALLOC(strm, s->w_size, 2*sizeof(Byte));
    s->prev   = (ush *)  ZALLOC(strm, s->w_size, sizeof(ush));
    s->head   = (ush *)  ZALLOC(strm, s->hash_size, sizeof(ush));

    s->lit_bufsize = 1 << (memLevel + 6); /* 16K elements by default */

    overlay = (ush *) ZALLOC(strm, s->lit_bufsize, sizeof(ush)+2);
    s->pending_buf = (Byte *) overlay;
    s->pending_buf_size = (uLong)s->lit_bufsize * (sizeof(ush)+2L);

    if (s->window == Z_NULL || s->prev == Z_NULL || s->head == Z_NULL ||
        s->pending_buf == Z_NULL) {
        deflateEnd (strm);
        return Z_MEM_ERROR;
    }
    s->d_buf = overlay + s->lit_bufsize/sizeof(ush);
    s->l_buf = s->pending_buf + (1+sizeof(ush))*s->lit_bufsize;

    s->level = level;
    s->strategy = strategy;
    s->method = (Byte)method;

    return deflateReset(strm);
}
// =========================================================================
 void fluxh_pending(z_stream* strm)
{
    unsigned len = strm->state->pending;

    if (len > strm->avail_out) len = strm->avail_out;
    if (len == 0) return;

    zmemcpy(strm->next_out, strm->state->pending_out, len);
    strm->next_out  += len;
    strm->state->pending_out  += len;
    strm->total_out += len;
    strm->avail_out  -= len;
    strm->state->pending -= len;
    if (strm->state->pending == 0) {
        strm->state->pending_out = strm->state->pending_buf;
    }
}

/* ========================================================================= */
int deflate(z_stream* strm,int fluxh)
{
    int old_fluxh; /* value of fluxh param for previous deflate call */
    deflate_state *s;

    if (strm == Z_NULL || strm->state == Z_NULL || fluxh > Z_FINISH || fluxh < 0) 
        return Z_STREAM_ERROR;

	s = strm->state;

    if (strm->next_out == Z_NULL || (strm->next_in == Z_NULL && strm->avail_in != 0) ||	(s->status == FINISH_STATE && fluxh != Z_FINISH)) 
        return Z_STREAM_ERROR;

	if (strm->avail_out == 0) 
		return Z_BUF_ERROR;

    s->strm = strm; /* just in case */
    old_fluxh = s->last_fluxh;
    s->last_fluxh = fluxh;

    /* Fluxh as mByte pending output as possible */
    if (s->pending != 0) {
        fluxh_pending(strm);
        if (strm->avail_out == 0) {
			s->last_fluxh = -1;
			return Z_OK;
		}
	} else if (strm->avail_in == 0 && fluxh <= old_fluxh && fluxh != Z_FINISH) {
		return Z_BUF_ERROR;
	}

    /* User must not provide more input after the first FINISH: */
    if (s->status == FINISH_STATE && strm->avail_in != 0) {
        return Z_BUF_ERROR;
    }

    /* Start a new block or continue the current one.
     */
    if (strm->avail_in != 0 || s->lookahead != 0 || (fluxh != Z_NO_FLUSH && s->status != FINISH_STATE)) {
        block_state bstate;

		bstate = (*(configuration_table[s->level].func))(s, fluxh);

        if (bstate == finish_started || bstate == finish_done) {
            s->status = FINISH_STATE;
        }
        if (bstate == need_more || bstate == finish_started) {
			if (strm->avail_out == 0) {
				s->last_fluxh = -1; /* avoid BUF_ERROR next call, see above */
			}
			return Z_OK;
		}
    }
    Assert(strm->avail_out > 0, "bug2");

    if (fluxh != Z_FINISH) return Z_OK;
    if (s->noheader) 
		return Z_STREAM_END;
	
	// will not attend here
	return Z_ERRNO;
}

// ===========================================================================
 int read_buf(z_stream* strm, Byte *buf, unsigned size)
{
    unsigned len = strm->avail_in;

    if (len > size) len = size;
    if (len == 0) return 0;

    strm->avail_in  -= len;

    if (!strm->state->noheader) {
        //strm->adler = adler32(strm->adler, strm->next_in, len);
    }
    zmemcpy(buf, strm->next_in, len);
    strm->next_in  += len;
    strm->total_in += len;

    return (int)len;
}

/* ===========================================================================
 * Initialize the "longest match" routines for a new zlib stream
 */
 void lm_init(deflate_state *s)
{
    s->window_size = (uLong)2L*s->w_size;

    s->head[s->hash_size-1] = 0; 
    zmemzero((Byte *)s->head, (unsigned)(s->hash_size-1)*sizeof(*s->head));

    /* Set the default configuration parameters:
     */
    s->max_lazy_match   = configuration_table[s->level].max_lazy;
    s->good_match       = configuration_table[s->level].good_length;
    s->nice_match       = configuration_table[s->level].nice_length;
    s->max_chain_length = configuration_table[s->level].max_chain;

    s->strstart = 0;
    s->block_start = 0L;
    s->lookahead = 0;
    s->match_length = s->prev_length = MIN_MATCH-1;
    s->match_available = 0;
    s->ins_h = 0;
}

// ===========================================================================
 uInt longest_match(deflate_state *s,uInt cur_match)
{
    unsigned chain_length = s->max_chain_length;/* max hash chain length */
    register Byte *scan = s->window + s->strstart; /* current string */
    register Byte *match;                       /* matched string */
    register int len;                           /* length of current match */
    int best_len = s->prev_length;              /* best match length so far */
    int nice_match = s->nice_match;             /* stop if match long enough */
    uInt limit = s->strstart > (uInt)(s->w_size-MIN_LOOKAHEAD) ? s->strstart - (uInt)(s->w_size-MIN_LOOKAHEAD) : 0;
    /* Stop when cur_match becomes <= limit. To simplify the code,
     * we prevent matches with the string of window index 0.
     */

    ush *prev = s->prev;
    uInt wmask = s->w_mask;

    register Byte *strend = s->window + s->strstart + MAX_MATCH;
    register Byte scan_end1  = scan[best_len-1];
    register Byte scan_end   = scan[best_len];

    /* The code is optimized for HASH_BITS >= 8 and MAX_MATCH-2 multiple of 16.
     * It is easy to get rid of this optimization if necessary.
     */
    Assert(s->hash_bits >= 8 && MAX_MATCH == 258, "Code too clever");

    /* Do not waste too mByte time if we already have a good match: */
    if (s->prev_length >= s->good_match) {
        chain_length >>= 2;
    }
    /* Do not look for matches beyond the end of the input. This is necessary
     * to make deflate deterministic.
     */
    if ((uInt)nice_match > s->lookahead) nice_match = s->lookahead;

    Assert((uLong)s->strstart <= s->window_size-MAX_MATCH+MIN_MATCH+1, "need lookahead");

    do {
        Assert(cur_match < s->strstart, "no future");
        match = s->window + cur_match;

        if (match[best_len]   != scan_end  ||
            match[best_len-1] != scan_end1 ||
            *match            != *scan     ||
            *++match          != scan[1])      continue;

        scan += 2, match++;
        Assert(*scan == *match, "match[2]?");

        /* We check for insufficient lookahead only every 8th comparison;
         * the 256th check will be made at strstart+258.
         */
        do {
        } while (*++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 scan < strend);

        Assert(scan <= s->window+(unsigned)(s->window_size-1), "wild scan");

        len = MAX_MATCH - (int)(strend - scan);
        scan = strend - MAX_MATCH;

        if (len > best_len) {
            s->match_start = cur_match;
            best_len = len;
            if (len >= nice_match) 
				break;
            scan_end1  = scan[best_len-1];
            scan_end   = scan[best_len];
        }
    } while ((cur_match = prev[cur_match & wmask]) > limit && --chain_length != 0);

    if ((uInt)best_len <= s->lookahead) 
		return (uInt)best_len;
    return s->lookahead;
}

// ===========================================================================
 void fill_window(deflate_state *s)
{
    register unsigned n, m;
    register ush *p;
    unsigned more;    /* Amount of free space at the end of the window. */
    uInt wsize = s->w_size;

    do {
        more = (unsigned)(s->window_size -(uLong)s->lookahead -(uLong)s->strstart);

        /* Deal with !@#$% 64K limit: */
        if (more == 0 && s->strstart == 0 && s->lookahead == 0) {
            more = wsize;

        } else if (more == (unsigned)(-1)) {
            /* Very unlikely, but possible on 16 bit machine if strstart == 0
             * and lookahead == 1 (input done one byte at time)
             */
            more--;

        /* If the window is almost full and there is insufficient lookahead,
         * move the upper half to the lower one to make room in the upper half.
         */
        } else if (s->strstart >= wsize+s->w_size-MIN_LOOKAHEAD) {

        zmemcpy(s->window, s->window+wsize, (unsigned)wsize);
        s->match_start -= wsize;
        s->strstart    -= wsize; /* we now have strstart >= MAX_DIST */
        s->block_start -= (long) wsize;
	    n = s->hash_size;
	    p = &s->head[n];
	    do {
		m = *--p;
		*p = (ush)(m >= wsize ? m-wsize : 0);
	    } while (--n);

	    n = wsize;

		p = &s->prev[n];
	    do {
		m = *--p;
		*p = (ush)(m >= wsize ? m-wsize : 0);
		/* If n is not on any hash chain, prev[n] is garbage but
		 * its value will never be used.
		 */
	    } while (--n);
            more += wsize;
        }
        if (s->strm->avail_in == 0) return;

        Assert(more >= 2, "more < 2");

        n = read_buf(s->strm, s->window + s->strstart + s->lookahead, more);
        s->lookahead += n;

        /* Initialize the hash value now that we have some input: */
        if (s->lookahead >= MIN_MATCH) {
            s->ins_h = s->window[s->strstart];
			s->ins_h = ((s->ins_h<<s->hash_shift) ^ s->window[s->strstart+1]) & s->hash_mask;
        }
    } while (s->lookahead < MAX_MATCH+MIN_MATCH+1 && s->strm->avail_in != 0);
}

/* ===========================================================================
 * Same as above, but achieves better compression. We use a lazy
 * evaluation for matches: a match is finally adopted only if there is
 * no better match at the next window position.
 */
 block_state deflate_slow(deflate_state *s,int fluxh)
{
    uInt hash_head = 0;    /* head of hash chain */
    int bfluxh;              /* set if current block must be fluxhed */

    /* Process the input block. */
    for (;;) {
        if (s->lookahead < MAX_MATCH+MIN_MATCH+1) {
            fill_window(s);
            if (s->lookahead < MAX_MATCH+MIN_MATCH+1 && fluxh == Z_NO_FLUSH) {
	        return need_more;
	    }
            if (s->lookahead == 0) break; /* fluxh the current block */
        }

        if (s->lookahead >= MIN_MATCH) {
			s->ins_h = ((s->ins_h<<s->hash_shift) ^ s->window[s->strstart + (MIN_MATCH-1)]) & s->hash_mask;
			s->prev[s->strstart & s->w_mask] = hash_head = s->head[s->ins_h]; 
			s->head[s->ins_h] = (ush)s->strstart;
        }

        /* Find the longest match, discarding those <= prev_length.
         */
        s->prev_length = s->match_length, s->prev_match = s->match_start;
        s->match_length = MIN_MATCH-1;

        if (hash_head != 0 && s->prev_length < s->max_lazy_match &&
            s->strstart - hash_head <= s->w_size-MIN_LOOKAHEAD) {
            if (s->strategy != Z_HUFFMAN_ONLY) {
                s->match_length = longest_match (s, hash_head);
            }
            /* longest_match() sets match_start */

            if (s->match_length <= 5 && (s->strategy == Z_FILTERED || (s->match_length == MIN_MATCH && s->strstart - s->match_start > 4096))) {

                /* If prev_match is also MIN_MATCH, match_start is garbage
                 * but we will ignore the current match anyway.
                 */
                s->match_length = MIN_MATCH-1;
            }
        }
        /* If there was a match at the previous step and the current
         * match is not better, output the previous match:
         */
        if (s->prev_length >= MIN_MATCH && s->match_length <= s->prev_length) {
            uInt max_insert = s->strstart + s->lookahead - MIN_MATCH;
            /* Do not insert strings in hash table beyond this. */

            //check_match(s, s->strstart-1, s->prev_match, s->prev_length);

			Byte len = s->prev_length - MIN_MATCH;
			ush dist = s->strstart -1 - s->prev_match;
			s->d_buf[s->last_lit] = dist; 
			s->l_buf[s->last_lit++] = len; 
			dist--; 
			s->dyn_ltree[_length_code[len]+LITERALS+1].Freq++;
			s->dyn_dtree[dist < 256 ? _dist_code[dist] : _dist_code[256+(dist>>7)]].Freq++; 
			bfluxh = (s->last_lit == s->lit_bufsize-1); 

            s->lookahead -= s->prev_length-1;
            s->prev_length -= 2;
            do {
                if (++s->strstart <= max_insert) {
					s->ins_h = ((s->ins_h<<s->hash_shift) ^ s->window[s->strstart + (MIN_MATCH-1)]) & s->hash_mask;
					s->prev[s->strstart & s->w_mask] = hash_head = s->head[s->ins_h]; 
					s->head[s->ins_h] = (ush)s->strstart;
                }
            } while (--s->prev_length != 0);
            s->match_available = 0;
            s->match_length = MIN_MATCH-1;
            s->strstart++;
        } else if (s->match_available) {
            Tracevv((stderr,"%c", s->window[s->strstart-1]));
			_tr_tally_lit(s, s->window[s->strstart-1], bfluxh);
			if (bfluxh) {
				_tr_fluxh_block(s, (s->block_start >= 0L ? (char *)&s->window[(unsigned)s->block_start] : (char *)Z_NULL), (uLong)((long)s->strstart - s->block_start), 0); 
				s->block_start = s->strstart; 
				fluxh_pending(s->strm); 
            }
            s->strstart++;
            s->lookahead--;
            if (s->strm->avail_out == 0) return need_more;
        } else {
            s->match_available = 1;
            s->strstart++;
            s->lookahead--;
        }
    }
    Assert (fluxh != Z_NO_FLUSH, "no fluxh?");
    if (s->match_available) {
        Tracevv((stderr,"%c", s->window[s->strstart-1]));
        _tr_tally_lit(s, s->window[s->strstart-1], bfluxh);
        s->match_available = 0;
    }
   
	_tr_fluxh_block(s, (s->block_start >= 0L ? (char *)&s->window[(unsigned)s->block_start] : (char *)Z_NULL), (uLong)((long)s->strstart - s->block_start), (fluxh == Z_FINISH)); 
	s->block_start = s->strstart; 
	fluxh_pending(s->strm); 

	if (s->strm->avail_out == 0) 
	   return fluxh == Z_FINISH ? finish_started : need_more; 

    return fluxh == Z_FINISH ? finish_done : block_done;
}

/* the arguments must not have side effects */
/* ===========================================================================
 * Initialize a new block.
 */
 void init_block(deflate_state *s)
{
    int n; /* iterates over tree elements */

    /* Initialize the trees. */
    for (n = 0; n < L_CODES;  n++) s->dyn_ltree[n].Freq = 0;
    for (n = 0; n < D_CODES;  n++) s->dyn_dtree[n].Freq = 0;
    for (n = 0; n < BL_CODES; n++) s->bl_tree[n].Freq = 0;

    s->dyn_ltree[END_BLOCK].Freq = 1;
    s->opt_len = s->static_len = 0L;
    s->last_lit = s->matches = 0;
}

/* ===========================================================================
 * Initialize the tree data structures for a new zlib stream.
 */
void _tr_init(deflate_state *s)
{
    s->l_desc.dyn_tree = s->dyn_ltree;
    s->l_desc.stat_desc = &static_l_desc;

    s->d_desc.dyn_tree = s->dyn_dtree;
    s->d_desc.stat_desc = &static_d_desc;

    s->bl_desc.dyn_tree = s->bl_tree;
    s->bl_desc.stat_desc = &static_bl_desc;

    s->bi_buf = 0;
    s->bi_valid = 0;
    s->last_eob_len = 8; /* enough lookahead for inflate */

    /* Initialize the first block of the first file: */
    init_block(s);
}

#define SMALLEST 1
/* Index within the heap array of least frequent node in the Huffman tree */

#define smaller(tree, n, m, depth) (tree[n].Freq < tree[m].Freq || (tree[n].Freq == tree[m].Freq && depth[n] <= depth[m]))
/* ===========================================================================
 * Compares to subtrees, using the tree depth as tie breaker when
 * the subtrees have equal frequency. This minimizes the worst case length.
 */

/* ===========================================================================
 * Restore the heap property by moving down the tree starting at node k,
 * exchanging a node with the smallest of its two sons if necessary, stopping
 * when the heap property is re-established (each father smaller than its
 * two sons).
 */
 void pqdownheap(deflate_state *s,ct_data *tree,int k)
{
    int v = s->heap[k];
    int j = k << 1;  /* left son of k */
    while (j <= s->heap_len) {
        /* Set j to the smallest of the two sons: */
		
		if (j < s->heap_len && smaller(tree, s->heap[j+1], s->heap[j], s->depth)) 
            j++;

		/* Exit if v is smaller than both sons */
        if (smaller(tree, v, s->heap[j], s->depth)) 
			break;

        /* Exchange v with the smallest son */
        s->heap[k] = s->heap[j];  k = j;

        /* And continue down the tree, setting j to the left son of k */
        j <<= 1;
    }
    s->heap[k] = v;
}

/* ===========================================================================
 * Compute the optimal bit lengths for a tree and update the total bit length
 * for the current block.
 * IN assertion: the fields freq and dad are set, heap[heap_max] and
 *    above are the tree nodes sorted by increasing frequency.
 * OUT assertions: the field len is set to the optimal bit length, the
 *     array bl_count contains the frequencies for each bit length.
 *     The length opt_len is updated; static_len is also updated if stree is
 *     not null.
 */
 void gen_bitlen(deflate_state *s,tree_desc *desc)
{
    ct_data *tree        = desc->dyn_tree;
    int max_code         = desc->max_code;
    const ct_data *stree = desc->stat_desc->static_tree;
    const int *extra    = desc->stat_desc->extra_bits;
    int base             = desc->stat_desc->extra_base;
    int max_length       = desc->stat_desc->max_length;
    int h;              /* heap index */
    int n, m;           /* iterate over the tree elements */
    int bits;           /* bit length */
    int xbits;          /* extra bits */
    ush f;              /* frequency */
    int overflow = 0;   /* number of elements with bit length too large */

    for (bits = 0; bits <= MAX_BITS; bits++) s->bl_count[bits] = 0;

    /* In a first pass, compute the optimal bit lengths (which may
     * overflow in the case of the bit length tree).
     */
    tree[s->heap[s->heap_max]].Len = 0; /* root of the heap */

    for (h = s->heap_max+1; h < HEAP_SIZE; h++) {
        n = s->heap[h];
        bits = tree[tree[n].Dad].Len + 1;
        if (bits > max_length) bits = max_length, overflow++;
        tree[n].Len = (ush)bits;
        /* We overwrite tree[n].Dad which is no longer needed */

        if (n > max_code) continue; /* not a leaf node */

        s->bl_count[bits]++;
        xbits = 0;
        if (n >= base) xbits = extra[n-base];
        f = tree[n].Freq;
        s->opt_len += (uLong)f * (bits + xbits);
        if (stree) s->static_len += (uLong)f * (stree[n].Len + xbits);
    }
    if (overflow == 0) return;

    Trace((stderr,"\nbit length overflow\n"));
    /* This happens for example on obj2 and pic of the Calgary corpus */

    /* Find the first bit length which could increase: */
    do {
        bits = max_length-1;
        while (s->bl_count[bits] == 0) bits--;
        s->bl_count[bits]--;      /* move one leaf down the tree */
        s->bl_count[bits+1] += 2; /* move one overflow item as its brother */
        s->bl_count[max_length]--;
        /* The brother of the overflow item also moves one step up,
         * but this does not affect bl_count[max_length]
         */
        overflow -= 2;
    } while (overflow > 0);

    /* Now recompute all bit lengths, scanning in increasing frequency.
     * h is still equal to HEAP_SIZE. (It is simpler to reconstruct all
     * lengths instead of fixing only the wrong ones. This idea is taken
     * from 'ar' written by Haruhiko Okumura.)
     */
    for (bits = max_length; bits != 0; bits--) {
        n = s->bl_count[bits];
        while (n != 0) {
            m = s->heap[--h];
            if (m > max_code) continue;
            if (tree[m].Len != (unsigned) bits) {
                Trace((stderr,"code %d bits %d->%d\n", m, tree[m].Len, bits));
                s->opt_len += ((long)bits - (long)tree[m].Len)
                              *(long)tree[m].Freq;
                tree[m].Len = (ush)bits;
            }
            n--;
        }
    }
}

/* ===========================================================================
 * Reverse the first len bits of a code, using straightforward code (a faster
 * method would use a table)
 * IN assertion: 1 <= len <= 15 */
 unsigned bi_reverse(unsigned code,int len)
{
    register unsigned res = 0;
    do {
        res |= code & 1;
        code >>= 1, res <<= 1;
    } while (--len > 0);
    return res >> 1;
}

/* ===========================================================================
 * Generate the codes for a given tree and bit counts (which need not be
 * optimal).
 * IN assertion: the array bl_count contains the bit length statistics for
 * the given tree and the field len is set for all tree elements.
 * OUT assertion: the field code is set for all tree elements of non
 *     zero code length. */
 void gen_codes(ct_data *tree,int max_code,ush *bl_count)
{
    ush next_code[MAX_BITS+1]; /* next code value for each bit length */
    ush code = 0;              /* running code value */
    int bits;                  /* bit index */
    int n;                     /* code index */

    /* The distribution counts are first used to generate the code values
     * without bit reversal.
     */
    for (bits = 1; bits <= MAX_BITS; bits++) {
        next_code[bits] = code = (code + bl_count[bits-1]) << 1;
    }
    /* Check that the bit counts in bl_count are consistent. The last code
     * must be all ones.
     */
    Assert (code + bl_count[MAX_BITS]-1 == (1<<MAX_BITS)-1,
            "inconsistent bit counts");
    Tracev((stderr,"\ngen_codes: max_code %d ", max_code));

    for (n = 0;  n <= max_code; n++) {
        int len = tree[n].Len;
        if (len == 0) continue;
        /* Now reverse the bits */
        tree[n].Code = bi_reverse(next_code[len]++, len);

        Tracecv(tree != static_ltree, (stderr,"\nn %3d %c l %2d c %4x (%x) ",
             n, (isgraph(n) ? n : ' '), len, tree[n].Code, next_code[len]-1));
    }
}

/* ===========================================================================
 * Construct one Huffman tree and assigns the code bit strings and lengths.
 * Update the total bit length for the current block.
 * IN assertion: the field freq is set for all tree elements.
 * OUT assertions: the fields len and code are set to the optimal bit length
 *     and corresponding code. The length opt_len is updated; static_len is
 *     also updated if stree is not null. The field max_code is set.
 */
 void build_tree(deflate_state *s,tree_desc *desc)
{
    ct_data *tree         = desc->dyn_tree;
    const ct_data *stree  = desc->stat_desc->static_tree;
    int elems             = desc->stat_desc->elems;
    int n, m;          /* iterate over heap elements */
    int max_code = -1; /* largest code with non zero frequency */
    int node;          /* new node being created */

    s->heap_len = 0, s->heap_max = HEAP_SIZE;

    for (n = 0; n < elems; n++) {
        if (tree[n].Freq != 0) {
            s->heap[++(s->heap_len)] = max_code = n;
            s->depth[n] = 0;
        } else {
            tree[n].Len = 0;
        }
    }

    while (s->heap_len < 2) {
        node = s->heap[++(s->heap_len)] = (max_code < 2 ? ++max_code : 0);
        tree[node].Freq = 1;
        s->depth[node] = 0;
        s->opt_len--; if (stree) s->static_len -= stree[node].Len;
        /* node is 0 or 1 so it does not have extra bits */
    }
    desc->max_code = max_code;

    /* The elements heap[heap_len/2+1 .. heap_len] are leaves of the tree,
     * establish sub-heaps of increasing lengths:
     */
    for (n = s->heap_len/2; n >= 1; n--) pqdownheap(s, tree, n);

    /* Construct the Huffman tree by repeatedly combining the least two
     * frequent nodes.
     */
    node = elems;              /* next internal node of the tree */
    do {

		n = s->heap[SMALLEST]; 
		s->heap[SMALLEST] = s->heap[s->heap_len--]; 
		pqdownheap(s, tree, SMALLEST); 

       // pqremove(s, tree, n);  /* n = node of least frequency */
        m = s->heap[SMALLEST]; /* m = node of next least frequency */

        s->heap[--(s->heap_max)] = n; /* keep the nodes sorted by frequency */
        s->heap[--(s->heap_max)] = m;

        /* Create a new node father of n and m */
        tree[node].Freq = tree[n].Freq + tree[m].Freq;
        s->depth[node] = (Byte) (MAX(s->depth[n], s->depth[m]) + 1);
        tree[n].Dad = tree[m].Dad = (ush)node;

		/* and insert the new node in the heap */
        s->heap[SMALLEST] = node++;
        pqdownheap(s, tree, SMALLEST);

    } while (s->heap_len >= 2);

    s->heap[--(s->heap_max)] = s->heap[SMALLEST];

    /* At this point, the fields freq and dad are set. We can now
     * generate the bit lengths.
     */
    gen_bitlen(s, (tree_desc *)desc);

    /* The field len is now set, we can generate the bit codes */
    gen_codes ((ct_data *)tree, max_code, s->bl_count);
}

/* ===========================================================================
 * Scan a literal or distance tree to determine the frequencies of the codes
 * in the bit length tree.
 */
 void scan_tree(deflate_state *s,ct_data *tree,int max_code)
{
    int n;                     /* iterates over all tree elements */
    int prevlen = -1;          /* last emitted length */
    int curlen;                /* length of current code */
    int nextlen = tree[0].Len; /* length of next code */
    int count = 0;             /* repeat count of the current code */
    int max_count = 7;         /* max repeat count */
    int min_count = 4;         /* min repeat count */

    if (nextlen == 0) max_count = 138, min_count = 3;
    tree[max_code+1].Len = (ush)0xffff; /* guard */

    for (n = 0; n <= max_code; n++) {
        curlen = nextlen; nextlen = tree[n+1].Len;
        if (++count < max_count && curlen == nextlen) {
            continue;
        } else if (count < min_count) {
            s->bl_tree[curlen].Freq += count;
        } else if (curlen != 0) {
            if (curlen != prevlen) s->bl_tree[curlen].Freq++;
            s->bl_tree[REP_3_6].Freq++;
        } else if (count <= 10) {
            s->bl_tree[REPZ_3_10].Freq++;
        } else {
            s->bl_tree[REPZ_11_138].Freq++;
        }
        count = 0; prevlen = curlen;
        if (nextlen == 0) {
            max_count = 138, min_count = 3;
        } else if (curlen == nextlen) {
            max_count = 6, min_count = 3;
        } else {
            max_count = 7, min_count = 4;
        }
    }
}

/* ===========================================================================
 * Send a literal or distance tree in compressed form, using the codes in
 * bl_tree.
 */
 void send_tree (deflate_state *s,ct_data *tree,int max_code)
{
    int n;                     /* iterates over all tree elements */
    int prevlen = -1;          /* last emitted length */
    int curlen;                /* length of current code */
    int nextlen = tree[0].Len; /* length of next code */
    int count = 0;             /* repeat count of the current code */
    int max_count = 7;         /* max repeat count */
    int min_count = 4;         /* min repeat count */

    /* tree[max_code+1].Len = -1; */  /* guard already set */
    if (nextlen == 0) max_count = 138, min_count = 3;

    for (n = 0; n <= max_code; n++) {
        curlen = nextlen; nextlen = tree[n+1].Len;
        if (++count < max_count && curlen == nextlen) {
            continue;
        } else if (count < min_count) {
            do { send_code(s, curlen, s->bl_tree); } while (--count != 0);

        } else if (curlen != 0) {
            if (curlen != prevlen) {
                send_code(s, curlen, s->bl_tree); count--;
            }
            Assert(count >= 3 && count <= 6, " 3_6?");
            send_code(s, REP_3_6, s->bl_tree); send_bits(s, count-3, 2);

        } else if (count <= 10) {
            send_code(s, REPZ_3_10, s->bl_tree); send_bits(s, count-3, 3);

        } else {
            send_code(s, REPZ_11_138, s->bl_tree); send_bits(s, count-11, 7);
        }
        count = 0; prevlen = curlen;
        if (nextlen == 0) {
            max_count = 138, min_count = 3;
        } else if (curlen == nextlen) {
            max_count = 6, min_count = 3;
        } else {
            max_count = 7, min_count = 4;
        }
    }
}

/* ===========================================================================
 * Construct the Huffman tree for the bit lengths and return the index in
 * bl_order of the last bit length code to send.
 */
 int build_bl_tree(deflate_state *s)
{
    int max_blindex;  /* index of last bit length code of non zero freq */

    /* Determine the bit length frequencies for literal and distance trees */
    scan_tree(s, (ct_data *)s->dyn_ltree, s->l_desc.max_code);
    scan_tree(s, (ct_data *)s->dyn_dtree, s->d_desc.max_code);

    /* Build the bit length tree: */
    build_tree(s, (tree_desc *)(&(s->bl_desc)));
    /* opt_len now includes the length of the tree representations, except
     * the lengths of the bit lengths codes and the 5+5+4 bits for the counts.
     */

    /* Determine the number of bit length codes to send. The pkzip format
     * requires that at least 4 bit length codes be sent. (appnote.txt says
     * 3 but the actual value used is 4.)
     */
    for (max_blindex = BL_CODES-1; max_blindex >= 3; max_blindex--) {
        if (s->bl_tree[bl_order[max_blindex]].Len != 0) break;
    }
    /* Update opt_len to include the bit length tree and counts */
    s->opt_len += 3*(max_blindex+1) + 5+5+4;
    Tracev((stderr, "\ndyn trees: dyn %ld, stat %ld",
            s->opt_len, s->static_len));

    return max_blindex;
}

/* ===========================================================================
 * Send the header for a block using dynamic Huffman trees: the counts, the
 * lengths of the bit length codes, the literal tree and the distance tree.
 * IN assertion: lcodes >= 257, dcodes >= 1, blcodes >= 4.
 */
 void send_all_trees(deflate_state *s,int lcodes, int dcodes, int blcodes)
{
    int rank;                    /* index in bl_order */

    Assert (lcodes >= 257 && dcodes >= 1 && blcodes >= 4, "not enough codes");
    Assert (lcodes <= L_CODES && dcodes <= D_CODES && blcodes <= BL_CODES,
            "too many codes");
    Tracev((stderr, "\nbl counts: "));
    send_bits(s, lcodes-257, 5); /* not +255 as stated in appnote.txt */
    send_bits(s, dcodes-1,   5);
    send_bits(s, blcodes-4,  4); /* not -3 as stated in appnote.txt */
    for (rank = 0; rank < blcodes; rank++) {
        Tracev((stderr, "\nbl code %2d ", bl_order[rank]));
        send_bits(s, s->bl_tree[bl_order[rank]].Len, 3);
    }
    Tracev((stderr, "\nbl tree: sent %ld", s->bits_sent));

    send_tree(s, (ct_data *)s->dyn_ltree, lcodes-1); /* literal tree */
    Tracev((stderr, "\nlit tree: sent %ld", s->bits_sent));

    send_tree(s, (ct_data *)s->dyn_dtree, dcodes-1); /* distance tree */
    Tracev((stderr, "\ndist tree: sent %ld", s->bits_sent));
}

/* ===========================================================================
 * Send the block data compressed using the given Huffman trees
 */
 void compress_block(deflate_state *s,ct_data *ltree,ct_data *dtree)
{
    unsigned dist;      /* distance of matched string */
    int lc;             /* match length or unmatched char (if dist == 0) */
    unsigned lx = 0;    /* running index in l_buf */
    unsigned code;      /* the code to send */
    int extra;          /* number of extra bits to send */

    if (s->last_lit != 0) do {
        dist = s->d_buf[lx];
        lc = s->l_buf[lx++];
        if (dist == 0) {
            send_code(s, lc, ltree); /* send a literal byte */
            Tracecv(isgraph(lc), (stderr," '%c' ", lc));
        } else {
            /* Here, lc is the match length - MIN_MATCH */
            code = _length_code[lc];
            send_code(s, code+LITERALS+1, ltree); /* send the length code */
            extra = extra_lbits[code];
            if (extra != 0) {
                lc -= base_length[code];
                send_bits(s, lc, extra);       /* send the extra length bits */
            }
            dist--; /* dist is now the match distance - 1 */
            code = dist < 256 ? _dist_code[dist] : _dist_code[256+((dist)>>7)];//d_code(dist);
            Assert (code < D_CODES, "bad d_code");

            send_code(s, code, dtree);       /* send the distance code */
            extra = extra_dbits[code];
            if (extra != 0) {
                dist -= base_dist[code];
                send_bits(s, dist, extra);   /* send the extra distance bits */
            }
        } /* literal or match pair ? */

        /* Check that the overlay between pending_buf and d_buf+l_buf is ok: */
        Assert(s->pending < s->lit_bufsize + 2*lx, "pendingBuf overflow");

    } while (lx < s->last_lit);

    send_code(s, END_BLOCK, ltree);
    s->last_eob_len = ltree[END_BLOCK].Len;
}

/* ===========================================================================
 * Fluxh the bit buffer and align the output on a byte boundary
 */
 void bi_windup(deflate_state *s)
{
    if (s->bi_valid > 8) {
        put_short(s, s->bi_buf);
    } else if (s->bi_valid > 0) {
        put_byte(s, (Byte)s->bi_buf);
    }
    s->bi_buf = 0;
    s->bi_valid = 0;
}

/* ===========================================================================
 * Determine the best encoding for the current block: dynamic trees, static
 * trees or store, and output the encoded block to the zip file.
 */
void _tr_fluxh_block(deflate_state *s,char *buf,uLong stored_len,int eof)
{
    uLong opt_lenb, static_lenb; /* opt_len and static_len in bytes */
    int max_blindex = 0;  /* index of last bit length code of non zero freq */

    /* Build the Huffman trees unless a stored block is forced */
    if (s->level > 0) {

	/* Construct the literal and distance trees */
	build_tree(s, (tree_desc *)(&(s->l_desc)));
	Tracev((stderr, "\nlit data: dyn %ld, stat %ld", s->opt_len,
		s->static_len));

	build_tree(s, (tree_desc *)(&(s->d_desc)));
	Tracev((stderr, "\ndist data: dyn %ld, stat %ld", s->opt_len,
		s->static_len));
	/* At this point, opt_len and static_len are the total bit lengths of
	 * the compressed block data, excluding the tree representations.
	 */

	/* Build the bit length tree for the above two trees, and get the index
	 * in bl_order of the last bit length code to send.
	 */
	max_blindex = build_bl_tree(s);

	/* Determine the best encoding. Compute first the block length in bytes*/
	opt_lenb = (s->opt_len+3+7)>>3;
	static_lenb = (s->static_len+3+7)>>3;

	Tracev((stderr, "\nopt %lu(%lu) stat %lu(%lu) stored %lu lit %u ",
		opt_lenb, s->opt_len, static_lenb, s->static_len, stored_len,
		s->last_lit));

	if (static_lenb <= opt_lenb) opt_lenb = static_lenb;

    } else {
        Assert(buf != (char*)0, "lost buf");
	opt_lenb = static_lenb = stored_len + 5; /* force a stored block */
    }

#ifdef FORCE_STORED
    if (buf != (char*)0) { /* force stored block */
#else
    if (stored_len+4 <= opt_lenb && buf != (char*)0) {
                       /* 4: two words for the lengths */
#endif
        /* The test buf != NULL is only necessary if LIT_BUFSIZE > WSIZE.
         * Otherwise we can't have processed more than WSIZE input bytes since
         * the last block fluxh, because compression would have been
         * successful. If LIT_BUFSIZE <= WSIZE, it is never too late to
         * transform a block into a stored block.
         */
        //_tr_stored_block(s, buf, stored_len, eof);
		int a = 0;

#ifdef FORCE_STATIC
    } else if (static_lenb >= 0) { /* force static trees */
#else
    } else if (static_lenb == opt_lenb) {
#endif
        send_bits(s, (STATIC_TREES<<1)+eof, 3);
        compress_block(s, (ct_data *)static_ltree, (ct_data *)static_dtree);
    } else {
        send_bits(s, (DYN_TREES<<1)+eof, 3);
        send_all_trees(s, s->l_desc.max_code+1, s->d_desc.max_code+1,
                       max_blindex+1);
        compress_block(s, (ct_data *)s->dyn_ltree, (ct_data *)s->dyn_dtree);
    }
    Assert (s->compressed_len == s->bits_sent, "bad compressed size");
    /* The above check is made mod 2^32, for files larger than 512 MB
     * and uLong implemented on 32 bits.
     */
    init_block(s);

    if (eof) {
        bi_windup(s);
    }
    Tracev((stderr,"\ncomprlen %lu(%lu) ", s->compressed_len>>3,s->compressed_len-7*eof));
}

int compress(Byte* inbuf,int len, Byte* outbuf)
{
	int err		= Z_OK;
	z_stream	strm;
	uLong crc	= 0;     /* crc32 of uncompressed data */
	int done	= 0;
	int	outlen	= 0;

	Byte*		nextout; /* output buffer */
	strm.zalloc	= (alloc_func)0;
	strm.zfree	= (free_func)0;
	strm.opaque	= (Byte*)0;
	strm.next_in	= Z_NULL;
	strm.next_out = Z_NULL;
	strm.avail_in = strm.avail_out = 0;
	
	err = deflateInit2(&strm, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	/* windowBits is passed < 0 to suppress zlib header */
	strm.next_out = (Byte*)ALLOC(Z_BUFSIZE);
	nextout = strm.next_out;

	strm.avail_out = Z_BUFSIZE;

	// write gzip header
	outbuf[outlen++] = 0x1f;
	outbuf[outlen++] = 0x8b;
	outbuf[outlen++] = Z_DEFLATED;
	for(;outlen<9;)
		outbuf[outlen++] = 0;
	outbuf[outlen++] = 0xff;

	strm.next_in	= (Byte*)inbuf;
	strm.avail_in = len;

	while (strm.avail_in != 0)
	{
	    err = deflate(&strm, Z_NO_FLUSH);
        if (err != Z_OK) break;
	}

	crc = crc32(crc, (const Byte *)inbuf, len);
	if (strm.avail_in != 0) 
		return -1;  
	
	/* should be zero already anyway */
	for (;;) {
		len = Z_BUFSIZE - strm.avail_out;

		if (len != 0) {
			zmemcpy(outbuf+outlen, nextout, len);
			
			outlen += len;

			strm.next_out = nextout;
			strm.avail_out = Z_BUFSIZE;
		}
		if (done) 
			break;
	    
		err = deflate(&strm, Z_FINISH);
		done = (strm.avail_out != 0 || err == Z_STREAM_END);
	}

	// write gzip tailer
	outbuf[outlen++] = (Byte)crc&0xff;
	outbuf[outlen++] = (Byte)crc>>8&0xff;
	outbuf[outlen++] = (Byte)crc>>16&0xff;
	outbuf[outlen++] = (Byte)crc>>24&0xff;
	
	outbuf[outlen++] = (Byte)strm.total_in;
	outbuf[outlen++] = (Byte)strm.total_in>>8;
	outbuf[outlen++] = (Byte)strm.total_in>>16;
	outbuf[outlen++] = (Byte)strm.total_in>>24;

	if (strm.state != Z_NULL) 
		err = deflateEnd(&strm);

	TRYFREE(strm.next_out);
	return outlen;
}
#endif
