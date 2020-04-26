libmba
A library of generic C modules
http://www.ioplex.com/~miallen/libmba/

Fri Apr 29 01:21:04 EDT 2005
libmba-0.9.1 released

Forgot  ./ in front of mktool in Makefile and tcase/Makefile need -lutil. I
also fixed a typo and removed the mailing list links from the webpage. 

Thu Apr 28 04:12:31 EDT 2005
libmba-0.9.0 released

Portability  has  been  improved greatly. The library has been compiled and
tested  (albeit  not  extensively)  on  OSF1,  HP-UX,  Linux, Mac OS X, and
FreeBSD with gcc, DECC, and aC++.

The bitset(3m) module macros have been converted to functions so that using
expressions  as arguments (e.g. i++) does not result in undefined behavior.
Some semantics have also changed (e.g. return values are different). 

A  debug module has been added that provides some useful backtrace oriented
functions but it is not documented because it is specific GNUC on i386.

The  linkedlist_insert_sorted  function  has  been  modified  to  support a
context  parameter  that  is passed as-is to the supplied cmp_fn defined in
hashmap(3m). The <ident>compare_fn</ident> type has been removed.

The msgno(3m) module has been significantly reworked. The MSG, MNO, and MNF
macros  have  been  renamed  to MMSG, MMNO, and MMNF respectively to reduce
namespace  collisions.  There is no longer a dependance on variadic macros.
The code is now highly portable. The MSGNO macro is no longer used -- msgno
is now enabled at all times.

A path_name function has been added to the path(3m) module.

Some  parameters  of  shellout(3m) function have been changed from unsigned
char to char.

The  CSV  module  will  now exclude carriage returns preceeding newlines in
elements. 

A variety of bugfixes have been applied.

A  SUBA_PTR_SIZE macro has been added to the suba(3m) module that evaluates
to the size of the cell backing the suba allocated object.

The  SEM_UNDO  flag  has  been  been replaced with O_UNDO to avoid possible
collision with other O_* bits.

A varray_index function has been added. 

Sat Aug 28 00:42:30 EDT 2004
libmba-0.8.10 released

Two  minor  bugs  have been fixed in the csv module and the eval(3m) module
has  been  modified  to  support  a user supplied context parameter. Also a
WIN32  build issue has been fixed -- libmba builds cleanly in Windows again
(debug options by default).

The  bitset_find_first  function will set errno to ENOENT if the target bit
is not found. The associated documentation has also been updated.

Some  issues  regarding  the  initialization  of svsem semaphores have been
fixed.  The  module  should  now  properly  handle  the initialization race
outlined in Stevens' UNPv2 in addition to the scenario where a semaphore is
removed during initialization. 

Fri May 21 18:22:20 EDT 2004
libmba-0.8.9 released

The  sho_loop  function  now  acceptes  a  pattern  vector and timeout like
sho_expect.  The  cfg module has been modified to more closely support Java
Properties escape sequences for spaces and Unicode characters. 

Sat May  8 19:05:59 EDT 2004
libmba-0.8.8 released

The diff module has been added and the path module is now documented.

Wed Mar 10 02:01:51 EST 2004
libmba-0.8.5 released

The  hashmap_remove  function  did  not  work as advertised and could cause
other  functions to subsequently work improperly. The CSV functions now use
a  flag  instead of the trim parameter to permit controlling how quotes are
interpreted.  To preserve the previous behavior it will now be necessary to
specify  CSV_QUOTES.  The  domnode  module  has  been deprecated. Libmba no
longer  depends on Expat. Other modules are being considered for removal as
well.  Previously  the  svcond_wait function could return without returning
reaquiring  the  lock  if  a  signal  was  received. This bahavior has been
changed  so  that the function will return if a signal is recieved but will
not  return  without  relocking  the  specified  semaphore if the signal is
EINTR.  This  will  likely  need  further  adjustement  but it satisfies my
current use cases. 

Sat Jan  3 02:28:14 EST 2004
libmba-0.8.0 released

Three  have  been  significant  additions  and changes. There are seven new
modules;  allocator,  suba,  bitset, hashmap has been reimplemented, svsem,
svcond,  and  time.  Many  constructors  now accept the specification of an
allocator. Many constructors and deconstructors have changed. Many ADTs can
be  initialized  from memory provided by the user and many function pointer
parameters  have  changed.  There  are  "clean"  functions  (not completely
implemented yet) designed for use with the allocator's reclaim_fn. 

Wed Oct 15 04:39:40 EDT 2003
libmba-0.7.0 released

Microsoft  Windows  support  has  been  improved. The Win32 debug build now
properly  creates DLLs with PDB information for listing source code after a
memory fault. The standard __cplusplus macro guards have been added. Macros
for  prefixing __declspec(dllexport) directives have been added in favor of
an  explicit  DEF  file.  These  changes  have  been  performed  during the
development  of  a  non-trial  MFC  application so this release should work
smoothly  in a Win32 or MFC environment. The text module appears to work as
advertised although a few adjustments have been made.

The csv module has been converted to support the text module text handling.
The  multibyte  function  is  now  csv_row_parse_str,  the  wide  character
function  is  csv_row_parse_wcs,  and  the  csv_row_parse function is now a
macro that accepts tchar parameters. The prototypes of these functions have
also been changed to accept the specification of the separator that is used
(e.g. '\t' rather than ',').

A  new  eval  module  has  been added that will "calculate" the value of an
expression  such  as  '(5 + 3) * N', '(10+10-((10*10/11)|(10&10)))^0xFF78',
etc.

The  msgno  functions  have been adjusted to perform better in environments
where variadic macros are not supported (e.g. MSVC). 

Mon Aug 25 15:02:44 EDT 2003
libmba-0.6.15 released

There have been significant and pervasive changes however to emphasize that
all of these changes are binary compatible I have not incremented the major
version  number.  All  code  that  uses  the published interfaces of libmba
should work without modification.

The  most  significant  change  is  the  addition  of  src/mba/text.h which
contains  many  macros  that abstract wide and multi-byte string functions.
Depending  on  whether  or  not  USE_WCHAR  is defined the string functions
defined  in text.h will accept wide or multi-byte strings. This will permit
programs  to  run  using  wide  character or local dependent multibyte text
behavior.  Some  of  the  libmba modules such as cfg have been converted to
support  both  wide and local dependent multi-byte text using the new tchar
typedef. Do not be alarmed that these prototypes have changed. Because this
typedef  is  defined as either unsigned char or wchar_t you can continue to
use these modules without using tchar at all. If however you choose to take
advantage of this new I18N functionality please read the following document
for important information:

  http://www.ioplex.com/~miallen/libmba/dl/docs/ref/text_details.html

The  test suite has been cleaned up considerably. Just run make followed by
the generated tmba program in the tcase directory to run all tests.

The  build  process  has  been formalized further. The code is now compiled
using  -D_XOPEN_SOURCE=500  meaning  SUSv2/UNIX98 but most of the code does
not  require  this  standards  level.  In  fact  #ifdefs have been added to
consider lesser environments although they have not been thoroughly tested.

Finally,  a path module has been introduced. Currently this module contains
one  function; path_canon which canonicalizes a pathname. The state machine
design  is  very  safe  when  given  the full range of possible inputs (see
tcase/tests/data/PathCanonExamples.data). 

Sat May 17 16:30:37 EDT 2003

Multiple bugs have been fixed in the csv module.

Mon May  5 23:44:43 EDT 2003

There have been siginificant bugs fixed and changes.

The  iterate  and  next  functions of the linkedlist, cfg (now documented),
hashmap,  pool,  stack,  and  varray  modules  now  accept an iter_t object
defined  in  new  mba/iterator.h  header to hold the state of an iteration.
These  modules  now  permit  multiple  iterators  per  object and are fully
reentrant.

The library has been profiled with ccmalloc. Several memory leaks have been
detected and repaired. The shellout module was not freeing the struct sho *
in  sho_close.  The  linkedlist_insert_sorted function (now documented) did
not  free  a  struct  entry  * if it was the first element and the replaced
parameter  was  specified.  The  cfg_store  function did not close the file
pointer  it  opened.  The cfg_del function did not delete the struct cfg *.
The  linkedlist_remove_last  function did not free the struct node * if the
list contained only one element. 

The  linkedlist_get  function  has  been  optimized  for forward-sequential
access  (an  O(1) operation now whereas previously it was O(log N)). It may
also be faster for non-sequential access altough it will likely be slightly
slower  for  reverse-sequential access. A small cache of 2 elements is used
to save pointers recently accessed elements. The cache is adjustable with a
change  to  the source but 2 elements is probably optimal because it covers
the case of two nested iterators.

The  linkedlist_insert_sorted  function has been reviewed and documented as
an  official  part  of  the linkedlist interface. As mentioned previously a
memory leak in that function was found and fixed. 

Tue Apr 29 16:08:21 EDT 2003

A  csv module has been added. This module properly parses the CSV format in
that  it  can  handle  quoted elementes, quotes within quoted elements, and
commas  and  quotes  withing  quotes.  It should be able to handle anything
exported by Excel or Gnumeric (but don't quote me on that :-) 

Tue Apr  1 19:18:39 EST 2003

The  varray  module did not zero out the array pointing to allocated memory
chunks.  This  could  lead  to dereferencing an invalid pointer. The bug is
fixed in this release. 

Sat Mar 22 04:51:01 EST 2003

libmba-0.5.0 released

This  release introduces the varray module which is a "variable array" that
dynamically allocates storage in increasingly larger chunks are elements of
an index are accessed. The documentation has also been improved. 

Fri Dec 20 00:30:32 EST 2002

libmba-0.4.7 released

A  pool module has been added for reusing objects that are costly to create
like large buffers, database connections, etc. 

Sat Nov 23 01:16:43 EST 2002

libmba-0.4.6 released

A  symbol  was  left  out of the MSVC .def file and a mailing list has been
established for questions, suggestions, and patches. 

Sat Nov 16 17:53:42 EST 2002

libmba-0.4.5 released

A  cfg  module  for  loading  and  storing properties files has been added,
portability  has  been  improved,  and the web page, API reference, and man
pages  are  now  generated  with  the CStyleX package. There have also been
numerous other small adjustments to the code. 

--

INSTALLATION

Edit the 'prefix' line in the Makefile and run: 

  # make install

If you only require a select module, some can be
extracted  from  the  package without too much work.

On  other  platforms it may only be necessary to adjust src/defines.h. With
everything  turned off in defines.h the code is ANSI although natrually all
possible combinations have not been tested. 

>>IMPORTANT<<
The  msgno  module is disabled by default. To activate it, define the MSGNO
macro before including the msgno.h header or set it with a -DMSGNO compiler
flag. 

See README.win32 regarding Windows installation.

