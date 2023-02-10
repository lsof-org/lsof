# lsof (LiSt Open Files)

**lsof** is a command for `LiSting Open Files`. You can use lsof for example to:

- Find uses of a specific open file: `lsof /path/to/file`
- Find an unlinked open file: `lsof +L1`
- Find processes blocking umount: `lsof /mnt`
- Find tcp/udp sockets: `lsof -i`
- Find files open to a process with known PID: `lsof -p 1234`
- Find files open to a named command: `lsof -c bash`
- Find files open by a specific user: `lsof -u somebody`

## History

lsof was originally developed and maintained by Vic Abell since 1994. The [lsof-org team at GitHub](https://github.com/lsof-org/lsof) takes over the maintainership of lsof. You can find the latest release at [GitHub Release](https://github.com/lsof-org/lsof/releases).

## OS Support

Actively maintained and supported:

- Linux
- FreeBSD
- Darwin(macOS)
- NetBSD
- OpenBSD
- Solaris/OpenIndiana

Not maintained for lack of maintainers but pull requests are welcome:

- IBM AIX
- HP-UX
- SCO OpenServer
- UnixWare

lsof is tested in ci on the following platforms:

- Alpine Linux 3.17
- Arch Linux
- CentOS 8/9
- Debian 11
- Fedora 36/37
- FreeBSD 12/13/14
- macOS Big Sur
- NixOS
- openSUSE Leap 15
- Ubuntu 18.04/20.04/22.04
- NetBSD 9
- OpenBSD 7

Additionally, lsof is tested by maintainers manually on the following platforms:

- Solaris 11
- OpenIndiana 5

lsof is provided by package manager in the following repositories:

<a href="https://repology.org/project/lsof/versions">
    <img src="https://repology.org/badge/vertical-allrepos/lsof.svg" alt="Packaging status">
</a>