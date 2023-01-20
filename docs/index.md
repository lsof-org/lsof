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

Not maintained for lack of maintainers but pull requests are welcome:

- NetBSD
- OpenBSD
- Solaris
- IBM AIX
- HP-UX
- SCO OpenServer
- UnixWare

lsof is tested in ci on the following platforms:

- Debian 11
- NixOS
- CentOS 8/9
- Fedora 36/37
- openSUSE Leap 15
- Arch Linux
- Ubuntu 18.04/20.04/22.04
- FreeBSD 12/13/14
- macOS Big Sur