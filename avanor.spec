Name:    avanor
Version: 0.6.0
Release: 1%{?dist}
Summary: Dungeon exploration game in the tradition of Rogue and Moria
License: GPL-2.0-or-later
URL:     https://github.com/jaydg/avanor
Source:  %{url}/archive/refs/tags/%{version}/%{name}-%{version}.tar.gz

BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: pkgconf-pkg-config
BuildRequires: argparse-devel
BuildRequires: cereal-devel
BuildRequires: fmt-devel
BuildRequires: libzstd-devel
BuildRequires: luajit-devel
BuildRequires: sol2-devel

%description
Avanor is a single-player dungeon exploration game in the tradition of
Rogue and Moria, set in a valley the player leaves to find what lies
under it: caves, a dwarven city, and the wizard who put it all there.

Nearly everything in it - the creatures, the items, the levels, the
quests and the people who give them - is written in Lua rather than
compiled in, so the world can be changed without a compiler.

%prep
%setup -q

%build
# DATA_DIR is compiled into the binary and is where it looks for the world
# and the manual once neither the working directory nor the directory the
# executable sits in has them - see vEnterDataDir() in engine/global.cpp.
# It has to match where %%install puts them below.
make %{?_smp_mflags} DATA_DIR=%{_datadir}/%{name}/

%install
mkdir -p %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_datadir}/%{name}
install -m 0755 avanor %{buildroot}%{_bindir}/avanor
cp -a world manual %{buildroot}%{_datadir}/%{name}/

%check
# The game's test modes want a terminal, which a build root has not got.
# --version is answered by the argument parser before the display is
# opened, so it is the one thing worth asking here: it proves the binary
# links and starts.
./avanor --version

%files
%license COPYING
%doc README.md CHANGELOG.md
%{_bindir}/avanor
%{_datadir}/%{name}/

%changelog
* Sat Sep 19 2026 Joachim de Groot <jdegroot@web.de> - 0.6.0-1
- Initial spec file. The Version above is a placeholder: CI rewrites it
  to match `make -s version` before every build.
