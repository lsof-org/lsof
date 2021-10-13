#!/usr/bin/perl
###################################################################
# identd.pl    : An implementation of RFC 1413 Ident Server
#                using Vic Abell's lsof.
#
# - Started from inetd with 'nowait' option. This entry in
#   /etc/inetd.conf will suffice :
#
#   ident   stream  tcp     nowait  root    /usr/local/bin/identd.pl -t200
#
# - Multiple instances of the server are not a performance penalty
#   since they shall use lsof's cacheing mechanism. (compare with
#   Peter Eriksson's pidentd)
# - Command line arguments :
#   -t TIMEOUT Number of seconds to wait for a query before aborting.
#              Default is 120.
#
# Kapil Chowksey <kchowksey@hss.hns.com>
# Nicholas Bamber <nicholas@periapt.co.uk>
###################################################################
use strict;
use Socket;
use Getopt::Long;

# Set path to lsof.

my $LSOF;
if (($LSOF = &isexec("../lsof")) eq "") {	# Try .. first
    if (($LSOF = &isexec("lsof")) eq "") {	# Then try . and $PATH
	print "can't execute $LSOF\n"; exit 1
    }
}

# redirect lsof's warnings/errors to /dev/null
close(STDERR);
open(STDERR, ">/dev/null");

my $Timeout = "120";

GetOptions('timeout=i'=>\$Timeout);

my ($port, $iaddr) = sockaddr_in(getpeername(STDIN));
my $peer_addr = inet_ntoa($iaddr);
my $query;

# read ident-query from socket (STDIN) with a timeout.
my $timeout = int($Timeout);
eval {
    local $SIG{ALRM} = sub { die "alarm\n" };
    alarm $timeout;
    $query = <STDIN>;
    alarm 0;
};
die if $@ && $@ ne "alarm\n";
if ($@) {
    # timed out
    exit;
}

# remove all white-spaces from query
$query =~ s/\s//g;

my $serv_port = "";
my $cli_port = "";
($serv_port,$cli_port) = split(/,/,$query);

if ($serv_port =~ /^[0-9]+$/) {
    if (int($serv_port) < 1 || int($serv_port) > 65535) {
        print $query." : ERROR : INVALID-PORT"."\n";
        exit;
    }
} else {
    print $query." : ERROR : INVALID-PORT"."\n";
    exit;
}

if ($cli_port =~ /^[0-9]+$/) {
    if (int($cli_port) < 1 || int($cli_port) > 65535) {
        print $query." : ERROR : INVALID-PORT"."\n";
        exit;
    }
} else {
    print $query." : ERROR : INVALID-PORT"."\n";
    exit;
}

open(LSOFP,"$LSOF -nPDi -T -FLn -iTCP@".$peer_addr.":".$cli_port."|");

my $user = "UNKNOWN";
while (my $a_line = <LSOFP>) {
    # extract user name.
    if ($a_line =~ /^L.*/) {
        ($user) = ($a_line =~ /^L(.*)/);
    }

    # make sure local port matches.
    if ($a_line =~ /^n.*:\Q$serv_port->/) {
        print $serv_port.", ".$cli_port." : USERID : UNIX :".$user."\n";
        exit;
    }
}

print $serv_port.", ".$cli_port." : ERROR : NO-USER"."\n";


## isexec($path) -- is $path executable
#
# $path   = absolute or relative path to file to test for executabiity.
#	    Paths that begin with neither '/' nor '.' that arent't found as
#	    simple references are also tested with the path prefixes of the
#	    PATH environment variable.

sub
isexec {
    my ($path) = @_;
    my ($i, @P, $PATH);

    $path =~ s/^\s+|\s+$//g;
    if ($path eq "") { return(""); }
    if (($path =~ m#^[\/\.]#)) {
	if (-x $path) { return($path); }
	return("");
    }
    $PATH = $ENV{PATH};
    @P = split(":", $PATH);
    for ($i = 0; $i <= $#P; $i++) {
	if (-x "$P[$i]/$path") { return("$P[$i]/$path"); }
    }
    return("");
}
