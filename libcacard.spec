# Copyright 2024 Wong Hoi Sing Edison <hswong3i@pantarei-design.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

%global debug_package %{nil}

%global source_date_epoch_from_changelog 0

%global _lto_cflags %{?_lto_cflags} -ffat-lto-objects

Name: libcacard
Epoch: 100
Version: 2.8.1
Release: 1%{?dist}
Summary: Virtual Common Access Card (CAC) Emulator
License: LGPL-2.1-or-later
URL: https://github.com/freedesktop/spice-libcacard/tags
Source0: %{name}_%{version}.orig.tar.gz
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: glib2-devel
BuildRequires: gnupg2
BuildRequires: gnutls-utils
BuildRequires: meson
BuildRequires: ninja-build
BuildRequires: nss-devel
BuildRequires: nss-tools
BuildRequires: opensc
BuildRequires: openssl
BuildRequires: pkgconfig

%description
This emulator is designed to provide emulation of actual smart cards to
a virtual card reader running in a guest virtual machine. The emulated
smart cards can be representations of real smart cards, where the
necessary functions such as signing, card removal/insertion, etc. are
mapped to real, physical cards which are shared with the client machine
the emulator is running on, or the cards could be pure software
constructs.

%prep
%autosetup -T -c -n %{name}_%{version}-%{release}
tar -zx -f %{S:0} --strip-components=1 -C .

%build
%meson \
    -Dpcsc=disabled \
    -Ddisable_tests=true
%meson_build

%install
%meson_install
sed -i '/Requires.private: nss/d' %{buildroot}%{_libdir}/pkgconfig/libcacard.pc

%check

%if 0%{?suse_version} > 1500 || 0%{?sle_version} > 150000
%package -n libcacard0
Summary: Virtual Common Access Card (CAC) Emulator

%description -n libcacard0
This emulator is designed to provide emulation of actual smart cards to
a virtual card reader running in a guest virtual machine. The emulated
smart cards can be representations of real smart cards, where the
necessary functions such as signing, card removal/insertion, etc. are
mapped to real, physical cards which are shared with the client machine
the emulator is running on, or the cards could be pure software
constructs.

%package -n libcacard-devel
Summary: Virtual Common Access Card (CAC) Emulator
Requires: libcacard0 = %{epoch}:%{version}-%{release}

%description -n libcacard-devel
This package contains the development headers and library for libcacard.

%post -n libcacard0 -p /sbin/ldconfig
%postun -n libcacard0 -p /sbin/ldconfig

%files -n libcacard0
%license COPYING
%{_libdir}/*.so.*

%files -n libcacard-devel
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/pkgconfig/libcacard.pc
%endif

%if !(0%{?suse_version} > 1500) && !(0%{?sle_version} > 150000)
%package -n libcacard-devel
Summary: Virtual Common Access Card (CAC) Emulator
Requires: libcacard = %{epoch}:%{version}-%{release}

%description -n libcacard-devel
This package contains the development headers and library for libcacard.

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%license COPYING
%{_libdir}/*.so.*

%files -n libcacard-devel
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/pkgconfig/libcacard.pc
%endif

%changelog
