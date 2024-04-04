%global groupname dpch560
%global username  dpch560
%global homedir   /


Name:           dp-ch560-daemon
Version:        0.1
Release:        1%{?dist}
Summary:        Deep Cool CH560 case digital monitoring display daemon

License:        GPL v2
URL:            https://github.com/mike-hmelov/dpch560
Source0:        %{name}-%{version}.tar.gz

#Requires(pre):  shadow-utils

BuildRequires:  clang
BuildRequires:  make
BuildRequires:  cmake
BuildRequires:  systemd
BuildRequires:  systemd-rpm-macros

%description
Deep Cool CH560 case digital monitoring display daemon


%pre
getent group %{groupname} >/dev/null || groupadd -r %{groupname}
getent passwd %{username} >/dev/null || \
    useradd -r -g %{groupname} -d %{homedir} -s /sbin/nologin \
    -c "User used for running example of daemon" %{username}
exit 0


# Section for preparation of build
%prep

# Following macro just has to be here. It unpacks the original source from
# tag.gz archive. It is "interesting" that rpmbuild does not do this
# automatically, when Source0 is defined, but you have to call it explicitly.
%setup -q


# Build section
%build

# We have to use build type "Debug" to be able to create all variants of
# rpm packages (debuginfo, debug source). The normal rpm is stripped from
# debug information. Following macro just run cmake and it generates Makefile
%cmake -DCMAKE_BUILD_TYPE="Debug"

# This macro runs make -f Makefile generated in previous step
%cmake_build


# Install section
%install

# Remove previous build results
rm -rf $RPM_BUILD_ROOT

# This macro runs make -f Makefile install and it installs
# all files to $RPM_BUILD_ROOT
%cmake_install


# This is special section again. You have to list here all files
# that are part of final RPM package. You can specify owner of
# files and permissions to files
%files

# Files and directories owned by root:root
%attr(755,root,root) %{_bindir}/dp-ch560-daemon
%attr(755,root,root) %dir %{_sysconfdir}/dp-ch560
%attr(644,root,root) %{_unitdir}/dp-ch560-display-daemon.service

# File owned by root, but group can read it
%attr(640,root,%{groupname}) %{_sysconfdir}/dp-ch560/display-daemon.conf

# Files and directories owned by daemoner:daemoner user
%attr(755,%{username},%{groupname}) %{_var}/log/dp-ch560
%attr(755,%{username},%{groupname}) %{_rundir}/dp-ch560



# This is section, where you should describe all important changes
# in RPM
%changelog
* Thu Apr 4 2024 Mihail Hmelov <darkdemon@inbox.lv>
- First version of display daemon
