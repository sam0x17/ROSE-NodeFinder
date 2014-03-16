/**
 * \file    sage_support.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    April 4, 2012
 */

/*-----------------------------------------------------------------------------
 *  Dependencies
 *---------------------------------------------------------------------------*/
#include "sage_support.h"
#include "dwarfSupport.h"
#include "keep_going.h"
#include "failSafePragma.h"
#include "cmdline.h"

#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
#   include "FortranModuleInfo.h"
#   include "FortranParserState.h"
#   include "unparseFortran_modfile.h"
#endif

#include <algorithm>

// DQ (2/10/2014): We now want to avoid specifying this explicitly if possible.
// #define BOOST_FILESYSTEM_VERSION 2

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#ifdef __INSURE__
// Provide a dummy function definition to support linking with Insure++.
// We have not identified why this is required.  This fixes the problem of
// a link error for the "std::ostream & operator<<()" used with "std::vector<bool>".
std::ostream & 
operator<<(std::basic_ostream<char, std::char_traits<char> >& os, std::vector<bool, std::allocator<bool> >& m)
   {
     printf ("Inside of std::ostream & operator<<(std::basic_ostream<char, std::char_traits<char> >& os, std::vector<bool, std::allocator<bool> >& m): A test! \n");
  // ROSE_ASSERT(false);
     return os;
   }
#endif



using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace OmpSupport;

const string FileHelper::pathDelimiter = "/";

/* These symbols are defined when we include sage_support.h above - ZG (4/5/2013)
// DQ (9/17/2009): This appears to only be required for the GNU 4.1.x compiler (not for any earlier or later versions).
extern const std::string ROSE_GFORTRAN_PATH;

// CER (10/11/2011): Added to allow OFP jar file to depend on version number based on date.
extern const std::string ROSE_OFP_VERSION_STRING;
*/

#ifdef _MSC_VER
// DQ (11/29/2009): MSVC does not support sprintf, but "_snprintf" is equivalent
// (note: printf_S is the safer version but with a different function argument list).
// We can use a macro to handle this portability issue for now.
#define snprintf _snprintf
#endif

// DQ (2/12/2011): Added const so that this could be called in get_mangled() (and more generally).
// std::string SgValueExp::get_constant_folded_value_as_string()
std::string
SgValueExp::get_constant_folded_value_as_string() const
   {
  // DQ (8/18/2009): Added support for generating a string from a SgValueExp.
  // Note that the point is not to call unparse since that would provide the
  // expression tree and we want the constant folded value.

  // DQ (7/24/2012): Added a test.
     ROSE_ASSERT(this != NULL);

     string s;
     const int max_buffer_size = 500;
     char buffer[max_buffer_size];
     switch (variantT())
        {
          case V_SgIntVal:
             {
               const SgIntVal* integerValueExpression = isSgIntVal(this);
               ROSE_ASSERT(integerValueExpression != NULL);
               int numericValue = integerValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %d \n",numericValue);
               snprintf (buffer,max_buffer_size,"%d",numericValue);
               s = buffer;
               break;
             }

       // DQ (10/4/2010): Added case
          case V_SgLongIntVal:
             {
               const SgLongIntVal* integerValueExpression = isSgLongIntVal(this);
               ROSE_ASSERT(integerValueExpression != NULL);
               long int numericValue = integerValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %ld \n",numericValue);
               snprintf (buffer,max_buffer_size,"%ld",numericValue);
               s = buffer;
               break;
             }
          
         case V_SgLongLongIntVal:
         {
            const SgLongLongIntVal* integerValueExpression = isSgLongLongIntVal(this);
            ROSE_ASSERT(integerValueExpression != NULL);
            long long int numericValue = integerValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %ld \n",numericValue);
            snprintf (buffer,max_buffer_size,"%lld",numericValue);
            s = buffer;
            break;
         }

       // DQ (10/5/2010): Added case
          case V_SgShortVal:
             {
               const SgShortVal* integerValueExpression = isSgShortVal(this);
               ROSE_ASSERT(integerValueExpression != NULL);
               short int numericValue = integerValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %ld \n",numericValue);
               snprintf (buffer,max_buffer_size,"%d",numericValue);
               s = buffer;
               break;
             }

          case V_SgUnsignedShortVal:
             {
               const SgUnsignedShortVal* integerValueExpression = isSgUnsignedShortVal(this);
               ROSE_ASSERT(integerValueExpression != NULL);
               unsigned short int numericValue = integerValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %ld \n",numericValue);
               snprintf (buffer,max_buffer_size,"%u",numericValue);
               s = buffer;
               break;
             }

          case V_SgUnsignedLongLongIntVal:
             {
               const SgUnsignedLongLongIntVal* integerValueExpression = isSgUnsignedLongLongIntVal(this);
               ROSE_ASSERT(integerValueExpression != NULL);
               unsigned long long int numericValue = integerValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %llu \n",numericValue);
               snprintf (buffer,max_buffer_size,"%llu",numericValue);
               s = buffer;
               break;
             }

       // DQ (8/19/2009): Added case
          case V_SgUnsignedLongVal:
             {
               const SgUnsignedLongVal* integerValueExpression = isSgUnsignedLongVal(this);
               ROSE_ASSERT(integerValueExpression != NULL);
               unsigned long int numericValue = integerValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %llu \n",numericValue);
               snprintf (buffer,max_buffer_size,"%lu",numericValue);
               s = buffer;
               break;
             }

       // DQ (8/19/2009): Added case
          case V_SgUnsignedIntVal:
             {
               const SgUnsignedIntVal* integerValueExpression = isSgUnsignedIntVal(this);
               ROSE_ASSERT(integerValueExpression != NULL);
               unsigned int numericValue = integerValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %llu \n",numericValue);
               snprintf (buffer,max_buffer_size,"%u",numericValue);
               s = buffer;
               break;
             }

       // DQ (8/19/2009): Added case
          case V_SgBoolValExp:
             {
               const SgBoolValExp* booleanValueExpression = isSgBoolValExp(this);
               ROSE_ASSERT(booleanValueExpression != NULL);
               bool booleanValue = booleanValueExpression->get_value();
               snprintf (buffer,max_buffer_size,"%s",booleanValue == true ? "true" : "false");
               s = buffer;
               break;
             }

       // DQ (8/19/2009): Added case
          case V_SgStringVal:
             {
               const SgStringVal* stringValueExpression = isSgStringVal(this);
               ROSE_ASSERT(stringValueExpression != NULL);
               s = stringValueExpression->get_value();
               break;
             }

       // DQ (8/19/2009): Added case
          case V_SgCharVal:
             {
               const SgCharVal* charValueExpression = isSgCharVal(this);
               ROSE_ASSERT(charValueExpression != NULL);
            // DQ (9/24/2011): Handle case where this is non-printable character (see test2011_140.C, where
            // the bug was the the dot file had a non-printable character and caused zgrviewer to crash).
            // s = charValueExpression->get_value();
               char value = charValueExpression->get_value();
               if (isalnum(value) == true)
                  {
                 // Leave this as a alpha or numeric value where possible.
                    s = charValueExpression->get_value();
                  }
                 else
                  {
                 // Convert this to be a string of the numeric value so that it will print.
                    snprintf (buffer,max_buffer_size,"%d",value);
                    s = buffer;
                  }
               break;
             }

       // DQ (10/4/2010): Added case
          case V_SgFloatVal:
             {
               const SgFloatVal* floatValueExpression = isSgFloatVal(this);
               ROSE_ASSERT(floatValueExpression != NULL);
               float numericValue = floatValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %f \n",numericValue);
               snprintf (buffer,max_buffer_size,"%f",numericValue);
               s = buffer;
               break;
             }

       // DQ (10/4/2010): Added case
          case V_SgDoubleVal:
             {
               const SgDoubleVal* floatValueExpression = isSgDoubleVal(this);
               ROSE_ASSERT(floatValueExpression != NULL);
               double numericValue = floatValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %f \n",numericValue);
               snprintf (buffer,max_buffer_size,"%lf",numericValue);
               s = buffer;
               break;
             }

       // DQ (10/4/2010): Added case
          case V_SgLongDoubleVal:
             {
               const SgLongDoubleVal* floatValueExpression = isSgLongDoubleVal(this);
               ROSE_ASSERT(floatValueExpression != NULL);
               long double numericValue = floatValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %f \n",numericValue);
               snprintf (buffer,max_buffer_size,"%Lf",numericValue);
               s = buffer;
               break;
             }

       // DQ (10/4/2010): Added case
          case V_SgEnumVal:
             {
               const SgEnumVal* enumValueExpression = isSgEnumVal(this);
               ROSE_ASSERT(enumValueExpression != NULL);
               int numericValue = enumValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %d \n",numericValue);
               snprintf (buffer,max_buffer_size,"%d",numericValue);
               s = string("_enum_") + string(buffer);
               break;
             }

        // \pp (03/15/2011): Added case
          case V_SgUpcThreads:
             {
               s = "_upc_threads_";
               break;
             }

       // DQ (9/24/2011): Added support for complex values to be output as strings.
          case V_SgComplexVal:
             {
               const SgComplexVal* complexValueExpression = isSgComplexVal(this);
               ROSE_ASSERT(complexValueExpression != NULL);
#if 0
               float numericValue_realPart      = complexValueExpression->get_real_value();
               float numericValue_imaginaryPart = complexValueExpression->get_imaginary_value();
               printf ("numericValue of constant folded expression = (%f,%f) \n",numericValue_realPart,numericValue_imaginaryPart);
               snprintf (buffer,max_buffer_size,"(%f,%f)",numericValue_realPart,numericValue_imaginaryPart);
               s = buffer;
#else
            // ROSE_ASSERT(complexValueExpression->get_real_value() != NULL);
               string real_string = "null";
               if (complexValueExpression->get_real_value() != NULL)
                    real_string = complexValueExpression->get_real_value()->get_constant_folded_value_as_string();

            // ROSE_ASSERT(complexValueExpression->get_imaginary_value() != NULL);
               string imaginary_string = "null";
               if (complexValueExpression->get_imaginary_value() != NULL)
                    imaginary_string = complexValueExpression->get_imaginary_value()->get_constant_folded_value_as_string();

               s = "(" + real_string + "," + imaginary_string + ")";
#endif
               break;
             }

       // DQ (11/28/2011): Adding support for template declarations in the AST.
          case V_SgTemplateParameterVal:
             {
            // Note that constant folding on SgTemplateParameterVal expressions does not make any sense!
#if 1
               const SgTemplateParameterVal* templateParameterValueExpression = isSgTemplateParameterVal(this);
               ROSE_ASSERT(templateParameterValueExpression != NULL);
               string stringName = templateParameterValueExpression->get_template_parameter_name();
            // printf ("stringName of constant folded expression = %s \n",stringName);
            // snprintf (buffer,max_buffer_size,"%s",stringName.c_str());
            // s = buffer;
               s = stringName;
#else
            // DQ (11/28/2011): I hope that it does not make since to call this case...make it an error to do so...
               printf ("Note that constant folding on SgTemplateParameterVal expressions does not make any sense! \n");
            // ROSE_ASSERT(false);
               s = "_constant_folding_on_SgTemplateParameterVal_expressions_does_not_make_any_sense_";
#endif
               break;
             }

          default:
             {
               printf ("Error case of value = %s not handled \n",this->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

void
whatTypeOfFileIsThis( const string & name )
   {
  // DQ (2/3/2009): It is helpful to report what type of file this is where possible.
  // Call the Unix "file" command, it would be great if this was an available
  // system call (but Robb thinks it might not always be available).

     vector<string> commandLineVector;
     commandLineVector.push_back("file -b " + name);

  // printf ("Unknown file: %s ",name.c_str());
     printf ("Error: unknown file type: ");
     flush(cout);

  // I could not make this work!
  // systemFromVector (commandLineVector);

  // Use "-b" for brief mode!
     string commandLine = "file " + name;
     system(commandLine.c_str());
   }



void
outputTypeOfFileAndExit( const string & name )
   {
  // DQ (8/20/2008): The code (from Robb) identifies what kind of file this is or
  // more specifically what kind of file most tools would think this
  // file is (using the system file(1) command as a standard way to identify
  // file types using their first few bytes.

  // printf ("In outputTypeOfFileAndExit(%s): Evaluate the file type \n",name.c_str());

#if 0
  // DQ (2/3/2009): This works now, I think that Andreas fixed it.

  // Use file(1) to try to figure out the file type to report in the exception
     int child_stdout[2];
     pipe(child_stdout);
     pid_t pid = fork();

     printf ("pid = %d \n",pid);

     if (pid == -1)
        { // Error
          perror("fork: error in outputTypeOfFileAndExit ");
          exit (1);
        }
     if (0 == pid)
        {
          close(0);
          dup2(child_stdout[1], 1);
          close(child_stdout[0]);
          close(child_stdout[1]);
          execlp("/usr/bin/file", "/usr/bin/file", "-b", name.c_str(), NULL);
          exit(1);
        }
       else
        {
          int status;
          if (waitpid(pid, &status, 0) == -1)
             {
               perror("waitpid");
               abort();
             }

          char buf[4096];
          memset(buf, 0, sizeof buf);
          read(child_stdout[0], buf, sizeof buf);
          std::string buffer(buf);
          buffer =  name+ " unrecognized file format: " + buffer;

       // DQ (2/3/2009): It is helpful to report what type of file this is where possible.
          whatTypeOfFileIsThis(name);

          throw SgAsmGenericFile::FormatError(buffer.c_str());
        }
#else
     whatTypeOfFileIsThis(name);

  // printf ("\n\nExiting: Unknown file Error \n\n");
  // ROSE_ASSERT(false);
     abort();
#endif
   }



// DQ (1/5/2008): These are functions separated out of the generated
// code in ROSETTA.  These functions don't need to be generated since
// there implementation is not as dependent upon the IR as other functions
// (e.g. IR node member functions).
//
// Switches taking a second parameter need to be added to CommandlineProcessing::isOptionTakingSecondParameter().

string
findRoseSupportPathFromSource(const string& sourceTreeLocation,
                              const string& installTreeLocation) {
  string installTreePath;
  bool inInstallTree = roseInstallPrefix(installTreePath);
  if (inInstallTree) {
    return installTreePath + "/" + installTreeLocation;
  } else {
    return string(ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR) + "/" + sourceTreeLocation;
  }
}

string
findRoseSupportPathFromBuild(const string& buildTreeLocation,
                             const string& installTreeLocation) {
  string installTreePath;
  bool inInstallTree = roseInstallPrefix(installTreePath);
  if (inInstallTree) {
    return installTreePath + "/" + installTreeLocation;
  } else {
    return string(ROSE_AUTOMAKE_TOP_BUILDDIR) + "/" + buildTreeLocation;
  }
}
//! Check if we can get an installation prefix of rose based on the current running translator.
// There are two ways
//   1. if dladdr is supported: we resolve a rose function (roseInstallPrefix()) to obtain the
//      file (librose.so) defining this function
//      Then we check the parent directory of librose.so
//          if .libs or src --> in a build tree
//          otherwise: librose.so is in an installation tree
//   2. if dladdr is not supported or anything goes wrong, we check an environment variable
//     ROSE_IN_BUILD_TREE to tell if the translator is started from a build tree or an installation tree
//     Otherwise we pass the --prefix= ROSE_AUTOMAKE_PREFIX as the installation prefix
bool roseInstallPrefix(std::string& result) {
#ifdef HAVE_DLADDR
  {
 // This is built on the stack and initialized using the function: dladdr().
    Dl_info info;

 // DQ (4/8/2011): Initialize this before it is used as a argument to strdup() below. This is initialized 
 // by dladdr(), so this is likely redundant; but we can initialize it anyway.
 // info.dli_fname = NULL;
    info.dli_fname = "";

    int retval = dladdr((void*)(&roseInstallPrefix), &info);
    if (retval == 0) goto default_check;

 // DQ (4/9/2011): I think the issue here is that the pointer "info.dli_fname" pointer (char*) is pointing to 
 // a position inside a DLL and thus is a region of memory controled/monitored or allocated by Insure++. Thus
 // Insure++ is marking this as an issue while it is not an issue. The reported issue by Insure++ is: "READ_WILD",
 // implying that a pointer set to some wild area of memory is being read.
#if __INSURE__
 // Debugging information. Trying to understand this insure issue and the value of "info.dli_fname" data member.
 // if (retval != 0)
 //    fprintf(stderr, "      %08p file: %s\tfunction: %s\n",info.dli_saddr, info.dli_fname ? info.dli_fname : "???", info.dli_sname ? info.dli_sname : "???"); 

    _Insure_checking_enable(0); // disable Insure++ checking
#endif
 // DQ (4/8/2011): Check for NULL pointer before handling it as a parameter to strdup(), 
 // but I think it is always non-NULL (added assertion and put back the original code).
 // char* libroseName = (info.dli_fname == NULL) ? NULL : strdup(info.dli_fname);
    ROSE_ASSERT(info.dli_fname != NULL);
    char* libroseName = strdup(info.dli_fname);
#if __INSURE__
    _Insure_checking_enable(1); // re-enable Insure++ checking
#endif

    if (libroseName == NULL) goto default_check;
    char* libdir = dirname(libroseName);
    if (libdir == NULL) {free(libroseName); goto default_check;}
    char* libdirCopy1 = strdup(libdir);
    char* libdirCopy2 = strdup(libdir);
    if (libdirCopy1 == NULL || libdirCopy2 == NULL) { free(libroseName); free(libdirCopy1); free(libdirCopy2); goto default_check;}
    char* libdirBasenameCS = basename(libdirCopy1);
    if (libdirBasenameCS == NULL) {free(libroseName); free(libdirCopy1); free(libdirCopy2); goto default_check;}
    string libdirBasename = libdirBasenameCS;
    free(libdirCopy1);
    char* prefixCS = dirname(libdirCopy2);
    if (prefixCS == NULL) {free(libroseName); goto default_check;}
    string prefix = prefixCS;
    free(libdirCopy2); 

    // Zack Galbreath, June 2013
    // When building with CMake, detect build directory by searching
    // for the presence of a CMakeCache.txt file.  If this cannot
    // be found, then assume we are running from within an install tree.
    #ifdef USE_CMAKE
    std::string pathToCache = libdir;
    pathToCache += "/../CMakeCache.txt";
    if (boost::filesystem::exists(pathToCache)) {
      return false;
    } else {
      return true;
    }
    #endif

    free(libroseName);
// Liao, 12/2/2009
// Check the librose's parent directory name to tell if it is within a build or installation tree
// This if statement has the assumption that libtool is used to build librose so librose.so is put under .libs
// which is not true for cmake building system
// For cmake, librose is created directly under build/src
//    if (libdirBasename == ".libs") {
    if (libdirBasename == ".libs" || libdirBasename == "src") {
      return false;
    } else {
      // the translator must locate in the installation_tree/lib
       if (libdirBasename != "lib" && libdirBasename != "lib64")
          {
            printf ("Error: unexpected libdirBasename = %s (result = %s, prefix = %s) \n",libdirBasename.c_str(),result.c_str(),prefix.c_str());
          }

   // DQ (12/5/2009): Is this really what we need to assert?
   // ROSE_ASSERT (libdirBasename == "lib");

      result = prefix;
      return true;
    }
  }
#endif
default_check:
#ifdef HAVE_DLADDR
  // Emit a warning that the hard-wired prefix is being used
  cerr << "Warning: roseInstallPrefix() is using the hard-wired prefix and ROSE_IN_BUILD_TREE even though it should be relocatable" << endl;
#endif
  // dladdr is not supported, we check an environment variables to tell if the
  // translator is running from a build tree or an installation tree
  if (getenv("ROSE_IN_BUILD_TREE") != NULL) {
    return false;
  } else {
// Liao, 12/1/2009
// this variable is set via a very bad way, there is actually a right way to use --prefix VALUE within automake/autoconfig
// config/build_rose_paths.Makefile
// Makefile:       @@echo "const std::string ROSE_AUTOMAKE_PREFIX        = \"/home/liao6/opt/roseLatest\";" >> src/util/rose_paths.C
// TODO fix this to support both automake and cmake 's installation configuration options
    result = ROSE_AUTOMAKE_PREFIX;
    return true;
  }
}

/* This function suffers from the same problems as CommandlineProcessing::isExecutableFilename(), namely that the list of
 * magic numbers used here needs to be kept in sync with changes to the binary parsers. */
bool
isBinaryExecutableFile ( string sourceFilename )
   {
     bool returnValue = false;

     if ( SgProject::get_verbose() > 1 )
          printf ("Inside of isBinaryExecutableFile(%s) \n",sourceFilename.c_str());

  // Open file for reading
     FILE* f = fopen(sourceFilename.c_str(), "rb");
     if (!f)
        {
          printf ("Could not open file");
          ROSE_ASSERT(false);
        }

     int character0 = fgetc(f);
     int character1 = fgetc(f);

  // The first character of an ELF binary is '\127' and for a PE binary it is 'M'
  // Note also that some MS-DOS headers can start with "ZM" instead of "MZ" due to
  // early confusion about little endian handling for MS-DOS where it was ported
  // to not x86 plaforms.  I am not clear how wide spread loaders of this type are.

     if (character0 == 127 || character0 == 77)
        {
          if (character1 == 'E' || character1 == 'Z')
             {
               returnValue = true;
             }
        }

      fclose(f);

      return returnValue;
    }

bool
isLibraryArchiveFile ( string sourceFilename )
   {
  // The if this is a "*.a" file, not that "*.so" files
  // will appear as an executable (same for Windows "*.dll"
  // files.

     bool returnValue = false;

     if ( SgProject::get_verbose() > 1 )
          printf ("Inside of isLibraryArchiveFile(%s) \n",sourceFilename.c_str());

  // Open file for reading
     FILE* f = fopen(sourceFilename.c_str(), "rb");
     if (!f)
        {
          printf ("Could not open file in isLibraryArchiveFile()");
          ROSE_ASSERT(false);
        }

     string magicHeader;
     for (int i = 0; i < 7; i++)
        {
          magicHeader = magicHeader + (char)getc(f);
        }

  // printf ("magicHeader = %s \n",magicHeader.c_str());
     returnValue = (magicHeader == "!<arch>");

  // printf ("isLibraryArchiveFile() returning %s \n",returnValue ? "true" : "false");

     fclose(f);

     return returnValue;
   }


void
SgFile::initializeSourcePosition( const std::string & sourceFilename )
   {
     ROSE_ASSERT(this != NULL);

  // printf ("Inside of SgFile::initializeSourcePosition() \n");

     Sg_File_Info* fileInfo = new Sg_File_Info(sourceFilename,1,1);
     ROSE_ASSERT(fileInfo != NULL);

  // set_file_info(fileInfo);
     set_startOfConstruct(fileInfo);
     fileInfo->set_parent(this);
     ROSE_ASSERT(get_startOfConstruct() != NULL);
     ROSE_ASSERT(get_file_info() != NULL);
   }

void
SgSourceFile::initializeGlobalScope()
   {
     ROSE_ASSERT(this != NULL);

  // printf ("Inside of SgSourceFile::initializeGlobalScope() \n");

  // Note that SgFile::initializeSourcePosition() should have already been called.
     ROSE_ASSERT(get_startOfConstruct() != NULL);

     string sourceFilename = get_startOfConstruct()->get_filename();

  // DQ (8/31/2006): Generate a NULL_FILE (instead of SgFile::SgFile) so that we can
  // enforce that the filename is always an absolute path (starting with "/").
  // Sg_File_Info* globalScopeFileInfo = new Sg_File_Info("SgGlobal::SgGlobal",0,0);
     Sg_File_Info* globalScopeFileInfo = new Sg_File_Info(sourceFilename,0,0);
     ROSE_ASSERT (globalScopeFileInfo != NULL);

  // printf ("&&&&&&&&&& In SgSourceFile::initializeGlobalScope(): Building SgGlobal (with empty filename) &&&&&&&&&& \n");

     set_globalScope( new SgGlobal( globalScopeFileInfo ) );
     ROSE_ASSERT (get_globalScope() != NULL);

#if 0
  // DQ (6/23/2011): Changed my mind, I would like to avoid using this if possible.
  // DQ (6/15/2011): Added scope to hold unhandled declarations (see test2011_80.C).
     Sg_File_Info* holdingScopeFileInfo = new Sg_File_Info(sourceFilename,0,0);
     ROSE_ASSERT (holdingScopeFileInfo != NULL);
     set_temp_holding_scope( new SgGlobal( holdingScopeFileInfo ) );
     ROSE_ASSERT (get_temp_holding_scope() != NULL);

     get_temp_holding_scope()->set_parent(this);
#endif

     if (SageBuilder::symbol_table_case_insensitive_semantics == true)
        {
          get_globalScope()->setCaseInsensitive(true);
        }

  // DQ (2/15/2006): Set the parent of the SgGlobal IR node
     get_globalScope()->set_parent(this);

  // DQ (8/21/2008): Set the end of the global scope (even if it is updated later)
  // printf ("In SgFile::initialization(): p_root->get_endOfConstruct() = %p \n",p_root->get_endOfConstruct());
     ROSE_ASSERT(get_globalScope()->get_endOfConstruct() == NULL);
     get_globalScope()->set_endOfConstruct(new Sg_File_Info(sourceFilename,0,0));
     ROSE_ASSERT(get_globalScope()->get_endOfConstruct() != NULL);

  // DQ (1/21/2008): Set the filename in the SgGlobal IR node so that the traversal to add CPP directives and comments will succeed.
     ROSE_ASSERT (get_globalScope() != NULL);
     ROSE_ASSERT(get_globalScope()->get_startOfConstruct() != NULL);

  // DQ (8/21/2008): Modified to make endOfConstruct consistant (avoids warning in AST consistancy check).
  // ROSE_ASSERT(p_root->get_endOfConstruct()   == NULL);
     ROSE_ASSERT(get_globalScope()->get_endOfConstruct()   != NULL);

  // p_root->get_file_info()->set_filenameString(p_sourceFileNameWithPath);
  // ROSE_ASSERT(p_root->get_file_info()->get_filenameString().empty() == false);

#if 0
     Sg_File_Info::display_static_data("Resetting the SgGlobal startOfConstruct and endOfConstruct");
     printf ("Resetting the SgGlobal startOfConstruct and endOfConstruct filename (p_sourceFileNameWithPath = %s) \n",p_sourceFileNameWithPath.c_str());
#endif

  // DQ (12/22/2008): Added to support CPP preprocessing of Fortran files.
     string filename = p_sourceFileNameWithPath;
     if (get_requires_C_preprocessor() == true)
        {
       // This must be a Fortran source file (requiring the use of CPP to process its directives).
          filename = generate_C_preprocessor_intermediate_filename(filename);
        }

  // printf ("get_requires_C_preprocessor() = %s filename = %s \n",get_requires_C_preprocessor() ? "true" : "false",filename.c_str());

  // get_globalScope()->get_startOfConstruct()->set_filenameString(p_sourceFileNameWithPath);
     get_globalScope()->get_startOfConstruct()->set_filenameString(filename);
     ROSE_ASSERT(get_globalScope()->get_startOfConstruct()->get_filenameString().empty() == false);

  // DQ (8/21/2008): Uncommented to make the endOfConstruct consistant (avoids warning in AST consistancy check).
  // get_globalScope()->get_endOfConstruct()->set_filenameString(p_sourceFileNameWithPath);
     get_globalScope()->get_endOfConstruct()->set_filenameString(filename);
     ROSE_ASSERT(get_globalScope()->get_endOfConstruct()->get_filenameString().empty() == false);

#if 0
     printf ("DONE: Resetting the SgGlobal startOfConstruct and endOfConstruct filename (filename = %s) \n",filename.c_str());
     Sg_File_Info::display_static_data("DONE: Resetting the SgGlobal startOfConstruct and endOfConstruct");
#endif

  // DQ (12/23/2008): These should be in the Sg_File_Info map already.
     ROSE_ASSERT(Sg_File_Info::getIDFromFilename(get_file_info()->get_filename()) >= 0);
     if (get_requires_C_preprocessor() == true)
        {
          ROSE_ASSERT(Sg_File_Info::getIDFromFilename(generate_C_preprocessor_intermediate_filename(get_file_info()->get_filename())) >= 0);
        }
   }

SgFile*
#if 0
// FMZ (07/07/2010): "nextErrorCode" should be call by reference argument
determineFileType ( vector<string> argv, int nextErrorCode, SgProject* project )
#else
determineFileType ( vector<string> argv, int & nextErrorCode, SgProject* project )
#endif
   {
     SgFile* file = NULL;

  // DQ (2/4/2009): The specification of "-rose:binary" causes filenames to be interpreted
  // differently if they are object files or libary archive files.
  // DQ (4/21/2006): New version of source file name handling (set the source file name early)
  // printf ("In determineFileType(): Calling CommandlineProcessing::generateSourceFilenames(argv) \n");
  // Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argv);
     ROSE_ASSERT(project != NULL);
     Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argv,project->get_binary_only());

#if 0
  // this->display("In SgFile::setupSourceFilename()");
     printf ("In determineFileType(): listToString(argv) = %s \n",StringUtility::listToString(argv).c_str());
     printf ("In determineFileType(): listToString(fileList) = %s \n",StringUtility::listToString(fileList).c_str());
#endif

  // DQ (2/6/2009): This fails for the build function SageBuilder::buildFile(), so OK to comment it out.
  // DQ (12/23/2008): I think that we may be able to assert this is true, if so then we can simplify the code below.
     ROSE_ASSERT(fileList.empty() == false);

     if (fileList.empty() == false)
        {
if (fileList.size() != 1){
cout << endl;
for ( Rose_STL_Container<string>::iterator i = fileList.begin(); i != fileList.end(); i++) {
cout << (*i) << endl;
}
cout << endl;
cout.flush();
}
          ROSE_ASSERT(fileList.size() == 1);

       // DQ (8/31/2006): Convert the source file to have a path if it does not already
       // p_sourceFileNameWithPath    = *(fileList.begin());
          string sourceFilename = *(fileList.begin());

       // printf ("Before conversion to absolute path: sourceFilename = %s \n",sourceFilename.c_str());

       // sourceFilename = StringUtility::getAbsolutePathFromRelativePath(sourceFilename);
          sourceFilename = StringUtility::getAbsolutePathFromRelativePath(sourceFilename, true);

       // printf ("After conversion to absolute path: sourceFilename = %s \n",sourceFilename.c_str());

       // This should be an absolute path
          string targetSubstring = "/";
       // if (sourceFilename.substr(0,targetSubstring.size()) != targetSubstring)
       //      printf ("@@@@@@@@@@@@@@@@@@@@ In SgFile::setupSourceFilename(int,char**): sourceFilename = %s @@@@@@@@@@@@@@@@@@@@\n",sourceFilename.c_str());
       // ROSE_ASSERT(sourceFilename.substr(0,targetSubstring.size()) == targetSubstring);

       // Rama: 12/06/06: Fixup for problem with file names.
            // Made changes to this file and string utilities function getAbsolutePathFromRelativePath by cloning it with name getAbsolutePathFromRelativePathWithErrors
            // Also refer to script that tests -- reasonably exhaustively -- to various combinarions of input files.

       // Zack Galbreath 1/9/2014: Windows absolute paths do not begin with "/".
       // The following printf could cause problems for our testing systems because
       // it contains the word "error".
       #ifndef _MSC_VER
          if (sourceFilename.substr(0,targetSubstring.size()) != targetSubstring)
               printf ("sourceFilename encountered an error in filename\n");
       #endif
       
       // DQ (11/29/2006): Even if this is C mode, we have to define the __cplusplus macro
       // if we detect we are processing a source file using a C++ filename extension.
          string filenameExtension = StringUtility::fileNameSuffix(sourceFilename);

#if 0
          printf ("In determineFileType(): filenameExtension = %s \n",filenameExtension.c_str());
       // ROSE_ASSERT(false);
#endif

       // DQ (1/8/2014): We need to handle the case of "/dev/null" being used as an input filename.
          if (filenameExtension == "/dev/null")
             {
               printf ("Warning: detected use of /dev/null as input filename: not yet supported (exiting with 0 exit code) \n");
               exit(0);
             }

       // DQ (5/18/2008): Set this to true (redundant, since the default already specified as true)
       // file->set_requires_C_preprocessor(true);

       // DQ (11/17/2007): Mark this as a file using a Fortran file extension (else this turns off options down stream).
          if (CommandlineProcessing::isFortranFileNameSuffix(filenameExtension) == true)
             {
               SgSourceFile* sourceFile = new SgSourceFile ( argv,  project );
               file = sourceFile;

            // printf ("----------- Great location to set the sourceFilename = %s \n",sourceFilename.c_str());

            // DQ (12/23/2008): Moved initialization of source position (call to initializeSourcePosition())
            // to earliest position in setup of SgFile.

            // printf ("Calling file->set_sourceFileUsesFortranFileExtension(true) \n");
               file->set_sourceFileUsesFortranFileExtension(true);

            // Use the filename suffix as a default means to set this value
               file->set_outputLanguage(SgFile::e_Fortran_output_language);

               file->set_Fortran_only(true);

            // DQ (11/30/2010): This variable activates scopes built within the SageBuilder
            // interface to be built to use case insensitive symbol table handling.
               SageBuilder::symbol_table_case_insensitive_semantics = true;

               // determine whether to run this file through the C preprocessor
               bool requires_C_preprocessor =
                          // DXN (02/20/2011): rmod file should never require it
                     (filenameExtension != "rmod")
                      &&
                     (
                          // if the file extension implies it
                          CommandlineProcessing::isFortranFileNameSuffixRequiringCPP(filenameExtension)
                       ||
                          //if the command line includes "-D" options
                          ! getProject()->get_macroSpecifierList().empty()
#if 0
            // SKW: disabled because the "*_postprocessed" dregs cause "ompLoweringTests/fortran" to fail 'distcleancheck'
                       ||
                          //if the command line includes "-I" options
                          ! getProject()->get_includeDirectorySpecifierList().empty()
#endif
                      );

               file->set_requires_C_preprocessor(requires_C_preprocessor);

            // DQ (12/23/2008): This needs to be called after the set_requires_C_preprocessor() function is called.
            // If CPP processing is required then the global scope should have a source position using the intermediate
            // file name (generated by generate_C_preprocessor_intermediate_filename()).
               sourceFile->initializeGlobalScope();

            // Now set the specific types of Fortran file extensions
               if (CommandlineProcessing::isFortran77FileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling file->set_sourceFileUsesFortran77FileExtension(true) \n");
                    file->set_sourceFileUsesFortran77FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    file->set_outputFormat(SgFile::e_fixed_form_output_format);
                    file->set_backendCompileFormat(SgFile::e_fixed_form_output_format);

                    file->set_F77_only(true);
                  }

               if (CommandlineProcessing::isFortran90FileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling file->set_sourceFileUsesFortran90FileExtension(true) \n");
                    file->set_sourceFileUsesFortran90FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    file->set_outputFormat(SgFile::e_free_form_output_format);
                    file->set_backendCompileFormat(SgFile::e_free_form_output_format);

                    file->set_F90_only(true);
                  }

               if (CommandlineProcessing::isFortran95FileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling file->set_sourceFileUsesFortran95FileExtension(true) \n");
                    file->set_sourceFileUsesFortran95FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    file->set_outputFormat(SgFile::e_free_form_output_format);
                    file->set_backendCompileFormat(SgFile::e_free_form_output_format);

                    file->set_F95_only(true);
                  }

               if (CommandlineProcessing::isFortran2003FileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling file->set_sourceFileUsesFortran2003FileExtension(true) \n");
                    file->set_sourceFileUsesFortran2003FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    file->set_outputFormat(SgFile::e_free_form_output_format);
                    file->set_backendCompileFormat(SgFile::e_free_form_output_format);

                    file->set_F2003_only(true);
                  }

               if (CommandlineProcessing::isCoArrayFortranFileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling file->set_sourceFileUsesFortran2003FileExtension(true) \n");
                    file->set_sourceFileUsesCoArrayFortranFileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    file->set_outputFormat(SgFile::e_free_form_output_format);
                    file->set_backendCompileFormat(SgFile::e_free_form_output_format);

                 // DQ (1/23/2009): I think that since CAF is an extension of F2003, we want to mark this as F2003 as well.
                    file->set_F2003_only(true);
                    file->set_CoArrayFortran_only(true);
                  }

               if (CommandlineProcessing::isFortran2008FileNameSuffix(filenameExtension) == true)
                  {
                    printf ("Sorry, Fortran 2008 specific support is not yet implemented in ROSE ... \n");
                    ROSE_ASSERT(false);

                 // This is not yet supported.
                 // file->set_sourceFileUsesFortran2008FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    file->set_outputFormat(SgFile::e_free_form_output_format);
                    file->set_backendCompileFormat(SgFile::e_free_form_output_format);
                  }
             }
            else
             {
               if (CommandlineProcessing::isPHPFileNameSuffix(filenameExtension) == true)
                  {
                 // file = new SgSourceFile ( argv,  project );
                    SgSourceFile* sourceFile = new SgSourceFile ( argv,  project );
                    file = sourceFile;

                    file->set_sourceFileUsesPHPFileExtension(true);

                    file->set_outputLanguage(SgFile::e_PHP_output_language);

                    file->set_PHP_only(true);

                 // DQ (12/23/2008): We don't handle CPP directives and comments for PHP yet.
                 // file->get_skip_commentsAndDirectives(true);

                 // DQ (12/23/2008): This is the eariliest point where the global scope can be set.
                 // Note that file->get_requires_C_preprocessor() should be false.
                    ROSE_ASSERT(file->get_requires_C_preprocessor() == false);
                    sourceFile->initializeGlobalScope();
                  }
                 else
                  {
                 // printf ("Calling file->set_sourceFileUsesFortranFileExtension(false) \n");

                 // if (StringUtility::isCppFileNameSuffix(filenameExtension) == true)
                    if (CommandlineProcessing::isCppFileNameSuffix(filenameExtension) == true)
                       {
                      // file = new SgSourceFile ( argv,  project );
                         SgSourceFile* sourceFile = new SgSourceFile ( argv,  project );
                         file = sourceFile;

                      // This is a C++ file (so define __cplusplus, just like GNU gcc would)
                      // file->set_requires_cplusplus_macro(true);
                         file->set_sourceFileUsesCppFileExtension(true);

                      // Use the filename suffix as a default means to set this value
                         file->set_outputLanguage(SgFile::e_Cxx_output_language);

                         file->set_Cxx_only(true);

                      // DQ (12/23/2008): This is the eariliest point where the global scope can be set.
                      // Note that file->get_requires_C_preprocessor() should be false.
                         ROSE_ASSERT(file->get_requires_C_preprocessor() == false);
                         sourceFile->initializeGlobalScope();
                       }
                      else
                       {
                      // Liao, 6/6/2008, Assume AST with UPC will be unparsed using the C unparser
                         if ( ( CommandlineProcessing::isCFileNameSuffix(filenameExtension)   == true ) ||
                              ( CommandlineProcessing::isUPCFileNameSuffix(filenameExtension) == true ) )
                            {
                           // file = new SgSourceFile ( argv,  project );
                              SgSourceFile* sourceFile = new SgSourceFile ( argv,  project );
                              file = sourceFile;

                           // This a not a C++ file (assume it is a C file and don't define the __cplusplus macro, just like GNU gcc would)
                              file->set_sourceFileUsesCppFileExtension(false);

                           // Use the filename suffix as a default means to set this value
                              file->set_outputLanguage(SgFile::e_C_output_language);

                              file->set_C_only(true);

                           // Liao 6/6/2008  Set the newly introduced p_UPC_only flag.
                              if (CommandlineProcessing::isUPCFileNameSuffix(filenameExtension) == true)
                                 {
                                   file->set_UPC_only(true);
                                 }
                                else
                                 {
                                // DQ (7/4/2013): Added default behavior to be C99 to make this consistant with EDG default 
                                // behavior (changed to be C99 in March of 2013), (but we need to discuss this).
                                   file->set_C99_only(true);

                                // DQ (9/3/2013): Set the default to use gnu99 when defaulting to C mode (this will be reset if the -std=c99 option is used).
                                // However, it might be better still to check the backend compiler before we default to GNU.
                                   file->set_C99_gnu_only(true);
#if 0
                                   printf ("In determineFileType(): Setting the default mode for detected C cile to C99 (specifically generated code will use: -std=gnu99 option) \n");
#endif
                                 }

                           // DQ (12/23/2008): This is the eariliest point where the global scope can be set.
                           // Note that file->get_requires_C_preprocessor() should be false.
                              ROSE_ASSERT(file->get_requires_C_preprocessor() == false);
                              sourceFile->initializeGlobalScope();
                            }
                           else
                            {
                              if ( CommandlineProcessing::isCudaFileNameSuffix(filenameExtension) == true )
                                 {
                                   SgSourceFile* sourceFile = new SgSourceFile ( argv,  project );
                                   file = sourceFile;

                                   file->set_outputLanguage(SgFile::e_Cxx_output_language);

                                   file->set_Cuda_only(true);

                                // DQ (12/23/2008): This is the eariliest point where the global scope can be set.
                                // Note that file->get_requires_C_preprocessor() should be false.
                                   ROSE_ASSERT(file->get_requires_C_preprocessor() == false);
                                   sourceFile->initializeGlobalScope();
                                 }
                                else if ( CommandlineProcessing::isOpenCLFileNameSuffix(filenameExtension) == true )
                                 {
                                   SgSourceFile* sourceFile = new SgSourceFile ( argv,  project );
                                   file = sourceFile;
                                   file->set_OpenCL_only(true);

                                // DQ (12/23/2008): This is the eariliest point where the global scope can be set.
                                // Note that file->get_requires_C_preprocessor() should be false.
                                   ROSE_ASSERT(file->get_requires_C_preprocessor() == false);
                                   sourceFile->initializeGlobalScope();
                                 }
                                else if ( CommandlineProcessing::isJavaFileNameSuffix(filenameExtension) == true )
                                 {
                                // DQ (10/11/2010): Adding support for Java.
                                // file = new SgSourceFile ( argv,  project );
                                   SgSourceFile* sourceFile = new SgSourceFile ( argv,  project );
                                   file = sourceFile;

                                // This a not a C++ file (assume it is a C file and don't define the __cplusplus macro, just like GNU gcc would)
                                   file->set_sourceFileUsesCppFileExtension(false);

                                // Note that we can use the C++ unparser to provide output that will support inspection of 
                                // code from the AST, but this is a temporary solution.  The only correct setting is to use
                                // the ongoing support within the Java specific unparser.
                                // file->set_outputLanguage(SgFile::e_C_output_language);
                                   file->set_outputLanguage(SgFile::e_Java_output_language);

                                   file->set_Java_only(true);

                                // DQ (4/2/2011): Java code is only compiled, not linked as is C/C++ and Fortran.
                                   file->set_compileOnly(true);

                                // DQ (12/23/2008): This is the eariliest point where the global scope can be set.
                                // Note that file->get_requires_C_preprocessor() should be false.
                                   ROSE_ASSERT(file->get_requires_C_preprocessor() == false);
                                   sourceFile->initializeGlobalScope();

                                // file->display("Marked as java file based on file suffix");
                                 }
                                else if (CommandlineProcessing::isX10FileNameSuffix(filenameExtension) == true)
                                {
                                   SgSourceFile* sourceFile = new SgSourceFile (argv,  project);
                                   file = sourceFile;

                                  // This a not a C++ file so don't define the __cplusplus macro, just like GNU gcc would
                                   file->set_sourceFileUsesCppFileExtension(false);

                                // Note that we can use the C++ unparser to provide output that will support inspection of
                                // code from the AST, but this is a temporary solution.  The only correct setting is to use
                                // the ongoing support within the Java specific unparser.
                                // file->set_outputLanguage(SgFile::e_C_output_language);
                                   file->set_outputLanguage(SgFile::e_X10_output_language);

                                   file->set_X10_only(true);

                                // TOO1 (2/20/2013): X10 code is only compiled, not linked as is C/C++ and Fortran.
                                   file->set_compileOnly(true);

                                // DQ (12/23/2008): This is the eariliest point where the global scope can be set.
                                // Note that file->get_requires_C_preprocessor() should be false.
                                   ROSE_ASSERT(file->get_requires_C_preprocessor() == false);
                                   sourceFile->initializeGlobalScope();

                                // file->display("Marked as X10 file based on file suffix");
                                }
                               else if (CommandlineProcessing::isPythonFileNameSuffix(filenameExtension) == true)
                                {
                                  // file = new SgSourceFile ( argv,  project );
                                  SgSourceFile* sourceFile = new SgSourceFile ( argv,  project );
                                  file = sourceFile;

                                  file->set_sourceFileUsesPythonFileExtension(true);
                                  file->set_outputLanguage(SgFile::e_Python_output_language);
                                  file->set_Python_only(true);

                                  // DQ (12/23/2008): This is the eariliest point where the global scope can be set.
                                  // Note that file->get_requires_C_preprocessor() should be false.
                                  ROSE_ASSERT(file->get_requires_C_preprocessor() == false);
                                  sourceFile->initializeGlobalScope();
                                }
                                else
                                 {
                                // This is not a source file recognized by ROSE, so it is either a binary executable or library archive or something that we can't process.

                                // printf ("This still might be a binary file (can not be an object file, since these are not accepted into the fileList by CommandlineProcessing::generateSourceFilenames()) \n");

                                // Detect if this is a binary (executable) file!
                                   bool isBinaryExecutable = isBinaryExecutableFile(sourceFilename);
                                   bool isLibraryArchive   = isLibraryArchiveFile(sourceFilename);

                                // If -rose:binary was specified and the relatively simple-minded checks of isBinaryExecutableFile()
                                // and isLibararyArchiveFile() both failed to detect anything, then assume this is an executable.
                                   if (!isBinaryExecutable && !isLibraryArchive)
                                       isBinaryExecutable = true;

                                // printf ("isBinaryExecutable = %s isLibraryArchive = %s \n",isBinaryExecutable ? "true" : "false",isLibraryArchive ? "true" : "false");
                                   if (isBinaryExecutable == true || isLibraryArchive == true)
                                      {
                                     // Build a SgBinaryComposite to represent either the binary executable or the library archive.
                                        SgBinaryComposite* binary = new SgBinaryComposite ( argv,  project );
                                        file = binary;

                                     // This should have already been setup!
                                     // file->initializeSourcePosition();

                                        file->set_sourceFileUsesBinaryFileExtension(true);

                                     // If this is an object file being processed for binary analysis then mark it as an object
                                     // file so that we can trigger analysis to mar the sections that will be disassembled.
                                        string binaryFileName = file->get_sourceFileNameWithPath();
                                        if (CommandlineProcessing::isObjectFilename(binaryFileName) == true)
                                           {
                                             file->set_isObjectFile(true);
                                           }

                                      // DQ (2/5/2009): Put this at both the SgProject and SgFile levels.
                                      // DQ (2/4/2009):  This is now a data member on the SgProject instead of on the SgFile.
                                      file->set_binary_only(true);

                                      // DQ (5/18/2008): Set this to false (since binaries are never preprocessed using the C preprocessor).
                                      file->set_requires_C_preprocessor(false);

                                      ROSE_ASSERT(file->get_file_info() != NULL);

                                      if (isLibraryArchive == true)
                                      {
#ifdef _MSC_VER
                                          /* The following block of code deals with *.a library archives files found on
                                           * Unix systems. I added better temporary file and directory names, but this
                                           * block of code also has commands that likely won't run on Windows
                                           * systems, so I'm commenting out the whole block. [RPM 2010-11-03] */
                                          ROSE_ASSERT(!"Windows not supported");
#else

                                          // This is the case of processing a library archive (*.a) file. We want to process these files so that
                                          // we can test the library identification mechanism to build databases of the binary functions in
                                          // libraries (so that we detect these in staticaly linked binaries).
                                          ROSE_ASSERT(isBinaryExecutable == false);

                                          // Note that since a archive can contain many *.o files each of these will be a SgAsmGenericFile object and
                                          // the SgBinaryComposite will contain a list of SgAsmGenericFile objects to hold them all.
                                          string archiveName = file->get_sourceFileNameWithPath();

                                          printf ("archiveName = %s \n",archiveName.c_str());

                                          // Mark this as a library archive.
                                          file->set_isLibraryArchive(true);

                                          // Extract the archive into a temporary directory and create a file in that
                                          // directory that will have the names of objects stored in the archive.  We have
                                          // to be careful about name choices since this function could be called multiple
                                          // times from one process, and by multiple processes.
                                          static int tmpDirectorySequence = 0;
                                          string tmpDirectory = "/tmp/ROSE-" + StringUtility::numberToString(getpid()) +
                                              "-" + StringUtility::numberToString(tmpDirectorySequence++);
                                          string objectNameFile = tmpDirectory + "/object_names.txt";
                                          string commandLine = "mkdir -p " + tmpDirectory +
                                              "&& cd " + tmpDirectory +
                                              "&& ar -vox " + archiveName +
                                              ">" + objectNameFile;
                                          printf ("Running System Command: %s \n",commandLine.c_str());

                                          // Run the system command...
                                          system(commandLine.c_str());

                                          vector<string> wordList = StringUtility::readWordsInFile(objectNameFile);
                                          vector<string> objectFileList;

                                          for (vector<string>::iterator i = wordList.begin(); i != wordList.end(); i++)
                                          {
                                              // Get each word in the file of names (*.o)
                                              string word = *i;
                                              // printf ("word = %s \n",word.c_str());
                                              size_t wordSize = word.length();
                                              string targetSuffix = ".o";
                                              size_t targetSuffixSize = targetSuffix.length();
                                              if (wordSize > targetSuffixSize &&
                                                      word.substr(wordSize-targetSuffixSize) == targetSuffix)
                                                  objectFileList.push_back(tmpDirectory + "/" + word);
                                          }

                                          for (vector<string>::iterator i = objectFileList.begin(); i != objectFileList.end(); i++)
                                          {
                                              // Get each object file name (*.o)
                                              string objectFileName = *i;
                                              printf ("objectFileName = %s \n",objectFileName.c_str());
                                              binary->get_libraryArchiveObjectFileNameList().push_back(objectFileName);
                                              printf ("binary->get_libraryArchiveObjectFileNameList().size() = %zu \n",binary->get_libraryArchiveObjectFileNameList().size());
                                          }
#if 0
                                          printf ("Exiting in processing a library archive file. \n");
                                          // ROSE_ASSERT(false);
#endif
#endif /* _MSC_VER */
                                      }
#if 0
                                      printf ("Processed as a binary file! \n");
#endif
                                  }
                                  else
                                  {
                                      file = new SgUnknownFile ( argv,  project );

                                      // This should have already been setup!
                                      // file->initializeSourcePosition();

                                      ROSE_ASSERT(file->get_parent() != NULL);
                                      ROSE_ASSERT(file->get_parent() == project);

                                      // If all else fails, then output the type of file and exit.
                                      file->set_sourceFileTypeIsUnknown(true);
                                      file->set_requires_C_preprocessor(false);

                                      ROSE_ASSERT(file->get_file_info() != NULL);
                                      // file->set_parent(project);

                                      // DQ (2/3/2009): Uncommented this to report the file type when we don't process it...
                                      // outputTypeOfFileAndExit(sourceFilename);
                                      printf ("Warning: This is an unknown file type, not being processed by ROSE \n");
                                      outputTypeOfFileAndExit(sourceFilename);
                                  }
                              }
                            }
                       }
                  }

                 file->set_sourceFileUsesFortranFileExtension(false);
             }
        }
       else
        {
       // DQ (2/6/2009): This case is used by the build function SageBuilder::buildFile().

#if 1
       // DQ (12/22/2008): Make any error message from this branch more clear for debugging!
       // AS Is this option possible?
          printf ("Is this branch reachable? \n");
          ROSE_ASSERT(false);
       // abort();

       // ROSE_ASSERT (p_numberOfSourceFileNames == 0);
          ROSE_ASSERT (file->get_sourceFileNameWithPath().empty() == true);

       // If no source code file name was found then likely this is:
       //   1) a link command, or
       //   2) called as part of the SageBuilder::buildFile()
       // using the C++ compiler.  In this case skip the EDG processing.
          file->set_disable_edg_backend(true);
#endif
       // printf ("No source file found on command line, assuming to be linker command line \n");
        }

  // DQ (2/6/2009): Can use this assertion with the build function SageBuilder::buildFile().
  // DQ (2/3/2009): I think this is a new assertion!
  // ROSE_ASSERT(file != NULL);
  // file->display("SgFile* determineFileType(): before calling file->callFrontEnd()");

#if 0
  // DQ (6/12/2013): This is the functionality that we need to separate out and run after all 
  // of the SgSourceFile IR nodes are constructed.

  // The frontend is called explicitly outside the constructor since that allows for a cleaner
  // control flow. The callFrontEnd() relies on all the "set_" flags to be already called therefore
  // it was placed here.
  // if ( isSgUnknownFile(file) == NULL && file != NULL  )
     if ( file != NULL && isSgUnknownFile(file) == NULL )
        {
       // printf ("Calling file->callFrontEnd() \n");

       // TOO1 (05/14/2013): Signal handling for -rose:keep_going
          if (file->get_project()->get_keep_going())
             {
               struct sigaction act;
               act.sa_handler = HandleFrontendSignal;
               sigemptyset(&act.sa_mask);
               act.sa_flags = 0;
               sigaction(SIGSEGV, &act, 0);
               sigaction(SIGABRT, &act, 0);
             }

          if (sigsetjmp(rose__sgproject_parse_mark, 0) == -1)
             {
               std::cout
                    << "[WARN] Ignoring frontend failure "
                    << " as directed by -rose:keep_going"
                    << std::endl;
               file->set_frontendErrorCode(-1);
             }
            else
             {
               try
                  {
                    nextErrorCode = file->callFrontEnd();
                  }
               catch (...)
                  {
                 // TOO1 (05/14/2013): Handling for -rose:keep_going
                    std::cout << "[WARN] Caught frontend exception" << std::endl;
                    if (file->get_project()->get_keep_going())
                       {
                         file->set_frontendErrorCode(-1);
                       }
                      else
                       {
                         throw;
                       }
                  }
             }

       // printf ("DONE: Calling file->callFrontEnd() \n");
          ROSE_ASSERT ( nextErrorCode <= 3);
        }
#else
#if 0
  // DQ (6/12/2013): I think this is required to support having all of the SgSourceFile 
  // IR nodes pre-built as part of the new Java support required to be better performance 
  // in the Java frontend AST translation.
     printf ("***************************************************************************************************************************************************************************** \n");
     printf ("***************************************************************************************************************************************************************************** \n");
     printf ("Disabling the call to the frontend so that we can setup the SgSourceFile IR nodes once at the start and then call the frontend on each of them as we process all of the files \n");
     printf ("***************************************************************************************************************************************************************************** \n");
     printf ("***************************************************************************************************************************************************************************** \n");
#endif
#endif

  // Keep the filename stored in the Sg_File_Info consistant.  Later we will want to remove this redundency
  // The reason we have the Sg_File_Info object is so that we can easily support filename matching based on
  // the integer values instead of string comparisions.  Required for the handling co CPP directives and comments.

#if 0
     if (file != NULL)
        {
          printf ("Calling file->display() \n");
          file->display("SgFile* determineFileType()");
        }
#endif

  // DQ (9/24/2013): Added assertion.
     ROSE_ASSERT(file != NULL);

#if 0
     printf ("Leaving determineFileType() = %d \n",file->get_outputLanguage());
     printf ("Leaving determineFileType() = %s \n",SgFile::get_outputLanguageOptionName(file->get_outputLanguage()).c_str());
#endif

  // DQ (6/13/2013): Added to support error checking (seperation of construction of SgFile IR nodes from calling the fronend on each one).
     ROSE_ASSERT(file->get_parent() != NULL);
     ROSE_ASSERT(isSgProject(file->get_parent()) != NULL);
     ROSE_ASSERT(file->get_parent() == project);

     return file;
   }


void 
SgFile::runFrontend(int & nextErrorCode)
{
  // DQ (6/13/2013):  This function supports the seperation of the construction of the SgFile IR nodes from the 
  // invocation of the frontend on each SgFile IR node.

#if 0
     printf ("************************ \n");
     printf ("In SgFile::runFrontend() \n");
     printf ("************************ \n");
#endif

#if 0
  // Output state of the SgFile.
     display("In runFrontend()");
#endif

  // DQ (6/13/2013): Added to support error checking (seperation of construction of SgFile IR nodes from calling the fronend on each one).
     ROSE_ASSERT(get_parent() != NULL);

  // DQ (6/13/2013): This is wrong, the parent of the SgFile is the SgFileList IR node.
  // ROSE_ASSERT(isSgProject(get_parent()) != NULL);

  // DQ (6/12/2013): This is the functionality that we need to separate out and run after all 
  // of the SgSourceFile IR nodes are constructed.

  // The frontend is called explicitly outside the constructor since that allows for a cleaner
  // control flow. The callFrontEnd() relies on all the "set_" flags to be already called therefore
  // it was placed here.
  // if ( isSgUnknownFile(file) == NULL && file != NULL  )
    if ( this != NULL && isSgUnknownFile(this) == NULL )
    {
        nextErrorCode = this->callFrontEnd();
        this->set_frontendErrorCode(nextErrorCode);
    }

    ROSE_ASSERT(nextErrorCode <= 3);
}


// DQ (10/20/2010): Note that Java support can be enabled just because Java internal support was found on the
// current platform.  But we only want to inialize the JVM server if we require Fortran or Java language support.
// So use the explicit macros defined in rose_config header file for this level of control.
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
namespace Rose {
namespace Frontend {
namespace Fortran {
namespace Ofp {
  extern void jserver_init();
}// Rose::Frontend::Fortran::Ofp
}// Rose::Frontend::Fortran
}// Rose::Frontend
}// Rose
#endif

#ifdef ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
namespace Rose {
namespace Frontend {
namespace Java {
namespace Ecj {
  extern void jserver_init();
}// Rose::Frontend::Java::Ecj
}// Rose::Frontend::Java
}// Rose::Frontend
}// Rose
#endif

//! internal function to invoke the EDG frontend and generate the AST
int
SgProject::parse(const vector<string>& argv)
   {
  // Not sure that if we are just linking that we should call a function called "parse()"!!!

  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST (SgProject::parse(argc,argv)):");

    // TOO1 (2014/01/22): TODO: Consider moving CLI processing out of SgProject
    // constructor. We can't set any error codes on SgProject since SgProject::parse
    // is being called from the SgProject::SgProject constructor, meaning the SgProject
    // object is not properly constructed yet.. The only thing we can do, then, if
    // there is an error here in the commandline handling, is to halt the program.
    if (KEEP_GOING_CAUGHT_COMMANDLINE_SIGNAL)
    {
        std::cout
            << "[FATAL] "
            << "Unrecoverable signal generated during commandline processing"
            << std::endl;
        exit(1);
    }
    else
    {
        // builds file list (or none if this is a link line)
        processCommandLine(argv);
    }

     int errorCode = 0;

  // DQ (7/7/2005): Added support for AST Merge Mechanism
     if (p_astMerge == true)
        {
       // If astMerge is specified, then the command file is accessed to execute all
       // the commands from each of the associated working directories.  Each new AST
       // in merged with the previous AST.

          if (p_astMergeCommandFile != "")
             {
            // If using astMerge mechanism we have to save the command line and
            // working directories to a separate file.  This permits a makefile to
            // call a ROSE translator repeatedly and the command line for each
            // file be saved.
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
               errorCode = AstMergeSupport(this);
#endif
             }
            else
             {
            // DQ (5/26/2007): This case could make sense, if there were more than
            // one file on the command line (or if we wanted to force a single file
            // to share as much as possible in a merge with itself, there is a
            // typical 20% reduction in memory useage for this case since the
            // types are then better shared than is possible during initial construction
            // of the AST).
#if 0
            // error case
               printf ("astMerge requires specification of a command file \n");
               ROSE_ASSERT(false);
               errorCode = -1;
#endif
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
               errorCode = AstMergeSupport(this);
#endif
             }
        }
       else
        {
       // DQ (7/7/2005): Specification of the AST merge command filename triggers accumulation
       // of working directories and commandlines into the specified file (no other processing
       // is done, the AST (beyond the SgProject) is not built).
          if (p_astMergeCommandFile != "")
             {
            // If using astMerge mechanism we have to save the command line and
            // working directories to a separate file.

            // DQ (5/26/2007): This might be a problem where object files are required to be built
            // and so we might have to call the backend compiler as a way of forcing the correct
            // object files to be built so that, for example, libraries can be constructed when
            // operating across multiple directories.

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
               errorCode = buildAstMergeCommandFile(this);
#endif
             }
            else
             {
            // Normal case without AST Merge: Compiling ...
            // printf ("In SgProject::parse(const vector<string>& argv): get_sourceFileNameList().size() = %zu \n",get_sourceFileNameList().size());
               if (get_sourceFileNameList().size() > 0)
                  {
                 // This is a compile line
                 // printf ("Calling parse() from SgProject::parse(const vector<string>& argv) \n");


                  /*
                   * FMZ (5/19/2008)
                   *   "jserver_init()"   does nothing. The Java VM will be loaded at the first time
                   *                      it needed (i.e for parsing the 1st fortran file).
                   *   "jserver_finish()" will dostroy the Java VM if it is running.
                   */

                    if (SgProject::get_verbose() > 1)
                       {
                         printf ("Calling Open Fortran Parser: jserver_init() \n");
                       }

// DQ (10/20/2010): Note that Java support can be enabled just because Java internal support was found on the
// current platform.  But we only want to inialize the JVM server if we require Fortran or Java language support.
// So use the explicit macros defined in rose_config header file for this level of control.
#if (defined(ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT) || defined(ROSE_BUILD_JAVA_LANGUAGE_SUPPORT))
// #ifdef USE_ROSE_INTERNAL_JAVA_SUPPORT
// #ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
// #ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
                    Rose::Frontend::Fortran::Ofp::jserver_init();
#endif
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
                    Rose::Frontend::Java::Ecj::jserver_init();
#endif
// #endif // USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
#endif
                    errorCode = parse();

                 // FMZ deleteComm jserver_finish();
                  }

            // DQ (5/26/2007): This is meaningless, so remove it!
            // errorCode = errorCode;
             }
        }

#if 1
  // DQ (8/22/2009): We test the parent of SgFunctionTypeTable in the AST post processing,
  // so we need to make sure that it is set.
     SgFunctionTypeTable* functionTypeTable = SgNode::get_globalFunctionTypeTable();
     ROSE_ASSERT(functionTypeTable != NULL);
     if (functionTypeTable->get_parent() == NULL)
        {
#if 0
          printf ("This (globalFunctionTypeTable) should have been set to point to the SgProject not the SgFile \n");
          ROSE_ASSERT(false);
#endif
       // ROSE_ASSERT(numberOfFiles() > 0);
       // printf ("Inside of SgProject::parse(const vector<string>& argv): set the parent of SgFunctionTypeTable \n");
          if (numberOfFiles() > 0)
               functionTypeTable->set_parent(&(get_file(0)));
            else
               functionTypeTable->set_parent(this);
        }
     ROSE_ASSERT(functionTypeTable->get_parent() != NULL);

     ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
     ROSE_ASSERT(SgNode::get_globalFunctionTypeTable()->get_parent() != NULL);
#endif

#if 1
  // DQ (7/25/2010): We test the parent of SgTypeTable in the AST post processing,
  // so we need to make sure that it is set.
     SgTypeTable* typeTable = SgNode::get_globalTypeTable();
     ROSE_ASSERT(typeTable != NULL);
     if (typeTable->get_parent() == NULL)
        {
#if 0
          printf ("This (globalTypeTable) should have been set to point to the SgProject not the SgFile \n");
          ROSE_ASSERT(false);
#endif
       // ROSE_ASSERT(numberOfFiles() > 0);
       // printf ("Inside of SgProject::parse(const vector<string>& argv): set the parent of SgTypeTable \n");
          if (numberOfFiles() > 0)
               typeTable->set_parent(&(get_file(0)));
            else
               typeTable->set_parent(this);
        }
     ROSE_ASSERT(typeTable->get_parent() != NULL);

  // DQ (7/30/2010): This test fails in tests/CompilerOptionsTests/testCpreprocessorOption
  // DQ (7/25/2010): Added new test.
  // printf ("typeTable->get_parent()->class_name() = %s \n",typeTable->get_parent()->class_name().c_str());
  // ROSE_ASSERT(isSgProject(typeTable->get_parent()) != NULL);

     ROSE_ASSERT(SgNode::get_globalTypeTable() != NULL);
     ROSE_ASSERT(SgNode::get_globalTypeTable()->get_parent() != NULL);
#endif

     return errorCode;
   }


SgSourceFile::SgSourceFile ( vector<string> & argv , SgProject* project )
// : SgFile (argv,errorCode,fileNameIndex,project)
   {
  // printf ("In the SgSourceFile constructor \n");

     set_globalScope(NULL);

  // DQ (6/15/2011): Added scope to hold unhandled declarations (see test2011_80.C).
     set_temp_holding_scope(NULL);

  // This constructor actually makes the call to EDG/OFP/ECJ to build the AST (via callFrontEnd()).
  // printf ("In SgSourceFile::SgSourceFile(): Calling doSetupForConstructor() \n");
     doSetupForConstructor(argv,  project);
    }

#if 0
SgSourceFile::SgSourceFile ( vector<string> & argv , int & errorCode, int fileNameIndex, SgProject* project )
// : SgFile (argv,errorCode,fileNameIndex,project)
   {
  // printf ("In the SgSourceFile constructor \n");

     set_globalScope(NULL);

  // This constructor actually makes the call to EDG to build the AST (via callFrontEnd()).
     doSetupForConstructor(argv, errorCode, fileNameIndex, project);

    }
#endif

int
SgSourceFile::callFrontEnd()
   {
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  // FortranParserState* currStks = new FortranParserState();
#endif

     int frontendErrorLevel = SgFile::callFrontEnd();
  // DQ (1/21/2008): This must be set for all languages
     ROSE_ASSERT(get_globalScope() != NULL);
     ROSE_ASSERT(get_globalScope()->get_file_info() != NULL);
     ROSE_ASSERT(get_globalScope()->get_file_info()->get_filenameString().empty() == false);
  // printf ("p_root->get_file_info()->get_filenameString() = %s \n",p_root->get_file_info()->get_filenameString().c_str());

  // DQ (8/21/2008): Added assertion.
     ROSE_ASSERT (get_globalScope()->get_startOfConstruct() != NULL);
     ROSE_ASSERT (get_globalScope()->get_endOfConstruct()   != NULL);

#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  // delete  currStks ;
#endif

     return frontendErrorLevel;
   }

int
SgBinaryComposite::callFrontEnd()
   {
     int frontendErrorLevel = SgFile::callFrontEnd();
  // DQ (1/21/2008): This must be set for all languages
     return frontendErrorLevel;
   }

int
SgUnknownFile::callFrontEnd()
   {
  // DQ (2/3/2009): This function is defined, but should never be called.
     printf ("Error: calling SgUnknownFile::callFrontEnd() \n");
     ROSE_ASSERT(false);

     return 0;
   }

SgBinaryComposite::SgBinaryComposite ( vector<string> & argv ,  SgProject* project )
    : p_genericFileList(NULL), p_interpretations(NULL)
{
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
    p_interpretations = new SgAsmInterpretationList();
    p_interpretations->set_parent(this);

    p_genericFileList = new SgAsmGenericFileList();
    p_genericFileList->set_parent(this);

  // DQ (2/3/2009): This data member has disappeared (in favor of a list).
  // p_binaryFile = NULL;

  // printf ("In the SgBinaryComposite constructor \n");

  // This constructor actually makes the call to EDG to build the AST (via callFrontEnd()).
  // printf ("In SgBinaryComposite::SgBinaryComposite(): Calling doSetupForConstructor() \n");
     doSetupForConstructor(argv,  project);

  // printf ("Leaving SgBinaryComposite constructor \n");
#else
     printf ("Binary analysis not supported in this distribution (turned off in this restricted distribution) \n");
     ROSE_ASSERT(false);
#endif
}

int
SgProject::RunFrontend()
{
  TimingPerformance timer ("AST (SgProject::RunFrontend()):");

  int status_of_function = 0;

  //---------------------------------------------------------------------------
  // Pass each File to the Frontend
  //---------------------------------------------------------------------------
  std::vector<SgFile*> all_files = get_fileList();
  {
      int status_of_file = 0;
      BOOST_FOREACH(SgFile* file, all_files)
      {
          ROSE_ASSERT(file != NULL);
          if (KEEP_GOING_CAUGHT_FRONTEND_SIGNAL)
          {
              std::cout
                  << "[WARN] "
                  << "Configured to keep going after catching a "
                  << "signal in SgFile::RunFrontend()"
                  << std::endl;

              if (file != NULL)
              {
                  file->set_frontendErrorCode(100);
                  status_of_function =
                      std::max(100, status_of_function);
              }
              else
              {
                  std::cout
                      << "[FATAL] "
                      << "Unable to keep going due to an unrecoverable internal error"
                      << std::endl;
                  exit(1);
              }
          }
          else
          {
              try
              {
                  //-----------------------------------------------------------
                  // Pass File to Frontend
                  //-----------------------------------------------------------
                  file->runFrontend(status_of_file);
                  {
                      status_of_function =
                          max(status_of_file, status_of_function);
                  }
              }
              catch(...)
              {
                  if (file != NULL)
                  {
                     file->set_frontendErrorCode(100);
                  }
                  else
                  {
                      std::cout
                          << "[FATAL] "
                          << "Unable to keep going due to an unrecoverable internal error"
                          << std::endl;
                      exit(1);
                  }

                  if (ROSE::KeepGoing::g_keep_going)
                  {
                      raise(SIGABRT);// catch with signal handling above
                  }
                  else
                  {
                      throw;
                  }
              }
          }
      }//BOOST_FOREACH
  }//all_files->callFrontEnd

  this->set_frontendErrorCode(status_of_function);
  return status_of_function;
}//SgProject::RunFrontend

int
SgProject::parse()
   {
     int errorCode = 0;

  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST (SgProject::parse()):");

  // ROSE_ASSERT (p_fileList != NULL);

#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  // FMZ (5/29/2008)
     FortranModuleInfo::setCurrentProject(this);
     FortranModuleInfo::set_inputDirs(this );
#endif

  // Simplify multi-file handling so that a single file is just the trivial
  // case and not a special separate case.
#if 0
     printf ("Loop through the source files on the command line! p_sourceFileNameList = %zu \n",p_sourceFileNameList.size());
#endif

     Rose_STL_Container<string>::iterator nameIterator = p_sourceFileNameList.begin();
     unsigned int i = 0;

#if 0
  // DQ (6/13/2013): This is older code from when we build each SgFile and processed in immediately.
  // We don't want such a design, thus we now build all of the SgFile IR nodes first, and then iterate
  // over them to call the frontend on each of them (this could likely be done in parallel as well).

     while (nameIterator != p_sourceFileNameList.end())
        {
          int nextErrorCode = 0;

       // DQ (4/20/2006): Exclude other files from list in argc and argv
          vector<string> argv = get_originalCommandLineArgumentList();
          string currentFileName = *nameIterator;

          CommandlineProcessing::removeAllFileNamesExcept(argv,p_sourceFileNameList,currentFileName);

       // DQ (11/13/2008): Removed overly complex logic here!

          SgFile* newFile = determineFileType(argv, nextErrorCode, this);
          ROSE_ASSERT (newFile != NULL);

          ROSE_ASSERT (newFile->get_startOfConstruct() != NULL);
          ROSE_ASSERT (newFile->get_parent() != NULL);

       // This just adds the new file to the list of files stored internally
          set_file ( *newFile );

          errorCode = max(errorCode,nextErrorCode); // use STL max

          nameIterator++;
          i++;
        }
#else
  // The goal in this version of the code is to seperate the construction of the SgFile objects 
  // from the invocation of the fronend on each of the SgFile objects.  In general this allows
  // the comilation to reference the other SgFile objects on an as needed basis as part of running
  // the frontend.  This is importnat from the optimization of Java.
     std::vector<SgFile*> vectorOfFiles;
     while (nameIterator != p_sourceFileNameList.end())
        {
#if 0
          printf ("Build a SgFile object for file #%d \n",i);
#endif
          int nextErrorCode = 0;

       // DQ (4/20/2006): Exclude other files from list in argc and argv
          vector<string> argv = get_originalCommandLineArgumentList();
          string currentFileName = *nameIterator;
#if 0
          printf ("In SgProject::parse(): before removeAllFileNamesExcept() file = %s argv = %s \n",
               currentFileName.c_str(),CommandlineProcessing::generateStringFromArgList(argv,false,false).c_str());
#endif
          CommandlineProcessing::removeAllFileNamesExcept(argv,p_sourceFileNameList,currentFileName);
#if 0
          printf ("In SgProject::parse(): after removeAllFileNamesExcept() from command line for file = %s argv = %s \n",
               currentFileName.c_str(),CommandlineProcessing::generateStringFromArgList(argv,false,false).c_str());
          printf ("currentFileName = %s \n",currentFileName.c_str());
#endif
       // DQ (11/13/2008): Removed overly complex logic here!
#if 0
          printf ("+++++++++++++++ Calling determineFileType() currentFileName = %s \n",currentFileName.c_str());
#endif
          SgFile* newFile = determineFileType(argv, nextErrorCode, this);
          ROSE_ASSERT (newFile != NULL);
#if 0
          printf ("+++++++++++++++ DONE: Calling determineFileType() currentFileName = %s \n",currentFileName.c_str());
          printf ("In SgProject::parse(): newFile = %p = %s \n",newFile,newFile->class_name().c_str());
#endif
          ROSE_ASSERT (newFile->get_startOfConstruct() != NULL);
          ROSE_ASSERT (newFile->get_parent() != NULL);

       // DQ (6/13/2013): Added to support error checking (seperation of construction of SgFile IR nodes from calling the fronend on each one).
          ROSE_ASSERT(isSgProject(newFile->get_parent()) != NULL);
          ROSE_ASSERT(newFile->get_parent() == this);

       // This just adds the new file to the list of files stored internally (note: this sets the parent of the newFile).
          set_file ( *newFile );

       // DQ (6/13/2013): Added to support error checking (seperation of construction of SgFile IR nodes from calling the fronend on each one).
          ROSE_ASSERT(newFile->get_parent() != NULL);

       // This list of files will be iterated over to call the frontend in the next loop.
          vectorOfFiles.push_back(newFile);

       // newFile->display("Called from SgProject::parse()");

          nameIterator++;
          i++;
        }

#if 0
     printf ("In Project::parse(): (calling the frontend on all previously setup SgFile objects) vectorOfFiles.size() = %zu \n",vectorOfFiles.size());
#endif

  errorCode = this->RunFrontend();
  if (errorCode > 3)
  {
      return errorCode;
  }

#endif

  // DQ (6/13/2013): Test the new function to lookup the SgFile from the name with full path.
  // This is a simple consistancy test for that new function.
     for (size_t i = 0; i < vectorOfFiles.size(); i++)
        {
          string filename = vectorOfFiles[i]->get_sourceFileNameWithPath();
          SgFile* file = this->operator[](filename);
          ROSE_ASSERT(file != NULL);

          if ( SgProject::get_verbose() > 0 )
             {
               printf ("Testing: map of filenames to SgFile IR nodes: filename = %s is mapped to SgFile = %p \n",filename.c_str(),vectorOfFiles[i]);
             }

          ROSE_ASSERT(file == vectorOfFiles[i]);
        }

  // printf ("Inside of SgProject::parse() before AstPostProcessing() \n");

  // GB (8/19/2009): Moved the AstPostProcessing call from
  // SgFile::callFrontEnd to this point. Thus, it is only called once for
  // the whole project rather than once per file. Repeated calls to
  // AstPostProcessing are slow due to repeated memory pool traversals. The
  // AstPostProcessing is only to be called if there are input files to run
  // it on, and they are meant to be used in some way other than just
  // calling the backend on them. (If only the backend is used, this was
  // never called by SgFile::callFrontEnd either.)
  // if ( !get_fileList().empty() && !get_useBackendOnly() )
#ifndef ROSE_USE_CLANG_FRONTEND
     if ( (get_fileList().empty() == false) && (get_useBackendOnly() == false) )
        {
          AstPostProcessing(this);
        }
#endif
#if 0
       else
        {
       // Alternatively if this is a part of binary analysis then process via AstPostProcessing().
          if (this->get_binary_only() == true)
             {
               AstPostProcessing(this);
             }
        }
#endif

  // negara1 (06/23/2011): Collect information about the included files to support unparsing of those that are modified.
  // In the first step, get the include search paths, which will be used while attaching include preprocessing infos.
  // Proceed only if there are input files and they require header files unparsing.
     if (!get_fileList().empty() && (*get_fileList().begin())->get_unparseHeaderFiles())
        {
          if (SgProject::get_verbose() >= 1)
             {
               cout << endl << "***HEADER FILES ANALYSIS***" << endl << endl;
             }
          CompilerOutputParser compilerOutputParser(this);
          const pair<list<string>, list<string> >& includedFilesSearchPaths = compilerOutputParser.collectIncludedFilesSearchPaths();  
          set_quotedIncludesSearchPaths(includedFilesSearchPaths.first);
          set_bracketedIncludesSearchPaths(includedFilesSearchPaths.second);

          if (SgProject::get_verbose() >= 1)
             {
               CollectionHelper::printList(get_quotedIncludesSearchPaths(), "\nQuoted includes search paths:", "Path:");
               CollectionHelper::printList(get_bracketedIncludesSearchPaths(), "\nBracketed includes search paths:", "Path:");
             }
        }

  // GB (9/4/2009): Moved the secondary pass over source files (which
  // attaches the preprocessing information) to this point. This way, the
  // secondary pass over each file runs after all fixes have been done. This
  // is relevant where the AstPostProcessing mechanism must first mark nodes
  // to be output before preprocessing information is attached.
  SgFilePtrList &files = get_fileList();
  {
      BOOST_FOREACH(SgFile* file, files)
      {
          ROSE_ASSERT(file != NULL);

          if (KEEP_GOING_CAUGHT_FRONTEND_SECONDARY_PASS_SIGNAL)
          {
              std::cout
                  << "[WARN] "
                  << "Configured to keep going after catching a signal in "
                  << "SgFile::secondaryPassOverSourceFile()"
                  << std::endl;

              if (file != NULL)
              {
                  file->set_frontendErrorCode(100);
                  errorCode = std::max(100, errorCode);
              }
              else
              {
                  std::cout
                      << "[FATAL] "
                      << "Unable to keep going due to an unrecoverable internal error"
                      << std::endl;
                  exit(1);
              }
          }
          else
          {
              file->secondaryPassOverSourceFile();
          }
      }

      if (errorCode != 0)
      {
          return errorCode;
      }
  }

  // negara1 (06/23/2011): Collect information about the included files to support unparsing of those that are modified.
  // In the second step (after preprocessing infos are already attached), collect the including files map.
  // Proceed only if there are input files and they require header files unparsing.
     if (!get_fileList().empty() && (*get_fileList().begin())->get_unparseHeaderFiles())
        {
          CompilerOutputParser compilerOutputParser(this);
          const map<string, set<string> >& includedFilesMap = compilerOutputParser.collectIncludedFilesMap();

          IncludingPreprocessingInfosCollector includingPreprocessingInfosCollector(this, includedFilesMap);
          const map<string, set<PreprocessingInfo*> >& includingPreprocessingInfosMap = includingPreprocessingInfosCollector.collect();

          set_includingPreprocessingInfosMap(includingPreprocessingInfosMap);
         
          if (SgProject::get_verbose() >= 1)
             {
               CollectionHelper::printMapOfSets(includedFilesMap, "\nIncluded files map:", "File:", "Included file:");
               CollectionHelper::printMapOfSets(get_includingPreprocessingInfosMap(), "\nIncluding files map:", "File:", "Including file:");
             }
        }
     
     if ( get_verbose() > 0 )
        {
       // Report the error code if it is non-zero (but only in verbose mode)
          if (errorCode > 0)
             {
               printf ("Frontend Warnings only: errorCode = %d \n",errorCode);
               if (errorCode > 3)
                  {
                    printf ("Frontend Errors found: errorCode = %d \n",errorCode);
                  }
             }
        }

  // warnings from EDG processing are OK but not errors
     ROSE_ASSERT (errorCode <= 3);

  // if (get_useBackendOnly() == false)
     if ( SgProject::get_verbose() >= 1 )
          cout << "C++ source(s) parsed. AST generated." << endl;

     if ( get_verbose() > 3 )
        {
          printf ("In SgProject::parse() (verbose mode ON): \n");
          display ("In SgProject::parse()");
        }

     return errorCode;
   }

//negara1 (07/29/2011)
//The returned file path is not normalized. 
//TODO: Return the normalized path after the bug in ROSE is fixed. The bug manifests itself when the same header file is included in 
//multiple places using different paths. In such a case, ROSE treats the same file as different files and generates different IDs for them.
string SgProject::findIncludedFile(PreprocessingInfo* preprocessingInfo) {
    IncludeDirective includeDirective(preprocessingInfo -> getString());
    const string& includedPath = includeDirective.getIncludedPath();
    if (FileHelper::isAbsolutePath(includedPath)) {
        //the path is absolute, so no need to search for the file
        if (FileHelper::fileExists(includedPath)) {
            return includedPath;
        }
        return ""; //file does not exist, so return an empty string
    }
    if (includeDirective.isQuotedInclude()) {
        //start looking from the current folder, then proceed with the quoted includes search paths
        //TODO: Consider the presence of -I- option, which disables looking in the current folder for quoted includes.
        string currentFolder = FileHelper::getParentFolder(preprocessingInfo -> get_file_info() -> get_filenameString());
        p_quotedIncludesSearchPaths.insert(p_quotedIncludesSearchPaths.begin(), currentFolder);
        string includedFilePath = FileHelper::getIncludedFilePath(p_quotedIncludesSearchPaths, includedPath);
        p_quotedIncludesSearchPaths.erase(p_quotedIncludesSearchPaths.begin()); //remove the previously inserted current folder (for other files it might be different)
        if (!includedFilePath.empty()) {
            return includedFilePath;
        }
    }
    //For bracketed includes and for not yet found quoted includes proceed with the bracketed includes search paths
    return FileHelper::getIncludedFilePath(p_bracketedIncludesSearchPaths, includedPath);
}

void
SgSourceFile::doSetupForConstructor(const vector<string>& argv, SgProject* project)
   {
  // Call the base class implementation!
     SgFile::doSetupForConstructor(argv, project);
   }

void
SgBinaryComposite::doSetupForConstructor(const vector<string>& argv, SgProject* project)
   {
     SgFile::doSetupForConstructor(argv, project);
   }

void
SgUnknownFile::doSetupForConstructor(const vector<string>& argv, SgProject* project)
   {
     SgFile::doSetupForConstructor(argv, project);
   }

void
SgFile::doSetupForConstructor(const vector<string>& argv, SgProject* project)
   {
  // JJW 10-26-2007 ensure that this object is not on the stack
     preventConstructionOnStack(this);

#if 0
     printf ("!!!!!!!!!!!!!!!!!! Inside of SgFile::doSetupForConstructor() !!!!!!!!!!!!!!! \n");
#endif

  // Set the project early in the construction phase so that we can access data in
  // the parent if needed (useful for template handling but also makes sure the parent is
  // set (and avoids fixup (currently done, but too late in the construction process for
  // the template support).
     if (project != NULL)
          set_parent(project);

     ROSE_ASSERT(project != NULL);
     ROSE_ASSERT(get_parent() != NULL);

  // initalize all local variables to default values
     initialization();

     ROSE_ASSERT(get_parent() != NULL);

  // DQ (2/4/2009): The specification of "-rose:binary" causes filenames to be interpreted
  // differently if they are object files or libary archive files.
  // DQ (4/21/2006): Setup the source filename as early as possible
  // setupSourceFilename(argv);
  // Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argv);
  // Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argv,get_binary_only());
     Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argv,project->get_binary_only());

  // DQ (12/23/2008): Use of this assertion will simplify the code below!
     ROSE_ASSERT (fileList.empty() == false);
     string sourceFilename = *(fileList.begin());

  // printf ("Before conversion to absolute path: sourceFilename = %s \n",sourceFilename.c_str());
  // sourceFilename = StringUtility::getAbsolutePathFromRelativePath(sourceFilename);
     sourceFilename = StringUtility::getAbsolutePathFromRelativePath(sourceFilename, true);

     set_sourceFileNameWithPath(sourceFilename);

  // printf ("In SgFile::setupSourceFilename(const vector<string>& argv): p_sourceFileNameWithPath = %s \n",get_sourceFileNameWithPath().c_str());
  // tps: 08/18/2010, This should call StringUtility for WINDOWS- there are two implementations of this?
  // set_sourceFileNameWithoutPath( ROSE::stripPathFromFileName(get_sourceFileNameWithPath().c_str()) );
     set_sourceFileNameWithoutPath( StringUtility::stripPathFromFileName(get_sourceFileNameWithPath().c_str()) );

     initializeSourcePosition(sourceFilename);
     ROSE_ASSERT(get_file_info() != NULL);

  // printf ("In SgFile::doSetupForConstructor(): source position set for sourceFilename = %s \n",sourceFilename.c_str());

  // DQ (5/9/2007): Moved this call from above to where the file name is available so that we could include
  // the filename in the label.  This helps to identify the performance data with individual files where
  // multiple source files are specificed on the command line.
  // printf ("p_sourceFileNameWithPath = %s \n",p_sourceFileNameWithPath);
     string timerLabel = "AST SgFile Constructor for " + p_sourceFileNameWithPath + ":";
     TimingPerformance timer (timerLabel);

  // Build a DEEP COPY of the input parameters!
     vector<string> local_commandLineArgumentList = argv;

  // Save the commandline as a list of strings (we made a deep copy because the "callFrontEnd()" function might change it!
     set_originalCommandLineArgumentList( local_commandLineArgumentList );

  // DQ (5/22/2005): Store the file name index in the SgFile object so that it can figure out
  // which file name applies to it.  This helps support functions such as "get_filename()"
  // used elsewhere in Sage III.  Not clear if we really need this!
  // error checking
     ROSE_ASSERT (argv.size() > 1);

  // DQ (12/23/2008): Added assertion.
     ROSE_ASSERT(get_file_info() != NULL);

  // DQ (5/3/2007): Added assertion.
     ROSE_ASSERT (get_startOfConstruct() != NULL);

  // DQ (6/13/2013): Added to support error checking (seperation of construction of SgFile IR nodes from calling the fronend on each one).
     ROSE_ASSERT(get_parent() != NULL);
     ROSE_ASSERT(get_parent() == project);

#if 0
     printf ("Leaving  SgFile::doSetupForConstructor() \n");
#endif
   }


string
SgFile::generate_C_preprocessor_intermediate_filename( string sourceFilename )
   {
  // DQ (9/24/2013): We need this assertion to make sure we are not causing what might be strange errors in memory.
     ROSE_ASSERT(sourceFilename.empty() == false);

  // Note: for "foo.F90" the fileNameSuffix() returns "F90"
     string filenameExtension              = StringUtility::fileNameSuffix(sourceFilename);

  // DQ (9/24/2013): We need this assertion to make sure we are not causing what might be strange errors in memory.
     ROSE_ASSERT(filenameExtension.empty() == false);

     string sourceFileNameWithoutExtension = StringUtility::stripFileSuffixFromFileName(sourceFilename);

  // DQ (9/24/2013): We need this assertion to make sure we are not causing what might be strange errors in memory.
     ROSE_ASSERT(sourceFileNameWithoutExtension.empty() == false);

  // string sourceFileNameInputToCpp = get_sourceFileNameWithPath();
#if 0
     printf ("In SgFile::generate_C_preprocessor_intermediate_filename(): Before lowering case: filenameExtension = %s \n",filenameExtension.c_str());
#endif

  // We need to turn on the 5th bit to make the capital a lower case character (assume ASCII)
     filenameExtension[0] = filenameExtension[0] | (1 << 5);

#if 0
     printf ("In SgFile::generate_C_preprocessor_intermediate_filename(): After lowering case: filenameExtension = %s \n",filenameExtension.c_str());
#endif

  // Rename the CPP generated intermediate file (strip path to put it in the current directory)
  // string sourceFileNameOutputFromCpp = sourceFileNameWithoutExtension + "_preprocessed." + filenameExtension;
     string sourceFileNameWithoutPathAndWithoutExtension = StringUtility::stripPathFromFileName(sourceFileNameWithoutExtension);
     string sourceFileNameOutputFromCpp = sourceFileNameWithoutPathAndWithoutExtension + "_postprocessed." + filenameExtension;

     return sourceFileNameOutputFromCpp;
   }


#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
// This is the "C" function implemented in:
//    ROSE/src/frontend/OpenFortranParser_SAGE_Connection/openFortranParser_main.c
// This function calls the Java JVM to load the Java implemented parser (written
// using ANTLR, a parser generator).
int openFortranParser_main(int argc, char **argv );
int experimental_openFortranParser_main(int argc, char **argv );
#endif

#ifdef ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
// DQ (10/11/2010): Added the Java support.
int openJavaParser_main(int argc, char **argv );
#endif

// See $ROSE/src/frontend/X10_ROSE_Connection
extern int x10_main(int argc, char** argv);

int
SgFile::callFrontEnd()
   {
     if (SgProject::get_verbose() > 0)
        {
          std::cout << "[INFO] [SgFile::callFrontEnd]" << std::endl;
        }

  // DQ (1/17/2006): test this
  // ROSE_ASSERT(get_fileInfo() != NULL);

     int fileNameIndex = 0;

  // DQ (4/21/2006): I think we can now assert this!
     ROSE_ASSERT(fileNameIndex == 0);

  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Front End Processing (SgFile):");

  // This function processes the command line and calls the EDG frontend.
     int frontendErrorLevel = 0;

  // Build an argc,argv based C style commandline (we might not really need this)
     vector<string> argv = get_originalCommandLineArgumentList();

#if ROSE_INTERNAL_DEBUG
     if (ROSE_DEBUG > 9)
        {
       // Print out the input arguments, so we can set them up internally instead of
       // on the command line (which is driving me nuts)
          for (unsigned int i=0; i < argv.size(); i++)
               printf ("argv[%d] = %s \n",i,argv[i]);
        }
#endif

  // printf ("Inside of SgFile::callFrontEnd(): fileNameIndex = %d \n",fileNameIndex);

  // Save this so that it can be used in the template instantiation phase later.
  // This file is later written into the *.ti file so that the compilation can
  // be repeated as required to instantiate all function templates.
     std::string translatorCommandLineString = CommandlineProcessing::generateStringFromArgList(argv,false,true);
  // printf ("translatorCommandLineString = %s \n",translatorCommandLineString.c_str());
     set_savedEdgCommandLine(translatorCommandLineString);

  // display("At TOP of SgFile::callFrontEnd()");

  // local copies of argc and argv variables
  // The purpose of building local copies is to avoid
  // the modification of the command line by SLA
     vector<string> localCopy_argv = argv;
  // printf ("DONE with copy of command line! \n");

  // Process command line options specific to ROSE
  // This leaves all filenames and non-rose specific option in the argv list
     processRoseCommandLineOptions (localCopy_argv);

  // DQ (6/21/2005): Process template specific options so that we can generated
  // code for the backend compiler (this processing is backend specific).
     processBackendSpecificCommandLineOptions (localCopy_argv);

  // display("AFTER processRoseCommandLineOptions in SgFile::callFrontEnd()");

  // Use ROSE buildCommandLine() function
  // int numberOfCommandLineArguments = 24;
  // char** inputCommandLine = new char* [numberOfCommandLineArguments];
  // ROSE_ASSERT (inputCommandLine != NULL);
     vector<string> inputCommandLine;

  // Build the commandline for EDG
  // printf ("Inside of SgFile::callFrontEnd(): Calling build_EDG_CommandLine (fileNameIndex = %d) \n",fileNameIndex);
  if (get_C_only() ||
      get_Cxx_only() ||
      get_Cuda_only() ||
      get_OpenCL_only()
  ) {
      #ifndef ROSE_USE_CLANG_FRONTEND
         build_EDG_CommandLine (inputCommandLine,localCopy_argv,fileNameIndex );
      #else
         build_CLANG_CommandLine (inputCommandLine,localCopy_argv,fileNameIndex );
      #endif
  }
  // printf ("DONE: Inside of SgFile::callFrontEnd(): Calling build_EDG_CommandLine (fileNameIndex = %d) \n",fileNameIndex);

  // DQ (10/15/2005): This is now a single C++ string (and not a list)
  // Make sure the list of file names is allocated, even if there are no file names in the list.
  // DQ (1/23/2004): I wish that C++ string objects had been used uniformally through out this code!!!
  // ROSE_ASSERT (get_sourceFileNamesWithoutPath() != NULL);
  // ROSE_ASSERT (get_sourceFileNameWithoutPath().empty() == false);

  // display("AFTER build_EDG_CommandLine in SgFile::callFrontEnd()");

  // Exit if we are to ONLY call the vendor's backend compiler
     if (p_useBackendOnly == true)
        {
          return 0;
        }

     ROSE_ASSERT (p_useBackendOnly == false);

  // DQ (4/21/2006): If we have called the frontend for this SgFile then mark this file to be unparsed.
  // This will cause code to be generated and the compileOutput() function will then set the name of
  // the file that the backend (vendor) compiler will compile to be the the intermediate file. Else it
  // will be set to be the origianl source file.  In the new design, the frontendShell() can be called
  // to generate just the SgProject and SgFile nodes and we can loop over the SgFile objects and call
  // the frontend separately for each SgFile.  so we have to set the output file name to be compiled
  // late in the processing (at backend compile time since we don't know when or if the frontend will
  // be called for each SgFile).
     set_skip_unparse(false);

     if ((get_C_only() || get_Cxx_only()) &&
      // DQ (1/22/2004): As I recall this has a name that really
      // should be "disable_edg" instead of "disable_edg_backend".
          get_disable_edg_backend() == false && get_new_frontend() == true)
        {
       // ROSE::new_frontend = true;

       // We can either use the newest EDG frontend separately (useful for debugging)
       // or the EDG frontend that is included in SAGE III (currently EDG 3.3).
       // New EDG frontend:
       //      This permits testing with the most up-to-date version of the EDG frontend and
       //      can be useful in identifing errors or bugs in the SAGE processing (or ROSE itself).
       // EDG frontend used by SAGE III:
       //      The use of this frontend permits the continued processing via ROSE and the
       //      unparsing of the AST to rebuilt the C++ source code (with transformations if any
       //      were done).

       // DQ (10/15/2005): This is now a C++ string (and not char* C style string)
       // Make sure that we have generated a proper file name (or move filename
       // processing to processRoseCommandLineOptions()).
       // printf ("Print out the file name to make sure it is processed \n");
       // printf ("     filename = %s \n",get_unparse_output_filename());
       // ROSE_ASSERT (get_unparse_output_filename() != NULL);
       // ROSE_ASSERT (get_unparse_output_filename().empty() == false);

      // Use the current version of the EDG frontend from EDG (or any other version)
      // abort();
         printf ("ROSE::new_frontend == true (call edgFrontEnd using unix system() function!) \n");

         std::string frontEndCommandLineString;
         if ( get_KCC_frontend() == true )
            {
              frontEndCommandLineString = "KCC ";  // -cpfe_only is no longer supported (I think)
            }
           else
            {
           // frontEndCommandLineString = "edgFrontEnd ";
              frontEndCommandLineString = "edgcpfe --g++ --gnu_version 40201 ";
            }
         frontEndCommandLineString += CommandlineProcessing::generateStringFromArgList(inputCommandLine,true,false);

         if ( get_verbose() > -1 )
              printf ("frontEndCommandLineString = %s \n\n",frontEndCommandLineString.c_str());

      // ROSE_ASSERT (!"Should not get here");
         int status = system(frontEndCommandLineString.c_str());

         printf ("After calling edgcpfe as a test (status = %d) \n",status);
         ROSE_ASSERT(status == 0);
      // ROSE_ASSERT(false);
        }
       else
        {
          if ((get_C_only() || get_Cxx_only()) && get_disable_edg_backend() == true)
             {
               if (SgProject::get_verbose() > 0)
                  {
                    std::cout << "[INFO] [SgFile::callFrontEnd] Skipping EDG frontend" << std::endl;
                  }
             }
            else
             {
            // DQ (9/2/2008): Factored out the details of building the AST for Source code (SgSourceFile IR node) and Binaries (SgBinaryComposite IR node)
            // Note that making buildAST() a virtual function does not appear to solve the problems since it is called form the base class.  This is
            // awkward code which is temporary.

            // printf ("Before calling buildAST(): this->class_name() = %s \n",this->class_name().c_str());

               switch (this->variantT())
                  {
                    case V_SgFile:
                    case V_SgSourceFile:
                       {
                         SgSourceFile* sourceFile = const_cast<SgSourceFile*>(isSgSourceFile(this));
                         frontendErrorLevel = sourceFile->buildAST(localCopy_argv, inputCommandLine);
                         break;
                       }

                    case V_SgBinaryComposite:
                       {
                         SgBinaryComposite* binary = const_cast<SgBinaryComposite*>(isSgBinaryComposite(this));
                         frontendErrorLevel = binary->buildAST(localCopy_argv, inputCommandLine);
                         break;
                       }

                    case V_SgUnknownFile:
                       {
                         break;
                       }

                    default:
                       {
                         printf ("Error: default reached in Rose parser/IR translation processing: class name = %s \n",this->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }
             }
        }

  // printf ("After calling buildAST(): this->class_name() = %s \n",this->class_name().c_str());

  // if there are warnings report that there are in the verbose mode and continue
     if (frontendErrorLevel > 0)
        {
          if ( get_verbose() >= 1 )
               cout << "Warnings in Rose parser/IR translation processing! (continuing ...) " << endl;
        }

  // DQ (4/20/2006): This code was moved from the SgFile constructor so that is would
  // permit the separate construction of the SgProject and call to the front-end cleaner.

  // DQ (5/22/2005): This is a older function with a newer more up-to-date comment on why we have it.
  // This function is a repository for minor AST fixups done as a post-processing step in the
  // construction of the Sage III AST from the EDG frontend.  In some cases it fixes specific
  // problems in either EDG or the translation of EDG to Sage III (more the later than the former).
  // In other cases if does post-processing (e.g. setting parent pointers in the AST) can could
  // only done from a more complete global view of the staticly defined AST.  In many cases these
  // AST fixups are not so temporary so the name of the function might change at some point.
  // Notice that all AST fixup is done before attachment of the comments to the AST.
  // temporaryAstFixes(this);

#if 0
  // FMZ (this is just debugging support)
     list<SgScopeStatement*> *stmp = &astScopeStack;
     printf("FMZ :: before AstPostProcessing astScopeStack = %p \n",stmp);
#endif

  // GB (8/19/2009): Commented this out and moved it to SgProject::parse().
  // Repeated calls to AstPostProcessing (one per file) can be slow on
  // projects consisting of multiple files due to repeated memory pool
  // traversals.
  // AstPostProcessing(this);

#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  // FMZ: 05/30/2008.  Do not generate .rmod file for the PU imported by "use" stmt
  // DXN (01/18/2011): Fixed to build rmod file only when there is no error passed back from the frontend.
  // if (get_Fortran_only() == true && FortranModuleInfo::isRmodFile() == false)
     if (get_Fortran_only() == true && FortranModuleInfo::isRmodFile() == false && frontendErrorLevel == 0)
        {
          if (get_verbose() > 1)
               printf ("Generating a Fortran 90 module file (*.rmod) \n");

          generateModFile(this);

          if (get_verbose() > 1)
               printf ("DONE: Generating a Fortran 90 module file (*.rmod) \n");
        }
#endif

#if 0
     printf ("Leaving SgFile::callFrontEnd(): fileNameIndex = %d \n",fileNameIndex);
     display("At bottom of SgFile::callFrontEnd()");
#endif
#if 0
     printf ("Exiting as a test of the F03 module support \n");
     ROSE_ASSERT(false);
#endif

  // return the error code associated with the call to the C++ Front-end
     return frontendErrorLevel;
   }



void
SgFile::secondaryPassOverSourceFile()
   {
  // **************************************************************************
  //                      Secondary Pass Over Source File
  // **************************************************************************
  // This pass collects extra information about the source file that may not have
  // been available from previous tools that operated on the file. For example:
  //    1) EDG ignores comments and so we collect the whole token stream in this phase.
  //    2) OFP ignores comments similarly to EDG and so we collect the whole token stream.
  //    3) Binary disassembly ignores the binary format so we collect this information
  //       about the structure of the ELF binary separately.
  // For source code (C,C++,Fortran) we collect the whole token stream, for example:
  //    1) Comments
  //    2) Preprocessors directives
  //    3) White space
  //    4) All tokens (each is classified as to what specific type of token it is)
  //
  // There is no secondary processing for binaries.

  // GB (9/4/2009): Factored out the secondary pass. It is now done after
  // the whole project has been constructed and fixed up.

     if (get_binary_only() == true)
        {
       // What used to be done here is now done above so that we can know the machine specific details
       // of the executable as early as possible before disassembly.
        }
       else
        {
       // This is set in the unparser now so that we can handle the source file plus all header files
          ROSE_ASSERT (p_preprocessorDirectivesAndCommentsList == NULL);

       // Build the empty list container so that we can just add lists for new files as they are encountered
          p_preprocessorDirectivesAndCommentsList = new ROSEAttributesListContainer();
          ROSE_ASSERT (p_preprocessorDirectivesAndCommentsList != NULL);

#if 0
       // This is empty so there is nothing to display!
          p_preprocessorDirectivesAndCommentsList->display("Seconadary Source File Processing at bottom of SgFile::callFrontEnd()");
#endif

       // DQ (4/19/2006): since they can take a while and includes substantial
       // file I/O we make this optional (selected from the command line).
       // bool collectAllCommentsAndDirectives = get_collectAllCommentsAndDirectives();

       // DQ (12/17/2008): The merging of CPP directives and comments from either the
       // source file or including all the include files is not implemented as a single
       // traversal and has been rewritten.
          if (get_skip_commentsAndDirectives() == false)
             {
               if (get_verbose() > 1)
                  {
                    printf ("In SgFile::secondaryPassOverSourceFile(): calling attachAllPreprocessingInfo() \n");
                  }

            // printf ("Secondary pass over source file = %s to comment comments and CPP directives \n",this->get_file_info()->get_filenameString().c_str());
            // SgSourceFile* sourceFile = const_cast<SgSourceFile*>(this);
               SgSourceFile* sourceFile = isSgSourceFile(this);
               ROSE_ASSERT(sourceFile != NULL);

            // Save the state of the requirement fo CPP processing (fortran only)
               bool requiresCPP = false;
               if (get_Fortran_only() == true)
                  {
                    requiresCPP = get_requires_C_preprocessor();
                    if (requiresCPP == true)
                         set_requires_C_preprocessor(false);
                  }
#if 1
            // Debugging code (eliminate use of CPP directives from source file so that we
            // can debug the insertion of linemarkers from first phase of CPP processing.
            // printf ("In SgFile::secondaryPassOverSourceFile(): requiresCPP = %s \n",requiresCPP ? "true" : "false");
               if (requiresCPP == false)
                  {
                    attachPreprocessingInfo(sourceFile);
                 // printf ("Exiting as a test (should not be called for Fortran CPP source files) \n");
                 // ROSE_ASSERT(false);
                  }
#else
            // Normal path calling attachPreprocessingInfo()
               attachPreprocessingInfo(sourceFile);
#endif

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
            // Liao, 3/31/2009 Handle OpenMP here to see macro calls within directives
               processOpenMP(sourceFile);
#endif
               // Liao, 1/29/2014, handle failsafe pragmas for resilience work
               if (sourceFile->get_failsafe())
                 FailSafe::process_fail_safe_directives (sourceFile); 

            // Reset the saved state (might not really be required at this point).
               if (requiresCPP == true)
                    set_requires_C_preprocessor(false);

#if 0
               printf ("In SgFile::callFrontEnd(): exiting after attachPreprocessingInfo() \n");
               ROSE_ASSERT(false);
#endif
               if (get_verbose() > 1)
                  {
                    printf ("In SgFile::callFrontEnd(): Done with attachAllPreprocessingInfo() \n");
                  }

            // DQ (12/13/2012): Insert pass over AST to detect "#line" directives within only the input source file and 
            // accoumulate a list of filenames that will have statements incorrectly marked as being from another file.
            // This might be something we want to control via a command line option.  This sort fo processing can be
            // important for generated files (e.g. the sudo application has four generated C language files from lex
            // that are generated into a file toke.c but with #line N "toke.l" CPP directives and this fools ROSE into
            // not unparsing all of the code from the token.c file (because some of it is assigned to the filename "toke.l".
            // For the case of the sudo application code this prevents the generated code from linking properly (undefined symbols).
            // Since many application include generated code (including ROSE itself) we would like to support this better.
               ROSE_ASSERT(sourceFile != NULL);
            // sourceFile->gatherASTSourcePositionsBasedOnDetectedLineDirectives();
            // sourceFile->fixupASTSourcePositionsBasedOnDetectedLineDirectives();

             } //end if get_skip_commentsAndDirectives() is false
        }

#if 0
     printf ("Leaving SgFile::callFrontEnd(): fileNameIndex = %d \n",fileNameIndex);
     display("At bottom of SgFile::callFrontEnd()");
#endif

#if 1
  // DQ (8/22/2009): We test the parent of SgFunctionTypeTable in the AST post processing,
  // so we need to make sure that it is set.
     SgFunctionTypeTable* functionTypeTable = SgNode::get_globalFunctionTypeTable();
  // ROSE_ASSERT(functionTypeTable != NULL);
     if (functionTypeTable != NULL && functionTypeTable->get_parent() == NULL)
        {
       // printf ("In SgFile::callFrontEnd(): set the parent of SgFunctionTypeTable \n");
          functionTypeTable->set_parent(this);
        }
  // ROSE_ASSERT(functionTypeTable->get_parent() != NULL);
#endif

  // ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
  // ROSE_ASSERT(SgNode::get_globalFunctionTypeTable()->get_parent() != NULL);
   }


#if 0
namespace SgSourceFile_processCppLinemarkers
   {
  // This class (AST traversal) supports the traversal of the AST required
  // to translate the source position using the CPP linemarkers.

     class GatherASTSourcePositionsBasedOnDetectedLineDirectives : public AstSimpleProcessing
        {
          public:
              set<int> filenameIdList;

              GatherASTSourcePositionsBasedOnDetectedLineDirectives( const string & sourceFilename );

              void visit ( SgNode* astNode );
        };
   }

SgSourceFile_processCppLinemarkers::GatherASTSourcePositionsBasedOnDetectedLineDirectives::GatherASTSourcePositionsBasedOnDetectedLineDirectives( const string & sourceFilename )
   {
  // Build an initial element on the stack so that the original source file name will be used to
  // set the global scope (which will be traversed before we visit any statements that might have
  // CPP directives attached (or which are CPP direcitve IR nodes).

  // Get the fileId of the assocated filename
     int fileId = Sg_File_Info::getIDFromFilename(sourceFilename);

     if ( SgProject::get_verbose() > 1 )
          printf ("In GatherASTSourcePositionsBasedOnDetectedLineDirectives::GatherASTSourcePositionsBasedOnDetectedLineDirectives(): source file: fileId = %d sourceFilename = %s \n",fileId,sourceFilename.c_str());

  // Push an entry onto the stack before doing the traversal over the whole AST.
  // filenameIdList.insert(fileId);
   }


void
SgSourceFile_processCppLinemarkers::GatherASTSourcePositionsBasedOnDetectedLineDirectives::visit ( SgNode* astNode )
   {
  // DQ (12/14/2012): This functionality is being added to support applications like sudo which have generated code from tools like lex.
  // This traversal is defined to detect the use of "#line" directived and then accumulate the list of filenames used in the #line directives.
  // When #line directives are used in the input source file then we consider all statements associated with those filenames to be from the
  // input source file and direct them to be unparsed.  The actual way we trigger them to be unparsed is to make the source position to 
  // be unparsed in the Sg_File_Info object, this is enough to force the unparsed to output those statements.
  // TODO: It might be that this should operate on SgLocatedNode IR nodes instead of SgStatement IR nodes.

     SgStatement* statement = isSgStatement(astNode);

     printf ("GatherASTSourcePositionsBasedOnDetectedLineDirectives::visit(): statement = %p = %s \n",statement,(statement != NULL) ? statement->class_name().c_str() : "NULL");

     if (statement != NULL)
        {
          if ( SgProject::get_verbose() > 1)
               printf ("GatherASTSourcePositionsBasedOnDetectedLineDirectives::visit(): statement = %p = %s \n",statement,statement->class_name().c_str());

          AttachedPreprocessingInfoType *commentOrDirectiveList = statement->getAttachedPreprocessingInfo();

          if (SgProject::get_verbose() > 1)
               printf ("GatherASTSourcePositionsBasedOnDetectedLineDirectives::visit(): commentOrDirectiveList = %p (size = %zu) \n",commentOrDirectiveList,(commentOrDirectiveList != NULL) ? commentOrDirectiveList->size() : 0);

          if (commentOrDirectiveList != NULL)
             {
               AttachedPreprocessingInfoType::iterator i = commentOrDirectiveList->begin();
               while(i != commentOrDirectiveList->end())
                  {
#if 0
                    printf ("Directve type = %d = %s = %s \n",(*i)->getTypeOfDirective(),PreprocessingInfo::directiveTypeName((*i)->getTypeOfDirective()).c_str(),(*i)->getString().c_str());
#endif
                 // if ( (*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorCompilerGeneratedLinemarker )
                    if ( (*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorLineDeclaration )
                       {
                      // This is a CPP line directive
                         string directiveString = (*i)->getString();
#if 0
                         printf ("directiveString = %s \n",directiveString.c_str());
#endif
                      // Remove leading white space.
                         size_t p = directiveString.find_first_not_of("# \t");
                         directiveString.erase(0,p);
#if 0
                         printf ("directiveString (trimmed) = %s \n",directiveString.c_str());
#endif
                      // string directiveStringWithoutHash = directiveString;
                         size_t lengthOfLineKeyword = string("line").length();

                         string directiveStringWithoutHashAndKeyword = directiveString.substr(lengthOfLineKeyword,directiveString.length()-(lengthOfLineKeyword+1));
#if 0
                         printf ("directiveStringWithoutHashAndKeyword = %s \n",directiveStringWithoutHashAndKeyword.c_str());
#endif
                      // Remove white space between "#" and "line" keyword.
                         p = directiveStringWithoutHashAndKeyword.find_first_not_of(" \t");
                         directiveStringWithoutHashAndKeyword.erase(0, p);
#if 0
                         printf ("directiveStringWithoutHashLineAndKeyword (trimmed) = %s \n",directiveStringWithoutHashAndKeyword.c_str());
#endif
                      // At this point we have just '2 "toke.l"', and we can strip off the number.
                         p = directiveStringWithoutHashAndKeyword.find_first_not_of("0123456789");

                         string lineNumberString = directiveStringWithoutHashAndKeyword.substr(0,p);
#if 0
                         printf ("lineNumberString = %s \n",lineNumberString.c_str());
#endif
                         int line = atoi(lineNumberString.c_str());

                      // string directiveStringWithoutHashAndKeywordAndLineNumber = directiveStringWithoutHashAndKeyword.substr(p,directiveStringWithoutHashAndKeyword.length()-(p+1));
                         string directiveStringWithoutHashAndKeywordAndLineNumber = directiveStringWithoutHashAndKeyword.substr(p,directiveStringWithoutHashAndKeyword.length());
#if 0
                         printf ("directiveStringWithoutHashAndKeywordAndLineNumber = %s \n",directiveStringWithoutHashAndKeywordAndLineNumber.c_str());
#endif
                      // Remove white space between the line number and the filename.
                         p = directiveStringWithoutHashAndKeywordAndLineNumber.find_first_not_of(" \t");
                         directiveStringWithoutHashAndKeywordAndLineNumber.erase(0,p);
#if 0
                         printf ("directiveStringWithoutHashAndKeywordAndLineNumber (trimmed) = %s \n",directiveStringWithoutHashAndKeywordAndLineNumber.c_str());
#endif
                         string quotedFilename = directiveStringWithoutHashAndKeywordAndLineNumber;
#if 0                         
                         printf ("quotedFilename = %s \n",quotedFilename.c_str());
#endif
                         ROSE_ASSERT(quotedFilename[0] == '\"');
                         ROSE_ASSERT(quotedFilename[quotedFilename.length()-1] == '\"');
                         std::string filename = quotedFilename.substr(1,quotedFilename.length()-2);
#if 0
                         printf ("filename = %s \n",filename.c_str());
#endif
                      // Add the new filename to the static map stored in the Sg_File_Info (no action if filename is already in the map).
                         Sg_File_Info::addFilenameToMap(filename);

                         int fileId = Sg_File_Info::getIDFromFilename(filename);

                         if (SgProject::get_verbose() >= 0)
                              printf ("In SgSourceFile_processCppLinemarkers::GatherASTSourcePositionsBasedOnDetectedLineDirectives::visit(): line = %d fileId = %d quotedFilename = %s filename = %s \n",line,fileId,quotedFilename.c_str(),filename.c_str());

                         if (filenameIdList.find(fileId) == filenameIdList.end())
                            {
                              filenameIdList.insert(fileId);
                            }
                       }

                    i++;
                  }
             }
        }
   }
#endif

#if 0
void
SgSourceFile::gatherASTSourcePositionsBasedOnDetectedLineDirectives()
   {
  // DQ (12/18/2012): This is not inlined into the fixupASTSourcePositionsBasedOnDetectedLineDirectives() function to simplify the code.

     printf ("Error: This function should not be called \n");
     ROSE_ASSERT(false);

#if 0
     SgSourceFile* sourceFile = const_cast<SgSourceFile*>(this);

     SgSourceFile_processCppLinemarkers::GatherASTSourcePositionsBasedOnDetectedLineDirectives linemarkerTraversal(sourceFile->get_sourceFileNameWithPath());

     linemarkerTraversal.traverse(sourceFile,preorder);

#if 0
     set<int>::iterator i = linemarkerTraversal.filenameIdList.begin();
     int counter = 0;
     while (i != linemarkerTraversal.filenameIdList.end())
        {
          printf ("filenameIdList entry #%d = %d \n",counter,*i);
          counter++;

          i++;
        }
#endif
#else
     printf ("NOTE: SgSourceFile::gatherASTSourcePositionsBasedOnDetectedLineDirectives(): empty function \n");
#endif
#if 0
     printf ("Exiting as a test ... (processing gatherASTSourcePositionsBasedOnDetectedLineDirectives) \n");
     ROSE_ASSERT(false);
#endif
   }
#endif


#if 1
namespace SgSourceFile_processCppLinemarkers
   {
     class FixupASTSourcePositionsBasedOnDetectedLineDirectives : public AstSimpleProcessing
        {
          public:
              set<int> filenameIdList;

              FixupASTSourcePositionsBasedOnDetectedLineDirectives( const string & sourceFilename, set<int> & filenameSet );

              void visit ( SgNode* astNode );
        };
   }


SgSourceFile_processCppLinemarkers::FixupASTSourcePositionsBasedOnDetectedLineDirectives::FixupASTSourcePositionsBasedOnDetectedLineDirectives( const string & sourceFilename, set<int> & filenameSet )
   : filenameIdList(filenameSet)
   {
     if (SgProject::get_verbose() > 1)
          printf ("In FixupASTSourcePositionsBasedOnDetectedLineDirectives::FixupASTSourcePositionsBasedOnDetectedLineDirectives(): filenameIdList.size() = %zu \n",filenameIdList.size());
   }


void
SgSourceFile_processCppLinemarkers::FixupASTSourcePositionsBasedOnDetectedLineDirectives::visit ( SgNode* astNode )
   {
  // DQ (12/14/2012): This functionality is being added to support applications like sudo which have generated code from tools like lex.
  // This traversal is defined to detect the use of "#line" directived and then accumulate the list of filenames used in the #line directives.
  // When #line directives are used in the input source file then we consider all statements associated with those filenames to be from the
  // input source file and direct them to be unparsed.  The actual way we trigger them to be unparsed is to make the source position to 
  // be unparsed in the Sg_File_Info object, this is enough to force the unparsed to output those statements.
  // TODO: It might be that this should operate on SgLocatedNode IR nodes instead of SgStatement IR nodes.

     SgStatement* statement = isSgStatement(astNode);

#if 0
     printf ("FixupASTSourcePositionsBasedOnDetectedLineDirectives::visit(): statement = %p = %s \n",statement,(statement != NULL) ? statement->class_name().c_str() : "NULL");
#endif

     if (statement != NULL)
        {
          if ( SgProject::get_verbose() > 1)
               printf ("FixupASTSourcePositionsBasedOnDetectedLineDirectives::visit(): statement = %p = %s \n",statement,statement->class_name().c_str());

          ROSE_ASSERT(statement->get_file_info() != NULL);
          int fileId = statement->get_file_info()->get_file_id();

       // Check if this fileId is associated with set of fileId that we would like to output.
          if (filenameIdList.find(fileId) != filenameIdList.end())
             {
#if 0
               printf ("FixupASTSourcePositionsBasedOnDetectedLineDirectives::visit(): statement = %p = %s \n",statement,(statement != NULL) ? statement->class_name().c_str() : "NULL");
               printf ("Fixup the fileInfo for this statement to force it to be output fileId = %d \n",fileId);
               Sg_File_Info::display_static_data("FixupASTSourcePositionsBasedOnDetectedLineDirectives::visit()");
#endif
#if 0
               statement->get_startOfConstruct()->set_file_id(0);
               statement->get_endOfConstruct()->set_file_id(0);
#else
            // statement->get_fileInfo()->setOutputInCodeGeneration();
               statement->get_startOfConstruct()->setOutputInCodeGeneration();
               statement->get_endOfConstruct()  ->setOutputInCodeGeneration();
#endif
             }
        }
   }
#endif

#if 1
void
SgSourceFile::fixupASTSourcePositionsBasedOnDetectedLineDirectives(set<int> equivalentFilenames)
   {
#if 0
  // DQ (12/23/2012): This function is called from: AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute().
  // It is used to map filenames generated from #line directives into thoses associated with the
  // physical filename.  However, now that we internally keep references to both the logical 
  // source position AND the physical source position, this should not be required.

     printf ("ERROR: Now that we strore both logical and physical source position information, this function shuld not be used \n");
     ROSE_ASSERT(false);
#endif

#if 0
  // DQ (12/18/2012): This function is not called, since processing the #line directives after they have been added to the AST is TOO LATE!
  // We instead have to process the list of collected directives from the lexer more directly before they are added to the AST so that we
  // can build the list of equivalent filenames (to the input file) and thus get the all of the directives into the correct places using
  // the mapping of the input filename to the list of equivalent filenames (from the #line directives).  Then we get both the comments/CPP
  // directives into the correct locaions AND we can then process the statement's source positions to force the output in the unparser.

     SgSourceFile* sourceFile = const_cast<SgSourceFile*>(this);

     SgSourceFile_processCppLinemarkers::GatherASTSourcePositionsBasedOnDetectedLineDirectives linemarkerTraversal_1(sourceFile->get_sourceFileNameWithPath());

     linemarkerTraversal_1.traverse(sourceFile,preorder);
#endif
#if 0
     printf ("In SgSourceFile::fixupASTSourcePositionsBasedOnDetectedLineDirectives(): output set of associated filenames: \n");
     set<int>::iterator i = linemarkerTraversal_1.filenameIdList.begin();
     int counter = 0;
     while (i != linemarkerTraversal_1.filenameIdList.end())
        {
          printf ("   --- filenameIdList entry #%d = %d \n",counter,*i);
          counter++;

          i++;
        }
#endif
#if 0
     printf ("Exiting as a test ... (processing gatherASTSourcePositionsBasedOnDetectedLineDirectives) \n");
     ROSE_ASSERT(false);
#endif

  // SgSourceFile_processCppLinemarkers::FixupASTSourcePositionsBasedOnDetectedLineDirectives linemarkerTraversal(sourceFile->get_sourceFileNameWithPath(),linemarkerTraversal_1.filenameIdList);
     SgSourceFile_processCppLinemarkers::FixupASTSourcePositionsBasedOnDetectedLineDirectives linemarkerTraversal(this->get_sourceFileNameWithPath(),equivalentFilenames);

     linemarkerTraversal.traverse(this,preorder);

#if 0
     printf ("Exiting as a test ... (processing fixupASTSourcePositionsBasedOnDetectedLineDirectives) \n");
     ROSE_ASSERT(false);
#endif
   }
#endif

int
SgSourceFile::build_Fortran_AST( vector<string> argv, vector<string> inputCommandLine )
   {
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  // This is how we pass the pointer to the SgFile created in ROSE before the Open
  // Fortran Parser is called to the Open Fortran Parser.  In the case of C/C++ using
  // EDG the SgFile is passed through the edg_main() function, but not so with the
  // Open Fortran Parser's openFortranParser_main() function API.  So we use this
  // global variable to pass the SgFile (so that the parser c_action functions can
  // build the Fotran AST using the existing SgFile.

     // FMZ(7/27/2010): check command line options for Rice CAF syntax
     //  -rose:CoArrayFortran, -rose:CAF, -rose:caf

     bool using_rice_caf = false;
     vector<string> ArgTmp = get_project()->get_originalCommandLineArgumentList();
     int sizeArgs = ArgTmp.size();

     for (int i = 0; i< sizeArgs; i++)  {
       if (ArgTmp[i].find("-rose:caf",0)==0     ||
           ArgTmp[i].find("-rose:CAF2.0",0)==0  ||
           ArgTmp[i].find("-rose:CAF2.0",0)==0  ) {

         using_rice_caf=true;
         break;
       }
     }


     extern SgSourceFile* OpenFortranParser_globalFilePointer;

  // DQ (10/26/2010): Moved from SgSourceFile::callFrontEnd() so that the stack will
  // be empty when processing Java language support (not Fortran).
     FortranParserState* currStks = new FortranParserState();

  // printf ("######################### Inside of SgSourceFile::build_Fortran_AST() ############################ \n");

     bool requires_C_preprocessor = get_requires_C_preprocessor();
     if (requires_C_preprocessor == true)
        {
          int errorCode;
          
       // If we detect that the input file requires processing via CPP (e.g. filename of form *.F??) then 
       // we generate the command to run CPP on the input file and collect the results in a file with 
       // the suffix "_postprocessed.f??".  Note: instead of using CPP we use the target backend fortran 
       // compiler with the "-E" option.

          vector<string> fortran_C_preprocessor_commandLine;

       // Note: The `-traditional' and `-undef' flags are supplied to cpp by default [when used with cpp is used by gfortran],
       // to help avoid unpleasant surprises.  So to simplify use of cpp and make it more consistant with gfortran we use
       // gfortran to call cpp.
          fortran_C_preprocessor_commandLine.push_back(BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH);

       // DQ (10/23/2010): Added support for "-D" options (this will trigger CPP preprocessing, eventually, but this is just to support the syntax checking).
       // Note that we have to do this before calling the C preprocessor and not with the syntax checking.
          const SgStringList & macroSpecifierList = get_project()->get_macroSpecifierList();
          for (size_t i = 0; i < macroSpecifierList.size(); i++)
             {
            // Note that gfortran will only do macro substitution of "-D" command line arguments on files with *.F or *.F?? suffix.
               ROSE_ASSERT(get_requires_C_preprocessor() == true);
               fortran_C_preprocessor_commandLine.push_back("-D"+macroSpecifierList[i]);
             }

       // DQ (5/19/2008): Added support for include paths as required for relatively new Fortran specific include mechanism in OFP.
          const SgStringList & includeList = get_project()->get_includeDirectorySpecifierList();
          for (size_t i = 0; i < includeList.size(); i++)
             {
               fortran_C_preprocessor_commandLine.push_back(includeList[i]);
             }

       // add option to specify preprocessing only
          fortran_C_preprocessor_commandLine.push_back("-E");

       // add source file name
          string sourceFilename              = get_sourceFileNameWithPath();
          string preprocessFilename;

          // use a pseudonym for source file in case original extension does not permit preprocessing
             // compute absolute path for pseudonym
                // TODO: when boost 1.46 is available, use boost::filesystem to get 'dir', 'abs_dir', and 'base'
                string dir = StringUtility::getPathFromFileName(this->get_unparse_output_filename());
                string abs_dir = StringUtility::getAbsolutePathFromRelativePath(dir.empty() ? getWorkingDirectory() : dir);  // Windows 'tempnam' requires this
                string file = StringUtility::stripPathFromFileName(sourceFilename);
                string base = StringUtility::stripFileSuffixFromFileName(file);
                char * temp = tempnam(abs_dir.c_str(), (base + "-").c_str());   // not deprecated in Visual Studio 2010
                preprocessFilename = string(temp) + ".F90"; free(temp);
             // copy source file to pseudonym file
                try { boost::filesystem::copy_file(sourceFilename, preprocessFilename); }
                catch(exception &e)
                {
                  cout << "Error in copying file " << sourceFilename << " to " << preprocessFilename
                       << " (" << e.what() << ")" << endl;
                  ROSE_ASSERT(false);
                }
          fortran_C_preprocessor_commandLine.push_back(preprocessFilename);

       // add option to specify output file name
          fortran_C_preprocessor_commandLine.push_back("-o");
          string sourceFileNameOutputFromCpp = generate_C_preprocessor_intermediate_filename(sourceFilename);
          fortran_C_preprocessor_commandLine.push_back(sourceFileNameOutputFromCpp);

          if ( SgProject::get_verbose() > 0 )
               printf ("cpp command line = %s \n",CommandlineProcessing::generateStringFromArgList(fortran_C_preprocessor_commandLine,false,false).c_str());

#if USE_GFORTRAN_IN_ROSE
       // Some security checking here could be helpful!!!
          errorCode = systemFromVector (fortran_C_preprocessor_commandLine);
#endif

       // DQ (10/1/2008): Added error checking on return value from CPP.
          if (errorCode != 0)
          {
             printf ("Error in running cpp on Fortran code: errorCode = %d \n",errorCode);
             ROSE_ASSERT(false);
          }

       // clean up after alias processing
          try { boost::filesystem::remove(preprocessFilename); }
          catch(exception &e)
          {
            cout << "Error in removing file " << preprocessFilename
                 << " (" << e.what() << ")" << endl;
            ROSE_ASSERT(false);
          }
     }


  // DQ (9/30/2007): Introduce syntax checking on input code (initially we can just call the backend compiler
  // and let it report on the syntax errors).  Later we can make this a command line switch to disable (default
  // should be true).
  // bool syntaxCheckInputCode = true;
     bool syntaxCheckInputCode = (get_skip_syntax_check() == false);

  // printf ("In build_Fortran_AST(): syntaxCheckInputCode = %s \n",syntaxCheckInputCode ? "true" : "false");

     if (syntaxCheckInputCode == true)
        {
       // Note that syntax checking of Fortran 2003 code using gfortran versions greater than 4.1 can
       // be a problem because there are a lot of bugs in the Fortran 2003 support in later versions
       // of gfortran (not present in initial Fortran 2003 support for syntax checking only). This problem
       // has been verified in version 4.2 and 4.3 of gfortran.

       // DQ (9/30/2007): Introduce tracking of performance of ROSE.
          TimingPerformance timer ("Fortran syntax checking of input:");

       // DQ (9/30/2007): For Fortran, we want to run gfortran up front so that we can verify that
       // the input file is syntax error free.  First lets see what data is avilable to use to check
       // that we have a fortran file.
       // display("Before calling OpenFortranParser, what are the values in the SgFile");

       // DQ (9/30/2007): Call the backend Fortran compiler (typically gfortran) to check the syntax
       // of the input program.  When using GNU gfortran, use the "-S" option which means:
       // "Compile only; do not assemble or link".

       // DQ (11/17/2007): Note that syntax and semantics checking is turned on using -fno-backend not -S
       // as I previously thought.  Also I have turned on all possible warnings and specified the Fortran 2003
       // features.  I have also specified use of cray-pointers.
       // string syntaxCheckingCommandline = "gfortran -S " + get_sourceFileNameWithPath();
       // string warnings = "-Wall -Wconversion -Waliasing -Wampersand -Wimplicit-interface -Wline-truncation -Wnonstd-intrinsics -Wsurprising -Wunderflow -Wunused-labels";
       // DQ (12/8/2007): Added commandline control over warnings output in using gfortran sytax checking prior to use of OFP.

       // printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Setting up Fortran Syntax check @@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");

          vector<string> fortranCommandLine;
          fortranCommandLine.push_back(ROSE_GFORTRAN_PATH);
          fortranCommandLine.push_back("-fsyntax-only");

       // DQ (5/19/2008): Added support for include paths as required for relatively new Fortran specific include mechanism in OFP.
          const SgStringList & includeList = get_project()->get_includeDirectorySpecifierList();
          for (size_t i = 0; i < includeList.size(); i++)
             {
               fortranCommandLine.push_back(includeList[i]);
             }

          if (get_output_warnings() == true)
             {
            // These are gfortran specific options
            // As of 2004, -Wall implied: -Wunused-labels, -Waliasing, -Wsurprising and -Wline-truncation
            // Additional major options include:
            //      -fsyntax-only -pedantic -pedantic-errors -w -Wall -Waliasing -Wconversion -Wimplicit-interface
            //      -Wsurprising -Wunderflow -Wunused-labels -Wline-truncation -Werror -W
            // warnings = "-Wall -Wconversion -Waliasing -Wampersand -Wimplicit-interface -Wline-truncation -Wnonstd-intrinsics -Wsurprising -Wunderflow";

            // If warnings are requested (on the comandline to ROSE translator) then we want to output all possible warnings by defaul (at least for how)

            // Check if we are using GNU compiler backend (if so then we are using gfortran, though we have no test in place currently for what
            // version of gfortran (as we do for C and C++))
               string backendCompilerSystem = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
               if (backendCompilerSystem == "g++" || backendCompilerSystem == "mpicc" || backendCompilerSystem == "mpicxx")
                  {
                 // Since this is specific to gfortran version 4.1.2, we will exclude it (it is also redundant since it is included in -Wall)
                 // warnings += " -Wunused-labels";
                 // warnings = "-Wall -Wconversion -Wampersand -Wimplicit-interface -Wnonstd-intrinsics -Wunderflow";
                    fortranCommandLine.push_back("-Wall");

                 // Add in the gfortran extra warnings
                    fortranCommandLine.push_back("-W");

                 // More warnings not yet turned on.
                    fortranCommandLine.push_back("-Wconversion");
                    fortranCommandLine.push_back("-Wampersand");
                    fortranCommandLine.push_back("-Wimplicit-interface");
                    fortranCommandLine.push_back("-Wnonstd-intrinsics");
                    fortranCommandLine.push_back("-Wunderflow");
                  }
                 else
                  {
                    printf ("Currently only the GNU compiler backend is supported (gfortran) backendCompilerSystem = %s \n",backendCompilerSystem.c_str());
                    ROSE_ASSERT(false);
                  }
             }

       // Refactor the code below so that we can conditionally set the -ffree-line-length-none
       // or -ffixed-line-length-none options (not available in all versions of gfortran).
          string use_line_length_none_string;

          bool relaxSyntaxCheckInputCode = (get_relax_syntax_check() == true);

       // DQ (11/17/2007): Set the fortran mode used with gfortran.
          if (get_F90_only() == true || get_F95_only() == true)
             {
            // For now let's consider f90 to be syntax checked under f95 rules (since gfortran does not support a f90 specific mode)
               if (relaxSyntaxCheckInputCode == false)
                  {
                 // DQ (9/24/2010): Relaxed syntax checking would allow "REAL*8" syntax with F90 if we used "-std=legacy" instead of "-std=f95".
                 // We will implement a strict syntax checking option with a default of false so that we can by default support codes using
                 // the "REAL*8" syntax with F90 (which appear to be common).
                 // fortranCommandLine.push_back("-std=f95");
                    fortranCommandLine.push_back("-std=legacy");
                  }

            // DQ (5/20/2008)
            // fortranCommandLine.push_back("-ffree-line-length-none");
               use_line_length_none_string = "-ffree-line-length-none";
             }
            else
             {
               if (get_F2003_only() == true)
                  {
                 // fortranCommandLine.push_back("-std=f2003");
                    if (relaxSyntaxCheckInputCode == false)
                       {
                      // DQ (9/24/2010): We need to consider making a strict syntax checking option and allowing this to be relaxed
                      // by default.  It is however not clear that this is required for F2003 code where it does appear to be required
                      // for F90 code.  So this needs to be tested, see comments above relative to use of "-std=legacy".
                         fortranCommandLine.push_back("-std=f2003");
                       }

                 // DQ (5/20/2008)
                 // fortranCommandLine.push_back("-ffree-line-length-none");
                    use_line_length_none_string = "-ffree-line-length-none";
                  }
                 else
                  {
                 // This should be the default mode (fortranMode string is empty). So is it f77?

                 // DQ (5/20/2008)
                 // fortranCommandLine.push_back ("-ffixed-line-length-none");
                    use_line_length_none_string = "-ffixed-line-length-none";
                  }
             }

// We need this #if since if gfortran is unavailable the macros for the major and minor version numbers will be empty strings (blank).
#if USE_GFORTRAN_IN_ROSE
       // DQ (9/16/2009): This option is not available in gfortran version 4.0.x (wonderful).
       // DQ (5/20/2008): Need to select between fixed and free format
       // fortran_C_preprocessor_commandLine.push_back("-ffree-line-length-none");
          if ( (BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER >= 4) && (BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER >= 1) )
             {
               fortranCommandLine.push_back(use_line_length_none_string);
             }
#endif

       // DQ (12/8/2007): Added support for cray pointers from commandline.
          if (get_cray_pointer_support() == true)
             {
               fortranCommandLine.push_back("-fcray-pointer");
             }

       // Note that "-c" is required to enforce that we only compile and not link the result (even though -fno-backend is specified)
       // A web page specific to -fno-backend suggests using -fsyntax-only instead (so the "-c" options is not required).
#if 1
       // if ( SgProject::get_verbose() > 0 )
          if ( get_verbose() > 0 )
             {
               printf ("Checking syntax of input program using gfortran: syntaxCheckingCommandline = %s \n",CommandlineProcessing::generateStringFromArgList(fortranCommandLine,false,false).c_str());
             }
#endif
       // Call the OS with the commandline defined by: syntaxCheckingCommandline
#if 0
          fortranCommandLine.push_back(get_sourceFileNameWithPath());
#else
       // DQ (5/19/2008): Support for C preprocessing
          if (requires_C_preprocessor == true)
             {
            // If C preprocessing was required then we have to provide the generated filename of the preprocessed file!

            // Note that since we are using gfortran to do the syntax checking, we could just
            // hand the original file to gfortran instead of the one that we generate using CPP.
               string sourceFilename    = get_sourceFileNameWithPath();
               string sourceFileNameOutputFromCpp = generate_C_preprocessor_intermediate_filename(sourceFilename);
               fortranCommandLine.push_back(sourceFileNameOutputFromCpp);
             }
            else
             {
            // This will cause the original file to be used for syntax checking (instead of
            // the CPP generated one, if one was generated).
               fortranCommandLine.push_back(get_sourceFileNameWithPath());
             }
#endif

       // At this point we have the full command line with the source file name
          if ( get_verbose() > 0 )
             {
               printf ("Checking syntax of input program using gfortran: syntaxCheckingCommandline = %s \n",CommandlineProcessing::generateStringFromArgList(fortranCommandLine,false,false).c_str());
             }

          int returnValueForSyntaxCheckUsingBackendCompiler = 0;
#if USE_GFORTRAN_IN_ROSE
        returnValueForSyntaxCheckUsingBackendCompiler = systemFromVector (fortranCommandLine);
#else
        printf ("backend fortran compiler (gfortran) unavailable ... (not an error) \n");
#endif

     // Check that there are no errors, I think that warnings are ignored!
        if (returnValueForSyntaxCheckUsingBackendCompiler != 0)
           {
             printf ("Syntax errors detected in input fortran program ... \n");

          // We should define some convention for error codes returned by ROSE
             throw std::exception();
           }
        ROSE_ASSERT(returnValueForSyntaxCheckUsingBackendCompiler == 0);

     // printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@ DONE: Setting up Fortran Syntax check @@@@@@@@@@@@@@@@@@@@@@@@@ \n");

#if 0
        printf ("Exiting as a test ... (after syntax check) \n");
        ROSE_ASSERT(false);
#endif
      }

    // Build the classpath list for Fortran support.
    string classpath =
        Rose::Cmdline::Fortran::Ofp::GetRoseClasspath();

  //
  // In the case of Javam add the paths specified for the input program, if any.
  //
     list<string> classpath_list = get_project() -> get_Java_classpath();
     for (list<string>::iterator i = classpath_list.begin(); i != classpath_list.end(); i++) {
         classpath += ":";
         classpath += (*i);
     }


  // This is part of debugging output to call OFP and output the list of parser actions that WOULD be called.
  // printf ("get_output_parser_actions() = %s \n",get_output_parser_actions() ? "true" : "false");
     if (get_output_parser_actions() == true)
        {
       // DQ (1/19/2008): New version of OFP requires different calling syntax.
       // string OFPCommandLineString = std::string("java parser.java.FortranMain") + " --dump " + get_sourceFileNameWithPath();
          vector<string> OFPCommandLine;
          OFPCommandLine.push_back(JAVA_JVM_PATH);
          OFPCommandLine.push_back(classpath);
          OFPCommandLine.push_back("fortran.ofp.FrontEnd");
          OFPCommandLine.push_back("--dump");
       // OFPCommandLine.push_back("--tokens");

       // DQ (5/18/2008): Added support for include paths as required for relatively new Fortran specific include mechanism in OFP.
          const SgStringList & includeList = get_project()->get_includeDirectorySpecifierList();
          for (size_t i = 0; i < includeList.size(); i++)
             {
               OFPCommandLine.push_back(includeList[i]);
             }

       // DQ (5/19/2008): Support for C preprocessing
          if (requires_C_preprocessor == true)
             {
            // If C preprocessing was required then we have to provide the generated filename of the preprocessed file!
            // Note that OFP has no support for CPP directives and will ignore them all.
               string sourceFilename              = get_sourceFileNameWithPath();
               string sourceFileNameOutputFromCpp = generate_C_preprocessor_intermediate_filename(sourceFilename);
               OFPCommandLine.push_back(sourceFileNameOutputFromCpp);
             }
            else
             {
            // Build the command line using the original file (to be used by OFP).
               OFPCommandLine.push_back(get_sourceFileNameWithPath());
             }

#if 1
          printf ("output_parser_actions: OFPCommandLine = %s \n",CommandlineProcessing::generateStringFromArgList(OFPCommandLine,false,false).c_str());
#endif

#if 1
       // Some security checking here could be helpful!!!
       // Run OFP with the --dump option so that we can get the parset actions (used only for internal debugging support).
          int errorCode = systemFromVector(OFPCommandLine);

          if (errorCode != 0)
             {
               printf ("Running OFP ONLY causes an error (errorCode = %d) \n",errorCode);
#if 1
            // DQ (10/4/2008): Need to work with Liao to see why this passes for me but fails for him (and others).
            // for now we can comment out the error checking on the running of OFP as part of getting the
            // output_parser_actions option (used for debugging).
               ROSE_ASSERT(false);
#else
               printf ("Skipping enforcement of exit after running OFP ONLY as (part of output_parser_actions option) \n");
#endif
             }
#else

#error "REMOVE THIS CODE"

       // This fails, I think because we can't call the openFortranParser_main twice.
       // DQ (11/30/2008):  Does the work by Rice fix this now?
          int openFortranParser_dump_argc    = 0;
          char** openFortranParser_dump_argv = NULL;
          CommandlineProcessing::generateArgcArgvFromList(OFPCommandLine,openFortranParser_dump_argc,openFortranParser_dump_argv);
          frontendErrorLevel = openFortranParser_main (openFortranParser_dump_argc, openFortranParser_dump_argv);

#endif
       // If this was selected as an option then we can stop here (rather than call OFP again).
       // printf ("--- get_exit_after_parser() = %s \n",get_exit_after_parser() ? "true" : "false");
          if (get_exit_after_parser() == true)
             {
               printf ("Exiting after parsing... \n");
               exit(0);
             }

       // End of option handling to generate list of OPF parser actions.
        }

  // Option to just run the parser (not constructing the AST) and quit.
  // printf ("get_exit_after_parser() = %s \n",get_exit_after_parser() ? "true" : "false");
     if (get_exit_after_parser() == true)
        {
       // DQ (1/19/2008): New version of OFP requires different calling syntax.
       // string OFPCommandLineString = std::string("java parser.java.FortranMain") + " " + get_sourceFileNameWithPath();
          vector<string> OFPCommandLine;
          OFPCommandLine.push_back(JAVA_JVM_PATH);
          OFPCommandLine.push_back(classpath);
          OFPCommandLine.push_back("fortran.ofp.FrontEnd");

          bool foundSourceDirectoryExplicitlyListedInIncludePaths = false;

       // DQ (5/18/2008): Added support for include paths as required for relatively new Fortran specific include mechanism in OFP.
          const SgStringList & includeList = get_project()->get_includeDirectorySpecifierList();
          for (size_t i = 0; i < includeList.size(); i++)
             {
               OFPCommandLine.push_back(includeList[i]);

            // printf ("includeList[%d] = %s \n",i,includeList[i].c_str());

            // I think we have to permit an optional space between the "-I" and the path
               if ("-I" + getSourceDirectory() == includeList[i] || "-I " + getSourceDirectory() == includeList[i])
                  {
                 // The source file path is already included!
                    foundSourceDirectoryExplicitlyListedInIncludePaths = true;
                  }
             }

       // printf ("foundSourceDirectoryExplicitlyListedInIncludePaths = %s \n",foundSourceDirectoryExplicitlyListedInIncludePaths ? "true" : "false");
          if (foundSourceDirectoryExplicitlyListedInIncludePaths == false)
             {
            // Add the source directory to the include list so that we reproduce the semantics of gfortran
               OFPCommandLine.push_back("-I" + getSourceDirectory() );
             }

       // DQ (8/24/2010): Detect the use of CPP on the fortran file and use the correct generated file from CPP, if required.
       // OFPCommandLine.push_back(get_sourceFileNameWithPath());
          if (requires_C_preprocessor == true)
             {
               string sourceFilename = get_sourceFileNameWithoutPath();
               string sourceFileNameOutputFromCpp = generate_C_preprocessor_intermediate_filename(sourceFilename);
               OFPCommandLine.push_back(sourceFileNameOutputFromCpp);
             }
            else
             {
               OFPCommandLine.push_back(get_sourceFileNameWithPath());
             }

#if 0
          printf ("exit_after_parser: OFPCommandLine = %s \n",StringUtility::listToString(OFPCommandLine).c_str());
#endif
#if 1
       // Some security checking here could be helpful!!!
          int errorCode = systemFromVector (OFPCommandLine);

       // DQ (9/30/2008): Added error checking of return value
          if (errorCode != 0)
             {
               printf ("Using option -rose:exit_after_parser (errorCode = %d) \n",errorCode);
               ROSE_ASSERT(false);
             }
#else

// #error "REMOVE THIS CODE"

       // This fails, I think because we can't call the openFortranParser_main twice.
          int openFortranParser_only_argc    = 0;
          char** openFortranParser_only_argv = NULL;
          CommandlineProcessing::generateArgcArgvFromList(OFPCommandLine,openFortranParser_only_argc,openFortranParser_only_argv);
       // frontendErrorLevel = openFortranParser_main (openFortranParser_only_argc, openFortranParser_only_argv);
          int errorCode = openFortranParser_main (openFortranParser_only_argc, openFortranParser_only_argv);

#endif
          printf ("Skipping all processing after parsing fortran (OFP) ... (get_exit_after_parser() == true) errorCode = %d \n",errorCode);
       // exit(0);

          ROSE_ASSERT(errorCode == 0);
          return errorCode;
       }

  // DQ (1/19/2008): New version of OFP requires different calling syntax; new lib name is: libfortran_ofp_parser_java_FortranParserActionJNI.so old name: libparser_java_FortranParserActionJNI.so
  // frontEndCommandLineString = std::string(argv[0]) + " --class parser.java.FortranParserActionJNI " + get_sourceFileNameWithPath();
     vector<string> frontEndCommandLine;

     frontEndCommandLine.push_back(argv[0]);
  // frontEndCommandLine.push_back(classpath);
     frontEndCommandLine.push_back("--class");
     frontEndCommandLine.push_back("fortran.ofp.parser.c.jni.FortranParserActionJNI");

     //FMZ (7/26/2010)  added an option for using rice CAF.
     if (using_rice_caf==true)
          frontEndCommandLine.push_back("--RiceCAF");

#if 0
  // Debugging output
     get_project()->display("Calling SgProject display");
     display("Calling SgFile display");
#endif

     const SgStringList & includeList = get_project()->get_includeDirectorySpecifierList();

     bool foundSourceDirectoryExplicitlyListedInIncludePaths = false;

  // printf ("getSourceDirectory() = %s \n",getSourceDirectory().c_str());
     for (size_t i = 0; i < includeList.size(); i++)
        {
          frontEndCommandLine.push_back(includeList[i]);

       // printf ("includeList[%d] = %s \n",i,includeList[i].c_str());

       // I think we have to permit an optional space between the "-I" and the path
          if (  "-I" + getSourceDirectory() == includeList[i] || "-I " + getSourceDirectory() == includeList[i])
             {
            // The source file path is already included!
               foundSourceDirectoryExplicitlyListedInIncludePaths = true;
             }
        }

#if 0
     printf ("foundSourceDirectoryExplicitlyListedInIncludePaths = %s \n",foundSourceDirectoryExplicitlyListedInIncludePaths ? "true" : "false");
#endif
     if (foundSourceDirectoryExplicitlyListedInIncludePaths == false)
        {
       // Add the source directory to the include list so that we reproduce the semantics of gfortran
          frontEndCommandLine.push_back("-I" + getSourceDirectory() );
        }

  // DQ (5/19/2008): Support for C preprocessing
     if (requires_C_preprocessor == true)
        {
       // If C preprocessing was required then we have to provide the generated filename of the preprocessed file!

       // Note that the filename referenced in the Sg_File_Info objects will use the original file name and not
       // the generated file name of the CPP generated file.  This is because it gets the filename from the
       // SgSourceFile IR node and not from the filename provided on the internal command line generated for call OFP.

          string sourceFilename              = get_sourceFileNameWithPath();
          string sourceFileNameOutputFromCpp = generate_C_preprocessor_intermediate_filename(sourceFilename);

          frontEndCommandLine.push_back(sourceFileNameOutputFromCpp);
        }
       else
        {
       // If not being preprocessed, the fortran filename is just the original input source file name.
          frontEndCommandLine.push_back(get_sourceFileNameWithPath());
        }

#if 1
     if ( get_verbose() > 0 )
          printf ("Fortran numberOfCommandLineArguments = %zu frontEndCommandLine = %s \n",frontEndCommandLine.size(),CommandlineProcessing::generateStringFromArgList(frontEndCommandLine,false,false).c_str());
#endif

#if 0
     frontEndCommandLine.push_back("--tokens");
#endif

     if (get_unparse_tokens() == true)
        {
       // Note that this will cause all other c_actions to not be executed (resulting in an empty file).
       // So this makes since to run in an analysis mode only, not for generation of code or compiling
       // of the generated code.
          frontEndCommandLine.push_back("--tokens");
        }

     int openFortranParser_argc    = 0;
     char** openFortranParser_argv = NULL;
     CommandlineProcessing::generateArgcArgvFromList(frontEndCommandLine,openFortranParser_argc,openFortranParser_argv);

  // printf ("openFortranParser_argc = %d openFortranParser_argv = %s \n",openFortranParser_argc,CommandlineProcessing::generateStringFromArgList(openFortranParser_argv,false,false).c_str());

  // DQ (8/19/2007): Setup the global pointer used to pass the SgFile to which the Open Fortran Parser
  // should attach the AST.  This is a bit ugly, but the parser interface only takes a commandline so it
  // would be more ackward to pass a pointer to a C++ object through the commandline or the Java interface.
     OpenFortranParser_globalFilePointer = const_cast<SgSourceFile*>(this);
     ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);

     if ( get_verbose() > 1 )
          printf ("Calling openFortranParser_main(): OpenFortranParser_globalFilePointer = %p \n",OpenFortranParser_globalFilePointer);

#if USE_ROSE_SSL_SUPPORT
  // The use of the JVM required to support Java is a problem when linking to the SSL library (either -lssl or -lcrypto)
  // this may be fixed in Java version 6, but this is a hope, it has not been tested.  Java version 6 does
  // appear to fix the problem with zlib (we think) and this appears to be a similar problem).
     int frontendErrorLevel = 1;

     printf ("********************************************************************************************** \n");
     printf ("Fortran support using the JVM is incompatable with the use of the SSL library (fails in jvm).  \n");
     printf ("To enable the use of Fortran support in ROSE don't use --enable-ssl on configure command line. \n");
     printf ("********************************************************************************************** \n");
#else

  // DQ (11/11/2010): There should be no include files on the stack from previous files, see test2010_78.C and test2010_79.C when
  // compiled together on the same command line.
     ROSE_ASSERT(astIncludeStack.size() == 0);

  // DQ (6/7/2013): Added support for call the experimental frontran frontend (if the associated option is specified on the command line).
  // frontendErrorLevel = openFortranParser_main (numberOfCommandLineArguments, inputCommandLine);
  // int frontendErrorLevel = openFortranParser_main (openFortranParser_argc, openFortranParser_argv);
     int frontendErrorLevel = 0;
     if (get_experimental_fortran_frontend() == true)
        {
          vector<string> experimentalFrontEndCommandLine;

       // Push an initial argument onto the command line stack so that the command line can be interpreted 
       // as coming from an command shell command line (where the calling program is always argument zero).
          experimentalFrontEndCommandLine.push_back("dummyArg_0");

          string parseTableOption = "--parseTable";
          experimentalFrontEndCommandLine.push_back(parseTableOption);

       // string path_to_table = findRoseSupportPathFromSource("src/3rdPartyLibraries/experimental-fortran-parser/Fortran.tbl", "bin/Fortran.tbl");
          string path_to_table = findRoseSupportPathFromBuild("src/3rdPartyLibraries/experimental-fortran-parser/Fortran.tbl", "bin/Fortran.tbl");

          experimentalFrontEndCommandLine.push_back(path_to_table);

          experimentalFrontEndCommandLine.push_back(get_sourceFileNameWithPath());

       // experimentalFrontEndCommandLine.push_back(get_sourceFileNameWithoutPath());

          int experimental_openFortranParser_argc    = 0;
          char** experimental_openFortranParser_argv = NULL;
          CommandlineProcessing::generateArgcArgvFromList(experimentalFrontEndCommandLine,experimental_openFortranParser_argc,experimental_openFortranParser_argv);

          printf ("Calling the experimental fortran frontend (this work is incomplete) \n");
          printf ("   --- Fortran numberOfCommandLineArguments = %zu frontEndCommandLine = %s \n",experimentalFrontEndCommandLine.size(),CommandlineProcessing::generateStringFromArgList(experimentalFrontEndCommandLine,false,false).c_str());
          frontendErrorLevel = experimental_openFortranParser_main (experimental_openFortranParser_argc, experimental_openFortranParser_argv);
          printf ("DONE: Calling the experimental fortran frontend (this work is incomplete) \n");
        }
       else
        {
          frontendErrorLevel = openFortranParser_main (openFortranParser_argc, openFortranParser_argv);
        }
     

  // DQ (11/11/2010): There should be no include files left in the stack, see test2010_78.C and test2010_79.C when
  // compiled together on the same command line.
  // ROSE_ASSERT(astIncludeStack.size() == 0);
     if (astIncludeStack.size() != 0)
        {
          printf ("Warning: astIncludeStack not cleaned up after openFortranParser_main(): astIncludeStack.size() = %zu \n",astIncludeStack.size());
        }
#endif

     if ( get_verbose() > 1 )
          printf ("DONE: Calling the openFortranParser_main() function (which loads the JVM) \n");

  // Reset this global pointer after we are done (just to be safe and avoid it being used later and causing strange bugs).
     OpenFortranParser_globalFilePointer = NULL;

  // Now read the CPP directives such as "# <number> <filename> <optional numeric code>", in the generated file from CPP.
     if (requires_C_preprocessor == true)
        {
       // If this was part of the processing of a CPP generated file then read the preprocessed file
       // to get the CPP directives (different from those of the original *.F?? file) which will
       // indicate line numbers and files where text was inserted as part of CPP processing.  We
       // mostly want to read CPP declarations of the form "# <number> <filename> <optional numeric code>".
       // these are the only directives that will be in the CPP generated file (as I recall).

          string sourceFilename              = get_sourceFileNameWithPath();
          string sourceFileNameOutputFromCpp = generate_C_preprocessor_intermediate_filename(sourceFilename);
#if 0
          printf ("Need to preprocess the CPP generated fortran file so that we can attribute source code statements to files: sourceFileNameOutputFromCpp = %s \n",sourceFileNameOutputFromCpp.c_str());
#endif
#if 1
       // Note that the collection of CPP linemarker directives from the CPP generated file
       // (and their insertion into the AST), should be done before the collection and
       // insertion of the Comments and more general CPP directives from the original source
       // file and their insertion. This will allow the correct representation of source
       // position information to be used in the final insertion of comments and CPP directives
       // from the original file.

#if 0
       // DQ (12/19/2008): This is now done by the AttachPreprocessingInfoTreeTrav

       // List of all comments and CPP directives (collected from the generated CPP file so that we can collect the
       // CPP directives that indicate source file boundaries of included regions of source code.
       // E.g. # 42 "foobar.f" 2
          ROSEAttributesList* currentListOfAttributes = new ROSEAttributesList();
          ROSE_ASSERT(currentListOfAttributes != NULL);

       // DQ (11/28/2008): This will collect the CPP directives from the generated CPP file so that
       // we can associate parts of the AST included from different files with the correct file.
       // Without this processing all the parts of the AST will be associated with the same generated file.
          currentListOfAttributes->collectPreprocessorDirectivesAndCommentsForAST(sourceFileNameOutputFromCpp,ROSEAttributesList::e_C_language);
#endif

#if 0
          printf ("Secondary pass over Fortran source file = %s to comment comments and CPP directives (might still be referencing the original source file) \n",sourceFileNameOutputFromCpp.c_str());
          printf ("Calling attachPreprocessingInfo() \n");
#endif

          attachPreprocessingInfo(this);

       // printf ("DONE: calling attachPreprocessingInfo() \n");

       // DQ (12/19/2008): Now we have to do an analysis of the AST to interpret the linemarkers.
          processCppLinemarkers();

#endif
#if 0
          printf ("Exiting as a test ... (collect the CPP directives from the CPP generated file after building the AST)\n");
          ROSE_ASSERT(false);
#endif
        }

  // printf ("######################### Leaving SgSourceFile::build_Fortran_AST() ############################ \n");


  // DQ (10/26/2010): Moved from SgSourceFile::callFrontEnd() so that the stack will
  // be empty when processing Java language support (not Fortran).
     delete  currStks;
     currStks = NULL;

     return frontendErrorLevel;
#else
     fprintf(stderr, "Fortran parser not supported \n");
     ROSE_ASSERT(false);

     return -1;
#endif
   }

#ifdef ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
namespace Rose {
namespace Frontend {
namespace Java {
namespace Ecj {
  // TOO1 (2/13/2014): Declared in src/frontend/ECJ_ROSE_Connection/openJavaParser_main.C.
  extern SgSourceFile* Ecj_globalFilePointer;
}// ::Rose::Frontend::Java::Ecj
}// ::Rose::Frontend::Java
}// ::Rose::Frontend
}// ::Rose
#endif

int
SgSourceFile::build_Java_AST( vector<string> argv, vector<string> inputCommandLine )
   {
     if (this -> get_package() != NULL || this -> attributeExists("error")) { // Has this file been processed already? If so, ignore it.
        return 0;
     }

#ifdef ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
     ROSE_ASSERT(get_requires_C_preprocessor() == false);

 // If the classpath was specified, add it to the list of options here.
   string classpath = "";
   list<string> classpath_list = get_project()->get_Java_classpath();
   if (classpath_list.size()) {
       list<string>::iterator i = classpath_list.begin();
       classpath = (*i);
       for (i++; i != classpath_list.end(); i++) {
           classpath += ":";
           classpath += (*i);
       }
   }

   string sourcepath = "";
   list<string> sourcepath_list = get_project()->get_Java_sourcepath();
   if (sourcepath_list.size()) {
       list<string>::iterator i = sourcepath_list.begin();
       sourcepath = (*i);
       for (i++; i != sourcepath_list.end(); i++) {
           sourcepath += ":";
           sourcepath += (*i);
       }
   }

   // Extract java's rose arguments
   //TODO better handling of -rose:java variants
   Rose_STL_Container<string> javaRoseOptionList =
               CommandlineProcessing::generateOptionListWithDeclaredParameters(argv,"-rose:java:");

   string sourceString;
   if (!CommandlineProcessing::isOptionWithParameter(javaRoseOptionList, "", "source", sourceString, false)) {
       sourceString = "1.6";
   }

   string targetString;
   if (!CommandlineProcessing::isOptionWithParameter(javaRoseOptionList, "", "target", targetString, false)) {
       targetString = "1.6";
   }

#if 0
     printf ("Exiting as a test: SgSourceFile::build_Java_AST() \n");
     ROSE_ASSERT(false);
#endif

   // *******************************************************
   // Build syntax checking command line call
   // *******************************************************

  // DQ (9/19/2011): This is what I thought, but it does not appear to be true or we tie into ECJ at the
  // the wrong location so that we don't get any syntax checking.  So we need to introduce a pass using
  // the backend compiler to support the syntax checking for Java.
  // DQ (10/11/2010): We don't need syntax checking because ECJ will do that.
     bool syntaxCheckInputCode = (get_skip_syntax_check() == false);
     if (syntaxCheckInputCode == true)
        {
       // Introduce tracking of performance of ROSE.
          TimingPerformance timer ("Java syntax checking of input:");

       // DQ (9/19/2011): For Java we want to run the backend javacc compiler to do the syntax checking.
          string backendJavaCompiler = BACKEND_JAVA_COMPILER_NAME_WITH_PATH;
          if ( get_verbose() > 2 )
             {
               printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Setting up Java Syntax check @@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
               printf ("BACKEND_JAVA_COMPILER_NAME_WITH_PATH = %s \n",backendJavaCompiler.c_str());
             }
       // Start building the javac command line
          vector<string> javaCommandLine;
          javaCommandLine.push_back(backendJavaCompiler);

       // DQ (7/20/2011):Each part of the option for "-sourcepath <path>" or "-d <path>"
       // must be pushed onto the javaCommandLine separately.
          if ( get_verbose() > 2 )
               javaCommandLine.push_back("-verbose");

       // Always push the source information
          javaCommandLine.push_back("-source");
          javaCommandLine.push_back(sourceString);

       // We invoke javac to check the syntax of the input program
       // Since it doesn generates classes, we create a separate folder
       // just to make sure this gets contained somewhere.
          string backendClassOutput = "javac-syntax-check-classes";
          javaCommandLine.push_back("-d");
          javaCommandLine.push_back(backendClassOutput);
          if(mkdir(backendClassOutput.c_str(),0777)==-1) {
                  if(errno != EEXIST) {
                          printf ("Can't create destination folder for syntax checking\n");
                          ROSE_ASSERT(false);
                  }
          }

          if (classpath.size()) {
              javaCommandLine.push_back("-classpath");
              javaCommandLine.push_back(classpath);
          }

          if (sourcepath.size()) {
              javaCommandLine.push_back("-sourcepath");
              javaCommandLine.push_back(sourcepath);
          }

          // Specify warnings for javac compiler.
          if (backendJavaCompiler == "javac") {
              if (get_output_warnings() == true) {
                  javaCommandLine.push_back("-Xlint");
              } else {
                  javaCommandLine.push_back("-Xlint:none");
              }
              javaCommandLine.push_back("-proc:none");
          }
          else
          {
              printf ("Currently only the javac compiler backend is supported backendCompilerSystem = %s \n",backendJavaCompiler.c_str());
              ROSE_ASSERT(false);
          }

          javaCommandLine.push_back(get_sourceFileNameWithPath());

       // At this point we have the full command line with the source file name
          if (get_verbose() > 1)
             {
               printf ("Checking syntax of input program using javac: syntaxCheckingCommandline = %s \n",CommandlineProcessing::generateStringFromArgList(javaCommandLine,false,false).c_str());
             }

          int returnValueForSyntaxCheckUsingBackendCompiler = 0;
          // Call the OS with the commandline defined by: syntaxCheckingCommandline
          returnValueForSyntaxCheckUsingBackendCompiler = systemFromVector (javaCommandLine);

       // Check that there are no errors, I think that warnings are ignored!
          if (returnValueForSyntaxCheckUsingBackendCompiler != 0)
             {
               printf ("Syntax errors detected in input java program ... status = %d \n",returnValueForSyntaxCheckUsingBackendCompiler);

// TODO: Remove this!  PC 07/03/2013
//            // We should define some convention for error codes returned by ROSE
//               throw std::exception();
                 return returnValueForSyntaxCheckUsingBackendCompiler;
             }
// TODO: Remove this!  PC 07/03/2013
//          ROSE_ASSERT(returnValueForSyntaxCheckUsingBackendCompiler == 0);

          if ( get_verbose() > 2 )
             {
               printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@ DONE: Setting up Java Syntax check @@@@@@@@@@@@@@@@@@@@@@@@@ \n");
             }
        }

  // *******************************************************
     if (get_output_parser_actions() == true)
        {
          printf("Sorry, not implemented: output_parser_actions option is not supported for Java yet. \n");
          ROSE_ASSERT(false);
        }

  // Option to just run the parser (not constructing the AST) and quit.
     if (get_exit_after_parser() == true)
        {
          printf("Sorry, not implemented: get_exit_after_parser option is not supported for Java yet. \n");
          ROSE_ASSERT(false);
        }

     // *******************************************************
     // Build the call to the ECJ frontend
     // *******************************************************

  // Note that for the ECJ JVM support, the filename must appear last on the command line.
  // I think it is only an intermediate test before actually calling ECJ that requires 
  // this so it could be relaxed with a bit of work.
     vector<string> frontEndCommandLine;
     frontEndCommandLine.push_back(argv[0]);

  // Added an option to the ECJ command line to support different levels of output from the Java side of the house.
     string verboseOptionString = "-rose:verbose " + StringUtility::numberToString(SgProject::get_verbose());
     if (SgProject::get_verbose() > 0)
        {
          printf ("Debugging option to Java ECJ translation (java code) to build ROSE AST: verboseOptionString = %s \n",verboseOptionString.c_str());
        }
     frontEndCommandLine.push_back(verboseOptionString);

  // *******************************************************************
  // Handle java rose options we need to transmit to ecj
  // !! Warning !! ECJ does not accept '--' prefixed options.
  // *******************************************************************
     frontEndCommandLine.push_back("-source");
     frontEndCommandLine.push_back(sourceString);

     frontEndCommandLine.push_back("-target");
     frontEndCommandLine.push_back(targetString);

         if (!get_output_warnings()) {
                 frontEndCommandLine.push_back("-nowarn");
         }

     // This is to specify where ecj should output the .class it is temporarily generating.
     // Check if -decj has already been provided, if not default to a unique name consisting
     // of the prefix "ecj-classes" with the suffix "-" + source_file_name,  where source_file_name
     // is the simple name of the input source file without the ".java" suffix.
     string ecjDestDir;
         if (!CommandlineProcessing::isOptionWithParameter(javaRoseOptionList, "", "decj", ecjDestDir, false)) {
             string full_file_name = get_sourceFileNameWithPath();
             int last_slash = full_file_name.find_last_of("/\\");
             string file_name = full_file_name.substr(last_slash + 1);
             int dot = file_name.find_last_of(".");
             if (dot != -1) {
                 file_name = file_name.substr(0, dot);
             }

             ecjDestDir = "ecj-classes-" + file_name + "/";
         }
         frontEndCommandLine.push_back("-d");
         frontEndCommandLine.push_back(ecjDestDir);

     // Setup the classpath and append the classes folder ecj outputs its temporary files to
     // Note: it is important to append since we do not want to override any user provided paths
         frontEndCommandLine.push_back("-classpath");
         frontEndCommandLine.push_back((classpath.size() > 0 ? (classpath + ":") : "") + ecjDestDir);

         if (sourcepath.size()) {
             frontEndCommandLine.push_back("-sourcepath");
             frontEndCommandLine.push_back(sourcepath);
         }

  // Java does not use include files, so we can enforce this.
     ROSE_ASSERT(get_project()->get_includeDirectorySpecifierList().empty() == true);

  // Add the source file as the last argument on the command line (checked by intermediate testing before calling ECJ).
     frontEndCommandLine.push_back(get_sourceFileNameWithPath());

     if ( get_verbose() > 0 )
          printf ("Java numberOfCommandLineArguments = %zu frontEndCommandLine = %s \n",inputCommandLine.size(),CommandlineProcessing::generateStringFromArgList(frontEndCommandLine,false,false).c_str());

     int ecjArgc = 0;
     char** ecjArgv = NULL;
     CommandlineProcessing::generateArgcArgvFromList(frontEndCommandLine,ecjArgc,ecjArgv);

  // DQ (8/19/2007): Setup the global pointer used to pass the SgFile to which the Open Fortran Parser
  // should attach the AST.  This is a bit ugly, but the parser interface only takes a commandline so it
  // would be more ackward to pass a pointer to a C++ object through the commandline or the Java interface.
     Rose::Frontend::Java::Ecj::Ecj_globalFilePointer = const_cast<SgSourceFile*>(this);
     ROSE_ASSERT(Rose::Frontend::Java::Ecj::Ecj_globalFilePointer != NULL);

     if ( get_verbose() > 0 )
          printf ("Calling ecj_main(): Ecj_globalFilePointer = %p \n", Rose::Frontend::Java::Ecj::Ecj_globalFilePointer);

#if USE_ROSE_SSL_SUPPORT
  // The use of the JVM required to support Java is a problem when linking to the SSL library (either -lssl or -lcrypto)
  // this may be fixed in Java version 6, but this is a hope, it has not been tested.  Java version 6 does
  // appear to fix the problem with zlib (we think) and this appears to be a similar problem).
     int frontendErrorLevel = 1;
     printf ("********************************************************************************************** \n");
     printf ("Java support using the JVM is incompatable with the use of the SSL library (fails in jvm).  \n");
     printf ("To enable the use of Java support in ROSE don't use --enable-ssl on configure command line. \n");
     printf ("********************************************************************************************** \n");
#else
     int frontendErrorLevel = openJavaParser_main (ecjArgc, ecjArgv);
#endif

     if ( get_verbose() > 0 )
          printf ("DONE: Calling the openFortranParser_main() function (which loads the JVM) \n");

  // Reset this global pointer after we are done (just to be safe and avoid it being used later and causing strange bugs).
     Rose::Frontend::Java::Ecj::Ecj_globalFilePointer = NULL;

     return frontendErrorLevel;
#else
     fprintf(stderr, "Java language parser not supported \n");
     ROSE_ASSERT(false);
     return -1;
#endif
   }

int
SgSourceFile::build_X10_AST(const vector<string>& p_argv)
{
  #ifndef ROSE_BUILD_X10_LANGUAGE_SUPPORT
      ROSE_ASSERT (!
          "[FATAL] [ROSE] [frontend] [X10] "
          "ROSE was not configured to support the X10 frontend, see --help.");
  #else
      if (SgProject::get_verbose() >= 1)
          std::cout << "[INFO] Building the X10 AST" << std::endl;
  #endif

  int argc = p_argv.size();
  char** argv = NULL;

  CommandlineProcessing::
      generateArgcArgvFromList(p_argv, argc, argv);

  int status = x10_main(argc, argv);
  return status;
}

namespace SgSourceFile_processCppLinemarkers
   {
  // This class (AST traversal) supports the traversal of the AST required
  // to translate the source position using the CPP linemarkers.

     class LinemarkerTraversal : public AstSimpleProcessing
        {
          public:
           // list<PreprocessingInfo*> preprocessingInfoStack;
              list< pair<int,int> > sourcePositionStack;

              LinemarkerTraversal( const string & sourceFilename );

              void visit ( SgNode* astNode );
        };
   }


SgSourceFile_processCppLinemarkers::LinemarkerTraversal::LinemarkerTraversal( const string & sourceFilename )
   {
  // Build an initial element on the stack so that the original source file name will be used to
  // set the global scope (which will be traversed before we visit any statements that might have
  // CPP directives attached (or which are CPP direcitve IR nodes).

  // Get the fileId of the assocated filename
     int fileId = Sg_File_Info::getIDFromFilename(sourceFilename);

  // Assume this is line 1 (I forget why zero is not a great idea here,
  // I expect that it causes a consstancy test to fail somewhere).
     int line = 1;

     if ( SgProject::get_verbose() > 1 )
          printf ("In LinemarkerTraversal::LinemarkerTraversal(): Push initial stack entry for line = %d fileId = %d sourceFilename = %s \n",line,fileId,sourceFilename.c_str());

  // Push an entry onto the stack before doing the traversal over the whole AST.
     sourcePositionStack.push_front( pair<int,int>(line,fileId) );
   }


#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
     extern SgSourceFile* OpenFortranParser_globalFilePointer;
#endif

void
SgSourceFile_processCppLinemarkers::LinemarkerTraversal::visit ( SgNode* astNode )
   {
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

    // DXN (02/21/2011): Consider the case of SgInterfaceBody.
    // TODO: revise SgInterfaceBody::get_numberOfTraversalSuccessor() to return 1 and
    // TODO: revise SgInterfaceBody::get_traversalSuccessorByIndex(int ) to return p_functionDeclaration
    // Such changes will require some re-write of the code to build SgInterfaceBody.
    // With such changes, the patch below to treat the case of SgInterfaceBody will no longer be necessary.
     SgInterfaceBody* interfaceBody = isSgInterfaceBody(astNode);
     if (interfaceBody)
        {
          AstSimpleProcessing::traverse(interfaceBody->get_functionDeclaration(), preorder);
        }

     SgStatement* statement = isSgStatement(astNode);
  // printf ("LinemarkerTraversal::visit(): statement = %p = %s \n",statement,(statement != NULL) ? statement->class_name().c_str() : "NULL");
     if (statement != NULL)
        {
          if ( SgProject::get_verbose() > 1 )
               printf ("LinemarkerTraversal::visit(): statement = %p = %s \n",statement,statement->class_name().c_str());

          AttachedPreprocessingInfoType *commentOrDirectiveList = statement->getAttachedPreprocessingInfo();

          if ( SgProject::get_verbose() > 1 )
               printf ("LinemarkerTraversal::visit(): commentOrDirectiveList = %p (size = %zu) \n",commentOrDirectiveList,(commentOrDirectiveList != NULL) ? commentOrDirectiveList->size() : 0);

          if (commentOrDirectiveList != NULL)
             {
               AttachedPreprocessingInfoType::iterator i = commentOrDirectiveList->begin();
               while(i != commentOrDirectiveList->end())
                  {
                    if ( (*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorCompilerGeneratedLinemarker )
                       {
                      // This is a CPP linemarker
                         int line = (*i)->get_lineNumberForCompilerGeneratedLinemarker();

                      // DQ (12/23/2008): Note this is a quoted name, we need the unquoted version!
                         std::string quotedFilename = (*i)->get_filenameForCompilerGeneratedLinemarker();
                         ROSE_ASSERT(quotedFilename[0] == '\"');
                         ROSE_ASSERT(quotedFilename[quotedFilename.length()-1] == '\"');
                         std::string filename = quotedFilename.substr(1,quotedFilename.length()-2);

                         std::string options  = (*i)->get_optionalflagsForCompilerGeneratedLinemarker();

                      // Add the new filename to the static map stored in the Sg_File_Info (no action if filename is already in the map).
                         Sg_File_Info::addFilenameToMap(filename);

                         int fileId = Sg_File_Info::getIDFromFilename(filename);

                         if ( SgProject::get_verbose() > 1 )
                              printf ("line = %d fileId = %d quotedFilename = %s filename = %s options = %s \n",line,fileId,quotedFilename.c_str(),filename.c_str(),options.c_str());

                      // Just record the first linemarker so that we can test getting the filename correct.
                         if (line == 1 && sourcePositionStack.empty() == true)
                            {
                              sourcePositionStack.push_front( pair<int,int>(line,fileId) );
                            }
                       }

                    i++;
                  }
             }

       // ROSE_ASSERT(sourcePositionStack.empty() == false);
          if (sourcePositionStack.empty() == false)
             {
               int line   = sourcePositionStack.front().first;
               int fileId = sourcePositionStack.front().second;

               if ( SgProject::get_verbose() > 1 )
                    printf ("Setting the source position of statement = %p = %s to line = %d fileId = %d \n",statement,statement->class_name().c_str(),line,fileId);

            // DXN (02/18/2011): only reset the file id for the node whose file id corresponds to the "_postprocessed" file.
               string sourceFilename              = Sg_File_Info::getFilenameFromID(fileId) ;
               string sourceFileNameOutputFromCpp = OpenFortranParser_globalFilePointer->generate_C_preprocessor_intermediate_filename(sourceFilename);
               int cppFileId = Sg_File_Info::getIDFromFilename(sourceFileNameOutputFromCpp);
               if (statement->get_file_info()->get_file_id() == cppFileId)
                  {
                   statement->get_file_info()->set_file_id(fileId);
                  }
            // statement->get_file_info()->set_line(line);

               if ( SgProject::get_verbose() > 1 )
                    Sg_File_Info::display_static_data("Setting the source position of statement");

               string filename = Sg_File_Info::getFilenameFromID(fileId);

               if ( SgProject::get_verbose() > 1 )
                  {
                    printf ("filename = %s \n",filename.c_str());
                    printf ("filename = %s \n",statement->get_file_info()->get_filenameString().c_str());
                  }

               ROSE_ASSERT(statement->get_file_info()->get_filename() != NULL);
               ROSE_ASSERT(statement->get_file_info()->get_filenameString().empty() == false);
             }
        }
#else  // ! ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
     // SKW: not called except from Fortran
     printf(">>> SgSourceFile_processCppLinemarkers::LinemarkerTraversal::visit is not implemented for languages other than Fortran\n");
     ROSE_ASSERT(false);
#endif //   ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   }


void
SgSourceFile::processCppLinemarkers()
   {
     SgSourceFile* sourceFile = const_cast<SgSourceFile*>(this);

     SgSourceFile_processCppLinemarkers::LinemarkerTraversal linemarkerTraversal(sourceFile->get_sourceFileNameWithPath());

     linemarkerTraversal.traverse(sourceFile,preorder);

#if 0
     printf ("Exiting as a test ... (processing linemarkers)\n");
     ROSE_ASSERT(false);
#endif
   }

int
SgSourceFile::build_C_and_Cxx_AST( vector<string> argv, vector<string> inputCommandLine )
   {
     std::string frontEndCommandLineString;
     frontEndCommandLineString = std::string(argv[0]) + std::string(" ") + CommandlineProcessing::generateStringFromArgList(inputCommandLine,false,false);

     if ( get_verbose() > 1 )
          printf ("Before calling edg_main: frontEndCommandLineString = %s \n",frontEndCommandLineString.c_str());

     int c_cxx_argc = 0;
     char **c_cxx_argv = NULL;
     CommandlineProcessing::generateArgcArgvFromList(inputCommandLine, c_cxx_argc, c_cxx_argv);

#ifdef ROSE_BUILD_CXX_LANGUAGE_SUPPORT
  // This is the function call to the EDG front-end (modified in ROSE to pass a SgFile)

#if 0
       // If this was selected as an option then we can stop here (rather than call OFP again).
       // printf ("--- get_exit_after_parser() = %s \n",get_exit_after_parser() ? "true" : "false");
          if (get_exit_after_parser() == true)
             {
               printf ("Exiting after parsing... \n");
               exit(0);
             }
#endif

#ifdef ROSE_USE_CLANG_FRONTEND
     int clang_main(int, char *[], SgSourceFile & sageFile );
     int frontendErrorLevel = clang_main (c_cxx_argc, c_cxx_argv, *this);
#else /* default to EDG */
     int edg_main(int, char *[], SgSourceFile & sageFile );
     int frontendErrorLevel = edg_main (c_cxx_argc, c_cxx_argv, *this);
#endif /* clang or edg */

#else
     int frontendErrorLevel = 99;
     ROSE_ASSERT (! "[FATAL] [ROSE] [frontend] [C/C++] "
                    "ROSE was not configured to support the C/C++ frontend.");
#endif

     return frontendErrorLevel;
   }

int
SgSourceFile::build_PHP_AST()
   {
     string phpFileName = this->get_sourceFileNameWithPath();
#ifdef _MSC_VER
#pragma message ("WARNING: PHP not supported within initial MSVC port of ROSE.")
         printf ("WARNING: PHP not supported within initial MSVC port of ROSE.");
         ROSE_ASSERT(false);

         int frontendErrorLevel = -1;
#else
#ifdef ROSE_BUILD_PHP_LANGUAGE_SUPPORT
     int frontendErrorLevel = php_main(phpFileName, this);
#else
     int frontendErrorLevel = 99;
     ROSE_ASSERT (! "[FATAL] [ROSE] [frontend] [PHP] "
                    "ROSE was not configured to support the PHP frontend.");
#endif
#endif
     return frontendErrorLevel;
   }

int
SgSourceFile::build_Python_AST()
   {
     string pythonFileName = this->get_sourceFileNameWithPath();
#ifdef ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT
     int frontendErrorLevel = python_main(pythonFileName, this);
#else
     int frontendErrorLevel = 99;
     ROSE_ASSERT (! "[FATAL] [ROSE] [frontend] [Python] "
                    "ROSE was not configured to support the Python frontend.");
#endif
     return frontendErrorLevel;
   }

/* Parses a single binary file and adds a SgAsmGenericFile node under this SgBinaryComposite node. */
void
SgBinaryComposite::buildAsmAST(string executableFileName)
   {
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
     if ( get_verbose() > 0 || SgProject::get_verbose() > 0)
          printf ("Disassemble executableFileName = %s \n",executableFileName.c_str());

  // Parse the binary container, but do not disassemble instructions yet.
     SgAsmGenericFile *file = SgAsmExecutableFileFormat::parseBinaryFormat(executableFileName.c_str());
     ROSE_ASSERT(file != NULL);

  // Attach the file to this node
     get_genericFileList()->get_files().push_back(file);
     file->set_parent(get_genericFileList());

  // Add a disassembly interpretation for each header. Actual disassembly will occur later.
  // NOTE: This probably isn't the right place to add interpretation nodes, but I'm leaving it here for the time being. We
  //       probably don't want an interpretation for each header if we're doing dynamic linking. [RPM 2009-09-17]
     const SgAsmGenericHeaderPtrList &headers = file->get_headers()->get_headers();
     for (size_t i = 0; i < headers.size(); ++i)
        {
          SgAsmInterpretation* interp = new SgAsmInterpretation();
          get_interpretations()->get_interpretations().push_back(interp);
          interp->set_parent(get_interpretations());
          interp->get_headers()->get_headers().push_back(headers[i]);
        }

#if USE_ROSE_DWARF_SUPPORT
  // DQ (3/14/2009): Dwarf support now works within ROSE when used with Intel Pin
  // (was a huge problem until everything (e.g. libdwarf) was dynamically linked).
  // DQ (11/7/2008): New Dwarf support in ROSE (Dwarf IR nodes are generated in the AST).
     readDwarf(file);
#endif

  // Make sure this node is correctly parented
     SgProject* project = SageInterface::getEnclosingNode<SgProject>(this);
     ROSE_ASSERT(project != NULL);
#else
     ROSE_ASSERT (! "[FATAL] [ROSE] [frontend] [Binary analysis] "
                    "ROSE was not configured to support the binary analysis frontend.");
#endif

#if 0
     printf ("At base of SgBinaryComposite::buildAsmAST(): exiting... \n");
     ROSE_ASSERT(false);
#endif
   }

/* Builds the entire AST under the SgBinaryComposite node:
 *    - figures out what binary files are needed
 *    - parses binary container of each file (SgAsmGenericFile nodes)
 *    - optionally disassembles instructions (SgAsmInterpretation nodes) */
int
SgBinaryComposite::buildAST(vector<string> /*argv*/, vector<string> /*inputCommandLine*/)
   {
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
    /* Parse the specified binary file to create the AST. Do not disassemble instructions yet. If the file is dynamically
     * linked then optionally load (i.e., parse the container, map sections into process address space, and perform relocation
     * fixups) all dependencies also.  See the BinaryLoader class for details. */
     if (get_isLibraryArchive()) {
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty() == false);
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty() == (get_isLibraryArchive() == false));

        for (size_t i = 0; i < get_libraryArchiveObjectFileNameList().size(); i++) {
            printf("Build binary AST for get_libraryArchiveObjectFileNameList()[%zu] = %s \n",
                    i, get_libraryArchiveObjectFileNameList()[i].c_str());
            string filename = get_libraryArchiveObjectFileNameList()[i];
            printf("Build SgAsmGenericFile from: %s \n", filename.c_str());
            buildAsmAST(filename);
        }
    } else {
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty());
        BinaryLoader::load(this, get_read_executable_file_format_only());
    }

    /* Disassemble each interpretation */
    if (!get_read_executable_file_format_only()) {
        const SgAsmInterpretationPtrList &interps = get_interpretations()->get_interpretations();
        for (size_t i=0; i<interps.size(); i++) {
            Partitioner::disassembleInterpretation(interps[i]);
        }
    }

    // DQ (1/22/2008): The generated unparsed assemble code can not currently be compiled because the
    // addresses are unparsed (see Jeremiah for details).
    // Skip running gnu assemble on the output since we include text that would make this a problem.
     if (get_verbose() > 1)
          printf("set_skipfinalCompileStep(true) because we are on a binary '%s'\n", this->get_sourceFileNameWithoutPath().c_str());

     this->set_skipfinalCompileStep(true);

    // This is now done below in the Secondary file processing phase.
    // Generate the ELF executable format structure into the AST
    // generateBinaryExecutableFileInformation(executableFileName,asmFile);
#else
     ROSE_ASSERT (! "[FATAL] [ROSE] [frontend] [Binary analysis] "
                    "ROSE was not configured to support the binary analysis frontend.");
#endif

     int frontendErrorLevel = 0;
     return frontendErrorLevel;
   }


#if 0
/* Builds the entire AST under the SgBinaryFile node:
 *    - figures out what binary files are needed
 *    - parses binary container of each file (SgAsmGenericFile nodes)
 *    - optionally disassembles instructions (SgAsmInterpretation nodes) */
int
SgBinaryFile::buildAST(vector<string> /*argv*/, vector<string> /*inputCommandLine*/)
{
    if (get_isLibraryArchive()) {
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty() == false);
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty() == (get_isLibraryArchive() == false));

        for (size_t i = 0; i < get_libraryArchiveObjectFileNameList().size(); i++) {
            printf("Build binary AST for get_libraryArchiveObjectFileNameList()[%zu] = %s \n",
                    i, get_libraryArchiveObjectFileNameList()[i].c_str());
            string filename = "tmp_objects/" + get_libraryArchiveObjectFileNameList()[i];
            printf("Build SgAsmGenericFile from: %s \n", filename.c_str());
            buildAsmAST(filename);
        }
    } else {
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty() == true);
        buildAsmAST(this->get_sourceFileNameWithPath());
    }

    /* Disassemble each interpretation */
    if (get_read_executable_file_format_only()) {
        printf ("\nWARNING: Skipping instruction disassembly \n\n");
    } else {
        const SgAsmInterpretationPtrList &interps = get_interpretations()->get_interpretations();
        for (size_t i=0; i<interps.size(); i++) {
            Partitioner::disassembleInterpretation(interps[i]);
        }
    }

    // DQ (1/22/2008): The generated unparsed assemble code can not currently be compiled because the
    // addresses are unparsed (see Jeremiah for details).
    // Skip running gnu assemble on the output since we include text that would make this a problem.
    if (get_verbose() > 1)
        printf("set_skipfinalCompileStep(true) because we are on a binary '%s'\n", this->get_sourceFileNameWithoutPath().c_str());
    this->set_skipfinalCompileStep(true);

    // This is now done below in the Secondary file processing phase.
    // Generate the ELF executable format structure into the AST
    // generateBinaryExecutableFileInformation(executableFileName,asmFile);

    int frontendErrorLevel = 0;
    return frontendErrorLevel;
}
#endif

int
SgSourceFile::buildAST( vector<string> argv, vector<string> inputCommandLine )
   {
  // printf ("######################## Inside of SgSourceFile::buildAST() ##########################\n");

  // DXN (01/10/2011): except for building C and Cxx AST, frontend fails when frontend error level > 0.
     int frontendErrorLevel = 0;
     bool frontend_failed = false;
     if (get_Fortran_only() == true)
        {
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
          frontendErrorLevel = build_Fortran_AST(argv,inputCommandLine);
          frontend_failed = (frontendErrorLevel > 1);  // DXN (01/18/2011): needed to pass make check.  TODO: need fixing up
#else
          ROSE_ASSERT (! "[FATAL] [ROSE] [frontend] [Fortran] "
                         "ROSE was not configured to support the Fortran frontend.");
#endif
        }
       else
        {
          if ( get_PHP_only() == true )
             {
               frontendErrorLevel = build_PHP_AST();
               frontend_failed = (frontendErrorLevel > 0);
             }
            else
             {
               if ( get_Java_only() == true )
                  {
#ifdef ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
                    frontendErrorLevel = build_Java_AST(argv,inputCommandLine);
                    this -> set_javacErrorCode(frontendErrorLevel);
                    frontendErrorLevel = 0; // PC: Always keep going for Java!
#else
                    ROSE_ASSERT (! "[FATAL] [ROSE] [frontend] [Java] "
                                   "ROSE was not configured to support the Java frontend.");
#endif
                  }
                 else
                  {
                      if ( get_Python_only() == true )
                         {
#ifdef ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT
                             frontendErrorLevel = build_Python_AST();
                             frontend_failed = (frontendErrorLevel > 0);
#else
                             ROSE_ASSERT (! "[FATAL] [ROSE] [frontend] [Python] "
                                            "ROSE was not configured to support the Python frontend.");
#endif

                         }
                      else
                      {
                          if (get_X10_only() == true)
                          {
                              #ifdef ROSE_BUILD_X10_LANGUAGE_SUPPORT
                                   frontendErrorLevel = build_X10_AST(argv);
                                   frontend_failed = (frontendErrorLevel > 0);
                              #else
                                   ROSE_ASSERT (! "[FATAL] [ROSE] [frontend] [X10] "
                                                  "ROSE was not configured to support the X10 frontend.");
                              #endif
                          }
                          else
                          {
                             frontendErrorLevel = build_C_and_Cxx_AST(argv,inputCommandLine);

                             // DQ (12/29/2008): The newer version of EDG (version 3.10 and 4.0) use different return codes for indicating an error.
#ifdef ROSE_USE_NEW_EDG_INTERFACE
                             // Any non-zero value indicates an error.
                             frontend_failed = (frontendErrorLevel != 0);
#else
                             // non-zero error code can mean warnings were produced, values greater than 3 indicate errors.
                             frontend_failed = (frontendErrorLevel > 3);
#endif
                          }
                      }
                  }
             }
        }

  // Uniform processing of the error code!

     if ( get_verbose() > 1 )
          printf ("DONE: frontend called (frontendErrorLevel = %d) \n",frontendErrorLevel);

  // If we had any errors reported by the frontend then quite now
     if (frontend_failed == true)
        {
       // cout << "Errors in Processing: (frontendErrorLevel > 3)" << endl;
          if ( get_verbose() > 1 )
               printf ("frontendErrorLevel = %d \n",frontendErrorLevel);

       // DQ (9/22/2006): We need to invert the test result (return code) for
       // negative tests (where failure is expected and success is an error).
          if (get_negative_test() == true)
             {
               if ( get_verbose() > 1 )
                  {
                    printf ("(evaluation of frontend results) This is a negative tests, so an error in compilation is a PASS but a successful \n");
                    printf ("compilation is not a FAIL since the faulure might happen in the compilation of the generated code by the vendor compiler. \n");
                  }
               exit(0);
             }
            else
             {
            // Exit because there are errors in the input program
               //cout << "Errors in Processing: (frontend_failed)" << endl;
               ROSE_ABORT("Errors in Processing: (frontend_failed)");
             }
        }

     return frontendErrorLevel;
   }

// DQ (10/14/2010): Removing reference to macros defined in rose_config.h (defined in the header file as a default parameter).
// int SgFile::compileOutput ( vector<string>& argv, int fileNameIndex, const string& compilerNameOrig )
int
SgFile::compileOutput ( vector<string>& argv, int fileNameIndex )
   {
  // DQ (7/12/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Object Code Generation (compile output):");

  // DQ (4/21/2006): I think we can now assert this!
     ROSE_ASSERT(fileNameIndex == 0);

#if 0
     printf ("\n\n***************************************************** \n");
     printf ("Inside of SgFile::compileOutput() \n");
     printf ("   --- get_unparse_output_filename() = %s \n",get_unparse_output_filename().c_str());
     printf ("***************************************************** \n\n\n");
#endif
#if 0
     display("In SgFile::compileOutput()");
#endif

  // This function does the final compilation of the unparsed file
  // Remaining arguments from the original compile time are used as well
  // as additional arguments added by the buildCompilerCommandLineOptions() function

  // DQ NOTE: This function has to be modified to compile more than
  //       just one file (as part of the multi-file support)
  // ROSE_ASSERT (sageProject.numberOfFiles() == 1);

  // ******************************************************************************
  // At this point in the control flow (for ROSE) we have returned from the processing
  // via the EDG frontend (or skipped it if that option was specified).
  // The following has been done or explicitly skipped if such options were specified
  // on the commandline:
  //    1) The application program has been parsed
  //    2) All AST's have been build (one for each grammar)
  //    3) The transformations have been edited into the C++ AST
  //    4) The C++ AST has been unparsed to form the final output file (all code has
  //       been generated into a different filename)
  // ******************************************************************************

  // What remains is to run the specified compiler (typically the C++ compiler) using
  // the generated output file (unparsed and transformed application code).
     int returnValueForRose = 0;

  // DQ (1/17/2006): test this
  // ROSE_ASSERT(get_fileInfo() != NULL);

  // DQ (4/2/2011): Added language specific support.
  // const string compilerNameOrig = BACKEND_CXX_COMPILER_NAME_WITH_PATH;
     string compilerNameOrig = BACKEND_CXX_COMPILER_NAME_WITH_PATH;
     if (get_Java_only() == true)
        {
          compilerNameOrig = BACKEND_JAVA_COMPILER_NAME_WITH_PATH;
        }

     if (get_Fortran_only() == true)
        {
       // printf ("Fortran language support in SgFile::compileOutput() not implemented \n");
       // ROSE_ASSERT(false);
          compilerNameOrig = BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH;
        }

    if (get_X10_only() == true)
    {
        compilerNameOrig = BACKEND_X10_COMPILER_NAME_WITH_PATH;
    }

  // BP : 11/13/2001, checking to see that the compiler name is set
     string compilerName = compilerNameOrig + " ";

  // DQ (4/21/2006): Setup the output file name.
  // Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argc,argv);
  // ROSE_ASSERT (fileList.size() == 1);
  // p_sourceFileNameWithPath    = *(fileList.begin());
  // p_sourceFileNameWithoutPath = ROSE::stripPathFromFileName(p_sourceFileNameWithPath.c_str());

#if 1
  // ROSE_ASSERT (get_unparse_output_filename().empty() == true);

  // DQ (4/21/2006): If we have not set the unparse_output_filename then we could not have called
  // unparse and we want to compile the original source file as a backup mechanism to generate an
  // object file.
  // printf ("In SgFile::compileOutput(): get_unparse_output_filename() = %s \n",get_unparse_output_filename().c_str());

  // TOO1 (05/14/2013): Handling for -rose:keep_going
  //
  // Compile the original source code file if:
  //
  // 1. Unparsing was skipped
  // 2. The frontend encountered any errors, and the user specified to
  //    "keep going" with -rose:keep_going.
  //
  //    Warning: Apparently, a frontend error code <= 3 indicates an EDG
  //    frontend warning; however, existing logic says nothing about the
  //    other language frontends' exit statuses.
     bool use_original_input_file = false;
     use_original_input_file = ( get_unparse_output_filename().empty() == true) || 
                               ( ( this->get_frontendErrorCode() != 0 || this->get_project()->get_midendErrorCode() != 0 || 
                                   this->get_unparserErrorCode() != 0 || this->get_backendCompilerErrorCode() != 0 ) && 
                                 ( get_project()->get_keep_going() ) );

  // TOO1 (05/14/2013): Handling for -rose:keep_going
  // Replace the unparsed file with the original input file.
     if (use_original_input_file)
        {
       // ROSE_ASSERT(get_skip_unparse() == true);
          string outputFilename = get_sourceFileNameWithPath();

       // DQ (9/15/2013): Added support for generated file to be placed into the same directory as the source file.
       // It's use here is similar to that in the unparse.C file, but less clear here that it is correct since we 
       // don't have tests of the -rose:keep_going option (that I know of directly in ROSE).
          SgProject* project = TransformationSupport::getProject(this);
       // ROSE_ASSERT(project != NULL);
          if (project != NULL)
             {
#if 0
               printf ("In SgFile::compileOutput(): project->get_unparse_in_same_directory_as_input_file() = %s \n",project->get_unparse_in_same_directory_as_input_file() ? "true" : "false");
#endif
               if (project->get_unparse_in_same_directory_as_input_file() == true)
                  {
                    outputFilename = ROSE::getPathFromFileName(get_sourceFileNameWithPath()) + "/rose_" + get_sourceFileNameWithoutPath();

                    printf ("In SgFile::compileOutput(): Using filename for unparsed file into same directory as input file: outputFilename = %s \n",outputFilename.c_str());

                    set_unparse_output_filename(outputFilename);
                  }
             }
            else
             {
               printf ("WARNING: In SgFile::compileOutput(): file = %p has no associated project \n",this);
             }

          if (get_unparse_output_filename().empty())
             {
               if (get_skipfinalCompileStep())
                  {
                  // nothing to do...
                  }
               else if ((! get_Java_only()) || this -> get_frontendErrorCode() == 0)
                  {
                 // DQ (7/14/2013): This is the branch taken when processing the -H option (which outputs the 
                 // header file list, and is required to be supported in ROSE as part of some application 
                 // specific configuration testing (when configure tests ROSE translators)).

                 // TOO1 (9/23/2013): There was never an else branch (or assertion) here before.
                 //                   Commenting out for now to allow $ROSE/tests/CompilerOptionTests to pass
                 //                   in order to expedite the transition from ROSE-EDG3 to ROSE-EDG4.
                 //   ROSE_ASSERT(! "Not implemented yet");
                  }
             }
            else
             {
               boost::filesystem::path original_file = outputFilename;
               boost::filesystem::path unparsed_file = get_unparse_output_filename();

               if (SgProject::get_verbose() >= 2)
                  {
                    std::cout
                      << "[DEBUG] "
                      << "unparsed_file "
                      << "'" << unparsed_file << "' "
                      << "exists = "
                      << std::boolalpha
                      << boost::filesystem::exists(unparsed_file)
                      << std::endl;
                  }
              // Don't replace the original input file with itself
               if (original_file.string() != unparsed_file.string())
                  {
                    if (SgProject::get_verbose() >= 1)
                       {
                         std::cout
                          << "[INFO] "
                          << "Replacing "
                          << "'" << unparsed_file << "' "
                          << "with "
                          << "'" << original_file << "'"
                          << std::endl;
                       }

                  // copy_file will only completely override the existing file in Boost 1.46+
                  // http://stackoverflow.com/questions/14628836/boost-copy-file-has-inconsistent-behavior-when-overwrite-if-exists-is-used
                    if (boost::filesystem::exists(unparsed_file))
                       {
                         boost::filesystem::remove(unparsed_file);
                       }

                    boost::filesystem::copy_file(original_file,unparsed_file,boost::filesystem::copy_option::overwrite_if_exists);
                  }
             }

          printf ("In SgFile::compileOutput(): outputFilename = %s \n",outputFilename.c_str());

          set_unparse_output_filename(outputFilename);
        }
#endif

     ROSE_ASSERT (get_unparse_output_filename().empty() == false); // TODO: may need to add condition:  "&& (! get_Java_only())"  here

  // Now call the compiler that rose is replacing
  // if (get_useBackendOnly() == false)
     if ( SgProject::get_verbose() >= 1 )
        {
          printf ("Now call the backend (vendor's) compiler compilerNameOrig = %s for file = %s \n",compilerNameOrig.c_str(),get_unparse_output_filename().c_str());
        }

  // Build the commandline to hand off to the C++/C compiler
     vector<string> compilerCmdLine = buildCompilerCommandLineOptions (argv,fileNameIndex, compilerName );
     
     // Support for compiling .C files as C++ on Visual Studio
     #ifdef _MSC_VER
        if (get_Cxx_only() == true)
           {
           vector<string>::iterator pos = compilerCmdLine.begin() + 1;
           compilerCmdLine.insert(pos, "/TP");
           }
     #endif
     
     int returnValueForCompiler = 0;

  // error checking
  // display("Called from SgFile::compileOutput()");

  // Allow conditional skipping of the final compile step for testing ROSE.
  // printf ("SgFile::compileOutput(): get_skipfinalCompileStep() = %s \n",get_skipfinalCompileStep() ? "true" : "false");
     if (get_skipfinalCompileStep() == false)
        {
       // Debugging code
          if ( get_verbose() > 1 )
             {
               printf ("calling systemFromVector() \n");
               printf ("Number of command line arguments: %zu\n", compilerCmdLine.size());
               for (size_t i = 0; i < compilerCmdLine.size(); ++i)
                  {
                    printf ("Backend compiler arg[%zu]: = %s\n", i, compilerCmdLine[i].c_str());
                  }
               printf("End of command line for backend compiler\n");

            // I need the exact command line used to compile the generate code with the backendcompiler (so that I can reuse it to test the generated code).
               printf ("SgFile::compileOutput(): compilerCmdLine = \n%s\n",CommandlineProcessing::generateStringFromArgList(compilerCmdLine,false,false).c_str());
             }

          if (get_Java_only() == true) 
             {
            // If the user specified a class destination folder through -rose:java:d
            // we try to create it now, if operation fail because it exists already, proceed.
               vector<string>::iterator itInput = find(compilerCmdLine.begin(), compilerCmdLine.end(), "-d");
               if (itInput != compilerCmdLine.end()) 
                  {
                    itInput++;
                    string destDirName = *itInput;
                    if(!boost::filesystem::create_directory(destDirName.c_str()))
                       {
                         if(errno != EEXIST) 
                            {
                              printf ("Can't create javac destination folder\n");
                              ROSE_ASSERT(false);
                            }
                       }
                  }

            // Insert warning flags to command line
            // if (BACKEND_JAVA_COMPILER_NAME_WITH_PATH == "javac")
               if (string(BACKEND_JAVA_COMPILER_NAME_WITH_PATH) == "javac") 
                  {
                    string warningOpt = "-Xlint";
                    if (!get_output_warnings())
                       {
                         warningOpt = "-Xlint:none";
                       }
                 // Insert after the first element which should be the backend name
                    compilerCmdLine.insert(compilerCmdLine.begin()+1, warningOpt);
                  }
             }

       // DQ (2/20/2013): The timer used in TimingPerformance is now fixed to properly record elapsed wall clock time.
       // CAVE3 double check that is correct and shouldn't be compilerCmdLine
          returnValueForCompiler = systemFromVector (compilerCmdLine);

       // TOO1 (05/14/2013): Handling for -rose:keep_going
       //
       // Compilation failed =>
       //
       //   1. Unparsed file is invalid => Try compiling the original input file
       //   2. Original input file is invalid => abort
          if (returnValueForCompiler != 0)
             {
               this->set_backendCompilerErrorCode(-1);
               if (this->get_project()->get_keep_going() == true)
                  {
                 // 1. We already failed the compilation of the ROSE unparsed file.
                 // 2. Now we tried to compile the original input file --
                 //    what was just compiled above -- and failed also.
                    if (this->get_unparsedFileFailedCompilation())
                       {
                         this->set_backendCompilerErrorCode(-1);
                         // TOO1 (11/16/2013): TODO: Allow user to catch InvalidOriginalInputFileException?
                         //throw std::runtime_error("Original input file is invalid");
                         std::cout  << "[FATAL] "
                                    << "Original input file is invalid: "
                                    << "'" << this->getFileName() << "'"
                                    << std::endl;
                         exit(1);
                       }
                      else
                       {
                      // The ROSE unparsed file is invalid...
                         this->set_frontendErrorCode(-1);
                         this->set_unparsedFileFailedCompilation(true);

                      // So try to compile the original input file instead...
                         returnValueForCompiler = this->compileOutput(argv, fileNameIndex);
                       }
                  }
             }
          //
          // If we are processing Java, ...
          //
          if (get_Java_only() == true) {
              //
              // Report if an error detected only while compilng the output file?
              //
              if (this -> get_javacErrorCode()                   == 0 &&
                  this -> get_frontendErrorCode()                == 0 &&
                  this -> get_project() -> get_midendErrorCode() == 0 &&
                  this -> get_unparserErrorCode()                == 0 &&
                  this -> get_backendCompilerErrorCode()         != 0) {
                  cout << "ERROR found in output file: "
                       << get_unparse_output_filename()
                       << endl;
              }

              //
              // Report Error or Success of this translation.
              //
              if (this -> get_javacErrorCode() != 0) {
                  cout << "SYNTAX ERROR(s) found in "
                       << this -> getFileName()
                       << endl;
                  cout.flush();
              }
              else if (this -> get_frontendErrorCode()                != 0 ||
                       this -> get_project() -> get_midendErrorCode() != 0 ||
                       this -> get_unparserErrorCode()                != 0 ||
                       this -> get_backendCompilerErrorCode()         != 0) {
                  cout << "ERROR compiling "
                       << this -> getFileName()
                       << endl;
                  cout.flush();
              }
              else {
                  cout << "SUCCESS compiling "
                       << this -> getFileName()
                       << endl;
                  cout.flush();
              }
          }
         }
       else
        {
          if ( get_verbose() > 1 )
               printf ("COMPILER NOT CALLED: compilerNameString = %s \n", "<unknown>" /* compilerNameString.c_str() */);

       // DQ (8/21/2008): If this is a binary then we don't need the message output.
       // Liao 8/29/2008: disable it for non-verbose model
          if ( (get_binary_only() == false) && (get_verbose() > 0) )
             {
               printf ("Skipped call to backend vendor compiler! \n");
             }
        }

  // DQ (7/20/2006): Catch errors returned from unix "system" function
  // (commonly "out of memory" errors, suggested by Peter and Jeremiah).
     if (returnValueForCompiler < 0)
        {
          perror("Serious Error returned from internal systemFromVector command");
        }

  // Assemble an exit status that combines the values for ROSE and the C++/C compiler
  // return an exit status which is the boolean OR of the bits from the EDG/SAGE/ROSE and the compile step
     int finalCompiledExitStatus = returnValueForRose | returnValueForCompiler;

  // It is a strange property of the UNIX $status that it does not map uniformally from
  // the return value of the "exit" command (or "return" statement).  So if the exit
  // status from the compilation stage is nonzero then we just make the exit status 1
  // (this does seem to be a portable solution).
  // FYI: only the first 8 bits of the exit value are significant (Solaris uses 'exit_value mod 256').
     if (finalCompiledExitStatus != 0)
        {
       // If this it is non-zero then make it 1 to be more clear to external tools (e.g. make)
          finalCompiledExitStatus = 1;
        }

  // DQ (9/19/2006): We need to invert the test result (return code) for
  // negative tests (where failure is expected and success is an error).
     if (get_negative_test() == true)
        {
          if ( get_verbose() > 1 )
               printf ("This is a negative tests, so an error in compilation is a PASS and successful compilation is a FAIL (vendor compiler return value = %d) \n",returnValueForCompiler);

          finalCompiledExitStatus = (finalCompiledExitStatus == 0) ? /* error */ 1 : /* success */ 0;
        }

  // printf ("Program Terminated Normally (exit status = %d)! \n\n\n\n",finalCompiledExitStatus);

     return finalCompiledExitStatus;
   }


//! project level compilation and linking
// three cases: 1. preprocessing only
//              2. compilation:
//              3. linking:
// int SgProject::compileOutput( const std::string& compilerName )
int
SgProject::compileOutput()
   {
  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Backend Compilation (SgProject):");

     int errorCode = 0;
     int linkingReturnVal = 0;
     int i = 0;

     std::string compilerName;

  // DQ (1/19/2014): Adding support for gnu "-S" option.
     if (get_stop_after_compilation_do_not_assemble_file() == true)
        {
       // DQ (1/19/2014): Handle special case (issue a single compile command for all files using the "-S" option).
          vector<string> argv = get_originalCommandLineArgumentList();

       // strip out any rose options before passing the command line.
          SgFile::stripRoseCommandLineOptions( argv );

       // strip out edg specific options that would cause an error in the backend linker (compiler).
          SgFile::stripEdgCommandLineOptions( argv );

          vector<string> originalCommandLine = argv;
          ROSE_ASSERT (!originalCommandLine.empty());

          string & compilerNameString = originalCommandLine[0];
          if (get_C_only() == true)
             {
               compilerNameString = BACKEND_C_COMPILER_NAME_WITH_PATH;
             }
            else
             {
               printf ("Error: GNU \"-S\" option is not supported for more than the C language in ROSE at present! \n");
               ROSE_ASSERT(false);
             }

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif

          if ( SgProject::get_verbose() > 0 )
             {
               printf ("In SgProject::compileOutput(): listToString(originalCommandLine) = %s \n",StringUtility::listToString(originalCommandLine).c_str());
             }

          errorCode = systemFromVector(originalCommandLine);

          return errorCode + linkingReturnVal;
        }
     

     if (numberOfFiles() == 0)
        {
       // printf ("Note in SgProject::compileOutput(%s): numberOfFiles() == 0 \n",compilerName.c_str());
       // printf ("ROSE using %s as backend compiler: no input files \n",compilerName.c_str());

       // DQ (8/24/2008): We can't recreate same behavior on exit as GNU on exit with no
       // files since it causes the test immediately after building librose.so to fail.
       // exit(1);
        }

  // printf ("In SgProject::compileOutput(): get_C_PreprocessorOnly() = %s \n",get_C_PreprocessorOnly() ? "true" : "false");

  // case 1: preprocessing only
     if (get_C_PreprocessorOnly() == true)
        {
       // DQ (10/16/2005): Handle special case (issue a single compile command for all files)
          vector<string> argv = get_originalCommandLineArgumentList();

       // strip out any rose options before passing the command line.
          SgFile::stripRoseCommandLineOptions( argv );

       // strip out edg specific options that would cause an error in the backend linker (compiler).
          SgFile::stripEdgCommandLineOptions( argv );

       // Skip the name of the ROSE translator (so that we can insert the backend compiler name, below)
       // bool skipInitialEntry = true;

       // Include all the specified source files
       // bool skipSourceFiles  = false;

          vector<string> originalCommandLine = argv;
          ROSE_ASSERT (!originalCommandLine.empty());

       // DQ (8/13/2006): Use the path to the compiler specified as that backend compiler (should not be specifi to GNU!)
       // DQ (8/6/2006): Test for g++ and use gcc with "-E" option
       // (makes a different for header file processing in ARES configuration)
       // string compilerNameString = compilerName;
          string & compilerNameString = originalCommandLine[0];
          if (get_C_only() == true)
             {
               compilerNameString = BACKEND_C_COMPILER_NAME_WITH_PATH;
             }
            else
             {
               compilerNameString = BACKEND_CXX_COMPILER_NAME_WITH_PATH;
               if (get_Fortran_only() == true)
                  {
                    compilerNameString = "f77";
                  }
             }

       // DQ (8/13/2006): Add a space to avoid building "g++-E" as output.
       // compilerNameString += " ";

       // Prepend the compiler name to the original command line
       // originalCommandLine = std::string(compilerName) + std::string(" ") + originalCommandLine;
       // originalCommandLine = compilerNameString + originalCommandLine;

       // Prepend the compiler name to the original command line
       // originalCommandLine = std::string(compilerName) + std::string(" ") + originalCommandLine;

       // printf ("originalCommandLine = %s \n",originalCommandLine.c_str());

#if 0
          printf ("Support for \"-E\" not implemented yet. \n");
          ROSE_ASSERT(false);
#endif

          errorCode = systemFromVector(originalCommandLine);

#if 0
       // DQ (8/22/2009): We test the parent of SgFunctionTypeTable in the AST post processing,
       // so we need to make sure that it is set.
          SgFunctionTypeTable* functionTypeTable = SgNode::get_globalFunctionTypeTable();
          ROSE_ASSERT(functionTypeTable != NULL);
          if (functionTypeTable->get_parent() == NULL)
             {
               ROSE_ASSERT(numberOfFiles() > 0);
               printf ("set the parent of SgFunctionTypeTable \n");
               functionTypeTable->set_parent(&get_file(0));
             }
          ROSE_ASSERT(functionTypeTable->get_parent() != NULL);
#endif

          ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
          ROSE_ASSERT(SgNode::get_globalFunctionTypeTable()->get_parent() != NULL);

       // printf ("Exiting after call to compiler using -E option! \n");
       // ROSE_ASSERT(false);
        }
       else // non-preprocessing-only case
        {
       // printf ("In Project::compileOutput(): Compiling numberOfFiles() = %d \n",numberOfFiles());

// case 2: compilation  for each file
       // Typical case
          for (i=0; i < numberOfFiles(); i++)
          {
              int localErrorCode = 0;
              SgFile & file = get_file(i);

              if (KEEP_GOING_CAUGHT_BACKEND_COMPILER_SIGNAL)
              {
                  std::cout
                      << "[WARN] "
                      << "Configured to keep going after catching a "
                      << "signal in SgProject::compileOutput()"
                      << std::endl;

                  localErrorCode = 100;
                  file.set_backendCompilerErrorCode(localErrorCode);
              }
              else
              {
                  localErrorCode = file.compileOutput(0);
              }

              if (localErrorCode > errorCode)
              {
                  errorCode = localErrorCode;
              }
          }

       // case 3: linking at the project level
          if (! (get_Java_only()   ||
                 get_Python_only() ||
                 get_X10_only()))
          {
            // Liao, 11/19/2009, 
            // I really want to just move the SgFile::compileOutput() to SgProject::compileOutput() 
            // and have both compilation and linking finished at the same time, just as the original command line does.
            // Then we don't have to compose compilation command line for each of the input source file
            // or to compose the final linking command line. 
            //
            // But there may be some advantages of doing the compilation and linking separately at two levels.
            // I just discussed it with Dan.
            // The two level scheme is needed to support mixed language input, like a C file and a Fortran file
            // In this case, we cannot have a single one level command line to compile and link those two files
            // We have to compile each of them first and finally link the object files.
#ifndef _MSC_VER
            // tps 08/18/2010 : Do not link right now in Windows - it breaks - want test to pass here for now.
            // todo windows: put this back in.
            // linkingReturnVal = link (compilerName);
               linkingReturnVal = link (BACKEND_CXX_COMPILER_NAME_WITH_PATH);
#else
   #pragma message ("sageSupport.C : linkingReturnVal = link (compilerName); not implemented yet.")
#endif
          }
        } // end if preprocessing-only is false

  // return errorCode;
     return errorCode + linkingReturnVal;
   }


bool
SgFile::isPrelinkPhase() const
   {
  // This function checks if the "-prelink" option was passed to the ROSE preprocessor
  // It could alternatively just check the commandline and set a flag in the SgFile object.
  // But then there would be a redundent flag in each SgFile object (perhaps the design needs to
  // be better, using a common base class for commandline options (in both the SgProject and
  // the SgFile (would not be a new IR node)).

     bool returnValue = false;

  // DQ (5/9/2004): If the parent is not set then this was compiled as a SgFile directly
  // (likely by the rewrite mechanism). IF so it can't be a prelink phase, which is
  // called only on SgProjects). Not happy with this mechanism!
     if (get_parent() != NULL)
        {
          ROSE_ASSERT ( get_parent() != NULL );

       // DQ (1/24/2010): Now that we have directory support, the parent of a SgFile does not have to be a SgProject.
       // SgProject* project = isSgProject(get_parent());
          SgProject* project = TransformationSupport::getProject(this);

          ROSE_ASSERT ( project != NULL );
          returnValue = project->get_prelink();
        }

     return returnValue;

  // Note that project can be false if this is called on an intermediate file
  // generated by the rewrite system.
  // return (project == NULL) ? false : project->get_prelink();
   }

// DQ (10/14/2010): Removing reference to macros defined in rose_config.h (defined in the header file as a default parameter).
//! Preprocessing command line and pass it to generate the final linking command line
// int SgProject::link ()
int SgProject::link ( std::string linkerName )
   {
  // DQ (1/25/2010): We have to now test for both numberOfFiles() and numberOfDirectories(),
  // or perhaps define a more simple function to use more directly.
  // Liao, 11/20/2009
  // translator test1.o will have ZERO SgFile attached with SgProject
  // Special handling for this case
  // if (numberOfFiles() == 0)
     if (numberOfFiles() == 0 && numberOfDirectories() == 0)
        {
          if (get_verbose() > 0)
               cout << "SgProject::link maybe encountering an object file ..." << endl;

       // DQ (1/24/2010): support for directories not in place yet.
          if (numberOfDirectories() > 0)
             {
               printf ("Directory support for linking not implemented... (unclear what this means...)\n");
               return 0;
             }
        }
       else
        {
       // normal cases that rose translators will actually do something about the input files
       // and we have SgFile for each of the files.
       // if ((numberOfFiles()== 0) || get_compileOnly() || get_file(0).get_skipfinalCompileStep()
          if ( get_compileOnly() || get_file(0).get_skipfinalCompileStep() ||get_file(0).get_skip_unparse())
             {
               if (get_verbose() > 0)
                    cout << "Skipping SgProject::link ..." << endl;
               return 0;
             }
        }

  // Compile the output file from the unparsing
     vector<string> argcArgvList = get_originalCommandLineArgumentList();

  // error checking
     if (numberOfFiles()!= 0)
          ROSE_ASSERT (argcArgvList.size() > 1);

     ROSE_ASSERT(linkerName != "");

  // strip out any rose options before passing the command line.
     SgFile::stripRoseCommandLineOptions( argcArgvList );

  // strip out edg specific options that would cause an error in the backend linker (compiler).
     SgFile::stripEdgCommandLineOptions( argcArgvList );

   // remove the original compiler/linker name
     argcArgvList.erase(argcArgvList.begin());

     // remove all original file names
     Rose_STL_Container<string> sourceFilenames = get_sourceFileNameList();
     for (Rose_STL_Container<string>::iterator i = sourceFilenames.begin(); i != sourceFilenames.end(); i++)
        {
#if USE_ABSOLUTE_PATHS_IN_SOURCE_FILE_LIST
#error "USE_ABSOLUTE_PATHS_IN_SOURCE_FILE_LIST is not supported yet"
       // DQ (9/1/2006): Check for use of absolute path and convert filename to absolute path if required
          bool usesAbsolutePath = ((*i)[0] == '/');
          if (usesAbsolutePath == false)
             {
               string targetSourceFileToRemove = StringUtility::getAbsolutePathFromRelativePath(*i);
               printf ("Converting source file to absolute path to search for it and remove it! targetSourceFileToRemove = %s \n",targetSourceFileToRemove.c_str());
               argcArgvList.remove(targetSourceFileToRemove);
             }
            else
             {
               argcArgvList.remove(*i);
             }
#else
          if (find(argcArgvList.begin(),argcArgvList.end(),*i) != argcArgvList.end())
             {
               argcArgvList.erase(find(argcArgvList.begin(),argcArgvList.end(),*i));
             }
#endif
        }

     // fix double quoted strings
     // DQ (4/14/2005): Fixup quoted strings in args fix "-DTEST_STRING_MACRO="Thu Apr 14 08:18:33 PDT 2005"
     // to be -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\"  This is a problem in the compilation of
     // a Kull file (version.cc), when the backend is specified as /usr/apps/kull/tools/mpig++-3.4.1.  The
     // problem is that /usr/apps/kull/tools/mpig++-3.4.1 is a wrapper for a shell script /usr/local/bin/mpiCC
     // which does not tend to observe quotes well.  The solution is to add additional escaped quotes.
     for (Rose_STL_Container<string>::iterator i = argcArgvList.begin(); i != argcArgvList.end(); i++)
        {
          std::string::size_type startingQuote = i->find("\"");
          if (startingQuote != std::string::npos)
             {
               std::string::size_type endingQuote   = i->rfind("\"");

            // There should be a double quote on both ends of the string
               ROSE_ASSERT (endingQuote != std::string::npos);

               std::string quotedSubstring = i->substr(startingQuote,endingQuote);

            // DQ (11/1/2012): Robb has suggested using single quote instead of double quotes here.
            // This is a problem for the processing of mutt (large C application) (but we can figure 
            // out the link command after the compile command).
            // std::string fixedQuotedSubstring = std::string("\\\"") + quotedSubstring + std::string("\\\"");
            // std::string fixedQuotedSubstring = std::string("\\\'") + quotedSubstring + std::string("\\\'");
               std::string fixedQuotedSubstring = std::string("\\\"") + quotedSubstring + std::string("\\\"");

            // Now replace the quotedSubstring with the fixedQuotedSubstring
               i->replace(startingQuote,endingQuote,fixedQuotedSubstring);

               printf ("Modified argument = %s \n",(*i).c_str());
             }
        }

  // Call the compile
     int errorCode = link ( argcArgvList, linkerName );

  // return the error code from the compilation
     return errorCode;
   }

// DQ (10/14/2010): Removing reference to macros defined in rose_config.h (defined in the header file as a default parameter).
// int SgProject::link ( const std::vector<std::string>& argv )
int SgProject::link ( const std::vector<std::string>& argv, std::string linkerName )
   {
  // argv.size could be 0 after strip off compiler name, original source file, etc
  // ROSE_ASSERT(argv.size() > 0);

  // This link function will be moved into the SgProject IR node when complete
     const std::string whiteSpace = " ";
  // printf ("This link function is no longer called (I think!) \n");
  // ROSE_ASSERT(false);

  // DQ (10/15/2005): Trap out case of C programs where we want to make sure that we don't use the C++ compiler to do our linking!
     if (get_C_only() == true || get_C99_only() == true)
        {
          linkerName = BACKEND_C_COMPILER_NAME_WITH_PATH;
        }
       else
        {
       // The default is C++
          linkerName = BACKEND_CXX_COMPILER_NAME_WITH_PATH;

          if (get_Fortran_only() == true)
             {
            // linkerName = "f77 ";
               linkerName = BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH;
             }
            else
             {
            // DQ (4/2/2011): Added support for Java (though not clear if link support for Java is well understood within ROSE).
               if (get_Java_only() == true)
                  {
                    linkerName = BACKEND_JAVA_COMPILER_NAME_WITH_PATH;

                 // Debugging support.
                    display("SgProject::link()");

                 // Java programs don't link so this is an error.
                    printf ("Java programs don't link so this is an error. \n");
                    ROSE_ASSERT(false);
                  }
                 else
                  {
                 // Nothing to do here (case of C++)
                  }
             }
        }

  // This is a better implementation since it will include any additional command line options that target the linker
     Rose_STL_Container<string> linkingCommand ;

     linkingCommand.push_back (linkerName);
     // find all object files generated at file level compilation
     // The assumption is that -o objectFileName is made explicit and
     // is generated by SgFile::generateOutputFileName()
     for (int i=0; i < numberOfFiles(); i++)
        {
       // DQ (2/25/2014): If this file was supressed in the compilation to build an
       // object file then it should be supressed in being used in the linking stage.
       // linkingCommand.push_back(get_file(i).generateOutputFileName());
          if (get_file(i).get_skipfinalCompileStep() == false)
             {
                 linkingCommand.push_back(get_file(i).generateOutputFileName());
             }
        }

  // Add any options specified in the original command line (after preprocessing)
     linkingCommand.insert(linkingCommand.end(), argv.begin(), argv.end());

  // Check if -o option exists, otherwise append -o a.out to the command line

  // Additional libraries to be linked with
  // Liao, 9/23/2009, optional linker flags to support OpenMP lowering targeting GOMP
//     if ((numberOfFiles() !=0) && (get_file(0).get_openmp_lowering())
//     Liao 6/29/2012. sometimes rose translator is used as a wrapper for linking
//     There will be no SgFile at all in this case but we still want to append relevant linking options for OpenMP
     if( SageInterface::getProject()->get_openmp_linking())
     {
// Sara Royuela 12/10/2012:  Add GCC version check
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY
#if (__GNUC__ < 4 || \
    (__GNUC__ == 4 && (__GNUC_MINOR__ < 4)))
#warning "GNU version lower than expected"    
        printf("GCC version must be 4.4.0 or later when linking with GOMP OpenMP Runtime Library \n(OpenMP tasking calls are not implemented in previous versions)\n");
        ROSE_ASSERT(false);
#endif

       // add libxomp.a , Liao 6/12/2010
       string xomp_lib_path(ROSE_INSTALLATION_PATH);
       ROSE_ASSERT (xomp_lib_path.size() != 0);
       linkingCommand.push_back(xomp_lib_path+"/lib/libxomp.a"); // static linking for simplicity

       // lib path is available if --with-gomp_omp_runtime_library=XXX is used
         string gomp_lib_path(GCC_GOMP_OPENMP_LIB_PATH);
         ROSE_ASSERT (gomp_lib_path.size() != 0);
         linkingCommand.push_back(gomp_lib_path+"/libgomp.a");
         linkingCommand.push_back("-lpthread");
#else
  // GOMP has higher priority when both GOMP and OMNI are specified (wrongfully)
  #ifdef OMNI_OPENMP_LIB_PATH
           // a little redundant code to defer supporting 'ROSE_INSTALLATION_PATH' in cmake
           string xomp_lib_path(ROSE_INSTALLATION_PATH);
           ROSE_ASSERT (xomp_lib_path.size() != 0);
           linkingCommand.push_back(xomp_lib_path+"/lib/libxomp.a");

           string omni_lib_path(OMNI_OPENMP_LIB_PATH);
           ROSE_ASSERT (omni_lib_path.size() != 0);
           linkingCommand.push_back(omni_lib_path+"/libgompc.a");
           linkingCommand.push_back("-lpthread");
  #else
     printf("Warning: OpenMP lowering is requested but no target runtime library is specified!\n");
  #endif
#endif
     }

     if ( get_verbose() > 0 )
        {
          printf ("In SgProject::link command line = %s \n",CommandlineProcessing::generateStringFromArgList(linkingCommand,false,false).c_str());
        }

     int status = systemFromVector(linkingCommand);

     if ( get_verbose() > 1 )
          printf ("linker error status = %d \n",status);

     return status;
   }

// DQ (12/22/2005): Jochen's support for a constant (non-NULL) valued pointer
// to use to distinguish valid from invalid IR nodes within the memory pools.
namespace AST_FileIO
   {
     SgNode* IS_VALID_POINTER()
        {
       // static SgNode* value = (SgNode*)(new char[1]);

       // DQ (1/17/2006): Set to the pointer value 0xffffffff (as used by std::string::npos)
          static SgNode* value = (SgNode*)(std::string::npos);
       // printf ("In AST_FileIO::IS_VALID_POINTER(): value = %p \n",value);

          return value;
        }

  // similar vlue for reprentation of subsets of the AST
     SgNode* TO_BE_COPIED_POINTER()
        {
       // static SgNode* value = (SgNode*)(new char[1]);
          static SgNode* value = (SgNode*)((std::string::npos) - 1);
       // printf ("In AST_FileIO::TO_BE_COPIED_POINTER(): value = %p \n",value);
          return value;
        }
   }


//! Prints pragma associated with a grammatical element.
/*!       (fill in more detail here!)
 */
void
print_pragma(SgAttributePtrList& pattr, std::ostream& os)
   {
     SgAttributePtrList::const_iterator p = pattr.begin();
     if (p == pattr.end())
          return;
       else
          p++;

     while (p != pattr.end())
        {
          if ( (*p)->isPragma() )
             {
               SgPragma *pr = (SgPragma*)(*p);
               if (!pr->gotPrinted())
                  {
                    os << std::endl << "#pragma " <<pr->get_name() << std::endl;
                    pr->setPrinted(1);
                  }
             }
          p++;
        }
   }




// Temporary function to be later put into Sg_FileInfo
StringUtility::FileNameLocation
get_location ( Sg_File_Info* X )
   {
     SgFile* file = TransformationSupport::getFile(X->get_parent());
     ROSE_ASSERT(file != NULL);
     string sourceFilename = file->getFileName();
     string sourceDirectory = StringUtility::getPathFromFileName(sourceFilename);

     StringUtility::FileNameClassification classification = StringUtility::classifyFileName(X->get_filenameString(),sourceDirectory,StringUtility::getOSType());

  // return StringUtility::FILENAME_LOCATION_UNKNOWN;
     return classification.getLocation();
   }

StringUtility::FileNameLibrary
get_library ( Sg_File_Info* X )
   {
     SgFile* file = TransformationSupport::getFile(X->get_parent());
     ROSE_ASSERT(file != NULL);
     string sourceFilename = file->getFileName();
     string sourceDirectory = StringUtility::getPathFromFileName(sourceFilename);

     StringUtility::FileNameClassification classification = StringUtility::classifyFileName(X->get_filenameString(),sourceDirectory,StringUtility::getOSType());

  // return StringUtility::FILENAME_LIBRARY_UNKNOWN;
     return classification.getLibrary();
   }

std::string
get_libraryName ( Sg_File_Info* X )
   {
     SgFile* file = TransformationSupport::getFile(X->get_parent());
     ROSE_ASSERT(file != NULL);
     string sourceFilename = file->getFileName();
     string sourceDirectory = StringUtility::getPathFromFileName(sourceFilename);

     StringUtility::FileNameClassification classification = StringUtility::classifyFileName(X->get_filenameString(),sourceDirectory,StringUtility::getOSType());

  // return "";
     return classification.getLibraryName();
   }

StringUtility::OSType
get_OS_type ()
   {
  // return StringUtility::OS_TYPE_UNKNOWN;
     return StringUtility::getOSType();
   }

int
get_distanceFromSourceDirectory ( Sg_File_Info* X )
   {
     SgFile* file = TransformationSupport::getFile(X->get_parent());
     ROSE_ASSERT(file != NULL);
     string sourceFilename = file->getFileName();
     string sourceDirectory = StringUtility::getPathFromFileName(sourceFilename);

     StringUtility::FileNameClassification classification = StringUtility::classifyFileName(X->get_filenameString(),sourceDirectory,StringUtility::getOSType());

  // return 0;
     return classification.getDistanceFromSourceDirectory();
   }





int
SgNode::numberOfNodesInSubtree()
   {
     int value = 0;

     class CountTraversal : public SgSimpleProcessing
        {
          public:
              int count;
              CountTraversal() : count(0) {}
              void visit ( SgNode* n ) { count++; }
        };

     CountTraversal counter;
     SgNode* thisNode = const_cast<SgNode*>(this);
     counter.traverse(thisNode,preorder);
     value = counter.count;

     return value;
   }

namespace SgNode_depthOfSubtree
   {
  // This class (AST traversal) could not be defined in the function SgNode::depthOfSubtree()
  // So I have constructed a namespace for this class to be implemented outside of the function.

     class DepthInheritedAttribute
        {
          public:
               int treeDepth;
               DepthInheritedAttribute( int depth ) : treeDepth(depth) {}
        };

     class MaxDepthTraversal : public AstTopDownProcessing<DepthInheritedAttribute>
        {
          public:
              int maxDepth;
              MaxDepthTraversal() : maxDepth(0) {}

              DepthInheritedAttribute evaluateInheritedAttribute ( SgNode* astNode, DepthInheritedAttribute inheritedAttribute )
                 {
                   if (inheritedAttribute.treeDepth > maxDepth)
                        maxDepth = inheritedAttribute.treeDepth;
#if 0
                   printf ("maxDepth = %d for IR nodes = %p = %s \n",maxDepth,astNode,astNode->class_name().c_str());
#endif
                   return DepthInheritedAttribute(inheritedAttribute.treeDepth + 1);
                 }
        };
   }

int
SgNode::depthOfSubtree()
   {
     int value = 0;

     SgNode_depthOfSubtree::MaxDepthTraversal depthCounter;
     SgNode_depthOfSubtree::DepthInheritedAttribute inheritedAttribute(0);
     SgNode* thisNode = const_cast<SgNode*>(this);

     depthCounter.traverse(thisNode,inheritedAttribute);

     value = depthCounter.maxDepth;

     return value;
   }

#if 0
// We only need one definition for this function at the SgNode IR node.
size_t
SgFile::numberOfNodesInSubtree()
   {
     printf ("Base class of virtual function (SgFile::numberOfNodesInSubtree() should not be called! \n");
     ROSE_ASSERT(false);

     return 0;
   }

size_t
SgSourceFile::numberOfNodesInSubtree()
   {
     return get_globalScope()->numberOfNodesInSubtree() + 1;
   }

size_t
SgBinaryComposite::numberOfNodesInSubtree()
   {
     return get_binaryFile()->numberOfNodesInSubtree() + 1;
   }
#endif


// DQ (10/3/2008): Added support for getting interfaces in a module
std::vector<SgInterfaceStatement*>
SgModuleStatement::get_interfaces() const
   {
     std::vector<SgInterfaceStatement*> returnList;

     SgModuleStatement* definingModuleStatement = isSgModuleStatement(get_definingDeclaration());
     ROSE_ASSERT(definingModuleStatement != NULL);

     SgClassDefinition* moduleDefinition = definingModuleStatement->get_definition();
     ROSE_ASSERT(moduleDefinition != NULL);

     SgDeclarationStatementPtrList & declarationList = moduleDefinition->getDeclarationList();

     SgDeclarationStatementPtrList::iterator i = declarationList.begin();
     while (i != declarationList.end())
        {
          SgInterfaceStatement* interfaceStatement = isSgInterfaceStatement(*i);
          if (interfaceStatement != NULL)
             {
               returnList.push_back(interfaceStatement);
             }

          i++;
        }

     return  returnList;
   }

// DQ (11/23/2008): This is a static function
SgC_PreprocessorDirectiveStatement*
SgC_PreprocessorDirectiveStatement::createDirective ( PreprocessingInfo* currentPreprocessingInfo )
   {
  // This is the new factory interface to build CPP directives as IR nodes.
     PreprocessingInfo::DirectiveType directive = currentPreprocessingInfo->getTypeOfDirective();

  // SgC_PreprocessorDirectiveStatement* cppDirective = new SgEmptyDirectiveStatement();
     SgC_PreprocessorDirectiveStatement* cppDirective = NULL;

     switch(directive)
        {
          case PreprocessingInfo::CpreprocessorUnknownDeclaration:
             {
            // I think this is an error...
            // locatedNode->addToAttachedPreprocessingInfo(currentPreprocessingInfoPtr);
               printf ("Error: directive == PreprocessingInfo::CpreprocessorUnknownDeclaration \n");
               ROSE_ASSERT(false);
               break;
             }

          case PreprocessingInfo::C_StyleComment:
          case PreprocessingInfo::CplusplusStyleComment:
          case PreprocessingInfo::FortranStyleComment:
          case PreprocessingInfo::CpreprocessorBlankLine:
          case PreprocessingInfo::ClinkageSpecificationStart:
          case PreprocessingInfo::ClinkageSpecificationEnd:
             {
               printf ("Error: these cases could not generate a new IR node (directiveTypeName = %s) \n",PreprocessingInfo::directiveTypeName(directive).c_str());
               ROSE_ASSERT(false);
               break;
             }

          case PreprocessingInfo::CpreprocessorIncludeDeclaration:          { cppDirective = new SgIncludeDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorIncludeNextDeclaration:      { cppDirective = new SgIncludeNextDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorDefineDeclaration:           { cppDirective = new SgDefineDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorUndefDeclaration:            { cppDirective = new SgUndefDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorIfdefDeclaration:            { cppDirective = new SgIfdefDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorIfndefDeclaration:           { cppDirective = new SgIfndefDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorIfDeclaration:               { cppDirective = new SgIfDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorDeadIfDeclaration:           { cppDirective = new SgDeadIfDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorElseDeclaration:             { cppDirective = new SgElseDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorElifDeclaration:             { cppDirective = new SgElseifDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorEndifDeclaration:            { cppDirective = new SgEndifDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorLineDeclaration:             { cppDirective = new SgLineDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorErrorDeclaration:            { cppDirective = new SgErrorDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorWarningDeclaration:          { cppDirective = new SgWarningDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorEmptyDeclaration:            { cppDirective = new SgEmptyDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorIdentDeclaration:            { cppDirective = new SgIdentDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorCompilerGeneratedLinemarker: { cppDirective = new SgLinemarkerDirectiveStatement(); break; }

          default:
             {
               printf ("Error: directive not handled directiveTypeName = %s \n",PreprocessingInfo::directiveTypeName(directive).c_str());
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(cppDirective != NULL);

     cppDirective->set_directiveString(currentPreprocessingInfo->getString());

  // Set the defining declaration to be a self reference...
     cppDirective->set_definingDeclaration(cppDirective);

  // Build source position information...
     cppDirective->set_startOfConstruct(new Sg_File_Info(*(currentPreprocessingInfo->get_file_info())));
     cppDirective->set_endOfConstruct(new Sg_File_Info(*(currentPreprocessingInfo->get_file_info())));

     return cppDirective;
   }

bool
StringUtility::popen_wrapper ( const string & command, vector<string> & result )
   {
  // DQ (2/5/2009): Simple wrapper for Unix popen command.

     const int  SIZE = 10000;
     bool       returnValue = true;
     FILE*      fp = NULL;
     char       buffer[SIZE];

     result = vector<string>();




     // CH (4/6/2010): The Windows version of popen is _popen
#ifdef _MSC_VER
     if ((fp = _popen(command.c_str (), "r")) == NULL)
#else
     if ((fp = popen(command.c_str (), "r")) == NULL)
#endif
        {
          cerr << "Files or processes cannot be created" << endl;
          returnValue = false;
          return returnValue;
        }

     string  current_string;
     while (fgets(buffer, sizeof (buffer), fp))
        {
          current_string = buffer;
          if (current_string [current_string.size () - 1] != '\n')
             {
               cerr << "SIZEBUF too small (" << SIZE << ")" << endl;
               returnValue = false;
               return returnValue;
             }
          ROSE_ASSERT(current_string [current_string.size () - 1] == '\n');
          result.push_back (current_string.substr (0, current_string.size () - 1));
        }

#ifdef _MSC_VER
     if (_pclose(fp) == -1)
#else
     if (pclose(fp) == -1)
#endif
        {
          cerr << ("Cannot execute pclose");
          returnValue = false;
        }
//#endif

     return returnValue;
   }

string
StringUtility::demangledName ( string s )
   {
  // Support for demangling of C++ names. We take care of an empty
  // string, but an input string with a single space might be an issue.

     vector<string> result;
     if (s.empty() == false)
        {
          if (!popen_wrapper ("c++filt " + s, result))
             {
               cout << "Cannot execute popen_wrapper" << endl;
               return "unknown demangling " + s;
             }
#if 0
       // Debugging...
          for (size_t i = 0; i < result.size (); i++)
             {
               cout << "[" << i << "]\t : " << result [i] << endl;
             }
#endif
        }
       else
        {
          result.push_back("unknown");
        }

     return result[0];
   }


#if 1
// DQ (2/16/2009): Moved from expression.code to sageSupport.C while this is tested and debugged!

// DQ (2/8/2009): I always wanted to have this function!
SgFunctionDeclaration*
SgFunctionCallExp::getAssociatedFunctionDeclaration() const
   {
  // This is helpful in chasing down the associated declaration to this function reference.
     SgFunctionDeclaration* returnFunctionDeclaration = NULL;

     SgFunctionSymbol* associatedFunctionSymbol = getAssociatedFunctionSymbol();
     // It can be NULL for a function pointer
     //ROSE_ASSERT(associatedFunctionSymbol != NULL);
     if (associatedFunctionSymbol != NULL)
       returnFunctionDeclaration = associatedFunctionSymbol->get_declaration();

    // ROSE_ASSERT(returnFunctionDeclaration != NULL);

     return returnFunctionDeclaration;
   }


SgFunctionSymbol*
SgFunctionCallExp::getAssociatedFunctionSymbol() const
   {
  // This is helpful in chasing down the associated declaration to this function reference.
  // But this refactored function does the first step of getting the symbol, so that it
  // can also be used separately in the outlining support.
     SgFunctionSymbol* returnSymbol = NULL;

  // Note that as I recall there are a number of different types of IR nodes that
  // the functionCallExp->get_function() can return (this is the complete list,
  // as tested in astConsistancyTests.C):
  //   - SgDotExp
  //   - SgDotStarOp
  //   - SgArrowExp
  //   - SgArrowStarOp
  //   - SgPointerDerefExp
  //   - SgFunctionRefExp
  //   - SgMemberFunctionRefExp

  // Some virtual functions are resolved statically (e.g. for objects allocated on the stack)
     bool isAlwaysResolvedStatically = false;

     SgExpression* functionExp = this->get_function();
     switch (functionExp->variantT())
        {
       // EDG3 removes all SgPointerDerefExp nodes from an expression like this
       //    void f() { (***f)(); }
       // EDG4 does not.  Therefore, if the thing to which the pointers ultimately point is a SgFunctionRefExp then we
       // know the function, otherwise Liao's comment below applies. [Robb Matzke 2012-12-28]
       // 
       // Liao, 5/19/2009
       // A pointer to function can be associated to any functions with a matching function type
       // There is no single function declaration which is associated with it.
       // In this case return NULL should be allowed and the caller has to handle it accordingly
       //
          case V_SgPointerDerefExp:
             {
               SgPointerDerefExp *exp = isSgPointerDerefExp(functionExp);
               SgFunctionRefExp *fref = NULL;
               while (exp && !fref) 
                  {
                    fref = isSgFunctionRefExp(exp->get_operand_i());
                    exp  = isSgPointerDerefExp(exp->get_operand_i());
                  }

               if (!fref)
                    break;

               functionExp = fref;
             }
       // fall through

          case V_SgFunctionRefExp:
             {
               SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionExp);
               ROSE_ASSERT(functionRefExp != NULL);
               returnSymbol = functionRefExp->get_symbol();

            // DQ (2/8/2009): Can we assert this! What about pointers to functions?
               ROSE_ASSERT(returnSymbol != NULL);
               break;
             }

       // DQ (2/24/2013): Added case to support SgTemplateFunctionRefExp (now generated as a result of
       // work on 2/23/2013 specific to unknown function handling in templates, now resolved by name).
          case V_SgTemplateFunctionRefExp:
             {
               SgTemplateFunctionRefExp* functionRefExp = isSgTemplateFunctionRefExp(functionExp);
               ROSE_ASSERT(functionRefExp != NULL);
               returnSymbol = functionRefExp->get_symbol();

            // DQ (2/8/2009): Can we assert this! What about pointers to functions?
               ROSE_ASSERT(returnSymbol != NULL);
               break;
             }

       // DQ (2/25/2013): Added case to support SgTemplateFunctionRefExp (now generated as a result of
       // work on 2/23/2013 specific to unknown function handling in templates, now resolved by name).
          case V_SgTemplateMemberFunctionRefExp:
             {
               SgTemplateMemberFunctionRefExp* templateMemberFunctionRefExp = isSgTemplateMemberFunctionRefExp(functionExp);
               ROSE_ASSERT(templateMemberFunctionRefExp != NULL);
               returnSymbol = templateMemberFunctionRefExp->get_symbol();

            // DQ (2/8/2009): Can we assert this! What about pointers to functions?
               ROSE_ASSERT(returnSymbol != NULL);
               break;
             }

          case V_SgMemberFunctionRefExp:
             {
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(functionExp);
               ROSE_ASSERT(memberFunctionRefExp != NULL);
               returnSymbol = memberFunctionRefExp->get_symbol();

            // DQ (2/8/2009): Can we assert this! What about pointers to functions?
               ROSE_ASSERT(returnSymbol != NULL);
               break;
             }

          case V_SgArrowExp:
             {
            // The lhs is the this pointer (SgThisExp) and the rhs is the member function.
               SgArrowExp* arrayExp = isSgArrowExp(functionExp);
               ROSE_ASSERT(arrayExp != NULL);

               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(arrayExp->get_rhs_operand());

            // DQ (2/21/2010): Relaxed this constraint because it failes in fixupPrettyFunction test.
            // ROSE_ASSERT(memberFunctionRefExp != NULL);
               if (memberFunctionRefExp != NULL)
                  {
                    returnSymbol = memberFunctionRefExp->get_symbol();

                 // DQ (2/8/2009): Can we assert this! What about pointers to functions?
                    ROSE_ASSERT(returnSymbol != NULL);
                  }
               break;
             }

          case V_SgDotExp:
             {
               SgDotExp * dotExp = isSgDotExp(functionExp);
               ROSE_ASSERT(dotExp != NULL);

            // DQ (2/24/2013): Added assertion.
               ROSE_ASSERT(dotExp->get_rhs_operand() != NULL);
#if 0
               printf ("In SgFunctionCallExp::getAssociatedFunctionSymbol(): case V_SgDotExp: dotExp->get_rhs_operand() = %p = %s \n",dotExp->get_rhs_operand(),dotExp->get_rhs_operand()->class_name().c_str());
#endif
            // There are four different types of function call reference expression in ROSE.
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(dotExp->get_rhs_operand());
               if (memberFunctionRefExp == NULL)
                  {
                 // This could be a SgTemplateMemberFunctionRefExp (not derived from SgMemberFunctionRefExp or SgFunctionRefExp). See test2013_70.C
                    SgTemplateMemberFunctionRefExp* templateMemberFunctionRefExp = isSgTemplateMemberFunctionRefExp(dotExp->get_rhs_operand());
                    if (templateMemberFunctionRefExp == NULL)
                       {
                         SgTemplateFunctionRefExp* templateFunctionRefExp = isSgTemplateFunctionRefExp(dotExp->get_rhs_operand());
                         if (templateFunctionRefExp == NULL)
                            {
                              SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(dotExp->get_rhs_operand());
                              if (functionRefExp == NULL)
                                 {
                                   SgVarRefExp* varRefExp = isSgVarRefExp(dotExp->get_rhs_operand());
                                   if (varRefExp == NULL)
                                      {
                                        dotExp->get_rhs_operand()->get_file_info()->display("In SgFunctionCallExp::getAssociatedFunctionSymbol(): case of SgDotExp: templateMemberFunctionRefExp == NULL: debug");
                                        printf ("In SgFunctionCallExp::getAssociatedFunctionSymbol(): case of SgDotExp: dotExp->get_rhs_operand() = %p = %s \n",dotExp->get_rhs_operand(),dotExp->get_rhs_operand()->class_name().c_str());
                                      }
                                     else
                                      {
                                        ROSE_ASSERT(varRefExp != NULL);

                                     // DQ (8/20/2013): This is not a SgFunctionSymbol so we can't return a valid symbol from this case of a function call from a pointer to a function.
                                     // returnSymbol = varRefExp->get_symbol();
                                      }
                                 }
                                else
                                 {
                                // I am unclear when this is possible, but STL code exercises it.
                                   ROSE_ASSERT(functionRefExp != NULL);
                                   returnSymbol = functionRefExp->get_symbol();

                                // DQ (8/20/2013): Add assertion to the specific valide cases.
                                   ROSE_ASSERT(returnSymbol != NULL);
                                 }
                            }
                           else
                            {
                           // I am unclear when this is possible, but STL code exercises it.
                              ROSE_ASSERT(templateFunctionRefExp != NULL);
                              returnSymbol = templateFunctionRefExp->get_symbol();

                           // DQ (8/20/2013): Add assertion to the specific valide cases.
                              ROSE_ASSERT(returnSymbol != NULL);
                            }
                       }
                      else
                       {
                         ROSE_ASSERT(templateMemberFunctionRefExp != NULL);
                         returnSymbol = templateMemberFunctionRefExp->get_symbol();

                      // DQ (8/20/2013): Add assertion to the specific valide cases.
                         ROSE_ASSERT(returnSymbol != NULL);
                       }
                  }
                 else
                  {
                    ROSE_ASSERT(memberFunctionRefExp != NULL);
                    returnSymbol = memberFunctionRefExp->get_symbol();

                 // DQ (8/20/2013): Add assertion to the specific valide cases.
                    ROSE_ASSERT(returnSymbol != NULL);
                  }

            // DQ (8/20/2013): Function pointers don't allow us to generate a proper valid SgFunctionSymbol.
            // DQ (2/8/2009): Can we assert this! What about pointers to functions?
            // ROSE_ASSERT(returnSymbol != NULL);

           // Virtual functions called through the dot operator are resolved statically if they are not
           // called on reference types.
              isAlwaysResolvedStatically = !isSgReferenceType(dotExp->get_lhs_operand());

              break;
            }

       // DotStar (Section 5.5 of C++ standard) is used to call a member function pointer and implicitly specify
       // the associated 'this' parameter. In this case, we can't statically determine which function is getting called
       // and should return null.
          case V_SgDotStarOp:
             {
               break;
             }

       // ArrowStar (Section 5.5 of C++ standard) is used to call a member function pointer and implicitly specify
       // the associated 'this' parameter. In this case, we can't statically determine which function is getting called
       // and should return null.
          case V_SgArrowStarOp:
             {
               break;
             }

       // DQ (2/25/2013): Added support for this case, but I would like to review this (likely OK).
       // It might be that this should resolve to a symbol.
          case V_SgConstructorInitializer:
             {
               ROSE_ASSERT(functionExp->get_file_info() != NULL);
#if 0
               printf ("In SgFunctionCallExp::getAssociatedFunctionSymbol(): Found a case of SgConstructorInitializer \n");
               functionExp->get_file_info()->display("In SgFunctionCallExp::getAssociatedFunctionSymbol(): new case to be supported: checking this out: debug");
#endif
               break;
             }

       // DQ (2/25/2013): Added support for this case, but I would like to review this (likely OK).
          case V_SgFunctionCallExp:
             {
               ROSE_ASSERT(functionExp->get_file_info() != NULL);
#if 0
               printf ("In SgFunctionCallExp::getAssociatedFunctionSymbol(): Found a case of SgFunctionCallExp \n");
               functionExp->get_file_info()->display("In SgFunctionCallExp::getAssociatedFunctionSymbol(): new case to be supported: checking this out: debug");
#endif
               SgFunctionCallExp* nestedFunctionCallExp = isSgFunctionCallExp(functionExp);
               ROSE_ASSERT(nestedFunctionCallExp != NULL);

               returnSymbol = nestedFunctionCallExp->getAssociatedFunctionSymbol();
               break;
             }

       // DQ (2/25/2013): Added support for this case, verify as function call off of an array of function pointers.
       // This should not resolve to a symbol.
          case V_SgPntrArrRefExp:
             {
#if 0
               printf ("In SgFunctionCallExp::getAssociatedFunctionSymbol(): Found a case of SgPntrArrRefExp \n");
               ROSE_ASSERT(functionExp->get_file_info() != NULL);
               functionExp->get_file_info()->display("In SgFunctionCallExp::getAssociatedFunctionSymbol(): new case to be supported: checking this out: debug");
#endif
               break;
             }

       // DQ (2/25/2013): Added support for this case, from test2012_102.c.
       // Not clear if this should resolve to a symbol (I think likely yes).
          case V_SgCastExp:
             {
               break;
             }

       // DQ (2/25/2013): Added support for this case, from test2012_133.c.
       // This should not resolve to a symbol.
          case V_SgConditionalExp:
             {
               break;
             }

       // DQ (2/22/2013): added case to support someing reported in test2013_68.C, but not yet verified.
          case V_SgVarRefExp:
             {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("In SgFunctionCallExp::getAssociatedFunctionSymbol(): case of SgVarRefExp: returning NULL \n");
#endif
#if 0
               printf ("I would like to verify that I can trap this case \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          default:
             {
               ROSE_ASSERT(functionExp->get_file_info() != NULL);
               functionExp->get_file_info()->display("In SgFunctionCallExp::getAssociatedFunctionSymbol(): case not supported: debug");
               printf("Error: There should be no other cases functionExp = %p = %s \n", functionExp, functionExp->class_name().c_str());

#if 1
            // DQ (2/23/2013): Allow this to be commented out so that I can generate the DOT graphs to better understand the problem in test2013_69.C.
               ROSE_ASSERT(false);
#endif
             }
        }

  // If the function is virtual, the function call might actually be to a different symbol.
  // We should return NULL in this case to preserve correctness
     if (returnSymbol != NULL && !isAlwaysResolvedStatically)
        {
          SgFunctionModifier& functionModifier = returnSymbol->get_declaration()->get_functionModifier();
          if (functionModifier.isVirtual() || functionModifier.isPureVirtual())
             {
               returnSymbol = NULL;
             }
        }

     return returnSymbol;
   }
#endif


// DQ (10/19/2010): This is moved from src/ROSETTA/Grammar/Cxx_GlobalDeclarations.macro to here
// since it is code in the header files that we would like to avoid.  My fear is that because of
// the way it works it is required to be inlined onto the stack of the calling function.
// #ifndef ROSE_PREVENT_CONSTRUCTION_ON_STACK
// #define ROSE_PREVENT_CONSTRUCTION_ON_STACK
// inline void preventConstructionOnStack(SgNode* n)
void preventConstructionOnStack(SgNode* n)
   {
#ifndef NDEBUG
#ifdef _MSC_VER
  // DQ (11/28/2009): This was a fix suggested by Robb.
  // void* frameaddr = 0;
  // Build an "auto" variable (should be located near the stack frame, I think).
  // tps (12/4/2009)
#if _MSC_VER >= 1600  // 1600 == VC++ 10.0
  // ../Grammar/Cxx_GlobalDeclarations.macro(32): error C3530: 'auto' cannot be combined with any other type-specifier
     unsigned int nonStackFrameReferenceVariable;
    #pragma message ( " Cxx_GlobalDeclarations.macro: __builtin_frame_address not known in Windows. Workaround in VS 10.0")
#else
     auto unsigned int nonStackFrameReferenceVariable;
#endif
     void* frameaddr = &nonStackFrameReferenceVariable;

#else
  // GNU compiler specific code
     void* frameaddr = __builtin_frame_address(0);
#endif // _MSC_VER

     signed long dist = (char*)n - (char*)frameaddr;

  // DQ (12/6/2009): This fails for the 4.0.4 compiler, but only in 64-bit when run with Hudson.
  // I can't reporduce the problem using the 4.0.4 compiler, but it is uniformally a problem
  // since it fails on all tests (via hudson) which are using Boost 1.40 and either minimal or
  // full configurations (and also for the tests of the EDG binary).
  // assert (dist < -10000 || dist > 10000);

#ifdef __GNUC__
  // Note that this is a test of the backend compiler, it seems that we don't track
  // the compiler used to compile ROSE, but this is what we would want.

#if (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4) && (BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER == 0)
  // For the GNU 4.0.x make this a warning, since it appears to fail due to a
  // poor implementaiton for just this compiler and no other version of GNU.
  // Just that we are pringing this warning is a problem for many tests (maybe this should be enable within verbose mode).
     if (dist < -10000 || dist > 10000)
        {
#if 0
          printf ("Warning: preventConstructionOnStack() reporting object on stack, not allowed. dist = %ld \n",dist);
#endif
        }
#else
  // For all other versions of the GNU compiler make this an error.
     assert (dist < -10000 || dist > 10000);
#endif
#else
  // For all other compilers make this an error (e.g. on Windows MSVC, Intel icc, etc.).
     assert (dist < -10000 || dist > 10000);
#endif // __GNUC__

#endif // NDEBUG
   }
// #endif // ROSE_PREVENT_CONSTRUCTION_ON_STACK
