Name:           kdstatemachineeditor
Version:        1.2.8
Release:        1
Summary:        State Machine Editor Library
Source0:        %{name}-%{version}.tar.gz
Source1:        %{name}-%{version}.tar.gz.asc
Url:            https://github.com/KDAB/KDStateMachineEditor
Group:          System/Libraries
License:        LGPL-2.1-only
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Vendor:         Klaralvdalens Datakonsult AB (KDAB)
Packager:       Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>

BuildRequires: cmake graphviz-devel
%if %{defined suse_version}
BuildRequires: libqt5-qtdeclarative-devel libqt5-qtdeclarative-private-headers-devel libQt5Test-devel libqt5-qtxmlpatterns-devel
%endif

%if %{defined fedora}
BuildRequires: qt5-qtbase-devel qt5-qtdeclarative-devel qt5-qtxmlpatterns-devel util-linux
%if 0%{?fedora} > 23
BuildRequires: ghostscript
%endif
%if 0%{?fedora} > 30
BuildRequires: qt5-qtbase-private-devel
%endif
%endif

%description
The KDAB State Machine Editor Library is a framework that can be used to help develop
full-featured State Machine Editing graphical user interfaces and tools.  Output from such
applications is in metacode or QML that can then be used in larger Qt or QtQuick projects.

Authors:
--------
      Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>

%define debug_package %{nil}
%global __debug_install_post %{nil}

%package devel
Summary: Development files for %{name}
Group: Development/Libraries
Requires: %{name} = %{version}

%description devel
This package contains headers and libraries necessary to develop programs
using the KDAB State Machine Editor library.

%prep
%autosetup

%build
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DLIB_INSTALL_DIR=%{_lib} -DCMAKE_BUILD_TYPE=Release -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot} %{?_smp_mflags}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_libdir}/lib*.so.*
%doc ReadMe* LICENSE*

%files devel
%defattr(-,root,root)
%dir %{_includedir}/%{name}
%{_includedir}/%{name}/*
%dir %{_libdir}/cmake/KDSME
%{_libdir}/cmake/KDSME/*
%{_libdir}/lib*.so
%{_libdir}/qt5/mkspecs/modules/*

%changelog
* Wed Aug 12 2020 Allen Winter <allen.winter@kdab.com> 1.2.8
  1.2.8 Patch Release
* Sat Jan 11 2020 Allen Winter <allen.winter@kdab.com> 1.2.7
  1.2.7 Patch Release
* Mon Dec 16 2019 Allen Winter <allen.winter@kdab.com> 1.2.6
  1.2.6 Patch Release
* Sat Feb 16 2019 Allen Winter <allen.winter@kdab.com> 1.2.5
  1.2.5 Patch Release
* Thu Jun 21 2018 Allen Winter <allen.winter@kdab.com> 1.2.4
  1.2.4 Patch Release
* Wed Feb 28 2018 Allen Winter <allen.winter@kdab.com> 1.2.3
  1.2.3 Patch Release
* Mon Jan 29 2018 Allen Winter <allen.winter@kdab.com> 1.2.2
  1.2.2 Patch Release
* Mon Jun 19 2017 Allen Winter <allen.winter@kdab.com> 1.2.1
  1.2.1 Bugfix Release
* Fri Jan 20 2017 Allen Winter <allen.winter@kdab.com> 1.2.0
  1.2.0 Minor Release
* Thu Jun 30 2016 Allen Winter <allen.winter@kdab.com> 1.1.0
  1.1.0 Minor Release
* Mon Dec 14 2015 Allen Winter <allen.winter@kdab.com> 1.0.0
  1.0.0 Final
* Fri Dec 11 2015 Allen Winter <allen.winter@kdab.com> 0.0.99
  1.0 Release Candidate
