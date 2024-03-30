# Getting started

You can get lsof by building from source or installing from package managers.

Due to the nature that lsof talks to the kernel, it requires many build-time checks to enable support for optional features. If you are installing lsof from package manager, chances are that the build machine has a different configuration from your own ones.

If the prebuilt binary from package manager misses some feature or malfunctions, you should build lsof from source.

## Build from source

You can retrieve lsof sources from [GitHub Releases](https://github.com/lsof-org/lsof/releases) or via git. lsof currently uses two build systems:

- Legacy: supports more OSes
- Autotools(experimental): supports Linux/Darwin/FreeBSD

### Legacy build system

You should use the legacy build system if you are building lsof on old OSes or do not want to risk. The legacy build system supports the following UNIX dialects:

- aix|aixgcc              : IBM AIX xlc (aix) or gcc (aixgcc)
- darwin                  : Apple Darwin
- freebsd                 : FreeBSD
- hpux|hpuxgcc            : HP-UX cc (hpux) or gcc (hpuxgcc)
- linux                   : Linux
- netbsd                  : NetBSD
- openbsd                 : OpenBSD
- osr|sco                 : SCO OpenServer < 6.0.0, SCO developer's compiler
- osrgcc|scogcc           : SCO OpenServer < 6.0.0, gcc compiler
- osr6                    : SCO OpenServer 6.0.0, SCO compiler
- solaris|solariscc       : Solaris gcc (solaris) or cc (solariscc)
- unixware|uw             : SCO|Caldera UnixWare

You can build lsof with the following commands:

```shell
./Configure <dialect>
make
```

Testing:

```shell
bash check.bash <dialect>
```

### Autotools-based build system

Since 4.97.0, lsof introduces a new experimental autotools-based build system. It supports the following OSes:

- Linux
- Darwin
- FreeBSD
- NetBSD

Build dependencies:

- c compiler: gcc/clang/etc
- autotools: autoconf/automake
- make
- pkg-config: for package detection
- groff: to process manpages
- libtirpc(optional): if your libc does not provide rpc
- nc/iproute2(optional): for testing

You can build lsof with the following commands:

```shell
# If you are building from git sources
# autoreconf -vif
./configure
make
make install
```

Testing:

```shell
make check
```

## Install from package managers

lsof is packaged in many OSes. You can install lsof from package managers:

If you use Debian/Ubuntu/apt-based Linux distros:

```shell
sudo apt install lsof
```

If you use RHEL/CentOS/yum-based Linux distros:

```shell
sudo yum install lsof
```

If you use Arch/pacman-based Linux distros:

```shell
sudo pacman -Syu lsof
```

If you use NixOS/nix-based Linux distros:

```shell
nix-env -i lsof
```
