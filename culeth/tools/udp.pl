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
my $CULSERVER_OP_ECHO=       'e';
my $CULSERVER_OP_IP  =       'i';
my $CULSERVER_OP_STAT=       's';
my $CULSERVER_OP_CC1101INFO= 'c';

my %CULSERVER_COMMANDS= (
	"get remote ip address" => "i",
	"get remote statistics" => "s",
	"get CC1101 parameters" => "c",
	"send CC1101 test packet" => "p",
	"blink" => "b"
);



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
	my $input;
	my $output;

	my $token= 0;

	while(1) {

		#
		# print menu
		#
		print "\n\n*** Remote host $REMOTEHOST\n";
		my $n;
		my $description;
		my @descriptions= keys(%CULSERVER_COMMANDS);
		for($n= 0; $n<= $#descriptions; $n++) {
			printf("(%d) %s\n", $n+1, $descriptions[$n]);
		}
		print "> ";
		#
		# user choice from menu
		#
		my $choice=<STDIN>;
		chomp $choice;
		if($choice !~ /[0-9]+/) { next; }
		$choice--;
		if($choice<0 || $choice>$#descriptions) { next; }
		$description= $descriptions[$choice];
		#
		# build server request
		#
		my $input= $CULSERVER_COMMANDS{$description};
		$token++;
		print "Command= $description => $input, token $token\n";
		$input.= pack("C", $token) . "\0\0";
		send(SOCKET, $input, 0, $port)== length($input)
			or die "cannot snd to $REMOTEHOST:$REMOTEPORT: $!";
		#
		# get server reply
		#
		my $operation;
		my $exitcode;
		my $length;
		my $text;
		my @b;
		my $tokenrecv;
		my $gotreply;
		do {
			(my $remotepaddr= recv(SOCKET, $output, 3+$MAXLEN, 0)) or die "recv: $!";
			my ($remoteport, $remoteiaddr)= sockaddr_in($remotepaddr);
			my $remoteip= inet_ntoa($remoteiaddr);
			print "reply from $remoteip\n";

			# split received text string into characters
			my @chars= split("", $output);
			# retrieve operation
			$operation= shift @chars;
			# treat rest as bytes
			($tokenrecv, $exitcode, $length, @b)= map { ord($_) } @chars;
			# text representation of buffer
			$text= substr($output, 4, $length);

			# check if we received the current package
			$gotreply= ($tokenrecv == $token);

			if(!$gotreply) {
				print "ERROR, received token $tokenrecv\n";
			}
		} until $gotreply;

		if($exitcode == $ACK) {
			print "< ACK operation= $operation " .
				"token= $tokenrecv length= $length (" .
				unpack("H*", $text) . ")\n";
			if($operation eq $CULSERVER_OP_ECHO) {
				print $text . "\n";
			} elsif($operation eq $CULSERVER_OP_IP) {
				printf("%d.%d.%d.%d\n", $b[0],$b[1],$b[2],$b[3]);
			} elsif($operation eq $CULSERVER_OP_STAT) {
				printf("RNDIS  in= %3d out= %3d\nCC1101 in= %3d out= %3d\n", $b[0],$b[1],$b[2],$b[3]);
			} elsif($operation eq $CULSERVER_OP_CC1101INFO) {
				printf("Partnum= %d\n", $b[0]);
				printf("Version= %d\n", $b[1]);
				printf("FreqEst= %d\n", $b[2]);
				printf("Wortime= %d\n", $b[7]*256+$b[6]);
				printf("TX     = %d bytes\n", $b[10]);
				printf("RX     = %d bytes\n", $b[11]);
			}

		} elsif($exitcode == $NAK) {
			print "< NAK\n";
		} else {
			die "< Illegal exit code";
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



