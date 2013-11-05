include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_ENABLE_PROJECT_NODE_FINDER

# Forwards
EXTRA_DIST = 
MOSTLYCLEANFILES =
noinst_PROGRAMS =
bin_PROGRAMS =
CHECK_TARGETS =
CLEAN_TARGETS = NodeFinderTest NodeFinderBenchmark *.a *.o *.txt

#------------------------------------------------------------------------------------------------------------------------
# Header files, etc
EXTRA_DIST += NodeFinder.h NodeFinderResult.h

#------------------------------------------------------------------------------------------------------------------------
# Specimens, test inputs
SPECIMENS = 
EXTRA_DIST += $(SPECIMENS)

#------------------------------------------------------------------------------------------------------------------------
# NodeFinder
noinst_LIBRARIES = libnodefinder.a
libnodefinder_a_SOURCES = NodeFinderResult.C NodeFinder.C
INCLUDES = $(ROSE_INCLUDES)
LDADD = $(ROSE_LIBS)
#------------------------------------------------------------------------------------------------------------------------
# NodeFinder Tests
noinst_PROGRAMS += NodeFinderTest
NodeFinderTest_SOURCES = NodeFinderTest.C
NodeFinderTest_CPPFLAGS = $(ROSE_INCLUDES)
NodeFinderTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) libnodefinder.a
#------------------------------------------------------------------------------------------------------------------------
# automake check and clean rules
check-local: $(CHECK_TARGETS)
	make test
check:
	make test
clean: $(CLEAN_TARGETS)
	rm -f $(MOSTLYCLEANFILES)
test:
	./NodeFinderTest --edg:no_warnings $(srcdir)/test_sample.C

endif
