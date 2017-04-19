%define __usrdir /usr/lib/systemd/system

Name:       docker-launcher
Summary:    docker launcher
Version:    0.0.1
Release:    0
Group:      Docker Framework
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1:    %{name}.manifest
Source3: 	%{name}.service
Source4: 	%{name}-client.manifest
BuildRequires:  cmake
BuildRequires:  boost-devel
BuildRequires:  gcc-c++
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gio-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(capi-system-system-settings)
BuildRequires:  pkgconfig(cynara-client)
BuildRequires:  pkgconfig(cynara-session)
BuildRequires:  pkgconfig(cynara-creds-gdbus)
BuildRequires:  pkgconfig(mused)
BuildRequires:  pkgconfig(libcurl)
BuildRequires:  pkgconfig(capi-network-wifi)
BuildRequires:  pkgconfig(json-c)
Requires(post): /usr/bin/getent, /usr/sbin/useradd, /usr/sbin/groupadd
Requires(post): /sbin/ldconfig, /usr/bin/systemctl
Requires(postun): /sbin/ldconfig, /usr/bin/systemctl

%description
docker launcher

%package devel
Summary:    Docker launcher(devel)
Group:      Docker Framework
Requires:   %{name} = %{version}

%description devel
docker launcher development Kit

%package client
Summary:	Docker launcher client
Group:		Docker Framework
Requires:	%{name} = %{version}

%description	client
docker launcher client programs

%prep
%setup -q
chmod g-w %_sourcedir/*
cp %{SOURCE1} ./%{name}.manifest
cp %{SOURCE3} ./%{name}.service
cp %{SOURCE4} ./%{name}-client.manifest

%build
TZ_VER_3=1

export LDFLAGS+="-Wl,--as-needed"
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DMAJORVER=${MAJORVER} -DFULLVER=%{version} -DBIN_INSTALL_DIR:PATH=%{_bindir}

%install
%make_install
mkdir -p %{buildroot}/usr/lib/systemd/system/multi-user.target.wants
install -m 0644 %SOURCE3 %{buildroot}/usr/lib/systemd/system/docker-launcher.service
ln -s ../docker-launcher.service %{buildroot}/usr/lib/systemd/system/multi-user.target.wants/

%post
/sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_bindir}/docker-launcher*
%license LICENSE.APLv2
/usr/lib/systemd/system/%{name}.service
/usr/lib/systemd/system/multi-user.target.wants/%{name}.service

%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_bindir}/docker-launcher*
%{_libdir}/pkgconfig/%{name}.pc

%files client
%manifest %{name}-client.manifest
%defattr(-,root,root,-)
%{_bindir}/dockerl*
%license LICENSE.APLv2

