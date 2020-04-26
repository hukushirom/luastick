Summary: A library of generic C modules.
Name: libmba
Version: 0.9.0
Release: 1
Group: Development/Libraries
Source: http://www.ioplex.com/~miallen/libmba/dl/libmba-%{version}.tar.gz
URL: http://www.ioplex.com/~miallen/libmba/
Copyright: MIT/X Consortium
Prefix: %{_prefix}
BuildRoot: %{_tmppath}/%{name}-root
Autoreq: 0
Packager: Michael B. Allen <mba2000 ioplex.com>

%description
The libmba package is a collection of mostly independent C modules
potentially useful to any project. There are the usual ADTs including
linkedlist, hashmap, pool, stack, and varray, a flexible memory allocator,
CSV parser, I18N text abstraction, configuration file module, portible
semaphores, condition variables and more. The code is designed so that
individual modules can be integrated into existing codebases rather than
requiring the user to commit to the entire library. The code has no
typedefs, few comments, and extensive man pages and HTML documentation. 

%prep
%setup -q

%build
make

%install
rm -rf $RPM_BUILD_ROOT
make install prefix=${RPM_BUILD_ROOT}%{_prefix} libdir=${RPM_BUILD_ROOT}%{_libdir} includedir=${RPM_BUILD_ROOT}%{_includedir} mandir=${RPM_BUILD_ROOT}%{_mandir}

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%doc README.txt docs
%{_libdir}/libmba*
%{_includedir}/mba/*
%{_mandir}/man3/*

%changelog
* Sat Aug 28 2004 Michael B. Allen <mba2000 ioplex.com>
- 0.8.10

* Fri May 21 2004 Michael B. Allen <mba2000 ioplex.com>
- 0.8.9

* Sat May 8 2004 Michael B. Allen <mba2000 ioplex.com>
- 0.8.8

* Wed Mar 10 2004 Michael B. Allen <mba2000 ioplex.com>
- 0.8.5

* Thu Jan 3 2004 Michael B. Allen <mba2000 ioplex.com>
- 0.8.0

* Wed Oct 15 2003 Michael B. Allen <mba2000 ioplex.com>
- 0.7.0

* Sat Aug 25 2003 Michael B. Allen <mba2000 ioplex.com>
- 0.6.15

* Mon May 17 2003 Michael B. Allen <mba2000 ioplex.com>
- 0.6.1

* Mon May 5 2003 Michael B. Allen <mba2000 ioplex.com>
- 0.6.0

* Tue Apr 29 2003 Michael B. Allen <mba2000 ioplex.com>
- 0.5.2

* Tue Apr 1 2003 Michael B. Allen <mba2000 ioplex.com>
- 0.5.1

* Sat Mar 22 2003 Michael B. Allen <mba2000 ioplex.com>
- 0.5.0

* Sat Nov 23 2002 Michael B. Allen <mba2000 ioplex.com>
- 0.4.6

* Sat Nov 16 2002 Michael B. Allen <mba2000 ioplex.com>
- 0.4.5

* Thu Mar 28 2002 Michael B. Allen <mba2000 ioplex.com>
- Create.
