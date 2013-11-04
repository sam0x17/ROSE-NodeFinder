include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_ENABLE_PROJECT_NODE_FINDER

# Forwards
EXTRA_DIST = 
MOSTLYCLEANFILES =
noinst_PROGRAMS =
bin_PROGRAMS =
CHECK_TARGETS =
CLEAN_TARGETS =

#------------------------------------------------------------------------------------------------------------------------
# Header files, etc
EXTRA_DIST += NodeFinder.h NodeFinderResult.h

#------------------------------------------------------------------------------------------------------------------------
# Specimens, test inputs
SPECIMENS = 
EXTRA_DIST += $(SPECIMENS)

#------------------------------------------------------------------------------------------------------------------------
# NodeFinder (using "fsm_" as a unique test prefix)
#noinst_LIBRARIES = libnodefinder.a
#libnodefinder_a_SOURCES = NodeFinderResult.C NodeFinder.C
#libnodefinder_a_CPPFLAGS = $(ROSE_INCLUDES)
#libnodefinder_a_LIBADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
# NodeFinder (using "fsm_" as a unique test prefix)
bin_PROGRAMS += NodeFinder
NodeFinder_SOURCES = NodeFinderResult.C NodeFinder.C
NodeFinder_CPPFLAGS = $(ROSE_INCLUDES)
NodeFinder_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# automake check and clean rules
check-local: $(CHECK_TARGETS)
clean-local: $(CLEAN_TARGETS)
	rm -f $(MOSTLYCLEANFILES)

endif
