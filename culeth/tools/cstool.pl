#!/usr/bin/perl

use strict;
use IO::Socket;

###############################################################################

my $REMOTEHOST	= '192.168.108.127';
my $REMOTEPORT	= 7073;
my $VERBOSE	= 1;

###############################################################################

my $USAGE= "Usage: cstool [remotehost[:remoteport]]";


my $CULServer;
my $token;	# gets incremented every time a command is sent

my $ACK		=  6;
my $NAK		= 21;
my $OP_GETSTAT			= 's';
my $OP_GETCC1101STATUS		= 'u';
my $OP_GETCC1101CONFIG		= 'c';
my $OP_GETTIME			= 't';
my $OP_BLINK	 		= 'b';
my $OP_TESTPACKETONAIR		= 'a';
my $OP_TESTPACKETTOTARGET	= 'p';
my $OP_BOOTLOADER		= '>';
my $OP_GETEEPROMDATA		= 'e';
my $OP_EEPROMFACTORYRESET	= 'E';
my $OP_REBOOT			= '0';
my $OP_SETADAPTERMACADDRESS 	= 'A';
my $OP_SETCULSERVERMACADDRESS 	= 'M';
my $OP_SETCULSERVERIPPORT  	= 'I';
my $OP_SETTARGETIPPORT  	= 'T';


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


################################################################################

sub bit($$) {
	my ($byte, $bit)= @_;
	return(($byte >> $bit) & 1);
}

###############################################################################

sub printbuffer($) {
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


###############################################################################

sub CULServer_connect($$) {

	my ($remotehost, $remoteport)= @_;
	$CULServer= IO::Socket::INET->new(
		PeerHost => $remotehost,
		PeerPort => $remoteport,
		Proto    => 'udp'); # Timeout => 10

	$token= 0;
}

################################################################################

sub CULServer_sendrequest($$) {

	my ($command,$argument)= @_;

	#
	# build server request
	# the server request consists of 4+length($argument) bytes:
	#  byte 0:	command bytes
	#  byte 1:	unique token
	#  byte 2, 3:	reserved (always 0)
	#
	$token++;
 	my $input= $command . pack("C", $token) . "\0\0" . $argument;
	if($VERBOSE) {
		print("Request:\n");
		printbuffer($input);
	}
	$CULServer->send($input)== length($input)
		or die "FATAL: cannot send to CULServer: $!";

	#
	# get server reply
	#
	my $output;
	my $operation;
	my $exitcode;
	my $length;
	my $text;
	my @b;
	my $tokenrecv;

	(my $remotepaddr= $CULServer->recv($output, 128, 0))
		or die "FATAL: recv: $!";
	my ($remoteport, $remoteiaddr)= sockaddr_in($remotepaddr);
	my $remoteip= inet_ntoa($remoteiaddr);
	print("Reply from $remoteip:\n") if($VERBOSE);

	# split received text string into characters
	my @chars= split("", $output);
	# retrieve operation
	$operation= shift @chars;
	# treat rest as bytes
	($tokenrecv, $exitcode, $length, @b)= map { ord($_) } @chars;
	# text representation of buffer
	$text= substr($output, 4, $length);

	# check if we received a reply to our command
	if($tokenrecv != $token) {
		die "FATAL: received token $tokenrecv instead of $token.";
	}

	if($exitcode == $ACK) {
		if($VERBOSE) {
			printbuffer($text);
		}
	} elsif($exitcode == $NAK) {
		print "ERROR: NAK\n";
	} else {
		die "FATAL: Illegal exit code";
	}

	return($exitcode == $ACK ? 1:0, $text, @b);
}

###############################################################################

my %functions= (
	"?" => "show_menu",
	"B" => "start_bootloader",
	"r" => "reboot",
	"A" => "set_adapter_MAC_address",
	"M" => "set_CULServer_MAC_address",
	"S" => "set_CULServer_IP_address_and_port",
	"T" => "set_target_IP_address_and_port",
	"e" => "get_eeprom_data",
	"E" => "eeprom_factory_reset",
	"s" => "get_CC1101_status",
	"c" => "get_CC1101_configuration",
	"pa" => "send_test_packet_on_air",
	"pt" => "send_test_packet_to_target",
	"t" => "get_time",
	"b" => "blink",
	"x" => "exit_ui",
);

###############################################################################

sub enter_MAC($$) {

	my ($prompt, $default)= @_;
	print $prompt;
	my $MACtxt= <STDIN>;
	chomp $MACtxt;
	$MACtxt= $default if($MACtxt == "");
	$MACtxt= uc($MACtxt);
	if($MACtxt !~ /[0-9A-F]{2}(:[0-9A-F]{2}){4}/) {
		print "ERROR: wrong format\n";
		return undef;
	}
	$MACtxt=~ s/://g;
	return pack("H*",$MACtxt);

}

sub enter_IP($$) {

	my ($prompt, $default)= @_;
	print $prompt;
	my $IPtxt= <STDIN>;
	chomp $IPtxt;
	$IPtxt= $default if($IPtxt == "");
	if($IPtxt !~ /[0-9]+(\.[0-9]+){3}/) {
		print "ERROR: wrong format\n";
		return undef;
	}
	my @IPs= split /\./, $IPtxt;
	return pack("C*",@IPs);

}

sub enter_Port($$) {

	my ($prompt, $default)= @_;
	print $prompt;
	my $port= <STDIN>;
	chomp $port;
	$port= $default if($port == "");
	return chr($port & 0xFF) . chr($port>>8); # ATMEL byte order!
}


#------------------------------------------------------------------------------

sub show_menu() {

	print "CUL functions:\n";
	print "  B - start bootloader\n";
	print "  r - reboot\n";
	print "RNDIS functions:\n";
	print "  A - set adapter MAC address\n";
	print "CULServer functions:\n";
	print "  M - set CULServer MAC address\n";
	print "  S - set CULServer IP address and port\n";
	print "  T - set target IP address and port\n";
	print "  e - get EEPROM data\n";
	print "  E - EEPROM factory reset\n";
	print "  b - blink\n";
	print " pa - send test packet on air\n";
	print " pt - send test packet to target\n";
	print "CC1101 functions:\n";
	print "  s - get CC1101 status\n";
	print "  c - get CC1101 configuration\n";
	print "Miscellaneous functions:\n";
	print "  t - get CULServer time\n";
	print "  x - exit\n";
	print "\n";

}

#------------------------------------------------------------------------------

sub start_bootloader() {

	my ($rc, $text, @b)= CULServer_sendrequest($OP_BOOTLOADER, "");
	if($rc) {
		print "Entering bootloader...\n";
		exit 0;
	}

}

sub reboot() {

	my ($rc, $text, @b)= CULServer_sendrequest($OP_REBOOT, "");
	if($rc) {
		print "Rebooting...\n";
		exit 0;
	}

}

sub set_adapter_MAC_address() {

	my $MAC= enter_MAC("Adapter MAC address (02:00:02:00:02:00)? ", "02:00:02:00:02:00");
	return if(!defined($MAC));

	my ($rc, $text, @b)= CULServer_sendrequest($OP_SETADAPTERMACADDRESS, $MAC);
	if($rc) {
		print "Device will reboot.\n";
		exit 0;
	}

}

sub set_CULServer_MAC_address() {

	my $MAC= enter_MAC("CULServer MAC address (02:50:8b:00:00:01)? ", "02:50:8b:00:00:01");
	return if(!defined($MAC));

	my ($rc, $text, @b)= CULServer_sendrequest($OP_SETCULSERVERMACADDRESS, $MAC);
	if($rc) {
		print "Device will reboot. Please unplug und replug device.\n";
		exit 0;
	}

}

sub set_CULServer_IP_address_and_port() {

	my $IP= enter_IP("CULServer IP address (192.168.108.127)? ", "192.168.108.127");
	return if(!defined($IP));
	my $port= enter_Port("CULServer IP address (7073)? ", "7073");

	my ($rc, $text, @b)= CULServer_sendrequest($OP_SETCULSERVERIPPORT, $IP . $port);
	if($rc) {
		print "Please reconnect.\n";
		exit 0;
	}
}

sub set_target_IP_address_and_port() {

	my $IP= enter_IP("Target IP address (192.168.108.1)? ", "192.168.108.1");
	return if(!defined($IP));
	my $port= enter_Port("Target IP address (7073)? ", "7073");

	my ($rc, $text, @b)= CULServer_sendrequest($OP_SETTARGETIPPORT, $IP . $port);
}

sub eeprom_factory_reset() {

	my ($rc, $text, @b)= CULServer_sendrequest($OP_EEPROMFACTORYRESET, "");
	if($rc) {
		print "Device will reboot.\n";
		exit 0;
	}
}

sub get_eeprom_data() {

	my ($rc, $text, @b)= CULServer_sendrequest($OP_GETEEPROMDATA, "");

	print "EEPROM data:\n";
	printbuffer($text);

}

sub get_CC1101_status() {

	my ($rc, $text, @b)= CULServer_sendrequest($OP_GETCC1101STATUS, "");
	if($rc) {
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
}

sub get_CC1101_configuration() {

	my ($rc, $text, @b)= CULServer_sendrequest($OP_GETCC1101CONFIG, "");

	print "Configuration data:\n";
	printbuffer($text);
}

sub send_test_packet_on_air() {

	my ($rc, $text, @b)= CULServer_sendrequest($OP_TESTPACKETONAIR, "");

}

sub send_test_packet_to_target() {

	my ($rc, $text, @b)= CULServer_sendrequest($OP_TESTPACKETTOTARGET, "");

}

sub get_time() {

	my ($rc, $text, @b)= CULServer_sendrequest($OP_GETTIME, "");
	if($rc) {
		my $s= (($b[3]*256+$b[2])*256+$b[1])*256+$b[0];
		my $h= int($s/3600); $s-= $h*3600;
		my $m= int($s/60); $s-= $m*60;
		printf("Uptime: %d:%02d:%02d\n", $h, $m, $s);
	}

}

sub blink() {

	my ($rc, $text, @b)= CULServer_sendrequest($OP_BLINK, "");

}

sub exit_ui() {
	exit 0;

}

###############################################################################

sub ui() {

	my $text;
	my $exitcode;
	my $input;
	my $output;

	my $token= 0;

	while(1) {

		#
		# prompt
		#
		print "$REMOTEHOST:$REMOTEPORT> ";

		#
		# user choice from menu
		#
		my $choice=<STDIN>;
		chomp $choice;
		my $fn= $functions{$choice};
		if(!defined($fn)) {
			print "ERROR: unknown function\n";
			next;
		}
		no strict "refs";
		&{$fn}();
		use strict "refs";
	}


}

###############################################################################
#
# main
#
###############################################################################

($#ARGV<1) or die $USAGE;

if($#ARGV==0) {
	my $remoteport;
	($REMOTEHOST,$remoteport)= split(/:/, $ARGV[0]);
	$REMOTEPORT= $remoteport if(defined($remoteport));
}

CULServer_connect($REMOTEHOST,$REMOTEPORT);
ui();
