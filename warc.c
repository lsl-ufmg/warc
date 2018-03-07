#include "warc.h"

gzFile file_out;

static int
warc_parse_version(warc_t *warc) {
  char header[] = "WARC/1.0";

  if(!fgets(warc->buffer, warc->buffer_size, warc->file))
    return -1;

  warc->buffer[strcspn(warc->buffer, "\r\n")] = '\0';

  if(strcmp(warc->buffer, header))
    handle_error("Expecting '%s', got '%s'", header, warc->buffer);

  return 0;
}

static void
warc_parse_token(warc_t *warc) {
  if(!fgets(warc->tmpbuffer, BUFFSIZE, warc->file))
    handle_error("Error creating buffer");

  if(sscanf(warc->tmpbuffer, "%[^:]: %s\n\n", warc->token, warc->buffer) != 2)
    handle_error("Error on token %s\n", warc->token);

  // seta a flag de response
  if(!strcmp(warc->token, "WARC-Type") && !strcmp(warc->buffer, "response")){
    warc->response = 1;
    warc->metadata = 0;
  }
  else if(!strcmp(warc->token, "WARC-Type") && !strcmp(warc->buffer, "metadata")){
    warc->response = 0;
    warc->metadata = 1;
  }
  else if(!strcmp(warc->token, "WARC-Type")){
    warc->response = 0;
    warc->metadata = 0;
  }
  if(!strcmp(warc->token, "Content-Type") && !strcmp(warc->buffer, "text/dns")){
    warc->response = 0;
    warc->metadata = 0;
  }
}


int warc_next_response(warc_t *warc) {
  char filename[33], *dtd_found;
  char dir_file[100];
  char json_file[100];
  FILE *f;
  json_object *jobj;
  int ret;
  uint32_t length;

  dtd_found = NULL;

  ret = warc_parse_version(warc);

  if(ret == -1)
    return -1;

  warc_parse_token(warc);
  assert(!strcmp(warc->token, "WARC-Type"));
  // cria objeto json
  jobj = json_object_new_object();

  // enquanto não chegar em Content-Length
  while(strcmp("Content-Length", warc->token)){
    warc_parse_token(warc);
    add_key_value_to_json(jobj, warc->token, warc->buffer);

    //TODO: se vier pra por o conteudo no json, nao gera filename
    //isso pode gerar problemas depois
    if(!warc->content_json && !strcmp("WARC-Target-URI", warc->token) &&
        warc->response){
      md5(warc->buffer, filename, strlen(warc->buffer));
      add_key_value_to_json(jobj, "md5", filename);
    }
  }

  // chegou em content length
  length = atoi(warc->buffer);
  if(warc->response && length < warc->threshold &&
      hash_get(warc->universe, filename)){

    // warc->buffer é o content-length
    skip_bytes(warc, 2);
    if(fread(warc->tmpbuffer, sizeof(char), length, warc->file) != length)
      handle_error("Read wrong number of bytes\n");
    warc->tmpbuffer[length] = '\0';
    skip_bytes(warc, 4);

    parse_header(warc, jobj);

    if(warc->content_json){
      add_key_value_to_json(jobj, "content", warc->buffer);
    }
    else
    {
      // filename

      strcpy(dir_file, warc->str_out_dir);
      strcat(dir_file, "/");
      strcat(dir_file, filename);
      strcpy(json_file, dir_file);
      strcat(json_file, ".json");

      // writes to file
      f = fopen(dir_file, "w");
      fprintf(f, "%s", warc->buffer);
      fclose(f);

      // finds dtd
      find_dtd(warc->buffer, &dtd_found);

      // add xml_dtd validations to json
      add_xml_dtd_errors(jobj, dir_file, dtd_found);

      // add distance if available
      // check_and_calculate_distance(warc, jobj, dir_file, filename);
    }
    //gzprintf(file_out, "%s\n", json_object_to_json_string(jobj));
    //printf("%s\n", json_object_to_json_string(jobj));
    f = fopen(json_file, "w");
    fprintf(f, "%s", json_object_to_json_string(jobj));
    fclose(f);
  }
  else{
    skip_bytes(warc, length + 6);
  }

  json_object_put(jobj);
  free(dtd_found);

  return 0;
}

int warc_next_metadata(warc_t *warc) {
  int ret;
  uint32_t length;
  char filename[100], uri[100000], uri_md5[100];
  char *str, s[2] = "\n";

  ret = warc_parse_version(warc);

  if(ret == -1)
    return -1;

  warc_parse_token(warc);
  assert(!strcmp(warc->token, "WARC-Type"));

  // enquanto não chegar em Content-Length
  while(strcmp("Content-Length", warc->token)){
    warc_parse_token(warc);

    if(!strcmp("WARC-Target-URI", warc->token) && warc->metadata){
      md5(warc->buffer, filename, strlen(warc->buffer));
      if(hash_get(warc->universe, filename)){
        str_hash_insert(warc->dict, filename, warc->buffer);
      }
      strcpy(uri, warc->buffer);
    }
  }

  // chegou em content length
  length = atoi(warc->buffer);

  if(warc->metadata){
    //hash_get(warc->universe, filename)) olhar depois pra filtrar
    //duma vez

    // warc->buffer é o content-length
    skip_bytes(warc, 2);
    if(fread(warc->tmpbuffer, sizeof(char), length, warc->file) != length)
      handle_error("Read wrong number of bytes\n");

    warc->tmpbuffer[length] = '\0';
    skip_bytes(warc, 4);

    str = strtok(warc->tmpbuffer, s);

    // while its not null neither  a single \r
    while(str != NULL){
      if(sscanf(str, "%[^:]: %[^\n]\n", warc->token, warc->buffer) == 0)
        handle_error("Error on token %s\n", warc->token);

      if(!strcmp(warc->token, "outlink")){
        //printf("%s\n", warc->buffer);
        //printf("%s %d\n", warc->buffer, strlen(warc->buffer));
        //printf("%s: %s\n", warc->token, warc->buffer);
        sscanf(warc->buffer, "%s %*s %*s %*s\n", warc->token);
        md5(warc->token, uri_md5, strlen(warc->token));
        if(hash_get(warc->universe, uri_md5) && hash_get(warc->universe, filename)){
          str_hash_insert(warc->dict, uri_md5, warc->token);
          fprintf(stderr, "%s %s\n", filename, uri_md5);
          //fprintf(stderr, "%s %s\n", uri_md5, warc->token);
        }
      }
      str = strtok(NULL, s);
    }
  }
  else{
    skip_bytes(warc, length + 6);
  }

  return 0;
}

//void check_and_calculate_distance(warc_t *warc, json_object *jobj, char *current_file_path, char *filename){
//  char previous_file_path[200];
//  char str_dist[50];
//  int dist;
//
//  strcpy(previous_file_path, warc->str_previous);
//  strcat(previous_file_path, "/");
//  strcat(previous_file_path, filename);
//
//  if(access(previous_file_path, R_OK) == -1){
//    add_key_value_to_json(jobj, "dist", "-1");
//  }
//  else{
//    dist = calculate_dist_fragments(current_file_path, previous_file_path, 70.0, 90.0);
//    sprintf(str_dist, "%d", dist);
//    add_key_value_to_json(jobj, "dist", str_dist);
//  }
//}

void find_dtd(char *buffer, char **dtd_out){
  char temp, *doc_type;

  temp = buffer[100];
  buffer[100] = '\0';
  doc_type = strstr(buffer, "\"-//");
  if(doc_type != NULL){
    if(sscanf(doc_type, "\"%m[^\"]", dtd_out) != 1)
      handle_error("Could not read correct param number\n");
  }
  buffer[100] = temp;
}

warc_t *
warc_new(int threshold, int content_json, int out_dir, char *path) {
  char temp[100];
  warc_t *warc;

  warc = malloc(sizeof(warc_t));

  // prepares the dir path
  strcat(warc->str_out_dir, path);
  strcat(warc->str_out_dir, "/");

  // copies for previous dir
  strcpy(warc->str_previous, warc->str_out_dir);

  // converts to str
  sprintf(temp, "%d", out_dir);

  // completes de filepath
  strcat(warc->str_out_dir, temp);

  // adds trailing '/'
  strcat(warc->str_out_dir, "/");

  // checks if dir exists
  check_file(warc->str_out_dir);

  // saves integer
  warc->out_dir = out_dir;

  // get previous day dir
  warc->previous = warc->out_dir - 1;

  // converts to string
  sprintf(temp, "%d", warc->previous);

  // completes the filepath
  strcat(warc->str_previous, temp);

  // adds trailing '/'
  strcat(warc->str_out_dir, "/");

  warc->file = fopen("/dev/stdin", "r");

  warc->buffer_size = BUFFSIZE;

  warc->buffer    = calloc(sizeof(char), warc->buffer_size);
  warc->tmpbuffer = calloc(sizeof(char), warc->buffer_size);
  warc->token     = calloc(sizeof(char), warc->buffer_size);

  warc->threshold    = threshold;
  warc->content_json = content_json;
  warc->universe     = hash_new();
  warc->dict         = str_hash_new();
  warc->path         = path;

  //carrega universo no hash
  strcpy(temp, warc->path);
  strcat(temp, "/");
  strcat(temp, "universe");
  warc->universe_count = hash_read_file(warc->universe, temp);

  return warc;
}

void
add_key_value_to_json(json_object *jobj, char *token, char *buffer){
  json_object *jbuffer = json_object_new_string(buffer);
  json_object_object_add(jobj, token, jbuffer);
}

void
warc_free(warc_t *warc) {
  fclose(warc->file);
  hash_free(warc->universe);
  str_hash_free(warc->dict);
  free(warc->buffer);
  free(warc->tmpbuffer);
  free(warc->token);
  free(warc);
}

void
parse_header_first_line(warc_t *warc){
  if(sscanf(warc->tmpbuffer, "%s %s", warc->buffer, warc->token) != 2)
    handle_error("Error header first line %s\n", warc->buffer);
}

void
parse_header(warc_t *warc, json_object *jobj){
  char s[2] = "\n";
  char *str;

  parse_header_first_line(warc);

  add_key_value_to_json(jobj, "protocol", warc->buffer);
  add_key_value_to_json(jobj, "code", warc->token);

  str = strtok(warc->tmpbuffer, s);
  str = strtok(NULL, s);
  // while its not null neither  a single \r
  while(str != NULL && strcmp(str, "\r")){
    if(sscanf(str, "%[^:]: %[^\n]\n", warc->token, warc->buffer) == 0)
      handle_error("Error on token %s\n", warc->token);
    add_key_value_to_json(jobj, warc->token, warc->buffer);
    str = strtok(NULL, s);
  }

  // if the remaining is not NULL, its the content
  if(str != NULL){
    str[strlen(str)] = ' ';
    strcpy(warc->buffer, str);
  }
  else{
    strcpy(warc->buffer, "");
  }
}

void add_xml_dtd_errors(json_object *jobj, char *dir_file, char *dtd_found){
  int xml_errors, dtd_errors;
  char *xml, *dtd;

  w3c_validation_errors(dir_file, dtd_found, &dtd_errors, &xml_errors);


  if(asprintf(&xml, "%d", xml_errors) == -1)
    handle_error("Could not read xml_errors\n");

  if(asprintf(&dtd, "%d", dtd_errors) == -1)
    handle_error("Could not read dtd_errors\n");

  add_key_value_to_json(jobj, "xml_errors", xml);
  add_key_value_to_json(jobj, "dtd_errors", dtd);

  free(xml);
  free(dtd);
}

void skip_bytes(warc_t *warc, size_t dataToRead){
  char buffer[4096];
  //  printf("%d\n", dataToRead);
  while(dataToRead){
    size_t toread = min(sizeof(buffer), dataToRead);
    size_t nread = fread(buffer, 1, toread, warc->file);
    dataToRead -= nread;
    //   printf("dataToRead: %d, nread: %d\n", dataToRead, nread);
  }
}

void calculate_two_days_distance(warc_t *warc){
  char f1[200], f2[200];
  char f2_json[200];
  const char *md5;
  files_compare *files;
  uint32_t i, count = 0;

  files = malloc(warc->universe_count * sizeof(files_compare));

  // check if paths exist
  check_file(warc->str_previous);
  check_file(warc->str_out_dir);

  // em vez do ls
  // tenho o universe
  // para cada um
  // fazer a mesma coisa
  // com exceção de pular o arquivo que existe em universe e não existe
  // no diretorio passado como parametro
  for(i = kh_begin(warc->universe); i != kh_end(warc->universe); i++){
    if(!kh_exist(warc->universe, i)){
      continue;
    }
    md5 = kh_key(warc->universe, i);

    // para cada um pegar o correspondente e checar a existencia
    if(sprintf(f1, "%s/%s", warc->str_previous, md5) == -1)
      handle_error("Could not assemble previous filepath\n");

    if(sprintf(f2, "%s/%s", warc->str_out_dir, md5) == -1)
      handle_error("Could not assemble current filepath\n");

    if(sprintf(f2_json, "%s%s", f2, ".json") == -1)
      handle_error("Could not assemble current filepath\n");

    files[count].f1 = strdup(f1);
    files[count].f2 = strdup(f2);
    files[count].f2_json = strdup(f2_json);
    count++;
  }

  // dispara execução
  runs_distance(files, count, 48);

  // free files
  for(i = 0; i < count; i++){
    free(files[i].f1);
    free(files[i].f2);
    free(files[i].f2_json);
  }
  free(files);
}

void runs_distance(files_compare *files, int size, int n_threads){
  int i;
  int step;
  pthread_params *threads;

  threads = (pthread_params *) malloc(sizeof(pthread_params) * n_threads);

  step = size / n_threads;

  for(i = 0; i < n_threads; i++) {
    threads[i].i = i * step;
    threads[i].files = files;

    if(i == n_threads - 1)
      threads[i].i_bound = size;
    else
      threads[i].i_bound = (i + 1) * step;

    if(pthread_create(&threads[i].thread, NULL, run_thread, (void *) &threads[i]))
      printf("Thread could not be created.\n");
  }

  for(i = 0; i < n_threads; i++) {
    pthread_join(threads[i].thread, NULL);
  }

  free(threads);
}

void *run_thread(void *p){
  int i, i_bound;
  double dist;
  pthread_params *params;
  files_compare *files;
  struct stat fs;
  size_t size1, size2;
  int f1, f2;

  params = (pthread_params *) p;
  files = params->files;

  i_bound = params->i_bound;

  for(i = params->i; i < i_bound; i++){

    f2 = open(files[i].f2, O_RDONLY);
    if(f2 == -1){
      continue;
    }

    if (fstat(f2, &fs) == -1){
      handle_error("fstat");
    }

    size2 = fs.st_size;

    f1 = open(files[i].f1, O_RDONLY);
    if(f1 == -1){
      update_json_file(files[i].f2_json, "dist", -1);
      close(f2);
      continue;
    }

    if (fstat(f1, &fs) == -1){
      handle_error("fstat");
    }

    size1 = fs.st_size;

    if(size1 == size2){
      //update_json_file(files[i].f2_json, "dist", 0);
      close(f1);
      close(f2);
      continue;
    }

    dist = calculate_dist_fragments(f1, f2, size1, size2, 0.65, 0.90);
    update_json_file(files[i].f2_json, "dist", dist);
    //printf("%d %s %s\n", dist, f1, f2);
  }
  return 0;
}

void update_json_file(char *json, char *key, double value){
  FILE *f;

  f = fopen(json, "r+");

  fseek(f, -2, SEEK_END);

  fprintf(f, ", \"%s\": \"%f\" }", key, value);

  fclose(f);
  return;
}
