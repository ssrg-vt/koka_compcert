/* The Computer Language Shootout
   http://shootout.alioth.debian.org/

   Contributed by Josh Goldfoot
   to compile, use gcc -O3

   This revision uses "simple_hash.h," available from
   http://cvs.alioth.debian.org/cgi-bin/cvsweb.cgi/shootout/bench/Include/?cvsroot=shootout

*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

enum { ht_num_primes = 28 };

static unsigned long ht_prime_list[ht_num_primes] = {
    53ul,         97ul,         193ul,       389ul,       769ul,
    1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
    49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
    1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
    50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
    1610612741ul, 3221225473ul, 4294967291ul
};

struct ht_node {
    char *key;
    int val;
    struct ht_node *next;
};

struct ht_ht {
    int size;
    struct ht_node **tbl;
    int iter_index;
    struct ht_node *iter_next;
    int items;
#ifdef HT_DEBUG
    int collisions;
#endif /* HT_DEBUG */
};

static inline int ht_val(struct ht_node *node) {
    return(node->val);
}

static inline char *ht_key(struct ht_node *node) {
    return(node->key);
}

static inline int ht_hashcode(struct ht_ht *ht, char *key) {
    unsigned long val = 0;
    for (; *key; ++key) val = 5 * val + *key;
    return(val % ht->size);
}

struct ht_node *ht_node_create(char *key) {
    char *newkey;
    struct ht_node *node;
    if ((node = (struct ht_node *)malloc(sizeof(struct ht_node))) == 0) {
	perror("malloc ht_node");
	exit(1);
    }
    if ((newkey = malloc(strlen(key) + 1)) == 0) {
	perror("strdup newkey");
	exit(1);
    }
    strcpy(newkey, key);
    node->key = newkey;
    node->val = 0;
    node->next = (struct ht_node *)NULL;
    return(node);
}

struct ht_ht *ht_create(int size) {
    int i = 0;
    struct ht_ht *ht = (struct ht_ht *)malloc(sizeof(struct ht_ht));
    while (ht_prime_list[i] < size) { i++; }
    ht->size = ht_prime_list[i];
    ht->tbl = (struct ht_node **)calloc(ht->size, sizeof(struct ht_node *));
    ht->iter_index = 0;
    ht->iter_next = 0;
    ht->items = 0;
#ifdef HT_DEBUG
    ht->collisions = 0;
#endif /* HT_DEBUG */
    return(ht);
}

void ht_destroy(struct ht_ht *ht) {
    struct ht_node *cur, *next;
    int i;
#ifdef HT_DEBUG
    int chain_len;
    int max_chain_len = 0;
    int density = 0;
    fprintf(stderr, " HT: size            %d\n", ht->size);
    fprintf(stderr, " HT: items           %d\n", ht->items);
    fprintf(stderr, " HT: collisions      %d\n", ht->collisions);
#endif /* HT_DEBUG */
    for (i=0; i<ht->size; i++) {
	next = ht->tbl[i];
#ifdef HT_DEBUG
	if (next) {
	    density++;
	}
	chain_len = 0;
#endif /* HT_DEBUG */
	while (next) {
	    cur = next;
	    next = next->next;
	    free(cur->key);
	    free(cur);
#ifdef HT_DEBUG
	    chain_len++;
#endif /* HT_DEBUG */
	}
#ifdef HT_DEBUG
	if (chain_len > max_chain_len)
	    max_chain_len = chain_len;
#endif /* HT_DEBUG */
    }
    free(ht->tbl);
    free(ht);
#ifdef HT_DEBUG
    fprintf(stderr, " HT: density         %d\n", density);
    fprintf(stderr, " HT: max chain len   %d\n", max_chain_len);
#endif /* HT_DEBUG */
}

struct ht_node *ht_find(struct ht_ht *ht, char *key) {
    int hash_code = ht_hashcode(ht, key);
    struct ht_node *node = ht->tbl[hash_code];
    while (node) {
	if (strcmp(key, node->key) == 0) return(node);
	node = node->next;
    }
    return((struct ht_node *)NULL);
}

struct ht_node *ht_find_new(struct ht_ht *ht, char *key) {
    int hash_code = ht_hashcode(ht, key);
    struct ht_node *prev = 0, *node = ht->tbl[hash_code];
    while (node) {
	if (strcmp(key, node->key) == 0) return(node);
	prev = node;
	node = node->next;
#ifdef HT_DEBUG
	ht->collisions++;
#endif /* HT_DEBUG */
    }
    ht->items++;
    if (prev) {
	return(prev->next = ht_node_create(key));
    } else {
	return(ht->tbl[hash_code] = ht_node_create(key));
    }
}

/*
 *  Hash Table iterator data/functions
 */
struct ht_node *ht_next(struct ht_ht *ht) {
    unsigned long index;
    struct ht_node *node = ht->iter_next;
    if (node) {
	ht->iter_next = node->next;
	return(node);
    } else {
	while (ht->iter_index < ht->size) {
	    index = ht->iter_index++;
	    if (ht->tbl[index]) {
		ht->iter_next = ht->tbl[index]->next;
		return(ht->tbl[index]);
	    }
	}
    }
    return((struct ht_node *)NULL);
}

struct ht_node *ht_first(struct ht_ht *ht) {
    ht->iter_index = 0;
    ht->iter_next = (struct ht_node *)NULL;
    return(ht_next(ht));
}

static inline int ht_count(struct ht_ht *ht) {
    return(ht->items);
}

long
hash_table_size (int fl, long buflen)
{
  long maxsize1, maxsize2;

  maxsize1 = buflen - fl;
  maxsize2 = 4;
  while (--fl > 0 && maxsize2 < maxsize1)
    maxsize2 = maxsize2 * 4;
  if (maxsize1 < maxsize2)
    return maxsize1;
  return maxsize2;
}

struct ht_ht *
generate_frequencies (int fl, char *buffer, long buflen)
{
  struct ht_ht *ht;
  char *reader;
  long i;
  char nulled;

  if (fl > buflen)
    return NULL;

  ht = ht_create (hash_table_size (fl, buflen));
  for (i = 0; i < buflen - fl + 1; i++)
    {
      reader = &(buffer[i]);
      nulled = reader[fl];
      reader[fl] = 0x00;
      ht_find_new (ht, reader)->val++;
      reader[fl] = nulled;
    }
  return ht;
}

typedef struct ssorter
{
  char *string;
  int num;
} sorter;

void
write_frequencies (int fl, char *buffer, long buflen)
{
  struct ht_ht *ht;
  long total, i, j, size;
  struct ht_node *nd;
  sorter *s;
  sorter tmp;

  ht = generate_frequencies (fl, buffer, buflen);
  total = 0;
  size = 0;
  for (nd = ht_first (ht); nd != NULL; nd = ht_next (ht))
    {
      total = total + nd->val;
      size++;
    }
  s = calloc (size, sizeof (sorter));
  i = 0;
  for (nd = ht_first (ht); nd != NULL; nd = ht_next (ht))
    {
      s[i].string = nd->key;
      s[i++].num = nd->val;
    }
  for (i = 0; i < size - 1; i++)
    for (j = i + 1; j < size; j++)
      if (s[i].num < s[j].num)
	{
	  memcpy (&tmp, &(s[i]), sizeof (sorter));
	  memcpy (&(s[i]), &(s[j]), sizeof (sorter));
	  memcpy (&(s[j]), &tmp, sizeof (sorter));
	}
  for (i = 0; i < size; i++)
    printf ("%s %.3f\n", s[i].string, 100 *  s[i].num / total);
  printf ("\n");
  ht_destroy (ht);
  free (s);
}

void
write_count (char *searchFor, char *buffer, long buflen)
{
  struct ht_ht *ht;

  ht = generate_frequencies (strlen (searchFor), buffer, buflen);
  printf ("%d\t%s\n", ht_find_new (ht, searchFor)->val, searchFor);
  ht_destroy (ht);
}

#define NRUNS 50

int
main ()
{
  char c;
  char *line, *buffer, *tmp, *x;
  int i, linelen, nothree;
  long buflen, seqlen;
  FILE * f;

  line = malloc (256);
  if (!line)
    return 2;
  seqlen = 0;
  nothree = 1;

  f = fopen("Results/knucleotide-input.txt", "r");
  if (f == NULL) return 2;

  while (nothree && fgets (line, 255, f))
    if (line[0] == '>' && line[1] == 'T' && line[2] == 'H')
      nothree = 0;
  free (line);

  buflen = 10240;
  buffer = malloc (buflen + 1);
  if (!buffer)
    return 2;
  x = buffer;

  while (fgets (x, 255, f))
    {
      linelen = strlen (x);
      if (linelen)
	{
	  if (x[linelen - 1] == '\n')
	    linelen--;
	  c = x[0];
	  if (c == '>')
	    break;
	  else if (c != ';')
	    {
	      seqlen = seqlen + linelen;
	      if (seqlen + 512 >= buflen)
		{
		  buflen = buflen + 10240;
		  tmp = realloc (buffer, buflen + 1);
		  if (tmp == NULL)
		    return 2;
		  buffer = tmp;
		  x = &(buffer[seqlen]);
		}
	      else
		x = &(x[linelen]);
	      x[0] = 0;
	    }
	}
    }
  for (i = 0; i < seqlen; i++)
    buffer[i] = toupper (buffer[i]);
  write_frequencies (1, buffer, seqlen);
  write_frequencies (2, buffer, seqlen);
  write_count ("GGT", buffer, seqlen);
  write_count ("GGTA", buffer, seqlen);
  write_count ("GGTATT", buffer, seqlen);
  write_count ("GGTATTTTAATT", buffer, seqlen);
  write_count ("GGTATTTTAATTTATAGT", buffer, seqlen);
  for (i = 0; i < NRUNS; i++) {
    struct ht_ht *  ht = generate_frequencies (6, buffer, seqlen);
    ht_destroy(ht);
  }
  free (buffer);
  fclose (f);
  return 0;
}

/**********
 build & benchmark results

BUILD COMMANDS FOR: knucleotide.gcc

Fri Sep 15 13:56:07 PDT 2006

/usr/bin/gcc -pipe -Wall -O3 -fomit-frame-pointer -funroll-loops -march=pentium4  knucleotide.c -o knucleotide.gcc_run

=================================================================
COMMAND LINE (%A is single numeric argument):

knucleotide.gcc_run %A
N=2500

*******/
