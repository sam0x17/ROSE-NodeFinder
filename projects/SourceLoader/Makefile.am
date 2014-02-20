include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_ENABLE_PROJECT_SOURCE_LOADER

# Forwards
EXTRA_DIST = 
MOSTLYCLEANFILES =
noinst_PROGRAMS =
bin_PROGRAMS =
CHECK_TARGETS =
CLEAN_TARGETS = SourceLoaderTest srcloader *.a *.o *.txt

#------------------------------------------------------------------------------------------------------------------------
# Header files, etc
EXTRA_DIST += SourceLoader.h

#------------------------------------------------------------------------------------------------------------------------
# Specimens, test inputs
SPECIMENS = 
EXTRA_DIST += $(SPECIMENS)

#------------------------------------------------------------------------------------------------------------------------
# SourceLoader
noinst_LIBRARIES = libsourceloader.a
libsourceloader_a_SOURCES = SourceLoader.C
INCLUDES = $(ROSE_INCLUDES)
LDADD = $(ROSE_LIBS)
#------------------------------------------------------------------------------------------------------------------------
# srcloader
noinst_PROGRAMS += srcloader
srcloader_SOURCES = srcloader.C
srcloader_CPPFLAGS = $(ROSE_INCLUDES)
srcloader_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) libsourceloader.a
#------------------------------------------------------------------------------------------------------------------------
# SourceLoader Tests
noinst_PROGRAMS += SourceLoaderTest
SourceloaderTest_SOURCES = SourceLoaderTest.C
SourceLoaderTest_CPPFLAGS = $(ROSE_INCLUDES)
SourceLoaderTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) libsourceloader.a
#------------------------------------------------------------------------------------------------------------------------
# automake check and clean rules
check-local: $(CHECK_TARGETS)
	make test
check:
	make test
clean: $(CLEAN_TARGETS)
	rm -f $(MOSTLYCLEANFILES)
test:
	./SourceLoaderTest $(srcdir)/test_sample.C
endif
