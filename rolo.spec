# RPM spec file for Red Hat Linux
# $Id: rolo.spec_rh,v 1.1 2003/05/14 12:39:29 ahsu Exp $
Summary: Text-based contact management software.
Name: rolo
Version: 011
Release: 1
Source: http://osdn.dl.sourceforge.net/sourceforge/rolo/%{name}-%{version}.tar.gz
License: GPL
URL: http://rolo.sf.net/
Packager: Andrew Hsu <ahsu@users.sf.net>
Distribution: Red Hat Linux
Vendor: Andrew Hsu
Group: Applications/Productivity
BuildRoot: %{_tmppath}/%{name}-buildroot
BuildRequires: libvc
Prefix: /usr

%description
Rolo keeps track of contacts and displays with a text-based menu. It
strives to be a well-constructed tool for complementing text-based
email programs. It utilizes the vCard version 3.0 format for storing its
contacts and interfaces with the end-user through an NCurses front-end.

%prep
%setup -q

%build
./configure --prefix=/usr --mandir=/usr/share/man
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/*
%{_mandir}/man1/*
%doc AUTHORS ChangeLog COPYING INSTALL NEWS README THANKS TODO test/contacts.vcf doc/rfc2426.txt
