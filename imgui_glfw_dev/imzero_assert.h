static void imzeroAssert(const char *func,const char *file,int line) {
   const char *basePath = std::getenv("IMZERO_ASSERT_BASE_PATH");
   fprintf(stderr,",------------------------[ ASSERTION ]-----------------------\n");
   if(basePath == nullptr) {
      fprintf(stderr,"| assertion raised: func %s in %s:%d (note: use IMZERO_ASSERT_BASE_PATH env variable to enable automatic printing of source code)\n",func,file,line);
   } else {
      fprintf(stderr,"| assertion raised: func %s in %s/%s:%d\n",func,basePath,file,line);
      constexpr auto sz = 8192;
      char buffer[sz];
      int b = line-3;
      int e = line+3;
      if(b < 1) { b = 1; }
      if(snprintf(buffer,sz,"sed -n '%d,%d'p '%s/%s' 1>&2",b,e,basePath,file) < sz) {
	 fprintf(stderr,"| env variable IMZERO_ASSERT_BASE_PATH detected, executing %s\n",buffer);
         system(buffer);
      }
   }
   fprintf(stderr,"`------------------------\n");
   fflush(stderr);
   assert(false && "imzeroAssert");
}
