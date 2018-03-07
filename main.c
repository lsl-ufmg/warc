#include "main.h"

int main(int argc, char *argv[]) {
  warc_t *warc;
  args_t args;

  args.threshold = 10000000;
  args.json = 0;
  args.dir = -1;
  args.distance_warc = 0;
  args.meta = 0;

  parse_args(argc, argv, &args);

  warc = warc_new(args.threshold, args.json, args.dir, args.path);

  if(!args.meta && !args.distance_warc){
    while(!warc_next_response(warc)){}
  }
  else if(args.meta){
    while(!warc_next_metadata(warc)){}
    str_hash_print(warc->dict);
  }
  else if(args.distance_warc){
    calculate_two_days_distance(warc);
  }

  warc_free(warc);
  args_free(&args);

  return 0;
}

void parse_args(int argc, char **argv, args_t *args){
  extern char *optarg;
  int c;

  while((c = getopt(argc,argv,"t:jmd:lp:h")) != EOF)
    switch(c) {
      case 't': // size limit
        args->threshold = check_positive_int(optarg);
        if(args->threshold == -1){
          handle_error("THRESHOLD must be an integer");
        }
        break;
      case 'j': // json or file content
        args->json = 1;
        break;
      case 'm': // parse metadata
        args->meta = 1;
        break;
      case 'd': // output dir
        args->dir = check_positive_int(optarg);
        if(args->dir == -1){
          handle_error("DIR must be an integer");
        }
        break;
      case 'l': // warc or distance
        args->distance_warc = 1;
        break;
      case 'p':
        args->path = strdup(optarg);
        break;
      case 'h':
        usage();
        exit(0);
        break;
      default:
        usage();
        exit(1);
        break;
    }
}

void usage(){
  fprintf(stderr,"WARC Parser and Metric Calculator\n");
  fprintf(stderr,"Input consists of Warc files sento to stdin\n");
  fprintf(stderr,"The default behavior is to save the content of each html to a file\n");
  fprintf(stderr,"\t-t <threshold>    (content size threshold in MB)\n");
  fprintf(stderr,"\t-j                (if used, content is saved to json)\n");
  fprintf(stderr,"\t-m                (if used, parses only metadata from warcs\n");
  fprintf(stderr,"\t-d <dir>          (output dir - needs to be an integer)\n");
  fprintf(stderr,"\t-p <path>         (output path - it's the path that contains <dir>)\n");
  fprintf(stderr,"\t-l                (executes the distance between the <dir> passed\n");
  fprintf(stderr,"\t                  to -d and <dir - 1> for all possible files)\n");
  fprintf(stderr,"\t-h                (usage)\n");
}

void args_free(args_t *args){
  free(args->path);
}
