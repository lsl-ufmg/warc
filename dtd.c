#include "dtd.h"

void not_used_cb(void *ctx, const char *msg, ...) {
  (void) ctx;
  (void) msg;
}

void dtd_cb(void *ctx, const char *msg, ...) {
  (void) msg;
  (*(int *) ctx)++;
}

void w3c_validation_errors(char *file, char *dtd_str, int *w3c_errors, int *xml_errors){
  xmlDocPtr doc;
  xmlValidCtxtPtr cvp;
  static xmlDtdPtr dtd = NULL;
  int stder, out;
  char *command;
  FILE *java;

  *xml_errors = 0;
  *w3c_errors = 0;

  xmlInitParser();

  cvp = xmlNewValidCtxt();

  //parsing xml
  stder = dup(2);
  close(2);
  out = open("/dev/null", O_WRONLY);

  if((doc = xmlReadFile(file, NULL, XML_PARSE_RECOVER)) == NULL) {
    //*xml_errors = parse_xml_file(file);
  }
  else{
    if(dtd_str == NULL){
      if(asprintf(&command, "java -Xmx1024m -cp vnu.jar nu.validator.client.HttpClient %s | grep erro | wc -l", file) == -1)
        handle_error("Could not read java output\n");
      if((java = popen(command, "r")) != NULL){
        if(fscanf(java, "%d", w3c_errors) != 1)
          handle_error("Read wrong number of params\n");
        pclose(java);
      }
      else{
        printf("java not executed\n");
      }
      free(command);
    }
    else{
      //printf("%s\n", dtd_str);
      if(!dtd){
        xmlLoadCatalog("/etc/xml/catalog");
        dtd = xmlParseDTD((const xmlChar *) "-//W3C//DTD XHTML 1.0 Strict//EN", NULL);
      }
      cvp->userData = w3c_errors;
      cvp->error    = dtd_cb;
      cvp->warning  = dtd_cb;

      xmlValidateDtd(cvp, doc, dtd);
    }
  }

  close(out);
  dup2(stder, 2);
  close(stder);


  xmlFreeValidCtxt(cvp);
  xmlFreeDoc(doc);
  xmlCleanupParser();
}

int parse_xml_file(const char *filename){
  xmlSAXHandler my_handler = {0};
  my_handler.error = my_xml_error;

  int count = 0;

  if (xmlSAXUserParseFile(&my_handler, &count, filename) < 0){
    return 0;
  }
  else{
    return count;
  }
}

//static void
//my_warning(void *user_data, const char *msg, ...) {
//    va_list args;
//
//    va_start(args, msg);
//    g_logv("XML", G_LOG_LEVEL_WARNING, msg, args);
//    va_end(args);
//}


void my_xml_error(void *user_data, const char *msg, ...) {
  va_list args;
  int *count = user_data;
  (*count)++;
  va_start(args, msg);
  //vprintf(msg, args);
  va_end(args);
}

//static void
//my_fatalError(void *user_data, const char *msg, ...) {
//    va_list args;
//
//    va_start(args, msg);
//    g_logv("XML", G_LOG_LEVEL_ERROR, msg, args);
//    va_end(args);
//}
