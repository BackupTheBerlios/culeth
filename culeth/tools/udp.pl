#!/usr/bin/perl

use strict;
use IO::Socket;


my $REMOTEHOST	= '192.168.108.127';
my $REMOTEPORT	= 7073;
my $LOCALPORT	= 7073;
my $MAXLEN	= 128;

my $mode	= "client";

my $ACK		=  6;
my $NAK		= 21;
my $CULSERVER_OP_GETIP  	= 'i';
my $CULSERVER_OP_GETSTAT	= 's';
my $CULSERVER_OP_GETCC1101STATUS	= 'u';
my $CULSERVER_OP_GETCC1101CONFIG	= 'c';
my $CULSERVER_OP_GETTIME	= 't';
my $CULSERVER_OP_CC1101SEND 	= 'p';
my $CULSERVER_OP_BLINK	 	= 'b';
my $CULSERVER_OP_TEST		= '?';
my $CULSERVER_OP_BOOTLOADER	= '>';
my $CULSERVER_OP_EEPROM		= 'e';

my %CULSERVER_COMMANDS= (
	"get remote ip address" => $CULSERVER_OP_GETIP,
	"get remote statistics" => $CULSERVER_OP_GETSTAT,
	"get time" => $CULSERVER_OP_GETTIME,
	"get CC1101 status" => $CULSERVER_OP_GETCC1101STATUS,
	"send CC1101 test packet" => $CULSERVER_OP_CC1101SEND,
	"blink" => $CULSERVER_OP_BLINK,
	"test feature" => $CULSERVER_OP_TEST,
	"get CC1101 configuration" => $CULSERVER_OP_GETCC1101CONFIG,
	"start bootloader" => $CULSERVER_OP_BOOTLOADER,
	"get eeprom" => $CULSERVER_OP_EEPROM,
);

my @MARCSTATE= (
	"SLEEP",
	"IDLE",
	"XOFF",
	"VCOON_MC",
	"REGON_MC",
	"MANCAL",
	"VCOON",
	"REGON",
	"STARTCAL",
	"BWBOOST",
	"FS_LOCK",
	"IFADCON",
	"ENDCAL",
	"RX",
	"RX_END",
	"RX_RST",
	"TXRX_SWITCH",
	"RXFIFO_OVERFLOW",
	"FSTXON",
	"TX",
	"TX_END",
	"RXTX_SWITCH",
	"TXFIFO_UNDERFLOW",
 );


sub
bit($$) {
	my ($byte, $bit)= @_;
	return(($byte >> $bit) & 1);
}

sub
printbuffer($) {
	my $b= shift;
	my $l= length($b);
	my $i;
	for($i= 0; $i< $l; $i++) {
		my $c= substr($b, $i, 1);
		if(!($i % 0x010)) {
			printf("\n\t0x%04x:  ", $i);
		}
		printf(" %02x", ord($c));
	}
	print "\n";
}



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
				"token= $tokenrecv length= $length";
			printbuffer($text);
			if($operation eq $CULSERVER_OP_GETTIME) {
				my $elapsed_time= (($b[3]*256+$b[2])*256+$b[1])*256+$b[0];
				print "Elapsed time: $elapsed_time sec\n";
			} elsif($operation eq $CULSERVER_OP_GETIP) {
				printf("%d.%d.%d.%d\n", $b[0],$b[1],$b[2],$b[3]);
			} elsif($operation eq $CULSERVER_OP_GETSTAT) {
				printf("RNDIS  in= %3d out= %3d\nCC1101 in= %3d out= %3d\n", $b[0],$b[1],$b[2],$b[3]);
			} elsif($operation eq $CULSERVER_OP_GETCC1101STATUS) {
				printf("Part number for CC1101= %d\n", $b[0]);
				printf("Current version number= %d\n", $b[1]);
				printf("Frequency Offset Estimate= %d\n", $b[2]);
				printf("Demodulator estimate for Link Quality= %d\n", $b[3]);
				printf("Received signal strength indication= %d\n", $b[4]);
 				printf("Control state machine state= %d (%s)\n", $b[5], $MARCSTATE[$b[5]]);
				printf("WOR timer= %d\n", $b[6]*256+$b[7]);
				printf("Current GDOx status and packet status= %d\n", $b[8]);
				printf("  CRC OK         = %d\n", bit($b[8], 7));
				printf("  Carrier Sense  = %d\n", bit($b[8], 6));
				printf("  PQT reached    = %d\n", bit($b[8], 5));
				printf("  Channel clear  = %d\n", bit($b[8], 4));
				printf("  Sync word found= %d\n", bit($b[8], 3));
				printf("  GDO2           = %d\n", bit($b[8], 2));
				printf("  GDO0           = %d\n", bit($b[8], 0));
				printf("Current setting from PLL calibration module= %d\n", $b[9]);
				printf("Underflow and number of bytes in TX FIFO= %d bytes\n", $b[10]);
				printf("Overflow and number of bytes in RX FIFO= %d bytes\n", $b[11]);
				printf("Last RC oscillator calibration result= %d\n", $b[12]*256+$b[13]);
}

		} elsif($exitcode == $NAK) {
			print "< NAK\n";
		} else {
			die "< Illegal exit code";
		}
	}



