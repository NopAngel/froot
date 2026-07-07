use strict;
use warnings;
use File::Spec;

%ENV = (
	'PATH' => '/bin:/usr/bin:/sbin:/usr/sbin',
);

my $gcc = '/usr/bin/gcc';

sub __verify_gcc {
	if (-e $gcc && -x $gcc) {
		print(" [OK] gcc found.");
		return 1;
	} 
	else {
		warn " [-] ERR: gcc not found\n";
		return 0;
	}
}

if (__verify_gcc()) {
	print " [x] Done.\n";
	exit(0);
} else {
	print " [-] err \n";
	exit(1);
}
