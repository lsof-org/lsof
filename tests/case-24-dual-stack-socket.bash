#!/usr/bin/env bash
# Regression test for dual-stack IPv6 sockets.
#
# A dual-stack socket (AF_INET6 with IPV6_V6ONLY=0) is an IPv6 network file.
# Lsof.8 says an IPv4 address mapped in an IPv6 address is selected by '6',
# not '4'.  The accepted end of an IPv4 connection is still IPv6; the
# separate IPv4 client socket is IPv4.  Mapped addresses may be displayed
# in IPv4 form, but TYPE and -i selection use IPv6.
source tests/common.bash

case "${dialect:-}" in
    linux|darwin|freebsd|netbsd|openbsd) ;;
    *)
        echo "SKIP: dual-stack socket test is only for the BSD and Linux dialects" \
            >> "$report"
        exit 77
        ;;
esac

if ! command -v python3 >/dev/null 2>&1; then
    echo "SKIP: python3 not available" >> "$report"
    exit 77
fi

python3 - "$lsof" <<'PY' >> "$report" 2>&1
import os
import socket
import subprocess
import sys

lsof = sys.argv[1]

try:
    listener = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
    listener.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_V6ONLY, 0)
    listener.bind(("::", 0))
    listener.listen(1)
    listener.settimeout(5)
    port = listener.getsockname()[1]

    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(("127.0.0.1", port))
    accepted, addr = listener.accept()

    native = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
    native.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_V6ONLY, 0)
    native.bind(("::1", 0))
    native.listen(1)
    native_port = native.getsockname()[1]

    udp = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
    udp.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_V6ONLY, 0)
    udp.bind(("::", 0))
    udp_port = udp.getsockname()[1]
except (AttributeError, OSError) as e:
    print(f"SKIP: dual-stack IPv6 sockets are not available: {e}")
    sys.exit(77)

pid = str(os.getpid())
fd_listener = str(listener.fileno())
fd_client = str(client.fileno())
fd_accepted = str(accepted.fileno())
fd_native = str(native.fileno())
fd_udp = str(udp.fileno())
base = [lsof, "-w", "-nP", "-a", "-p", pid, "-Fpftn"]


def run(opt):
    try:
        result = subprocess.run(base + [opt], capture_output=True, text=True,
                                timeout=10)
    except (OSError, subprocess.TimeoutExpired) as e:
        print(f"FAIL: lsof {opt}: {e}")
        return None
    files = {}
    fd = None
    for line in result.stdout.splitlines():
        if line.startswith("f"):
            fd = line[1:]
            files[fd] = {"type": "", "name": ""}
        elif fd is not None and line.startswith("t"):
            files[fd]["type"] = line[1:]
        elif fd is not None and line.startswith("n"):
            files[fd]["name"] = line[1:]
    return result.returncode, result.stderr, files


def fail(msg):
    print(f"FAIL: {msg}")
    sys.exit(1)


def query(opt):
    result = run(opt)
    if result is None:
        sys.exit(1)
    status, stderr, files = result
    if status not in (0, 1):
        fail(f"lsof {opt} exited with status {status}: {stderr}")
    return files


def expect(opt, expected, label):
    files = query(opt)
    if set(files) != set(expected):
        fail(f"{label}: lsof {opt} selected fds {sorted(files)}, "
             f"expected {sorted(expected)}")
    return files


def expect_type(files, fd, typ, label):
    if files[fd]["type"] != typ:
        fail(f"{label}: fd {fd} type is {files[fd]['type']!r}, "
             f"expected {typ}")


# Dual-stack listener and the accepted IPv4 connection are both IPv6.
ipv6 = expect(f"-i6:{port}", {fd_listener, fd_accepted},
              "IPv6 port filter")
expect_type(ipv6, fd_listener, "IPv6", "listener")
expect_type(ipv6, fd_accepted, "IPv6", "accepted IPv4 connection")

# The AF_INET client socket is the only IPv4 file in that connection.
ipv4 = expect(f"-i4:{port}", {fd_client}, "IPv4 port filter")
expect_type(ipv4, fd_client, "IPv4", "IPv4 client")

# IPv4-mapped address filters, with and without a port.
mapped = expect(f"-i6@[::ffff:127.0.0.1]:{port}", {fd_accepted},
                "mapped IPv6 address/port filter")
expect_type(mapped, fd_accepted, "IPv6", "mapped accepted connection")
mapped_no_port = expect("-i6@[::ffff:127.0.0.1]", {fd_accepted},
                        "mapped IPv6 address filter")
expect_type(mapped_no_port, fd_accepted, "IPv6", "mapped no-port filter")

# IPv4 address filters select the client, never the AF_INET6 accepted end.
ipv4_addr = expect(f"-i4@127.0.0.1:{port}", {fd_client},
                   "IPv4 address/port filter")
if f":{port}" not in ipv4_addr[fd_client]["name"]:
    fail(f"IPv4 client name does not contain port: "
         f"{ipv4_addr[fd_client]['name']!r}")
expect(f"-i4@127.0.0.1", {fd_client}, "IPv4 address filter")
expect(f"-i4@127.0.0.1:{native_port}", set(), "IPv4 address at IPv6 port")

# Native IPv6 listener.
native_files = expect(f"-i6@[::1]:{native_port}", {fd_native},
                      "native IPv6 address/port filter")
expect_type(native_files, fd_native, "IPv6", "native IPv6 listener")
if f"[::1]:{native_port}" not in native_files[fd_native]["name"]:
    fail(f"native IPv6 listener name is not native IPv6: "
         f"{native_files[fd_native]['name']!r}")
expect("-i6@[::1]", {fd_native}, "native IPv6 address filter")
expect(f"-i6@[::1]:{port}", set(), "native IPv6 address at other port")

# UDP dual-stack socket.
udp_files = expect(f"-i6:{udp_port}", {fd_udp}, "IPv6 UDP port filter")
expect_type(udp_files, fd_udp, "IPv6", "IPv6 UDP socket")
expect(f"-i4:{udp_port}", set(), "IPv4 filter on IPv6 UDP socket")
if f":{udp_port}" not in udp_files[fd_udp]["name"]:
    fail(f"IPv6 UDP name does not contain port: "
         f"{udp_files[fd_udp]['name']!r}")

# Bare family filters use the FnetTy/SELNET path, not address matching.
all_ipv6 = expect("-i6", {fd_listener, fd_accepted, fd_native, fd_udp},
                  "bare IPv6 filter")
for fd in (fd_listener, fd_accepted, fd_native, fd_udp):
    expect_type(all_ipv6, fd, "IPv6", "bare IPv6 filter")
expect("-i4", {fd_client}, "bare IPv4 filter")

# A different mapped address must not match.
expect(f"-i6@[::ffff:127.0.0.2]:{port}", set(),
       "non-matching mapped IPv6 address")

# IPv6 address syntax under -i4 is rejected by the option parser.
result = run(f"-i4@[::ffff:127.0.0.1]:{port}")
if result is None:
    sys.exit(1)
status, stderr, files = result
if status != 1 or "IPv6 addresses are prohibited" not in stderr:
    fail(f"-i4@[::ffff:127.0.0.1] was not rejected: status {status}, "
         f"stderr {stderr!r}, stdout fds {sorted(files)}")

# Accepted embedded IPv4 address is displayed in IPv4 form, not ::-form.
if f"127.0.0.1:{port}" not in ipv6[fd_accepted]["name"]:
    fail(f"accepted name is not IPv4-form: {ipv6[fd_accepted]['name']!r}")
if "[::" in ipv6[fd_accepted]["name"] or "::ffff:" in ipv6[fd_accepted]["name"]:
    fail(f"accepted name is not dotted IPv4: "
         f"{ipv6[fd_accepted]['name']!r}")

print(f"PASS: covered TCP listener/client/accepted, native IPv6, UDP, "
      f"bare/port/address filters, and mapped-address rejection")
sys.exit(0)
PY
status=$?
exit $status
