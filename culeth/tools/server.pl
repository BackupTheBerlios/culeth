#!/usr/bin/perl -w

use strict;
use IO::Socket::INET;

#------------------------------------------------------------------------------
#
# configuration
#
#------------------------------------------------------------------------------

my $PORT= 7073;

#------------------------------------------------------------------------------
#
# functions
#
#------------------------------------------------------------------------------

sub
printbuffer($) {
	my $b= shift;
	my $l= length($b);
	my $line  = 0;
	my $column= 0;
	my $i;
	my $c;
	do {
		printf("  0x%04x:  ", $line*16);
		for($column= 0; $column< 16; $column++) {
			$i= $line*16+$column;
			if($i< $l) {
				$c= substr($b, $i, 1);
				printf(" %02x", ord($c));
			} else {
				print("   ");
			}
		}
		print("  ");
		for($column= 0; $column< 16; $column++) {
			$i= $line*16+$column;
			if($i< $l) {
				$c= substr($b, $i, 1);
				if((ord($c)>=32) & (ord($c)<=127)) {
					print $c;
				} else {
					print ".";
				}
			} else {
				print("   ");
			}
		}
		print("\n");
		$column= 0;
		$line++;
	} until($line*16>= $l);
}

#------------------------------------------------------------------------------
#
# main program
#
#------------------------------------------------------------------------------

print "UDP Server started.\n";

my $socket= new IO::Socket::INET->new(
		LocalPort => $PORT,
		Proto    => 'udp');

print "Listening on port $PORT...\n";
while(1) {
	my $buffer;
	my $clientpaddr= $socket->recv($buffer, 2048);
	my ($clientport, $clientiaddr)= sockaddr_in($clientpaddr);
	my $clientip= inet_ntoa($clientiaddr);
	my $clienthostname = gethostbyaddr($clientiaddr, AF_INET);
	$clienthostname= $clientip unless defined($clienthostname);
	print "message from $clienthostname [$clientip:$clientport]\n";
	printbuffer($buffer);
	exit if(chomp $buffer eq "exit");
}

$socket->close;

