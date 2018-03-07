#include "dist.h"

int
levenshtein(char *s1, char *s2, size_t s1len, size_t s2len) {
  unsigned int x, y, lastdiag, olddiag, *column, out;

  // Return 0 if the strings are the same
  if (s1len == s2len && strcmp(s1, s2) == 0) {
    return 0;
  }

  column = malloc(sizeof(unsigned int) * (s1len + 1));

  for (y = 1; y <= s1len; y++)
    column[y] = y;
  for (x = 1; x <= s2len; x++) {
    column[0] = x;
    for (y = 1, lastdiag = x-1; y <= s1len; y++) {
      olddiag = column[y];
      column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
      lastdiag = olddiag;
    }
  }
  out = column[s1len];
  free(column);

  return(out);
}

double calculate_dist_fragments(int file1, int file2, size_t s1, size_t s2, float begin, float end){
  char *buf1, *buf2, *temp1 = NULL, *temp2 = NULL;
  size_t t1, t2;
  int piece1_lenght, piece2_lenght;
  int begin_pos, end_pos;
  //int lev;
  double sift;

	buf1 = mmap(NULL, s1, PROT_READ, MAP_PRIVATE, file1, 0);

  if(buf1 == MAP_FAILED)
    handle_error("mmap");

	buf2 = mmap(NULL, s2, PROT_READ, MAP_PRIVATE, file2, 0);

  if(buf2 == MAP_FAILED)
    handle_error("mmap");

  close(file1);
  close(file2);

  t1 = remove_spaces_breaklines(buf1, &temp1, s1);
  t2 = remove_spaces_breaklines(buf2, &temp2, s2);

  // originals not needed anymore
  munmap(buf1, s1);
  munmap(buf2, s2);

  // calculates the positions
  begin_pos = begin * t1;
  end_pos = end * t1;
  piece1_lenght = end_pos - begin_pos;
  buf1 = temp1 + begin_pos;
  temp1[end_pos] = '\0';

  begin_pos = begin * t2;
  end_pos = end * t2;
  piece2_lenght = end_pos - begin_pos;
  buf2 = temp2 + begin_pos;
  temp2[end_pos] = '\0';

  // calculates levenshtein
  //lev = levenshtein(buf1, buf2, piece1_lenght, piece2_lenght);
  sift = sift4(buf1, buf2, piece1_lenght, piece2_lenght);

  free(temp1);
  free(temp2);

  return sift;
}

int remove_spaces_breaklines(char *in, char **out, size_t s){
  size_t i;
  char *ptr2;
  char c = ' ', c1 = '\n', c2 = '\r';
  int size = 0;
  *out = malloc(sizeof(char)*s);
  ptr2 = *out;

  for(i = 0; i < s; i++){
    if(in[i] != c && in[i] != c1 && in[i] != c2){
      *ptr2 = in[i];
      ptr2++;
      size++;
    }
  }
  *ptr2 = '\0';
  return size;
}

double sift4(char *s, char *t, int ss, int st) {
  if(!ss || !st)
    return 0;

  int i, c1, c2, lcss, local_cs, max_str, max_offset;

  c1 = 0;
  c2 = 0;
  lcss = 0;
  local_cs = 0;
  max_str = MAX2(ss, st);
  max_offset = max_str / 2 - 1;

  while (c1 < ss && c2 < st) {
    if (s[c1] == t[c2]) {
      local_cs++;
    } else {
      lcss += local_cs;
      local_cs = 0;
      if (c1 != c2)
        c1 = c2 = MAX2(c1, c2);
      for (i = 0; i < max_offset && (c1 + i < ss || c2 + i < st); i++) {
        if (c1 + i < ss && s[c1 + i] == t[c2]) {
          c1 += i;
          local_cs++;
          break;
        }
        if (c2 + i < st && s[c1] == t[c2 + i]) {
          c2 += i;
          local_cs++;
          break;
        }
      }
    }
    c1++;
    c2++;
  }
  return (lcss + local_cs) / (double) max_str;
}
