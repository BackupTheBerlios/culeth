#!/usr/bin/perl

use strict;
use IO::Socket;


my $REMOTEHOST	= '192.168.108.127';
my $REMOTEPORT	= 7073;
my $LOCALPORT	= 7073;
my $MAXLEN	= 16;

my $mode	= "client";

my $ACK		=  6;
my $NAK		= 21;

while($#ARGV>=0) {
	my $arg= $ARGV[0];

	if($arg eq "-c") {
		$mode= "client";
	}
	elsif($arg eq "-s") {
		$mode= "server";
	}
	shift @ARGV;
}

print "Entering $mode mode.\n";


if($mode eq "client") {

	#
	# client
	#

	socket(SOCKET, PF_INET, SOCK_DGRAM, getprotobyname('udp'))
		or die "socket: $@";

	my $ip  = inet_aton($REMOTEHOST);
	my $port= sockaddr_in($REMOTEPORT, $ip);
	my $text;
	my $exitcode;
	my $result;

	while(1) {
		print "$REMOTEHOST> ";
		$text=<STDIN>;
		chomp $text;
		$text.= "\0";
		send(SOCKET, $text, 0, $port)== length($text)
			or die "cannot snd to $REMOTEHOST:$REMOTEPORT: $!";

		recv(SOCKET, $text, 1+$MAXLEN, 0) or die "recv: $!";
		$exitcode= substr($text, 0, 1);
		if($exitcode eq chr($ACK) ) {
			print "ACK ";
			$result= substr($text, 1);
			my $raw= unpack("H*", $result);
			print "Raw result " . $raw . "\n";
			print "$result\n";
		} elsif($exitcode eq chr($NAK) ) {
			print "NAK\n";
		} else {
			die "Illegal exit code";
		}
	}


} elsif($mode eq "server") {

	#
	# server
	#

	my $socket= IO::Socket::INET->new(
		LocalPort => $LOCALPORT,
		Proto     => "udp")
		or die "socket: $@";

	my $text;

	while($socket->recv($text, $MAXLEN)) {
		print "$text";
		$socket->send($text) or die "send: $!";
	} die "recv: $!";


}



