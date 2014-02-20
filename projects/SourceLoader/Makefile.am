include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_ENABLE_PROJECT_SOURCE_LOADER

# Forwards
EXTRA_DIST = 
MOSTLYCLEANFILES =
noinst_PROGRAMS =
bin_PROGRAMS =
CHECK_TARGETS =
CLEAN_TARGETS = SourceLoaderTest *.a *.o *.txt

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
# SourceLoader Tests
noinst_PROGRAMS += SourceLoaderTest
NodeFinderTest_SOURCES = SourceLoaderTest.C
NodeFinderTest_CPPFLAGS = $(ROSE_INCLUDES)
NodeFinderTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) libsourceloader.a
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
